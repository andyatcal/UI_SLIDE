#include <iostream>

#include "debug.h"
#include "algebra3.h"
#include "image.h"
#include "mesh.h"

struct Viewport {
	int w, h;
	mat4 orientation;
	vec3 translate;
	Viewport() : w(640), h(480), orientation(identity3D()), translate(0,0,-5) {}
};

Viewport view;
TriangleMesh mesh;
GLuint texture;

void display() {
	// setup gl for rendering
	setProjection3D(view.w, view.h, 45);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// draw mesh
	applyMat4(translation3D(view.translate)*view.orientation);
	mesh.drawFlat();

	// swap buffers to put what we just drew on to the screen
	glutSwapBuffers();
}

// escape key quits, 's' takes a screenshot
void keypresses(unsigned char key, int x, int y) {
	static int screenshots = 0;
	switch (key) {
        case 's': // saves a picture of the current screen
			Image::saveScreen("screenshot", screenshots++);
            break;
		case 27:			// Escape key
			exit(0);
			break;
	}
}

// mouse handling functions -- lets you rotate the model around
int buttonDown = -1;
vec2 mousePos;
void mouseclicks(int button, int state, int x, int y) {
	if (state==GLUT_DOWN) {
		buttonDown = button;
	} else {
		buttonDown = -1;
	}
}
void mousemove_passive(int x, int y) {	
	mousePos = vec2((double)x / glutGet(GLUT_WINDOW_WIDTH),(double)y / glutGet(GLUT_WINDOW_HEIGHT));
}
void mousemove(int x, int y) {	
	vec2 newMouse = vec2((double)x / glutGet(GLUT_WINDOW_WIDTH),(double)y / glutGet(GLUT_WINDOW_HEIGHT));
	vec2 diff = (newMouse - mousePos);
	double len = diff.length();
	if (len > .001) {
		if (buttonDown == GLUT_LEFT_BUTTON) {	
			vec3 axis = vec3(-diff[1]/len, -diff[0]/len, 0);
			view.orientation = rotation3D(axis, -180 * len)*view.orientation;
		} else if (buttonDown == GLUT_RIGHT_BUTTON) {
			view.translate += vec3(diff[0],-diff[1],0)*50;
		} else if (buttonDown == GLUT_MIDDLE_BUTTON) {
			view.translate[2] += diff[1]*50;
		}
	}
	mousePos = newMouse;
	glutPostRedisplay();
}

void idle() {
	glutPostRedisplay();
}

int main( int argc, char **argv )
{
	DEBUGOUT("starting with debug prints on ...");

	//Initialize OpenGL
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);

	//Create OpenGL Window
	glutInitWindowSize(view.w,view.h); // view.w and view.h currently set in default constructor of Viewport
	glutInitWindowPosition(0,0);
	glutCreateWindow("window title goes here");

	//Register event handlers with OpenGL.
	glutDisplayFunc(display);
	glutKeyboardFunc(keypresses);
	glutPassiveMotionFunc(mousemove_passive);
	glutMotionFunc(mousemove);
	glutMouseFunc(mouseclicks);
	glutIdleFunc(idle);

	// turn on LIGHT0 (one of the 8 hardware lights provided by gl) so we get some shading
	{
		float ambient[3] = { .1f, .1f, .1f };
		float diffuse[3] = { .6f, .5f, .5f };
		float pos[4] = { 2, 3, -10, 0 };
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, pos);
		glEnable(GL_LIGHT0);
	}
	glEnable(GL_LIGHTING);

	// sets what glColor calls will affect
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// enables depth testing -- so the z buffer works
	glEnable(GL_DEPTH_TEST);

	// draw nicer lines
	glEnable(GL_LINE_SMOOTH);

	// draw both sides of our polygons
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// set the 'default' color that shows when we clear the screen
	glClearColor(0.0f, 0.0f, .5f, 0.0f); // it's blue

	string file = "cyl_textured.obj";
	if (argc >= 2)
		file = argv[1];
	DEBUGOUT("Loading mesh: " << file);
	
	if (!mesh.loadFile(file)) {
		cerr << "Failed to load mesh: " << file << endl;
		return 1;
	}
	mesh.computeVertexNormals();
	mesh.centerAndScaleToOne();

	string texturefile = "cylinder_tex.bmp";
	if (argc >= 3)
		texturefile = argv[2];
	DEBUGOUT("Loading texture: " << texturefile);

	if (!Image::loadTexture(texturefile, texture)) {
		cerr << "Failed to load texture: " << texturefile << endl;
		return 1;
	}

	//And Go!
	glutMainLoop();
}
