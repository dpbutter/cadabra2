
#include "TeXEngine.hh"
#include "Config.hh"
#include "InstallPrefix.hh"

#include <iostream>
#include <cstdio>
#include <boost/algorithm/string.hpp>
// #include "lodepng.h"
#include <fstream>
#include <sstream>
#include <internal/unistd.h>
#include <map>
#include <regex>
#include <glibmm/spawn.h>
#include "process.hpp"

//#define DEBUG

namespace tpl = TinyProcessLib;
using namespace cadabra;

#if defined(WIN32)
int setenv(const char *name, const char *value, int overwrite)
	{
	int errcode = 0;
	if(!overwrite) {
		size_t envsize = 0;
		errcode = getenv_s(&envsize, NULL, 0, name);
		if(errcode || envsize) return errcode;
		}
	return _putenv_s(name, value);
	}
#endif

double TeXEngine::millimeter_per_inch = 25.4;

TeXEngine::TeXException::TeXException(const std::string& str)
	: std::logic_error(str)
	{
	}

std::string TeXEngine::convert_unicode_to_tex(const std::string& orig) const
	{
	static std::map<std::string, std::string> symmap = {
			{"α", "\\alpha"    },
			{"β", "\\beta"     },
			{"γ", "\\gamma"    },
			{"δ", "\\delta"    },
			{"ε", "\\epsilon"  },
			{"ζ", "\\zeta"     },
			{"η", "\\eta"      },
			{"θ", "\\theta"    },
			{"ι", "\\iota"     },
			{"κ", "\\kappa"    },
			{"λ", "\\lambda"   },
			{"μ", "\\mu"       },
			{"ν", "\\nu"       },
			{"ξ", "\\xi"       },
			{"ο", "\\omicron"  },
			{"π", "\\pi"       },
			{"ρ", "\\rho"      },
			{"σ", "\\sigma"    },
			{"τ", "\\tau"      },
			{"υ", "\\upsilon"  },
			{"φ", "\\phi"      },
			{"χ", "\\chi"      },
			{"ψ", "\\psi"      },
			{"ω", "\\omega"    },

			{"Α", "\\Alpha"    },
			{"Β", "\\Beta"     },
			{"Γ", "\\Gamma"    },
			{"Δ", "\\Delta"    },
			{"Ε", "\\Epsilon"  },
			{"Ζ", "\\Zeta"     },
			{"Η", "\\Eta"      },
			{"ϴ", "\\Theta"    },
			{"Ι", "\\Iota"     },
			{"Κ", "\\Kappa"    },
			{"Λ", "\\Lambda"   },
			{"Μ", "\\Mu"       },
			{"Ν", "\\Nu"       },
			{"Ξ", "\\Xi"       },
			{"Ο", "\\Omicron"  },
			{"Π", "\\Pi"       },
			{"Ρ", "\\Rho"      },
			{"Σ", "\\Sigma"    },
			{"Τ", "\\Tau"      },
			{"Υ", "\\Upsilon"  },
			{"Φ", "\\Phi"      },
			{"Χ", "\\Chi"      },
			{"Ψ", "\\Psi"      },
			{"Ω", "\\Omega"    }
		};

	std::string ret=orig;
	auto it=symmap.begin();
	while(it!=symmap.end()) {
		std::cerr << "*** replacing " << (*it).first << std::endl;
		boost::replace_all(ret, (*it).first, (*it).second);
		++it;
		}
	std::cerr << ret << std::endl;
	return ret;
	}

unsigned TeXEngine::TeXRequest::width() const
	{
	return width_;
	}

unsigned TeXEngine::TeXRequest::height() const
	{
	return height_;
	}

std::string TeXEngine::TeXRequest::latex() const
	{
	return latex_string;
	}

const std::vector<unsigned char>& TeXEngine::TeXRequest::image() const
	{
	return image_;
	}

void TeXEngine::erase_file(const std::string& nm) const
	{
	std::remove(nm.c_str());
	}

