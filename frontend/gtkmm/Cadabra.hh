
#pragma once

#include <gtkmm/application.h>
#include <gtkmm/grid.h>
#include "NotebookWindow.hh"
#include "ComputeThread.hh"
#include "ScriptThread.hh"

/// The Cadabra notebook application.

class Cadabra : public Gtk::Application {
	public:
		static Glib::RefPtr<Cadabra> create(int, char **);

		bool open_help(const std::string& filename, const std::string& title);

	protected:
		Cadabra(int, char**);
		virtual ~Cadabra();

		virtual void on_startup() override;
		virtual void on_activate() override;
		virtual void on_open(const Gio::Application::type_vec_files& files, const Glib::ustring& hint) override;
		virtual int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& cmd) override;


		int on_handle_local_options(const Glib::RefPtr<Glib::VariantDict> &);

	private:
		cadabra::ComputeThread                *compute;
		std::thread                           *compute_thread;

		cadabra::ScriptThread                 *script;
		std::thread                           *script_thread;

		int server_port;
		std::string server_token;
		std::string server_ip_address;
		std::string window_geometry;
		std::string window_title;
		bool        no_registration;
	};
