#include <fstream>
#include <iostream>
#include "DataCell.hh"

int main(int argc, char **argv)
	{
	if(argc<2) {
		std::cerr << "Usage: cadabra2html [--segment] [--strip-code] [cadabra notebook] [html file]\n\n";
		std::cerr << "Convert a Cadabra notebook to an HTML segment or standalone HTML file.\n"
					 << "  --segment:          generate output for the cadabra web site.\n"
					 << "  --strip-code:       suppress Python cells with 'def' or 'from' lines.\n"
					 << "  --hide-input-cells: suppress all input cells (overrides notebook setting).\n"
		          << "If the HTML file name is not given, output goes to standard out.\n";
		return -1;
		}

	std::string cdb_file, html_file;
	bool segment_only=false;
	bool strip_code=false;
	bool hide_input_cells=false;
	int n=1;
	while(n<argc) {
		if(std::string(argv[n])=="--segment")
			segment_only=true;
		else if(std::string(argv[n])=="--strip-code")
			strip_code=true;
		else if(std::string(argv[n])=="--hide-input-cells")
			hide_input_cells=true;
		else if(cdb_file=="")
			cdb_file=argv[n];
		else
			html_file=argv[n];
		++n;
		}
	//std::cerr << "stripping code: " << strip_code << std::endl;

	auto from=cdb_file.find_last_of("/");
	++from;
	auto to  =cdb_file.find_last_of(".");
	std::string t=cdb_file.substr(from, to-from);
	t[0]=toupper(t[0]);
	std::string title="Cadabra manual: "+t;

	std::ifstream file(cdb_file);
	if(!file.is_open()) {
		std::cerr << "cadabra2html: cannot open " << cdb_file << std::endl;
		return -1;
		}
	
	std::string content, line;
	while(std::getline(file, line))
		content+=line;

	cadabra::DTree doc;
	JSON_deserialise(content, doc);
	if(hide_input_cells)
		doc.hide_input_cells=hide_input_cells;
	std::string html = export_as_HTML(doc, segment_only, strip_code, title);

	if(html_file!="") {
		std::ofstream htmlfile(html_file);
		htmlfile << html;
		}
	else {
		std::cout << html;
		}

	return 0;
	}