std::string TeXEngine::handle_latex_errors(const std::string& result, int exit_code) const
	{
	std::string::size_type pos=result.find("! LaTeX Error");
	if(pos != std::string::npos) {
		return "LaTeX error, probably a missing style file. See the output below.\n\n" +result;
		}

	pos=result.find("! TeX capacity exceeded");
	if(pos != std::string::npos) {
		return "Output cell too large (TeX capacity exceeded), output suppressed.";
		}

	pos=result.find("! Dimension too large");
	if(pos != std::string::npos) {
		return "Output cell too large (dimension too large), output suppressed.";
		}

	pos=result.find("! Double superscript.");
	if(pos != std::string::npos) {
		return "Internal typesetting error: double superscript. Please report a bug.\n\n" + result;
		}
	pos=result.find("! Double subscript.");
	if(pos != std::string::npos) {
		return "Internal typesetting error: double subscript. Please report a bug.\n\n" + result;
		}

	pos=result.find("! Package breqn Error: ");
	if(pos != std::string::npos) {
		return "Typesetting error (breqn.sty related). Please report a bug.\n\n" + result;
		}

	pos=result.find("! Undefined control sequence");
	if(pos != std::string::npos) {
		std::string::size_type undefpos=result.find("\n", pos+30);
		if(undefpos==std::string::npos)
			return "Undefined control sequence (failed to parse LaTeX output).";
		std::string::size_type backslashpos=result.rfind("\\", undefpos);
		if(backslashpos==std::string::npos || backslashpos < 2)
			return "Undefined control sequence (failed to parse LaTeX output).";

		std::string undefd=result.substr(backslashpos-1,undefpos-pos-30);
		return "Undefined control sequence:\n\n" +undefd+"\nNote that all symbols which you use in cadabra have to be valid LaTeX expressions.\nIf they are not, you can still use the LaTeXForm property\nto make them print correctly; see the manual for more information.";
		}

	if(exit_code!=0) {
		return "Generic typesetting error; LaTeX failed. Please report a bug.\n\n" + result;
		}

	return "";
	}

TeXEngine::~TeXEngine()
	{
	}

TeXEngine::TeXEngine()
	: horizontal_pixels_(0), font_size_(12), total_scale_(1.0), device_scale_(1.0)
	{
#ifndef _WIN32
	//	latex_packages.push_back("breqn");
	latex_packages.push_back("hyperref");

	// Load the pre-amble from file.
	std::string pname = cadabra::install_prefix()+"/share/cadabra2/latex/preamble.tex";
	std::ifstream preamble(pname);
	if(!preamble)
		throw std::logic_error("Cannot open TeXEngine preamble at "+pname);
	std::stringstream buffer;
	buffer << preamble.rdbuf();
	preamble_string = buffer.str();
#endif
	}

void TeXEngine::set_geometry(int horpix)
	{
	if(horizontal_pixels_!=horpix) {
		// flag all requests as requiring an update
		std::set<std::shared_ptr<TeXRequest> >::iterator reqit=requests.begin();
		while(reqit!=requests.end()) {
			(*reqit)->needs_generating=true;
			++reqit;
			}
		}
	horizontal_pixels_=horpix;
	}

int TeXEngine::get_geometry() const
	{
	return horizontal_pixels_;
	}

void TeXEngine::set_font_size(int fontsize)
	{
	if(font_size_!=fontsize) {
		// flag all requests as requiring an update
		std::set<std::shared_ptr<TeXRequest> >::iterator reqit=requests.begin();
		while(reqit!=requests.end()) {
			(*reqit)->needs_generating=true;
			++reqit;
			}
		}
	// std::cerr << "FONTSIZE " << fontsize << std::endl;
	font_size_=fontsize;
	}

int TeXEngine::get_font_size() const
	{
	return font_size_;
	}

void TeXEngine::set_scale(double total_scale, double device_scale)
	{
	if(total_scale_!=total_scale && device_scale_!=device_scale) {
		// flag all requests as requiring an update
		std::set<std::shared_ptr<TeXRequest> >::iterator reqit=requests.begin();
		while(reqit!=requests.end()) {
			(*reqit)->needs_generating=true;
			++reqit;
			}
		}
	total_scale_=total_scale;
	device_scale_=device_scale;
	}

double TeXEngine::get_scale() const
	{
	return total_scale_;
	}

double TeXEngine::get_device_scale() const
	{
	return device_scale_;
	}

TeXEngine::TeXRequest::TeXRequest()
	: needs_generating(true)
	{
	}

std::shared_ptr<TeXEngine::TeXRequest> TeXEngine::checkin(const std::string& txt,
      const std::string& startwrap, const std::string& endwrap)
	{
	auto req = std::make_shared<TeXRequest>();
	req->latex_string=txt;
	req->start_wrap=startwrap;
	req->end_wrap=endwrap;
	req->needs_generating=true;
	requests.insert(req);
	return req;
	}

void TeXEngine::checkout(std::shared_ptr<TeXRequest> req)
	{
	std::set<std::shared_ptr<TeXRequest> >::iterator it=requests.find(req);
	assert(it!=requests.end());
	requests.erase(it);
	}

