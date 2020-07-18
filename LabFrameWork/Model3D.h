#pragma once

#include <string>
#include <iostream>
#include <map>
#include <chrono>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Loader.h"
#include "ModelView.h"

class Model3D
{
public:
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;

	glm::vec3 diffuse;
	glm::vec3 ambient;
	glm::vec3 specular;
	float shininess;

	Model3D(char* path)
	{
		loadModel(path);
	}
	void Draw(ShaderProgram& shader)
	{
		for (GLuint i = 0; i < meshes.size(); i++)
			meshes[i].draw(shader);
	}

private:
	const aiScene *scene;
	Assimp::Importer importer;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	GLint TextureFromFile(const char* path, std::string directory);
};