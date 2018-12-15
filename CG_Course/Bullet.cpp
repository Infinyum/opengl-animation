//#include "stdafx.h"
#include "Bullet.h"
#include "Cylinder.h"


/**
* Bullet's Constructor: Initialise the coordinates of the bullet and its size
* @param x: a float representing the x-position of the bullet
* @param y: a float representing the y-position of the bullet
* @param z: a float representing the z-position of the bullet
* @return : NONE
*/
Bullet::Bullet(float x,float y, float z) {

	_bullet = new Cylinder();
	_bullet->buildCylinder(0.5, 0.001, 2, 100, 1, 1);
	_bullet->InitVBO();

	_Xposition = x;
	_Yposition = y;
	_Zposition = z;

	_Echo = new Cylinder();
	_Echo->buildCylinder(_bullet->_baseRadius, _bullet->_baseRadius, 0.1, 100, 1, 1);
	_Echo->InitVBO();

	_Yecho[0]=_Yposition+1;

	for (int i = 1; i < 100; i++) {

		_Yecho[i] = _Yecho[i - 1] - 1.5;
	}
}

/**
* Bullet's Destructor: Destroy the bullet
* @param : NONE
* @return : NONE
*/
Bullet::~Bullet(){
	
	delete _Echo;
	delete _bullet;

}


/**
* Bullet's Update Function: Update the bullet posititon (the y-position)
* @param : NONE
* @return : NONE
*/
void Bullet::update() {
	_Yposition -= 0.01;
}