void TeXEngine::checkout_all()
	{
	requests.clear();
	}

void TeXEngine::invalidate_all()
	{
	auto it=requests.begin();
	while(it!=requests.end()) {
		(*it)->needs_generating=true;
		++it;
		}
	}

std::shared_ptr<TeXEngine::TeXRequest> TeXEngine::modify(std::shared_ptr<TeXRequest> req, const std::string& txt)
	{
	req->latex_string=txt;
	req->needs_generating=true;
	return req;
	}

void TeXEngine::convert_all()
	{
	size_t needs_generating=0;
	for(const auto& req: requests) {
		if(req->needs_generating)
			++needs_generating;
		}

	if(needs_generating!=0 && horizontal_pixels_!=0) {
		std::string error_string="";
		do {
			try {
				// std::cerr << "TeXEngine::convert_all: running TeX on " << needs_generating << " requests" << std::endl;
				convert_set(requests);
				needs_generating=0;
				}
			catch(TeXException& e) {
				// std::cerr << "TeXEngine::convert_all: error happened." << std::endl;
				error_string += e.what();
				needs_generating=0;
				for(const auto& req: requests)
					if(req->needs_generating)
						++needs_generating;
				// Run again; the erroneous cell has had its `needs_generating` flag set
				// to false and its `has_error` set to true, so we can safely feed this
				// through again.
				}
			}
		while(needs_generating>0);

		if(error_string!="")
			throw TeXException(error_string);
		}
	}

void TeXEngine::convert_one(std::shared_ptr<TeXRequest> req)
	{
	//std::cerr << "cadabra-client: running TeX on 1 request" << std::endl;
	std::set<std::shared_ptr<TeXRequest> > reqset;
	reqset.insert(req);
	convert_set(reqset);
	}

