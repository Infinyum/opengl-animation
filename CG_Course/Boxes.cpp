//#include "stdafx.h"
#include "Boxes.h"

/**
* Boxe's Constructor: Initialise the lenght of the box
* @param : NONE
* @return : NONE
*/
Boxe::Boxe() {

	_Xlength=0;
	_Ylength=0;
	_Zlength=0;

}

/**
* Boxe's Destructor
* @param : NONE
* @return : NONE
*/
Boxe::~Boxe() {


}

/**
* Boxe's build Function: Initialise all the properties of the box
* @param Xlength: a float representing the X-axis length of the box
* @param Ylength: a float representing the Y-axis length of the box
* @param Zlength: a float representing the Z-axis length of the box
* @return : NONE
*/
void Boxe::buildBoxe(float Xlength, float Ylenght, float Zlength) {

	// vertex, normal and texture coordinates

	//Only need 2 triangles to render a box (3*12=36)
	_size = 36;

	_vertices = new float[_size * 3];
	_normals = new float[_size * 3];
	_texcoors = new float[_size * 2];

	_Xlength = Xlength;
	_Ylength = Ylenght;
	_Zlength = Zlength;

	float vertices[] = {
				0,0,0, _Xlength,0,0, 0,_Ylength,0,						//first triangle (base)
				 0,_Ylength,0, _Xlength,0,0,	_Xlength,_Ylength,0,	//second triangle (base)
				
				_Xlength,0,0, _Xlength,_Ylength,0, _Xlength,_Ylength,_Zlength,
				_Xlength,_Ylength,_Zlength, _Xlength,0,_Zlength, _Xlength,0,0,
				
				_Xlength,0,0, 0,0,0, _Xlength,0,_Zlength,
				_Xlength,0,_Zlength, 0,0,0, 0,0,_Zlength,

				0,0,0, 0,_Ylength,0, 0,0,_Zlength,
				0,0,_Zlength, 0,_Ylength,0, 0,_Ylength,_Zlength,

				0,_Ylength,0,  _Xlength,_Ylength,_Zlength, _Xlength,_Ylength,0,
				_Xlength,_Ylength,_Zlength, 0,_Ylength,0, 0,_Ylength,_Zlength,

				0,_Ylength,_Zlength, _Xlength,0,_Zlength, _Xlength,_Ylength,_Zlength, 	//11th triangle (top)
				_Xlength,0,_Zlength, 0,_Ylength,_Zlength, 0,0,_Zlength, 					//12nd triangle (top)
	};
	
	float normals[] = {
				0,0,-1, 0,0,-1, 0,0,-1,		//Outside Oriented Normals for the base of the box
				0,0,-1, 0,0,-1, 0,0,-1,

				1,0,0, 1,0,0, 1,0,0,
				1,0,0, 1,0,0, 1,0,0,

				0,-1,0, 0,-1,0, 0,-1,0,
				0,-1,0, 0,-1,0, 0,-1,0,

				-1,0,0, -1,0,0, -1,0,0,
				-1,0,0, -1,0,0, -1,0,0,

				0,1,0, 0,1,0, 0,1,0,
				0,1,0, 0,1,0, 0,1,0,
			
				0,0,1, 0,0,1, 0,0,1,		//Outside Oriented Normals for the top of the box
				0,0,1, 0,0,1, 0,0,1,

	};
	
	//DEFINE THE TEXTURE COORDINATES
	float texcoors[] = { 
		0, 1,   0, 0,	1, 1, //Below
		1, 1,   0, 0,	1, 0,

		0, 0,   1, 0,	1, 1, //Right
		1, 1,   0, 1,	0, 0,

		1, 0,   0, 0,	1, 1, //Back
		1, 1,   0, 0,	0, 1,

		0, 0,   1, 0,	0, 1, //Left
		0, 1,   1, 0,	1, 1,

		0, 0,   1, 1,	1, 0, //Front
		1, 1,   0, 0,	0, 1,

		1, 1,   0, 0,	1, 0, //On
		0, 0,   1, 1,	0, 1 
	};



	for (unsigned int i = 0; i < _size * 3; i = i + 1) {

		_vertices[i] = vertices[i];
		_normals[i] = normals[i];
	}

	for (unsigned int i = 0; i<_size * 2; i++)
	{
		_texcoors[i] = texcoors[i];
	}
	

}


/**
* Boxe's draw function: draw the box according to the last bound shader
* @param : NONE
* @return : NONE
*/
void Boxe::draw() {
	// draw triangle strips from the currently bound VAO
	// with current in-use shader
	int nbStackTriangles = _size*3;
	// draw triangles from the currently bound VAO with current in-use shader
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0,
		nbStackTriangles);

}