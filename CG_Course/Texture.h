#pragma once

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <string>

//fonction based on http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/ tutorial
GLuint loadBMP_custom(const char * imagepath);
