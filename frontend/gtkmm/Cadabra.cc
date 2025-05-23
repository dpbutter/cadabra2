
#include "Cadabra.hh"
#include <signal.h>
#include <fstream>
#include <gtkmm/messagedialog.h>
#include <gtkmm/entry.h>
#if GTKMM_MINOR_VERSION < 10
#include <gtkmm/main.h>
#endif
#include <gtkmm/settings.h>
#include <giomm.h>
#include "Snoop.hh"
#include "Config.hh"

// Signal handler for ctrl-C

cadabra::NotebookWindow *signal_window;

void signal_handler(int)
	{
#if GTKMM_MINOR_VERSION >= 10
	signal_window->close();
#else
	Gtk::Main::quit();
#endif
	}

Glib::RefPtr<Cadabra> Cadabra::create(int argc, char **argv)
	{
	return Glib::RefPtr<Cadabra>( new Cadabra(argc, argv) );
	}

Cadabra::Cadabra(int argc, char **argv)
	: Gtk::Application(argc, argv, "science.cadabra.cadabra2-gtk",
	                   Gio::APPLICATION_HANDLES_OPEN |
							 Gio::APPLICATION_HANDLES_COMMAND_LINE |
	                   Gio::APPLICATION_NON_UNIQUE),
	  compute(0), compute_thread(0),
	  server_port(0), server_ip_address("127.0.0.1"),
	  no_registration(false)
	{
	// https://stackoverflow.com/questions/43886686/how-does-one-make-gtk3-look-native-on-windows-7
	//	https://github.com/shoes/shoes3/wiki/Changing-Gtk-theme-on-Windows
#if defined(_WIN32)
	// Gtk::Settings::get_default()->property_gtk_theme_name()="win32";
#endif

	//https://github.com/GNOME/gtkmm-documentation/blob/master/examples/book/application/command_line_handling/exampleapplication.cc

	add_main_option_entry(Gio::Application::OptionType::OPTION_TYPE_INT,
	                      "server-port",
	                      's',
	                      "Connect to running server on given port.",
	                      "number");
	add_main_option_entry(Gio::Application::OptionType::OPTION_TYPE_STRING,
	                      "geometry",
	                      'g',
	                      "Specify the window size (and optionally the position).",
	                      "string");
	add_main_option_entry(Gio::Application::OptionType::OPTION_TYPE_STRING,
	                      "title",
	                      'w',
	                      "Specify the window title.",
	                      "string");
	add_main_option_entry(Gio::Application::OptionType::OPTION_TYPE_STRING,
	                      "server-ip-address",
	                      'a',
	                      "Connect to running server on given ip address.",
	                      "string");
	add_main_option_entry(Gio::Application::OptionType::OPTION_TYPE_STRING,
	                      "token",
	                      't',
	                      "Use the given authentication token to connect to the server.",
	                      "string");
	add_main_option_entry(Gio::Application::OptionType::OPTION_TYPE_BOOL,
	                      "no-registration",
	                      'n',
	                      "Do not show the registration dialog.",
	                      "string");
	}

template <typename T_ArgType>
static bool get_arg_value(const Glib::RefPtr<Glib::VariantDict>& options, const Glib::ustring& arg_name, T_ArgType& arg_value)
	{
	arg_value = T_ArgType();
	if(options->lookup_value(arg_name, arg_value)) return true;
	else return false;
	}

Cadabra::~Cadabra()
	{
	// The app is going away. First stop the script thread so external
	// programs can no longer talk to us.

	if(script)
		script->terminate();
	if(script_thread)
		script_thread->join();
	if(script)
		delete script;
	if(script_thread)
		delete script_thread;

	// Also stop the compute logic and join that thread waiting for it
	// to complete.

	if(compute)
		compute->terminate();
	if(compute_thread)
		compute_thread->join();
	if(compute)
		delete compute;
	if(compute_thread)
		delete compute_thread;


	//	for(auto w: windows)
	//		delete w;
	}

