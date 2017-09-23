#include <iostream>
#include <boost/program_options.hpp>

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif

struct Brush {
	int dummy=1;
};

struct theMap {
	std::string message = "Created by sago-map-generator-one";
};

static void writeBrush(std::ostream* output, int number, const Brush& b) {
	*output << "// brush " << number << "\n"
"{\n"
"( -192 0 0 ) ( -192 16 0 ) ( -192 0 16 ) e7/e7bricks01 32 96 0 0.5 0.5 0 0 0\n"
"( 448 0 0 ) ( 448 0 16 ) ( 448 16 0 ) e7/e7bricks01 16 96 0 0.5 0.5 0 0 0\n"
"( 0 -64 0 ) ( 0 -64 16 ) ( 16 -64 0 ) e7/e7bricks01 16 96 0 0.5 0.5 0 0 0\n"
"( 0 64 0 ) ( 16 64 0 ) ( 0 64 16 ) e7/e7bricks01 64 96 0 0.5 0.5 0 0 0\n"
"( 0 0 -128 ) ( 0 16 -128 ) ( 16 0 -128 ) e7/e7bricks01 48 0 0 0.5 0.5 0 0 0\n"
"( 0 0 -144 ) ( 16 0 -144 ) ( 0 16 -144 ) e7/e7bricks01 0 0 0 0.5 0.5 0 0 0\n"
"}\n";
}

static void writeMap(const theMap& m) {
	std::ostream* output = &std::cout;
	*output << "// entity 0\n";
	*output << "{\n";
	*output << "\"classname\" \"worldspawn\"\n" <<
		"\"message\" \""<< m.message<< "\"\n"; 	
	Brush b;
	writeBrush(output,0,b);
	*output << "}\n";
}

int main(int argc, const char* argv[]) {
	boost::program_options::options_description desc("Options");
	desc.add_options()
	("version", "Print version information and quit")
	("help,h", "Print basic usage information to stdout and quit")
	("output,o", boost::program_options::value<std::string>(), "Output file")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 0;
	}
	if (vm.count("version")) {
		std::cout << "sago-map-generator-one " << VERSIONNUMBER << "\n";
		return 0;
	}
	std::string output_filename;
	if (vm.count("output")) {
		output_filename = vm["output"].as<std::string>();
	}
	theMap m;
	writeMap(m);
	return 0;
}
