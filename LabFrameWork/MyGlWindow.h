#pragma warning(push)
#pragma warning(disable:4311)		// convert void* to long
#pragma warning(disable:4312)		// convert long to void*

#include <iostream>
#include <string>
#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"

#include "cube.h"
#include "Model3D.h"
#include "ModelManager.h"
#include "Viewer.h"
#include "ModelView.h"

#pragma warning(pop)

class MyGlWindow {
public:
	MyGlWindow(int w, int h);
	~MyGlWindow();
	void draw();
	void setSize(int w, int h) { m_width = w; m_height = h; }
	void setAspect(float r) { m_viewer->setAspectRatio(r); }
	Viewer *m_viewer;
	float m_rotate;
	float speed;

private:
	glm::vec3 eye, look, up;
	glm::mat4 model, view, projection, mview, imvp;
	glm::mat3 nmat;

	int m_width;
	int m_height;
	float startTime;

	void initialize();

	void setupShaders();
	void drawFloor(ShaderProgram * shader, glm::mat4 & view, glm::mat4 & projection);
	void drawCube(ShaderProgram * shader, glm::mat4 & view, glm::mat4 & projection);
	void drawModel3D(ShaderProgram* shader, glm::mat4& view, glm::mat4& projection);
	void setupFloor();
	void setupCube();
	void setupModel3D();

	checkeredFloor* m_floor;
	colorCube * m_cube;
	Model3D* m_Model3D;

	Model m_model;

	ShaderProgram * m_floorShader;
	ShaderProgram * m_cubeShader;
	ShaderProgram * m_model3DShader;
};
