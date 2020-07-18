#pragma once
#include <vector>
#include "Model3D.h"

// 애니메이션 적용된 모델의 데이터
struct AnimatedModelData {
	// 모델 데이터
	Model3D* model;

	// 인스턴싱 개수
	int instancingCount;
	
	// 애니메이션 속도
	float* animationSpeed;

	// 애니메이션의 현재 프레임
	float* animCurrentFrame;
	
	// 애니메이션의 최대 프레임
	float animMaxFrame;

	// 애니메이션 재생, 정지
	int* isAnimating;

	//위치, 회전 데이터
	std::vector<glm::mat4> transMatrix;
	
	//위치, 회전 시작 프레임
	float* transStartFrame;
	
	//위치, 회전 현재 프레임
	float* transCurrentFrame;

	//위치, 회전 최대 프레임
	float transMaxFrame;
};

class ModelManager
{
public:
	ModelManager();
	~ModelManager();

	std::vector<AnimatedModelData*> m_AnimatedModelData;

	// Add Animated Model data to Manager
	void AddAnimatedModelData(std::string transformMatrixPath, Model3D* model3D, int _instancingCount);
	
	// get position, rotation data from txt file
	void SetTransformData(std::string path, int index);
	
	// Animation on, off
	void SwitchisAnimating();

private:
	// Model Index
	int dataIndex;

	// Send Instancing data to shader
	void SetInstancingData(int index);
};