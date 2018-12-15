//#include "stdafx.h"
#include "Cylinder.h"


/**
* Cylinder's Constructor: Initialise the radiuses and the height of the cylinder
* @param : NONE
* @return : NONE
*/
Cylinder::Cylinder()
{

	_topRadius = 0;
	_baseRadius = 0;
	_height = 0;
}


/**
* Cylinder's Destructor: Destroy the Cylinder
* @param : NONE
* @return : NONE
*/
Cylinder::~Cylinder()
{

}

/**
* Cylinder's build Function: Initialise all the properties of the Cylinder
* @param baseRadius: a float representing the base radius of the cylinder
* @param topRadius: a float representing the top radius of the cylinder
* @param height: a float representing the height of the cylinder
* @param nbSlices: an integer representing the number of slices used to build the cylinder
* @param xcoeff: a float representing a x distortion of the cylinder (circles become ellipses)
* @param ycoeff: a float representing a y distortion of the cylinder (circles become ellipses)
* @return : NONE
*/
void Cylinder::buildCylinder(float baseRadius, float topRadius, float height, int nbSlices, float xcoeff, float ycoeff)
{
	// array is 71 floats (24 * 3 = 71).

	float deltaPhi;
	
	// vertex, normal and texture coordinates
	_slices = nbSlices;
	_size = (nbSlices + 1) * 2;

	_vertices = new float[_size * 3];
	_normals = new float[_size * 3];
	_texcoors = new float[_size * 2];

	_baseRadius = baseRadius;
	_topRadius = topRadius;
	_height = height;

	// angular distance between two lines
	deltaPhi = 2 * M_PI / nbSlices;

	float *vertices = _vertices;
	float *normals = _normals;
	float *texcoors = _texcoors;

	// triangle strips made of vertical stacks
	// tesselating the triangle strip into nbSlices
	// note <= instead of < for making sure that the last angle is used
	for (int j = 0; j <= nbSlices; j++) {
		// polar angle
		float phi = j * deltaPhi;

		// polar vector in a vertical plane
		float xPolar = cos(phi) / xcoeff;
		float yPolar = sin(phi) / ycoeff;

		float normZ = (height != 0 ? (baseRadius - topRadius) / height : 0);

		float normalNorm = sqrt(1 + normZ * normZ);

		// vertex #1 (theta , phi)
		*normals = xPolar / normalNorm;
		*vertices = xPolar * baseRadius;

		normals++;
		vertices++;

		*normals = yPolar / normalNorm;
		*vertices = yPolar * baseRadius;

		normals++;
		vertices++;

		*normals = normZ / normalNorm;
		*vertices = 0;

		normals++;
		vertices++;

		*texcoors = (float)j / (float)nbSlices;
		texcoors++;
		*texcoors = 0.f;
		texcoors++;

		// vertex #2 (theta , phiPrime)
		*normals = xPolar / normalNorm;
		*vertices = xPolar * topRadius;

		normals++;
		vertices++;

		*normals = yPolar / normalNorm;
		*vertices = yPolar * topRadius;

		normals++;
		vertices++;

		*normals = (baseRadius - topRadius) / height;
		*vertices = height;

		normals++;
		vertices++;

		*texcoors = (float)j / (float)nbSlices;
		texcoors++;
		*texcoors = 1.f;
		texcoors++;
	}
}

/**
* Cylinder's draw function: draw the cylinder according to the last bound shader
* @param : NONE
* @return : NONE
*/
void Cylinder::draw() {
	// draw triangle strips from the currently bound VAO
	// with current in-use shader
	int nbStackTriangles = (_slices + 1) * 2;
	// draw triangles from the currently bound VAO with current in-use shader
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0,
		nbStackTriangles);
}
