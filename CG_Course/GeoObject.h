#pragma once

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#define _USE_MATH_DEFINES // for C++
#include <cmath>


class GeoObject
{
public:
	GeoObject();
	virtual ~GeoObject();
	void buildSphere(float radius, int nbStacks, int nbSlices);
	void InitVBO(void);

	unsigned int getSize() { return _size; }

	unsigned int _vao;

protected:
	float * _vertices;
	float * _normals;
	float * _texcoors;
	int _stacks;
	int _slices;
	unsigned int _size; //Number of Vertices / Normals / Texture Coordonates
};

