//#include "stdafx.h"
#include "Object.h"

/**
* Object's Constructor: Initialise the protected fields of the object
* @param : NONE
* @return : NONE
*/
Object::Object()
{
	_size = 0;
	_vertices = _normals = _texcoors = NULL;
	_vao = 0;
}

/**
* Object's Destructor: Destroy the object
* @param : NONE
* @return : NONE
*/
Object::~Object()
{
	if (_vertices != NULL)
		delete[] _vertices;

	if (_normals != NULL)
		delete[] _normals;

	if (_texcoors != NULL)
		delete[] _texcoors;
}


/**
* Object's InitVBO function: Initialise the Vertex Buffer Object (VBO) and the Vertex Array of the object
* /!\ There are many version of this function, it must be modified if the objects are not correctly displayed /!\
* @param : NONE
* @return : NONE
*/
void Object::InitVBO(void) {
	// vertex buffer objects and vertex array
	unsigned int vboVertices = 0;
	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER,
		3 * _size * sizeof(float),
		_vertices,
		GL_STATIC_DRAW);

	unsigned int vboNormals = 0;
	glGenBuffers(1, &vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER,
		3 * _size * sizeof(float),
		_normals,
		GL_STATIC_DRAW);

	unsigned int vboTexCoords = 0;
	glGenBuffers(1, &vboTexCoords);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
	glBufferData(GL_ARRAY_BUFFER,
		2 * _size * sizeof(float),
		_texcoors,
		GL_STATIC_DRAW);

	_vao = 0;
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);


	/* normal version

	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);


	// normals are location 1
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // don't forget this!


	// normals are location 2
	glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(2); // don't forget this!

	*/

	/*School Computer Version*/
	/*
	// normals are location 0
	glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0); // don't forget this!

								  // normals are location 1
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // don't forget this!

								  // vertex positions are location 2
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(2);
	*/

	/*School Computer Version 2*/
	// normals are location 1
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0); // don't forget this!


								  // normals are location 2
	glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1); // don't forget this!

								  // vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(2);
}
