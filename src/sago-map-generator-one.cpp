#include <iostream>
#include <boost/program_options.hpp>
#include <vector>

#ifndef VERSIONNUMBER
#define VERSIONNUMBER "0.1.0"
#endif

struct Config {
	int unitSize = 16;  //All other sizes are multiplies of this
	int minSize = 20;
	int maxSize = 50;
	int maxPerLayer = 10;
	int numberOfLayers = 5;
	int layerDistance = 32;
	int platformThickness = 1;
	int minX = -1000;
	int maxX = 1000;
	int minY = -1000;
	int maxY = 1000;
};

Config config;

struct Point {
	int x = 0;
	int y = 0;
	int z = 0;
	Point() {};
	Point(int x, int y, int z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void SetPoint(int x, int y, int z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct Plane {
	//Technically these are all floats but we do not want to generate floating numbers!
	Point p1;
	Point p2;
	Point p3;
	std::string texture = "NULL";  //texture may never be empty!
	float x_off = 0.0;
	float y_off = 0.0;
	float rot_angle = 0.0;
	float x_scale = 0.5;
	float y_scale = 0.5;  
};

struct Brush {
	int dummy=1;
	std::vector<Plane> planes;
};

struct theMap {
	std::string message = "Created by sago-map-generator-one";
	std::vector<Brush> brushes;
};

static void writePlane(std::ostream* output, const Plane& p) {
	*output << "( " << p.p1.x << " " << p.p1.y << " "  << p.p1.z << " ) ";
	*output << "( " << p.p2.x << " "  << p.p2.y << " "  << p.p2.z << " ) ";
	*output << "( " << p.p3.x << " "  << p.p3.y << " "  << p.p3.z << " ) ";
	*output << p.texture << " " << p.x_off << " "  << p.y_off << " "  << p.rot_angle << " "  << p.x_scale << " "  << p.y_scale << " 0 0 0\n";
}

static Brush createBrush(int x1, int y1, int z1, int x2, int y2, int z2) {
	Brush ret;
	Plane p[6];
	p[0].p1.SetPoint(x1, 0, 0);
	p[0].p2.SetPoint(x1, 16, 0);
	p[0].p3.SetPoint(x1, 0, 16);
	
	p[1].p1.SetPoint(x2, 0, 0);
	p[1].p2.SetPoint(x2, 0, 16);
	p[1].p3.SetPoint(x2, 16, 0);
	
	p[2].p1.SetPoint(0, y1, 0);
	p[2].p2.SetPoint(0, y1, 16);
	p[2].p3.SetPoint(16, y1, 0);
	
	p[3].p1.SetPoint(0, y2, 0);
	p[3].p2.SetPoint(16, y2, 0);
	p[3].p3.SetPoint(0, y2, 16);
	
	p[4].p1.SetPoint(0, 0, z1);
	p[4].p2.SetPoint(0, 16, z1);
	p[4].p3.SetPoint(16, 0, z1);
	
	p[5].p1.SetPoint(0, 0, z2);
	p[5].p2.SetPoint(16, 0, z2);
	p[5].p3.SetPoint(0, 16, z2);
	ret.planes.assign(p,p+6);
	return ret;
}

static void brushAddTexture(Brush& b, const std::string& texture) {
	if (texture.empty()) {
		abort();
	}
	for (Plane& p : b.planes ) {
		p.texture = texture;
	}
}

static void writeBrush(std::ostream* output, int number, const Brush& b) {
	*output << "{\n";
	for (const Plane& p : b.planes) {
		writePlane(output,p);
	}
	*output << "}\n";
}

static void writeMap(const theMap& m) {
	std::ostream* output = &std::cout;
	*output << "// entity 0\n";
	*output << "{\n";
	*output << "\"classname\" \"worldspawn\"\n" <<
		"\"message\" \""<< m.message<< "\"\n";
	for (const Brush& b : m.brushes) {
		writeBrush(output,0,b);
	}
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
	Brush b = createBrush(10, 20, 30,100,80,10);
	brushAddTexture(b, "e7/e7bricks01");
	m.brushes.push_back(b);
	writeMap(m);
	return 0;
}
