#pragma once
#include "Cylinder.h"
#include "Spheres.h"
#include "Boxes.h"

class Decoration {
public:
	enum primitive {
		BOXE = 0, SPHERE = 1, CYLINDER = 2
	};
	
	primitive primitiveAspect;

};

class BoxeDecoration : public Decoration {
public:
	primitive primitiveAspect = BOXE;
	Boxe *Deco;

	BoxeDecoration(double Xlength, double Ylength, double Zlength);
};

class SphericalDecoration : public Decoration {
public:
	primitive primitiveAspect = SPHERE;
	Spheres *Deco;

	SphericalDecoration(float radius);

}; 
class CylinderDecoration : public Decoration {
public:
	primitive primitiveAspect = CYLINDER;
	Cylinder *Deco;

	CylinderDecoration(float baseRadius, float topRadius, float height);
};
