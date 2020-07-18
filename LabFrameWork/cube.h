#ifndef __CUBE
#define __CUBE

#include <GL/glew.h>
#include <glm/mat4x4.hpp>

#include "Loader.h"
#include <vector>


class colorCube
{
public:
	colorCube();
	void setup();
	void draw();

	int elementSize;

	GLuint vaoHandle;
	GLuint vbo_cube_vertices, vbo_cube_colors, ibo_cube_elements;
	ShaderProgram *shaderProgram;


};



class checkeredFloor
{
public:
	checkeredFloor();
	void setup(float size, int nSquares);
	void draw();
	GLuint vaoHandle;
	GLuint vbo_cube_vertices, vbo_cube_colors;
	
};

#endif