#include "Model3D.h"
#include <stb_image.h>
#include <FreeImage.h>
#include <glm/gtc/type_ptr.hpp>

// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model3D::loadModel(const std::string path)
{
	// Read file via ASSIMP
	scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// Retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

void Model3D::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model3D::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vec3;
		glm::vec2 vec2;

		if (mesh->HasPositions()) {
			vec3.x = mesh->mVertices[i].x;
			vec3.y = mesh->mVertices[i].y;
			vec3.z = mesh->mVertices[i].z;
			vertex.Position = vec3;
		}
		
		if (mesh->HasNormals()) {
			vec3.x = mesh->mNormals[i].x;
			vec3.y = mesh->mNormals[i].y;
			vec3.z = mesh->mNormals[i].z;
			vertex.Normal = vec3;
		}

		if (mesh->mTextureCoords[0])
		{
			vec2.x = mesh->mTextureCoords[0][i].x;
			vec2.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec2;
		}
		else
			vertex.TexCoords = glm::vec2(0, 0);

		vertices.push_back(vertex);
	}

	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	
	aiColor3D dcolor(0.f, 0.f, 0.f);
	aiColor3D acolor(0.f, 0.f, 0.f);	
	aiColor3D scolor(0.f, 0.f, 0.f);

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	if (mesh->mMaterialIndex >= 0)
	{
		material->Get(AI_MATKEY_SHININESS, shininess);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, dcolor);
		material->Get(AI_MATKEY_COLOR_AMBIENT, acolor);
		material->Get(AI_MATKEY_COLOR_SPECULAR, scolor);

		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		//// 3. normal maps
		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// 4. height maps
		std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	glm::vec3 a(acolor.r, acolor.g, acolor.b);
	glm::vec3 d(dcolor.r, dcolor.g, dcolor.b);
	glm::vec3 s(scolor.r, scolor.g, scolor.b);

	if (glm::length(a) == 0) a = glm::vec3(0.2, 0.2, 0.2);
	if (glm::length(d) == 0) d = glm::vec3(1, 1, 1);
	if (glm::length(s) == 0) s = glm::vec3(0.3, 0.3, 0.3);

	diffuse = d;
	ambient = a;
	specular = s;

	if (shininess < 0) shininess = 1.0f;

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model3D::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

GLint Model3D::TextureFromFile(const char* path, std::string directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}