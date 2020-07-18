#include "MyGlWindow.h"
#include "global.h"
#include <vector>
#include <algorithm>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <glm/gtc/type_ptr.hpp>

#include "Model3D.h"

static float DEFAULT_VIEW_POINT[3] = { 50, 50, 50 };
static float DEFAULT_VIEW_CENTER[3] = { 0, 0, 0 };
static float DEFAULT_UP_VECTOR[3] = { 0, 1, 0 };

glm::mat4 lookAt(glm::vec3 pos, glm::vec3 look, glm::vec3 up)
{
	glm::vec3 zaxis = glm::normalize(pos - look);
	glm::vec3 xaxis = glm::normalize(glm::cross(up, zaxis));
	glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));

	glm::mat4 R;

	R[0] = glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0.0f);
	R[1] = glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0.0f);
	R[2] = glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0.0f);
	R[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 T;

	T[0] = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	T[1] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	T[2] = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	T[3] = glm::vec4(-pos.x, -pos.y, -pos.z, 1.0f);

	return R * T;
}

glm::mat4 perspective(float fov, float aspect, float n, float f)
{
	glm::mat4 P(0.0f);

	const float tanHalfFOV = 1.0f / (tan(glm::radians(fov) / 2.0f));

	float A = -(n + f) / (f - n);
	float B = -(2 * (n * f)) / (f - n);

	P[0] = glm::vec4(tanHalfFOV / aspect, 0, 0, 0);
	P[1] = glm::vec4(0, tanHalfFOV, 0, 0);
	P[2] = glm::vec4(0, 0, A, -1.0f);
	P[3] = glm::vec4(0, 0, B, 0.0f);

	return P;
}

MyGlWindow::MyGlWindow(int w, int h)
{
	startTime = (float)clock();
	m_width = w;
	m_height = h;
	m_cube = 0;

	glm::vec3 viewPoint(DEFAULT_VIEW_POINT[0], DEFAULT_VIEW_POINT[1], DEFAULT_VIEW_POINT[2]);
	glm::vec3 viewCenter(DEFAULT_VIEW_CENTER[0], DEFAULT_VIEW_CENTER[1], DEFAULT_VIEW_CENTER[2]);
	glm::vec3 upVector(DEFAULT_UP_VECTOR[0], DEFAULT_UP_VECTOR[1], DEFAULT_UP_VECTOR[2]);

	float aspect = (w / (float)h);
	m_viewer = new Viewer(viewPoint, viewCenter, upVector, 45.0f, aspect);
	m_rotate = 0;

	m_floorShader = 0;
	m_cubeShader = 0;
	m_model3DShader = 0;

	setupShaders();
	initialize();
}

void MyGlWindow::initialize()
{
	m_floor = new checkeredFloor();

	m_Model3D = new Model3D("models/backpack/backpack.obj");
}

void MyGlWindow::setupShaders()
{
	setupFloor();
	setupModel3D();
}

void MyGlWindow::setupFloor()
{
	m_floorShader = new ShaderProgram();
	m_floorShader->initFromFiles("shaders/floor.vert", "shaders/floor.frag");
	m_floorShader->addAttribute("VertexPosition");
	m_floorShader->addAttribute("VertexColor");
	m_floorShader->addUniform("MVP");
}

void MyGlWindow::setupCube()
{
	m_cubeShader = new ShaderProgram();
	m_cubeShader->initFromFiles("shaders/simple.vert", "shaders/simple.frag");
	m_cubeShader->addAttribute("coord3d");
	m_cubeShader->addAttribute("v_color");
	m_cubeShader->addUniform("mvp");
}

void MyGlWindow::setupModel3D()
{
	m_model3DShader = new ShaderProgram();
	m_model3DShader->initFromFiles("shaders/model3D.vert", "shaders/model3D.frag");

	m_model3DShader->addUniform("model");
	m_model3DShader->addUniform("view");
	m_model3DShader->addUniform("projection");
	m_model3DShader->addUniform("NormalMatrix");
	m_model3DShader->addUniform("LightPosition");
	m_model3DShader->addUniform("LightIntensity");

	m_model3DShader->addUniform("Kd");
	m_model3DShader->addUniform("Ka");
	m_model3DShader->addUniform("Ks");
	m_model3DShader->addUniform("shininess");

	m_model3DShader->addUniform("texture_diffuse1");
	m_model3DShader->addUniform("texture_specular1");
}

void MyGlWindow::draw(void)
{
	glViewport(0, 0, m_width, m_height);

	eye = m_viewer->getViewPoint();
	look = m_viewer->getViewCenter();
	up = m_viewer->getUpVector();

	float time = (clock() - startTime) / 1000.0f;

	view = lookAt(eye, look, up);
	projection = perspective(45.0f, 1.0f * m_width / m_height, 0.1f, 1000.0f);

	drawFloor(m_floorShader, view, projection);
	drawModel3D(m_model3DShader, view, projection);
}

void MyGlWindow::drawFloor(ShaderProgram * shader, glm::mat4 & view, glm::mat4 & projection)
{
	m_model.glPushMatrix();
	m_model.glTranslate(0.0f, 0.0f, 0.0f);

	shader->use();
	glm::mat4 mvp = projection * view * m_model.getMatrix();
	glUniformMatrix4fv(shader->uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));

	if (m_floor)
		m_floor->draw();

	shader->disable();
	m_model.glPopMatrix();
}

void MyGlWindow::drawCube(ShaderProgram * shader, glm::mat4 & view, glm::mat4 & projection)
{
	m_model.glPushMatrix();

	m_model.glTranslate(0, 1, 0);
	
	shader->use();
	{
		glm::mat4 mvp = projection * view * m_model.getMatrix();
		glUniformMatrix4fv(shader->uniform("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

		if (m_cube)
			m_cube->draw();
	}
	shader->disable();

	m_model.glPopMatrix();
}

void MyGlWindow::drawModel3D(ShaderProgram* shader, glm::mat4& view, glm::mat4& projection) {
	m_model.glPushMatrix();

	m_model3DShader->use();

	model = m_model.getMatrix();
	mview = view * model;

	imvp = glm::inverse(mview);
	nmat = glm::mat3(glm::transpose(imvp));
	
	glm::vec4 lightPos(10, 10, 10, 0);
	glm::vec3 LightIntensity(1, 1, 1);
	
	glUniformMatrix4fv(m_model3DShader->uniform("model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(m_model3DShader->uniform("view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(m_model3DShader->uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix3fv(m_model3DShader->uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));
	
	glUniform4fv(m_model3DShader->uniform("LightPosition"), 1, glm::value_ptr(view * lightPos));
	glUniform3fv(m_model3DShader->uniform("LightIntensity"), 1, glm::value_ptr(LightIntensity));
	
	glUniform3fv(m_model3DShader->uniform("Kd"), 1, glm::value_ptr(m_Model3D->diffuse));
	glUniform3fv(m_model3DShader->uniform("Ka"), 1, glm::value_ptr(m_Model3D->ambient));
	glUniform3fv(m_model3DShader->uniform("Ks"), 1, glm::value_ptr(m_Model3D->specular));
	glUniform1f(m_model3DShader->uniform("shininess"), m_Model3D->shininess);
	
	m_Model3D->Draw(*m_model3DShader);
	
	m_model3DShader->disable();
	
	m_model.glPopMatrix();
}

MyGlWindow::~MyGlWindow()
{
	delete m_floorShader;
	delete m_cube;
	delete m_model3DShader;
}