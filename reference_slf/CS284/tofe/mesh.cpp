#include <vector>
#include <iostream>
#include <fstream>
#include <strstream>
#include <string>
#include <map>
using namespace std;

struct Point {
	float x, y, z;
};

struct Edge {
	size_t from, to;
};

struct Face {
	vector<int> eds;
};

vector<Point> points;
vector<Edge> edges;
vector<Face> faces;

int
FindEdge(size_t from, size_t to)
{
	for(size_t ii = 0; ii < edges.size(); ++ii) {
		if(edges[ii].from == from && edges[ii].to == to) return ii + 1;
		if(edges[ii].from == to && edges[ii].to == from) return -(ii + 1);
	}
	return 0;
}

void
AddEdge(size_t from, size_t to)
{
	Edge e;
	e.from = from;
	e.to = to;
	edges.push_back(e);
}

void
PrintPoint(const Point &p)
{
	cout << "(" << p.x << ", " << p.y << ", " << p.z << ")" << endl;
}

void
ReadFile(ifstream &in)
{
	char cline[1024];
	while(true) {
		in.getline(cline, 1024);
		if(!in) { cout << "Error reading file." << endl; exit(-1);}
		if(cline[0] == '=') break;
		strstream s;
		s << cline;
		Point newpt;
		s >> newpt.x;
		s >> newpt.y;
		s >> newpt.z;
		if(!s) { cout << "Error reading file." << endl; exit(-1);}
		points.push_back(newpt);
	}
	vector<int> pts;
	while(true) {
		in.getline(cline, 1024);
		if(!in) { cout << "Error reading file." << endl; exit(-1);}
		if(cline[0] == '=') break;
		strstream s;
		s << cline;
		int pn;
		pts.clear();
		while(true) {
			s >> pn;
			if(!s) break;
			pts.push_back(pn);
		}
		Face newface;
		for(size_t ii = 0; ii < pts.size(); ++ii) {
			int p0 = pts[ii];
			int p1 = pts[(ii+1)%pts.size()];
			if(!FindEdge(p0, p1)) AddEdge(p0, p1);
			newface.eds.push_back(FindEdge(p0, p1));
		}
		faces.push_back(newface);
	}
}

void
WriteFile(ofstream &out)
{
	out << "quantity smc energy method sq_mean_curvature global" << endl << endl;

	out << "vertices" << endl;
	for(size_t ii = 0; ii < points.size(); ++ii) {
		out << ii + 1 << "\t" << points[ii].x << " " << points[ii].y << " " << points[ii].z << endl;
	}

	out << endl;
	out << "edges" << endl;
	for(ii = 0; ii < edges.size(); ++ii) {
		out << ii + 1 << "\t" << edges[ii].from+1 << " " << edges[ii].to+1 << endl;
	}

	out << endl;
	out << "faces" << endl;
	for(ii = 0; ii < faces.size(); ++ii) {
		out << ii + 1 << "\t";
		for(size_t jj = 0; jj < faces[ii].eds.size(); ++jj) {
			out << faces[ii].eds[jj] << " ";
		}
		out << endl;
	}

	out << endl;
	out << "bodies" << endl << "1\t";
	for(ii = 0; ii < faces.size(); ++ii) out << " " << ii + 1;
	out << endl;
	out << "read" << endl;
	out << "set facet tension 0" << endl;
}

int
main(int argc, char ** argv)
{
	if(argc != 2) {
		cout << "use: tofe <infile>" << endl;
		return -1;
	}

	string inFile(argv[1]);
	string outFile(argv[1]);
	if(outFile.rfind(".") != string::npos) {
		outFile.erase(outFile.rfind("."), outFile.length());
	}
	outFile += ".fe";


	ifstream in(inFile.c_str());
	if(in.fail()) {
		cout << "Can't open input file " << inFile << "." << endl;
		return -1;
	}
	ReadFile(in);

	ofstream out(outFile.c_str());
	if(out.fail()) {
		cout << "Can't open output file " << outFile << "." << endl;
		return -1;
	}
	WriteFile(out);

	cout << inFile << " to " << outFile << endl;
	cout << "Point count:" << points.size() << endl;
	cout << "Edge count: " << edges.size() << endl;
	cout << "Face count: " << faces.size() << endl;

	return 0;
}
