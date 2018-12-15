#pragma once
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include "Object.h"
#define _USE_MATH_DEFINES 
#include <cmath>
class Spheres : public Object
{
public:
	Spheres();
	virtual ~Spheres();
	float        _radius;

	void buildSphere(float radius, int nbStacks, int nbSlices);
	void draw();

	unsigned int getSlices() { return _slices; }
	unsigned int getStacks() { return _stacks; }


protected:
	int _stacks;
	int _slices;

};

