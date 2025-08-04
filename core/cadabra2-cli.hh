#include <string>
#include <memory>
#include <fstream>

// Work around MSVC linking problem
#ifdef _DEBUG
#define CADABRA_CLI_DEBUG_MARKER
#undef _DEBUG
#endif
//#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#ifdef CADABRA_CLI_DEBUG_MARKER
#define _DEBUG
#undef CADABRA_CLI_DEBUG_MARKER
#endif

class Shell : public pybind11::scoped_interpreter {
	public:
		enum class Flags : unsigned int {
			None             = 0x00,
			NoBanner         = 0x01,
			IgnoreSemicolons = 0x02,
			NoColour         = 0x04,
			NoReadline       = 0x08,
			TeXmacs          = 0x10
		};
		
		Shell(Flags flags);
		~Shell();
		
		void start();
		void interact();
		void interact_texmacs();
		pybind11::object evaluate(const std::string& code, const std::string& filename = "<stdin>") const;
		void execute(const std::string& code, const std::string& filename = "<stdin>");
		void execute_file(const std::string& filename, bool preprocess = true);
		void interact_file(const std::string& filename, bool preprocess = true);

		void show_banner() const;
		void write_stdout(const std::string& text, const std::string& end = "\n", bool flush = false);
		void write_stderr(const std::string& text, const std::string& end = "\n", bool flush = false);
		
		class CatchOutput {
			public:
				CatchOutput();
				CatchOutput(const CatchOutput&);

				void        write(const std::string& txt);
				void        clear();
				void        flush();
				std::string str() const;
			private:
				std::string collect;
			};

		CatchOutput catchOut, catchErr;

	private:
		void set_histfile();
		std::string histfile;
		std::string site_path;
		std::ofstream logf;

		std::string str(const pybind11::handle& obj) const;
		std::string repr(const pybind11::handle& obj);
		std::string sanitize(std::string s);

		void process_ps1(const std::string& line);
		void process_ps2(const std::string& line);
		void set_completion_callback(const char* buffer, std::vector<std::string>& completions);

		std::string get_ps1();
		std::string get_ps2();

		void handle_error();
		void handle_error(pybind11::error_already_set& err);

		void open_texmacs_logfile();
		
		// These mimic what we do in Server.cc
		pybind11::module             main_module;
		pybind11::object             main_namespace;
		// FIXME: Do we need globals?
		pybind11::dict   globals;
		pybind11::object sys;
		pybind11::object py_stdout, py_stderr;
		std::string      collect;

		const char* colour_error;
		const char* colour_warning;
		const char* colour_info;
		const char* colour_success;
		const char* colour_reset;
		const char* colour_bold;
		Flags flags;
};

class ExitRequest : public std::exception {
	public:
		ExitRequest();
		ExitRequest(int code);
		ExitRequest(const std::string& message);

		virtual const char* what() const noexcept override;

		int code;
		std::string message;
};


Shell::Flags& operator |= (Shell::Flags& lhs, Shell::Flags rhs);
Shell::Flags operator | (Shell::Flags lhs, Shell::Flags rhs);
bool operator & (Shell::Flags lhs, Shell::Flags rhs);
