/*
===========================================================================
sago-map-generator-one
Copyright (C) 2017 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contact persons can be found at
https://github.com/sago007/sago-map-generator-one
===========================================================================
*/

#include <iostream>
#include <boost/program_options.hpp>
#include <vector>
#include <random>
#include <fstream>

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
	int minX = -100;
	int maxX = 100;
	int minY = -100;
	int maxY = 100;
	std::string texture = "e7/e7bricks01";
};

static Config config;

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


/**
 * Creates a brush from 9 Points. The first 4 points are the top surface clockwise. The remaining 4 are the bottom surface counter-clockwise(from the top).
 * point 5 is below point 4
 * @param p1
 * @param p2
 * @param p3
 * @param p4
 * @param p5
 * @param p6
 * @param p7
 * @param p8
 * @return 
 */
static Brush createBrush(const Point& p1, const Point& p2, const Point& p3, const Point& p4, const Point& p5, const Point& p6, const Point& p7, const Point& p8) {
	Brush ret;
	Plane p[6];
	p[0].p1 = p1;
	p[0].p2 = p2;
	p[0].p3 = p3;
	
	p[1].p1 = p4;
	p[1].p2 = p3;
	p[1].p3 = p6;
	
	p[2].p1 = p3;
	p[2].p2 = p2;
	p[2].p3 = p7;
	
	p[3].p1 = p2;
	p[3].p2 = p1;
	p[3].p3 = p8;
	
	p[4].p1 = p1;
	p[4].p2 = p4;
	p[4].p3 = p5;
	
	p[5].p1 = p5;
	p[5].p2 = p6;
	p[5].p3 = p7;
	ret.planes.assign(p,p+6);
	return ret;	
}

/*
 * 
 * The coordiantes expects this system:
        ^ z+
        |
        |
        |
        |
        |------------> y+
       /
      /
     /
    /
   <
 x+
		*/

static Brush createBrush(int x1, int y1, int z1, int x2, int y2, int z2) {
	return createBrush({x2,y1,z1},{x1,y1,z1},{x1,y2,z1},{x2,y2,z1},{x2,y2,z2},{x1,y2,z2},{x1,y1,z2},{x2,y1,z2});
}

static void brushAddTexture(Brush& b, const std::string& texture) {
	if (texture.empty()) {
		abort();
	}
	for (Plane& p : b.planes ) {
		p.texture = texture;
	}
}

static void writeBrush(std::ostream* output, const Brush& b) {
	*output << "{\n";
	for (const Plane& p : b.planes) {
		writePlane(output,p);
	}
	*output << "}\n";
}

static void writeMap(std::ostream* output, const theMap& m) {
	*output << "// entity 0\n";
	*output << "{\n";
	*output << "\"classname\" \"worldspawn\"\n" <<
		"\"message\" \""<< m.message<< "\"\n";
	for (const Brush& b : m.brushes) {
		writeBrush(output,b);
	}
	*output << "}\n";
}

struct Platform {
	int x;
	int y;
	int w;
	int h;
};

struct Layer {
	std::vector<Platform> platforms;
};

struct LayerMap {
	std::vector<Layer> layers;
};

static bool intersect(const Platform& p1, const Platform& p2) {
	bool intersecting = true;
	if (p1.x > p2.x+p2.w) {
		intersecting = false;
	}
	if (p1.x+p1.w < p2.x) {
		intersecting = false;
	}
	if (p1.y > p2.y+p2.h) {
		intersecting = false;
	}
	if (p1.y+p1.h < p2.y) {
		intersecting = false;
	}
	return intersecting;
}

static bool intersecting(const Platform& p1, const std::vector<Platform> platforms) {
	for (const auto& p2 : platforms) {
		if (intersect(p1, p2)) {
			return true;
		}
	}
	return false;
}

static Layer layerCreate (const Config& c) {
	Layer l;
	for (int i=0; i < c.maxPerLayer; ++i) {
		Platform p;
		p.x = (rand()%(c.maxX-c.minX))+c.minX;
		p.y = (rand()%(c.maxY-c.minY))+c.minY;
		p.w = (rand()%(c.maxSize-c.minSize))+c.minSize;
		p.h = (rand()%(c.maxSize-c.minSize))+c.minSize;
		if (!intersecting(p, l.platforms)) {
			l.platforms.push_back(p);
		}
	}
	return l;
}

