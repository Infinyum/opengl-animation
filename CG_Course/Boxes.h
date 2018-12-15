#pragma once

#include "Object.h"

class Boxe : public Object
{
public:
	Boxe();
	~Boxe();

	void buildBoxe(float Xlength, float Ylenght, float Zlength);
	void draw();

	unsigned int getSize() { return _size; }

	float        _Xlength;
	float        _Ylength;
	float        _Zlength;

};