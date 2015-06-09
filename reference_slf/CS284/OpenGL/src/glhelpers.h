#ifndef GLHELPERS_H_
#define GLHELPERS_H_

#include "algebra3.h"

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

// set up gl projection matrices for 2D or 3D drawing
void setProjection2D(int w, int h);
void setProjection3D(int w, int h, double fov, double zNear = 1.0, double zFar = 64.0);

// multiply's the current gl matrix by a mat4 matrix (algebra.h's matrix type)
void applyMat4(const mat4 &mat);

#endif