static LayerMap layerMapCreate (const Config& c) {
	LayerMap m;
	for (int i=0; i < c.numberOfLayers; ++i) {
		Layer l = layerCreate(c);
		m.layers.push_back(l);
	}
	return m;
}

static theMap LayerMapToMap(const Config& c, const LayerMap& m) {
	theMap ret;
	for (size_t i = 0; i < m.layers.size(); ++i) {
		for (const Platform& p : m.layers.at(i).platforms) {
			const int& u = c.unitSize;
			Brush b = createBrush(u*p.x, u*p.y, u*i*(c.layerDistance),u*(p.x+p.w),u*(p.y+p.h),u*(i*(c.layerDistance)-c.platformThickness));
			brushAddTexture(b, c.texture);
			ret.brushes.push_back(b);
		}
	}
	return ret;
}

static void AddHollowBox(const Config& c, theMap& m) {
	const int& u = c.unitSize;
	Brush floor = createBrush(u*c.minX, u*c.minY, -u*c.layerDistance, u*(c.maxX+c.maxSize), u*(c.maxY+c.maxSize), u*(-c.layerDistance-c.platformThickness));
	brushAddTexture(floor, c.texture);
	m.brushes.push_back(floor);
}

static void setIntIfSet(const boost::program_options::variables_map& vm, const char* name, int& value) {
	if (vm.count(name)) {
		value = vm[name].as<int>();
	}
}

int main(int argc, const char* argv[]) {
	boost::program_options::options_description desc("Options");
	desc.add_options()
	("version", "Print version information and quit")
	("help,h", "Print basic usage information to stdout and quit")
	("output,o", boost::program_options::value<std::string>(), "Output file")
	("unitSize", boost::program_options::value<int>(), "Unit size. All other coordiantes are multiplied by this. Default: 16")
	("minSize", boost::program_options::value<int>(), "Minimum platform size. Default: 20")
	("maxSize", boost::program_options::value<int>(), "Max platform size. Default: 50")
	("maxPerLayer", boost::program_options::value<int>(), "Max number of platforms per layer. Default: 10")
	("numberOfLayers", boost::program_options::value<int>(), "Number of layers. Default: 5")
	("platformThickness", boost::program_options::value<int>(), "How thick the platforms are. Default: 1")
	("minX", boost::program_options::value<int>(), "Min X coordiante. Default: -100")
	("maxX", boost::program_options::value<int>(), "Max X coordiante. Default: 100")
	("minY", boost::program_options::value<int>(), "Min Y coordiante. Default: -100")
	("maxY", boost::program_options::value<int>(), "Min Y coordiante. Default: 100")
	("texture", boost::program_options::value<std::string>(), "The texture to use. Default: e7/e7bricks01")
	("seed", boost::program_options::value<int>(), "Set the seed for the random generator. Must be an integer. This is implementation defined.")
	;
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
	setIntIfSet(vm, "unitSize", config.unitSize);
	setIntIfSet(vm, "minSize", config.minSize);
	setIntIfSet(vm, "maxSize", config.maxSize);
	setIntIfSet(vm, "maxPerLayer", config.maxPerLayer);
	setIntIfSet(vm, "numberOfLayers", config.numberOfLayers);
	setIntIfSet(vm, "platformThickness", config.platformThickness);
	setIntIfSet(vm, "minX", config.minX);
	setIntIfSet(vm, "maxX", config.maxX);
	setIntIfSet(vm, "minY", config.minY);
	setIntIfSet(vm, "maxY", config.maxY);
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
	if (vm.count("texture")) {
		config.texture = vm["texture"].as<std::string>();
	}
	if (vm.count("seed")) {
		srand(vm["seed"].as<int>());
	}
	theMap m;
	
	Brush b = createBrush(10, 20, 30,100,80,10);
	brushAddTexture(b, "e7/e7bricks01");
	m.brushes.push_back(b);
	LayerMap m2 = layerMapCreate(config);
	m = LayerMapToMap(config, m2);
	AddHollowBox(config, m);
	if (output_filename.length() > 0) {
		std::ofstream f;
		f.open(output_filename);
		writeMap(&f, m);
	}
	else {
		writeMap(&std::cout, m);
	}
	return 0;
}
