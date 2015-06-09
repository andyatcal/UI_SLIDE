#ifndef IMAGE_H_
#define IMAGE_H_

#include <sstream>
#include <fstream>
#include <iomanip>

#include "debug.h"
#include "algebra3.h"
#include "glhelpers.h"

// declare stb's basic image read and write functions
extern "C" {
	extern unsigned char *stbi_load            (char const *filename,     int *x, int *y, int *comp, int req_comp);
	extern int      stbi_write_bmp       (char const *filename,     int x, int y, int comp, void *data);
	extern void		stbi_image_free(void *retval_from_stbi_load);
}

struct Image {
	unsigned char* img;
	int width, height;
	int comp; // number of color components in image

	Image() {}

	Image(int w, int h, int comp = 3) : width(w), height(h), comp(comp) {
		img = new unsigned char[3*w*h];
		for (int i = 0; i < 3*w*h; ++i) {
			img[i] = 0;
		}
	}

	Image(const char *filename, int comp = 4) : comp(comp) {
		int outcomp;
		img = stbi_load(filename, &width, &height, &outcomp, comp);
	}

	~Image() {
		stbi_image_free(img);
	}
private: // for now I ban the copy constructor and the assignment operator
		// if you do want these, don't just use the defaults or you'll end up
		// having the destructor's free called multiple times on the same img ptr
	Image(const Image &i);
	Image &operator=(const Image &i);
public:

	void writeBMP(const char* filename) {
		stbi_write_bmp(filename, width, height, 3, img);
	}

	// save screen using filename prefix_[num].bmp
	static void saveScreen(string prefix, int num) {
       int w = glutGet(GLUT_WINDOW_WIDTH);
       int h = glutGet(GLUT_WINDOW_HEIGHT);

		std::stringstream filename;
		filename << prefix << "_" << std::setfill('0') << setw(4) << num << ".bmp";
        
        Image img(w, h);

        glPixelStorei(GL_PACK_ALIGNMENT,1);
        glReadBuffer(GL_BACK);
        glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,img.img);

		img.flipV();

		img.writeBMP(filename.str().c_str());
	}

	inline void swapPix(unsigned char *a, unsigned char *b, unsigned char *swap) {
		for (int i = 0; i < comp; ++i) {
			swap[i] = a[i];
			a[i] = b[i];
			b[i] = swap[i];
		}
	}

	// in-place vertical flip of image
	// useful because the image library thinks 0,0 is the top left
	// while opengl thinks 0,0 is the bottom left
	void flipV() {
		int hh = height / 2;
		unsigned char swap[4];
		assert(comp <= 4);
		for (int y = 0; y < hh; ++y) {
			int yopp = height-1-y;
			unsigned char *toprow = getPixel(0,y);
			unsigned char *botrow = getPixel(0,yopp);
			for (int x = 0; x < width; ++x, toprow+=comp, botrow+=comp) {
				swapPix(toprow,botrow,swap);
			}
		}
	}

	static bool loadTexture(string file, GLuint &texture) {
		Image img(file.c_str(), 4);
		if (!img.img)
			return false;
		img.flipV();

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, img.width, img.height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)img.img);
		return true;
	}
	
	inline unsigned char* getPixel(int x, int y) {
		return img + (comp*(width*y+x));
	}

};


#endif