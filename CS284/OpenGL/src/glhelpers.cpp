#include "glhelpers.h"

// for 2D I prefer an orthographic projection with dimensions equal to the input w,h variables
void setProjection2D(int w, int h) {
    int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    glOrtho(0,w,0,h, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// for 3D I prefer to just use the input w,h to set the aspect ratio
void setProjection3D(int w, int h, double fov, double zNear, double zFar) {
    int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(fov, ((double)w / MAX(h, 1)), zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// A simple helper function to load a mat4 into opengl
void applyMat4(const mat4 &mat) {
    double glmat[16];
    int k = 0;
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            glmat[k++] = mat[i][j];
        }
    }
    glMultMatrixd(glmat);
}
