//#include "stdafx.h"
#include "Decorations.h"

/**
* Boxe decoration's Constructor: Initialise the property of the box used as a decoration
* @param : NONE
* @return : NONE
*/
BoxeDecoration::BoxeDecoration(double Xlength, double Ylength, double Zlength){
	Deco = new Boxe();
	Deco->buildBoxe(Xlength, Ylength, Zlength);
	Deco->InitVBO();
}

/**
* Spherical decoration's Constructor: Initialise the property of the sphere used as a decoration
* @param : NONE
* @return : NONE
*/
SphericalDecoration::SphericalDecoration(float radius) {
	Deco = new Spheres();
	Deco->buildSphere(radius, 100, 100);
	Deco->InitVBO();
}

/**
* Cylinder decoration's Constructor: Initialise the property of the cylinder used as a decoration
* @param : NONE
* @return : NONE
*/
CylinderDecoration::CylinderDecoration(float baseRadius, float topRadius, float height) {
	Deco = new Cylinder();
	Deco->buildCylinder(baseRadius, topRadius, height, 100, 1, 1);
	Deco->InitVBO();
}