void TeXEngine::convert_set(std::set<std::shared_ptr<TeXRequest> >& reqs)
	{
	throw std::logic_error("TeXEngine::convert_set: disabled in favour of MicroTeX.");

#if 1==0	
	
	// We now no longer follow
	//
	// https://www.securecoding.cert.org/confluence/display/seccode/FI039-C.+Create+temporary+files+securely
	//
	// for temporary files; it needs to work on all platforms...

	char olddir[1024];
	if(getcwd(olddir, 1023)==NULL)
		olddir[0]=0;
	std::string tmpdir="/tmp";
	std::string tmppath=std::tmpnam(0);
#if defined(_WIN32)
	tmpdir=getenv("TEMP");
	// strip tmpdir from tmppath to get just the file
	auto sl=tmppath.find_last_of('\\');
	if(sl!=std::string::npos)
		tmppath=tmppath.substr(sl+1);
	tmpdir+="\\";
#else
	// strip tmpdir from tmppath to get just the file
	auto sl=tmppath.find_last_of('/');
	if(sl!=std::string::npos) {
		tmpdir=tmppath.substr(0, sl);
		tmppath=tmppath.substr(sl+1);
		}
	tmpdir+="/";
#endif

	if(chdir(tmpdir.c_str())==-1)
		throw TeXException("Failed to chdir to "+tmpdir+".");

#ifdef DEBUG
	std::cerr << "tmpdir  = " << tmpdir << std::endl;
	std::cerr << "tmppath = " << tmppath << std::endl;
#endif

	//	char templ[]="/tmp/cdbXXXXXX";

	// The size in mm or inches which we use will in the end determine how large
	// the font will come out.
	//
	// For given horizontal size, we stretch this up to the full window
	// width using horizontal_pixels/(horizontal_size/millimeter_per_inch) dots per inch.
	// The appropriate horizontal size in mm is determined by trial and error,
	// and of course scales with the number of horizontal pixels.

	// Note: the number here has no effect on the size in pixels of the generated
	// PDF. That is set with the -D parameter of dvipng.

	const double horizontal_mm=horizontal_pixels_*(12.0/font_size_)/4.4/(total_scale_/device_scale_);
	//#ifdef DEBUG
	//	std::cerr << "tex_it: font_size " << font_size << std::endl
	//				 << "        pixels    " << horizontal_pixels_ << std::endl
	//				 << "        mm        " << horizontal_mm << std::endl;
	//#endif

	//(int)(millimeter_per_inch*horizontal_pixels/100.0); //140;
	const double vertical_mm=std::min(10*horizontal_mm, 3000.0);


	// Write each string in the set of requests into a buffer, separating
	// them by a page eject.

	std::ostringstream total;
	std::ofstream outstr(tmppath);
	//	int fd = mkstemp(templ);
	//	if(fd == -1)
	//		 throw TeXException("Failed to create temporary file in /tmp.");

	total << "\\documentclass[11pt]{article}\n"
	      << "\\usepackage[dvips,verbose,voffset=0pt,hoffset=0pt,textwidth="
	      << horizontal_mm << "mm,textheight="
	      << vertical_mm << "mm]{geometry}\n"
#ifndef __APPLE__
	      //			<< "\\usepackage{inconsolata}\n"
#endif
	      ;

	for(size_t i=0; i<latex_packages.size(); ++i)
		total << "\\usepackage{" << latex_packages[i] << "}\n";

	total << preamble_string;


	std::set<std::shared_ptr<TeXRequest> >::iterator reqit=reqs.begin();
	while(reqit!=reqs.end()) {
		if((*reqit)->needs_generating) {
			if((*reqit)->latex_string.size()>100000)
				total << "Expression too long, output suppressed.\n";
			else {
				if((*reqit)->start_wrap.size()>0)
					total << (*reqit)->start_wrap;

				std::string lr=(*reqit)->latex_string;
				boost::replace_all(lr, "\\left(", "\\brwrap{(}{");
				boost::replace_all(lr, "\\right)", "}{)}");
				boost::replace_all(lr, "\\left[", "\\brwrap{[}{");
				boost::replace_all(lr, "\\right]", "}{]}");
				boost::replace_all(lr, "\\left\\{", "\\brwrap{\\{}{");
				boost::replace_all(lr, "\\right\\}", "}{\\}}");
				boost::replace_all(lr, "\\right.", "}{.}");
				boost::replace_all(lr, "\\begin{dmath*}", "$");
				boost::replace_all(lr, "\\end{dmath*}", "$");

				total << lr;

				if((*reqit)->end_wrap.size()>0)
					total << "\n" << (*reqit)->end_wrap;
				else total << "\n";
				}
			total << "\\eject\n";
			}
		++reqit;
		}
	total << "\\end{document}\n";

	// Make sure there are no greek unicode characters.
	std::string ltx = total.str(); // convert_unicode_to_tex(total.str());

	// Now write the 'total' buffer to the .tex file

	// std::cerr << total.str() << std::endl;

	outstr << ltx;
	outstr.close(); // to flush the buffer and allow us to rename

#ifdef DEBUG
	std::cerr  << tmppath << std::endl;
	std::cerr << "---\n" << ltx << "\n---" << std::endl;
#endif

	std::string nf=tmppath+".tex";
	if(std::rename((tmpdir+tmppath).c_str(), (tmpdir+nf).c_str())!=0)
		throw TeXException("Failed to rename "+tmpdir+tmppath+" to "+tmpdir+nf+" in folder "+tmpdir+" error "+std::to_string(errno));

#ifdef __CYGWIN__
	// MikTeX does not see /tmp, it needs \cygwin\tmp
	nf="\\cygwin"+nf;
	nf.replace(nf.begin(), nf.end(), '/', '\\');
#endif

	// Run LaTeX on the .tex file.
	std::string texinputs=cadabra::install_prefix()+"/share/cadabra2/latex/";
	std::string oldtexinputs;
	char *oti = getenv("TEXINPUTS");
	if(oti)
		oldtexinputs=std::string(oti);
	if(oldtexinputs.size()>0)
		texinputs=":"+std::string(oldtexinputs);
	texinputs+=":";
	setenv("TEXINPUTS", texinputs.c_str(), 1);

	// The options below are chosen so they are compatible with MikTeX (https://docs.miktex.org/manual/pdftex.html),
	// standard TeXLive on Linux and OSX. We use pdflatex because several latex distributions do not seem to have
	// a 'latex' command anymore which supports all these options (e.g. MikTeX).
	// For some reason, recent pdflatex versions no longer do anything with --halt-on-error, so we
	// now also add the -interaction=nonstopmode (errorstopmode does not work either).

	std::string latex_stdout, latex_stderr;
	tpl::Process latex_proc("pdflatex --halt-on-error -interaction=nonstopmode --output-format=dvi "+nf, "",
	[&](const char *bytes, size_t n) {
		latex_stdout+=std::string(bytes,n);
		},
	[&](const char *bytes, size_t n) {
		latex_stderr+=std::string(bytes,n);
		}
	                       );
	auto latex_exit_status=latex_proc.get_exit_status();

	std::string err=handle_latex_errors(latex_stdout+latex_stderr, latex_exit_status);
	setenv("TEXINPUTS", oldtexinputs.c_str(), 1);

	if(err.size()>0) {
		// std::cerr << "ERROR: " << latex_stderr << std::endl;
		// std::cerr << "OUTPUT: " << latex_stdout << std::endl;
		// std::cerr << "FILE: " << nf << std::endl;
		std::regex  pat(R"(\[([0-9]*)\])");
		std::smatch sm;
		int last_error_free_page=0;
		while(std::regex_search( latex_stdout, sm, pat)) {
			// std::cerr << "PAGE: " << sm.str() << std::endl;
			last_error_free_page = std::stoi( sm[1].str() );
			latex_stdout = sm.suffix();
			}
		// std::cerr << "PAGE WITH ERROR: " << last_error_free_page+1 << std::endl;
		// Label the error request so it can be ignored next time.
		reqit=reqs.begin();
		int current_page=1;
		while(reqit!=reqs.end()) {
			if((*reqit)->needs_generating) {
				if(current_page == last_error_free_page+1) {
					// std::cerr << "CONTAINS ERROR: " << (*reqit)->latex_string << std::endl;
					(*reqit)->has_error=true;
					(*reqit)->needs_generating=false;
					}
				++current_page;
				(*reqit)->image_.clear();
				}
			++reqit;
			}
		erase_file(tmppath+".dvi");
		if(chdir(olddir)==-1)
			throw TeXException(err+" (and cannot chdir back to original "+olddir+").");
		else err+=".";
		err += " See "+tmppath+".tex to debug this.\n\n";
		err += "TEXINPUTS = "+texinputs+"\n";
		err += "TMPDIR = "+tmpdir+"\n";
		throw TeXException(err);
		}

	erase_file(tmppath+".aux");
	erase_file(tmppath+".log");
	erase_file(tmppath+".out");

	erase_file(tmppath+".tex");

	// Convert the entire dvi file to png files.
	//
	std::ostringstream resspec;
	resspec << horizontal_pixels_/(1.0*horizontal_mm)*millimeter_per_inch*device_scale_;

	std::string dvipng_stdout, dvipng_stderr;
	tpl::Process dvipng_proc("dvipng -T tight -bg transparent -D "+resspec.str()+" "+tmppath+".dvi", "",
	[&](const char *bytes, size_t n) {
		dvipng_stdout=std::string(bytes,n);
		},
	[&](const char *bytes, size_t n) {
		dvipng_stderr=std::string(bytes,n);
		}
	                        );
	auto dvipng_exit_status=dvipng_proc.get_exit_status();

	if(dvipng_exit_status!=0) {
		// Erase all dvi and png files and put empty pixbufs into the TeXRequests.
		erase_file(tmppath+".dvi");
		reqit=reqs.begin();
		int pagenum=1;
		while(reqit!=reqs.end()) {
			if((*reqit)->needs_generating) {
				std::ostringstream pngname;
				pngname << tmppath << pagenum << ".png";
				erase_file(pngname.str());
				(*reqit)->image_.clear();
				++pagenum;
				}
			++reqit;
			}
		if(chdir(olddir)==-1)
			throw TeXException(
			   std::string("Cannot run dvipng, is it installed? (and cannot chdir back to original)\n\n")+dvipng_stdout+dvipng_stderr);
		throw TeXException(std::string("Cannot run dvipng, is it installed?\n\n")+dvipng_stdout+dvipng_stderr);
		}

	erase_file(tmppath+".dvi");

	// Conversion completed successfully, now convert all resulting PNG files to Pixbuf images.

	//std::cerr << "cadabra-client: reading png" << std::endl;
	reqit=reqs.begin();
	int pagenum=1;
	while(reqit!=reqs.end()) {
		if((*reqit)->needs_generating) {
			std::ostringstream pngname;
			pngname << tmppath << pagenum << ".png";
			std::ifstream tst(pngname.str().c_str());
			if(tst.good()) {
				(*reqit)->image_.clear();
				unsigned error = lodepng::decode((*reqit)->image_, (*reqit)->width_,
				                                 (*reqit)->height_, pngname.str());
				if(error!=0)
					throw TeXException("PNG conversion error");
				(*reqit)->needs_generating=false;
				erase_file(pngname.str());
				}
			++pagenum;
			}
		++reqit;
		}
	//std::cerr << "cadabra-client: conversion done" << std::endl;

	if(chdir(olddir)==-1)
		throw TeXException("Failed to chdir back to " +std::string(olddir)+".");

#endif
	}