int Cadabra::on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& cmd)
	{
	int argc;
	char** argv = cmd->get_arguments(argc);
	std::vector<Glib::RefPtr<Gio::File>> files;
	for(int i = 1; i < argc; i++) {
		if(argv[i][0] != '-') {
			auto file = Gio::File::create_for_path(argv[i]);
			files.push_back(file);
			}
		}
	
	 auto options = cmd->get_options_dict();

	// The template `get_arg_value` only works if the type matches exactly,
	// so we need to ask it to store into a `Glib::ustring` and then copy.
	Glib::ustring tmp;
	bool btmp;
	
	get_arg_value(options, "server-port",       server_port);
	get_arg_value(options, "server-ip-address", tmp);
	server_ip_address = tmp;
	get_arg_value(options, "token",             tmp);
	server_token = tmp;
	get_arg_value(options, "no-registration",   btmp);
	no_registration = btmp;
	get_arg_value(options, "geometry",          tmp);
	window_geometry = tmp;
	get_arg_value(options, "title",             tmp);
	if(tmp=="") tmp="Cadabra";
	window_title = tmp;

	if(files.size()>0)
		open(files, "");
	else
		activate();

	return 0;
	}

void Cadabra::on_startup()
	{
	Gtk::Application::on_startup();
	}

void Cadabra::on_activate()
	{
	Gtk::Application::on_activate();
	
	compute = new cadabra::ComputeThread(server_port, server_token, server_ip_address);
	compute_thread = new std::thread(&cadabra::ComputeThread::run, compute);

	auto nw = new cadabra::NotebookWindow(this, false, window_geometry, window_title);
	compute->set_master(nw, nw);
	nw->main_thread_id = std::this_thread::get_id();

	// Connect the two threads.
	nw->set_compute_thread(compute);

	// Setup ctrl-C handler so we can shut down gracefully (i.e. ask
	// for confirmation, shut down server).
	signal_window = nw;
	signal(SIGINT, signal_handler);

	add_window(*nw);
	nw->show();

	// Start the script handler so external programs can talk to us.
	script = new cadabra::ScriptThread(nw, nw);
	script_thread = new std::thread(&cadabra::ScriptThread::run, script);

	std::cerr << "control channel ws://localhost:" << script->get_local_port()
				 << "/" << script->get_authentication_token() << std::endl;

	std::string version=std::string(CADABRA_VERSION_SEM);
	snoop::log("start") << version << snoop::flush;

	if(!no_registration && !nw->prefs.is_registered && !nw->prefs.is_anonymous) {
		nw->on_help_register();
		}
	}

void Cadabra::on_open(const Gio::Application::type_vec_files& files, const Glib::ustring& hint)
	{
	on_activate();
	// Load the first file into a string.
	char* contents = nullptr;
	gsize length = 0;
	std::string text;
	bool file_exists=false;
	try {
		if(files[0]->load_contents(contents, length)) {
			if(contents && length) {
				text=std::string(contents);
				}
			g_free(contents);
			}
		file_exists=true;
		}
	catch (const Glib::Error& ex) {
		// std::cerr << "Open error: " << ex.what() << std::endl;
		// File cannot be opened, assume the user wants to create a
		// new file with this name.
		file_exists=false;
		}

	// Tell the window to open the notebook stored in the string.
	auto wins = get_windows();
	auto nw = static_cast<cadabra::NotebookWindow *>(wins[0]);
	add_window(*nw);
	nw->set_name(files[0]->get_path());
	snoop::log("open") << "command-line" << snoop::flush;
	if(file_exists) {
		nw->load_from_string(text);
		Gtk::Application::on_open(files, hint);
		}
	}

bool Cadabra::open_help(const std::string& nm, const std::string& title)
	{
#ifdef DEBUG
	std::cerr << "Opening help file " << nm << std::endl;
#endif
	std::ifstream fl(nm);
	if(fl) {
		auto nw = new cadabra::NotebookWindow(this, true);
		nw->set_title_prefix("Cadabra help for ");
		nw->set_name(title);
		add_window(*nw);
		std::stringstream buffer;
		buffer << fl.rdbuf();
		nw->load_from_string(buffer.str());
		nw->show();
		return true;
		}
	else return false;
	}
