#pragma once

#include "Cylinder.h"

class Bullet
{
public:
	Bullet(float x, float y, float z);
	~Bullet();
	void update();

	Cylinder *_bullet;
	Cylinder *_Echo;

	//Translation positions
	float _Xposition,_Yposition,_Zposition;
	float _Yecho[100];
};