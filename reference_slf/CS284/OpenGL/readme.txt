This program loads a .obj file and displays it with a specified texture.
For simplicity I ignore the .mtl file and just use the texture specified on the command line.

Image loading is done by the public domain stb_image.c library, which is renamed stb_image.cpp because the makefile liked it better that way.  This library is super convenient but does not load all image files; see the documentation at the top of stb_image.cpp for details.

I've only tested on windows (with msvc 2008) and mac.
It 'should' work on *nix, although it looks like the makefile currently adds unneeded libraries in the unix-specific section, so you may want to remove the lGLEW and imagemagick stuff if that causes you trouble.

Here's a brief walkthrough of the source files:

image.h is a struct that basically provides a convenient interface to the data the stb_image program reads and writes.  It can load textures and save the current screen out as a .bmp file.

mesh.h is a simple triangle mesh class with code to read in obj files.

glhelpers.h includes some useful functions for using opengl.
It also includes the glu and glut header files.

debug.h can toggle whether statements printed with DEBUGOUT will show up or not.

main.cpp contains the control flow of the actual program

algebra3.h is a vector & matrix library that cs184 tends to use

Feel free to use this code however you wish.

 - Jimmy