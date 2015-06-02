#ifndef MESH_H_
#define MESH_H_

#include "algebra3.h"
#include "glhelpers.h"

#include <vector>

using namespace std;

struct Tri
{
	int ind[3];
	int uvs[3];
	bool hasUVs;

	Tri(int i, int j, int k) : hasUVs(false) {
		ind[0] = i;
		ind[1] = j;
		ind[2] = k;
	}

	inline void setUVs(int i, int j, int k) {
		hasUVs = true;
		uvs[0] = i;
		uvs[1] = j;
		uvs[2] = k;
	}
};

struct Vert
{
	vec3 p;
	vec3 n;
    int temp;

	Vert(vec3 v) : p(v), n(0,0,1) {}
};


struct TriangleMesh
{
	vector<Tri> tris;
	vector<Vert> verts;
	vector<vec2> uvs;

	void centerAndScaleToOne(){
		if (verts.empty())
			return;

		vec3 maxp = verts[0].p, minp = verts[0].p;
		for (vector<Vert>::iterator it = verts.begin(); it != verts.end(); ++it) {
			maxp = max(it->p, maxp); // max and min def'd in algebra3.h take the max or min componentwise from each vector
			minp = min(it->p, minp);
		}
		vec3 center = (maxp+minp)*.5;
		vec3 size = maxp-minp;
		double maxSizeInv = MAX(size[0],MAX(size[1],size[2]));
		if (maxSizeInv == 0) // mesh is just one point
			return;
		maxSizeInv = 1.0/maxSizeInv;
		for (vector<Vert>::iterator it = verts.begin(); it != verts.end(); ++it) {
			it->p = (it->p-center)*maxSizeInv;
		}
	}

    vec3 getNormal(int tri)
    {
        vec3 e1 = verts[tris[tri].ind[0]].p - verts[tris[tri].ind[1]].p;
        vec3 e2 = verts[tris[tri].ind[2]].p - verts[tris[tri].ind[1]].p;
        vec3 n = e1 ^ e2;
        return n.normalize();
    }

	void draw()
	{
		glBegin(GL_TRIANGLES);
		for (unsigned int i = 0; i < tris.size(); ++i)
		{
			for (int ind = 0; ind < 3; ind++)
			{
				int v = tris[i].ind[ind];
				if (tris[i].hasUVs)
					glTexCoord2dv(uvs[tris[i].uvs[ind]].n);
				glNormal3dv(verts[v].n.n);
				glVertex3dv(verts[v].p.n);
			}
		}
		glEnd();
	}

    void drawFlat()
    {
        glBegin(GL_TRIANGLES);
		for (unsigned int i = 0; i < tris.size(); ++i)
		{
            vec3 n = getNormal(i);
            glNormal3dv(n.n);
			for (int ind = 0; ind < 3; ind++)
			{
				int v = tris[i].ind[ind];
				if (tris[i].hasUVs)
					glTexCoord2dv(uvs[tris[i].uvs[ind]].n);
				glVertex3dv(verts[v].p.n);
			}
		}
		glEnd();
    }

	// just naively sums all nearby triangle normals; no area weighting
    void computeVertexNormals()
    {
        for (unsigned int i = 0; i < verts.size(); ++i) {
            verts[i].n = vec3(0,0,0);
        }

        for (unsigned int tri = 0; tri < tris.size(); ++tri) {
            vec3 n = getNormal(tri);
            for (int i = 0; i < 3; ++i) {
                verts[tris[tri].ind[i]].n += n;
            }
        }

        for (unsigned int i = 0; i < verts.size(); ++i) {
			verts[i].n.normalize();
        }
    }

	int getNValues(stringstream &ss, vector<int> &values, char delim = '/') {
		values.clear();
		string sblock;
		if (ss >> sblock) {
			stringstream block(sblock);
			string s;
			int value;
			while (getline(block, s, delim)) {
				stringstream valuestream(s);
				if (valuestream >> value)
					values.push_back(value);
			}
		}
		return (int)values.size();
	}


	bool loadFile(string file)
	{
		tris.clear(); verts.clear();
		ifstream f(file.c_str());
		if (!f) {
			cerr << "Couldn't open file: " << file << endl;
			return false;
		}
		string line;
		while (getline(f,line)) {
			if (line.empty())
				continue;
			stringstream ss(line);
			string op;
			ss >> op;
			if (op[0] == '#')
				continue;
			if (op == "v") {
				vec3 v(0,0,0);
				ss >> v;
				verts.push_back(Vert(v));
			}
			if (op == "vt") {
				vec2 v;
				ss >> v;
				uvs.push_back(v);
			}
			if (op == "f")
			{
				vector<int> first, second, third;
				
				if (!getNValues(ss, first))
					continue;
				if (!getNValues(ss, second))
					continue;
				while (getNValues(ss, third)) {
					//DEBUGOUT("adding tri " << first[0] << " " << second[0] << " " << third[0]);
					tris.push_back(Tri(first[0]-1,second[0]-1,third[0]-1));
					if (first.size() > 1)
						tris.back().setUVs(first[1]-1,second[1]-1,third[1]-1);
					// if (first.size() > 2) read per-face vertex normals ... if you want to.
					second = third;
				}
			}
			/*
			if (op == "vn")
			{
				vec3 n;
				ss >> n;
				verts[normals++].n = n;
			}*/
		}
		return true;
	}
};


#endif

