#include "ModelManager.h"
#include <iostream>
#include <fstream>

ModelManager::ModelManager()
{
	dataIndex = 0;
}

ModelManager::~ModelManager()
{
	delete &m_AnimatedModelData;
}

// 애니메이션 모델 데이터를 추가
// 매개변수는 Transform 데이터 text 경로, 모델, 인스턴싱 개수
void ModelManager::AddAnimatedModelData(std::string transformMatrixPath, Model3D* model3D, int _instancingCount)
{
	AnimatedModelData* modelData = new AnimatedModelData();

	modelData->model = model3D;
	modelData->animationSpeed = new float[_instancingCount];
	modelData->instancingCount = _instancingCount;
	modelData->animCurrentFrame = new float[_instancingCount];
	modelData->animMaxFrame = (float)model3D->animScene->mAnimations[0]->mDuration;
	modelData->isAnimating = new int[_instancingCount];
	modelData->transCurrentFrame = new float[_instancingCount];

	// randomize animation's start frame
	srand((unsigned int)time(NULL));

	for (int i = 0; i < _instancingCount; i++)
	{
		int frame = rand() % (int)(modelData->animMaxFrame);
		modelData->animCurrentFrame[i] = (float)frame;
		modelData->animationSpeed[i] = 0.4f;
		modelData->isAnimating[i] = 1;

		modelData->transCurrentFrame[i] = 0.0f;
	}

	m_AnimatedModelData.push_back(modelData);

	SetTransformData(transformMatrixPath, dataIndex);
	SetInstancingData(dataIndex);
	dataIndex++;
}

// Set instancing transform data.
// Create tranform matrix from .txt file.
void ModelManager::SetTransformData(std::string path, int modelIndex)
{
	AnimatedModelData* data = m_AnimatedModelData[modelIndex];

	// read file
	std::fstream is(path);

	std::vector<float> nums;
	float num;
	
	// store data to int
	while (is >> num)
	{
		nums.push_back(num);
	}

	data->transMaxFrame = (float)nums.size() / data->instancingCount / 16;
	glm::mat4 mat;

	// Convert int vector into 4x4 matrix
	for (unsigned int i = 0; i < nums.size(); i++)
	{
		int index = i % 16;
		mat[index / 4][index % 4] = nums[i];
		
		if (i % 16 == 15)
		{
			data->transMatrix.push_back(mat);
		}
	}

	// Init trasnform current frame
	for (int i = 0; i < data->instancingCount; i++)
	{
		data->transCurrentFrame[i] = 0.0f;
	}
}

// Set Instancing data in shader
// Animation, instancing transform data
void ModelManager::SetInstancingData(int index)
{
	AnimatedModelData* data = m_AnimatedModelData[index];

	glBindVertexArray(data->model->VAO);
	glGenBuffers(1, &data->model->SSBO_animationFrame);
	glGenBuffers(1, &data->model->SSBO_isAnimating);
	glGenBuffers(1, &data->model->SSBO_animationSpeed);
	glGenBuffers(1, &data->model->SSBO_transformMatrix);
	glGenBuffers(1, &data->model->SSBO_transformFrame);

	// Animation Frame
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->model->SSBO_animationFrame);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data->instancingCount * sizeof(float), data->animCurrentFrame, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, data->model->SSBO_animationFrame);

	// is Animating
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->model->SSBO_isAnimating);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data->instancingCount * sizeof(int), data->isAnimating, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, data->model->SSBO_isAnimating);

	// Animation Speed
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->model->SSBO_animationSpeed);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data->instancingCount * sizeof(float), data->animationSpeed, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, data->model->SSBO_animationSpeed);

	// transform matrix
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->model->SSBO_transformMatrix);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data->transMatrix.size() * sizeof(glm::mat4), data->transMatrix.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, data->model->SSBO_transformMatrix);

	// transform Frame
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->model->SSBO_transformFrame);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data->instancingCount * sizeof(float), data->transCurrentFrame, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, data->model->SSBO_transformFrame);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindVertexArray(0);
}

// Switch animation with pause to play and play to pause
void ModelManager::SwitchisAnimating()
{
	std::vector<AnimatedModelData*>::iterator iter = m_AnimatedModelData.begin();

	for (; iter != m_AnimatedModelData.end(); iter++)
	{
		AnimatedModelData* data = *iter;
		for (int i = 0; i < data->instancingCount; i++)
		{
			data->isAnimating[i] = -data->isAnimating[i];
		}
	}

	glGenBuffers(1, &m_AnimatedModelData[0]->model->SSBO_isAnimating);
	glBindVertexArray(m_AnimatedModelData[0]->model->VAO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_AnimatedModelData[0]->model->SSBO_isAnimating);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_AnimatedModelData[0]->instancingCount * sizeof(int), m_AnimatedModelData[0]->isAnimating, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_AnimatedModelData[0]->model->SSBO_isAnimating);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindVertexArray(0);
}