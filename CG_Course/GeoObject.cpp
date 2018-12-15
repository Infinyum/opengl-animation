#include "stdafx.h"
#include "GeoObject.h"

GeoObject::GeoObject()
{

	_size = 0;
	_vertices = _normals = _texcoors = NULL;
	_vao = 0;

}

GeoObject::~GeoObject()
{

	if (_vertices != NULL)
		delete[] _vertices;

	if (_normals != NULL)
		delete[] _normals;

	if (_texcoors != NULL)
		delete[] _texcoors;

	_vertices = NULL;
	_normals = NULL;
	_texcoors = NULL;
	_vao = 0;

}


void GeoObject::buildSphere(float radius, int nbStacks, int nbSlices)
{
	float deltaTheta, deltaPhi;

	// vertex, normal and texture coordinates
	_slices = nbSlices;
	_stacks = nbStacks;
	_size = nbStacks * (nbSlices + 1) * 2;

	_vertices = new float[_size * 3];
	_normals = new float[_size * 3];
	_texcoors = new float[_size * 2];

	// angular distance between two latitudinal lines
	deltaPhi = -M_PI / nbSlices;

	// angular distance between two longitudinal lines
	deltaTheta = 2.0f * M_PI / nbStacks;

	float *vertices = _vertices;
	float *normals = _normals;
	float *texcoors = _texcoors;

	// triangle strips made of vertical stacks
	for (int i = 0; i < nbStacks; i++) {
		// azimuth angles of the two arcs defining the stack (longitudes)
		float theta = i * deltaTheta;
		float thetaPrime = theta + deltaTheta;

		// projection of the first theta in the horizontal plane
		float x0 = cos(theta);
		float y0 = sin(theta);

		// projection of the second theta in the horizontal plane
		float x1 = cos(thetaPrime);
		float y1 = sin(thetaPrime);

		// tesselating the triangle strip into nbSlices
		// note <= instead of < for making sure that the last angke is used
		for (int j = 0; j <= nbSlices; j++) {
			// polar angle
			float phi = j * deltaPhi;

			// polar vector in a vertical plane
			float xPolar = cos(phi);
			float yPolar = sin(phi);

			// vertex #1 (theta , phi)
			*normals = yPolar * x0;
			*vertices = *normals * radius;

			normals++;
			vertices++;

			*normals = yPolar * y0;
			*vertices = *normals * radius;

			normals++;
			vertices++;

			*normals = xPolar;
			*vertices = *normals * radius;

			normals++;
			vertices++;

			*texcoors = (float)i / (float)nbStacks;
			texcoors++;
			*texcoors = (float)j / (float)nbSlices;
			texcoors++;

			// vertex #2 (theta , phiPrime)
			*normals = yPolar * x1;
			*vertices = *normals * radius;

			normals++;
			vertices++;

			*normals = yPolar * y1;
			*vertices = *normals * radius;

			normals++;
			vertices++;

			*normals = xPolar;
			*vertices = *normals * radius;

			normals++;
			vertices++;

			*texcoors = (float)(i + 1) / (float)nbStacks;
			texcoors++;
			*texcoors = (float)j / (float)nbSlices;
			texcoors++;
		}
	}
}


void GeoObject::InitVBO(void) {
	// vertex buffer objects and vertex array
	unsigned int vboVertices = 0;
	//creates 1 buffer at the identifier "vboVertices"
	glGenBuffers(1, &vboVertices);
	//bind the buffer and tell it which kind of buffer it is. in this case, just an array
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	//load data into the buffer. (type of buffer, size of buffer, pointer to data, kind of usage)
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

	// vertex positions are location 0
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	//make OpenGL understand what the vertex is (position of the vertex to be defined, size (2 for 2D), type, normalized between 0 and 1,
	//stride, offset of the first component to the vertex to be defined)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(0);


	// vertex normals are location 2
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(1);

	// vertex colors are location 1
	glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);
	glEnableVertexAttribArray(2);

}
