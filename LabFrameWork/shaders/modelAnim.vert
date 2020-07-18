#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in ivec4 boneID;
layout (location = 4) in vec4 weight;

layout( std430, binding = 1 ) buffer ssbo1
{
	mat4 gBones[ ];
};

layout( std430, binding = 2 ) buffer ssbo2
{
	float currentAnimFrame[ ];
};

layout( std430, binding = 5) buffer ssbo5
{
	mat4 transform[ ];
};

layout( std430, binding = 6 ) buffer ssbo6
{
	float currentTransFrame[ ];
};

out vec2 TexCoords;
out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 NormalMatrix;

uniform vec4 LightPosition;
uniform vec3 LightIntensity;
uniform vec3 Kd;
uniform vec3 Ka;
uniform vec3 Ks;
uniform float shininess;
uniform int animIndex;
uniform float transMaxFrame;
uniform float animMaxFrame;

uniform int boneNum;

vec3 ads(vec4 position, vec3 norm)
{
   vec3 s = normalize(vec3(LightPosition));
   vec3 v = normalize(vec3(-position));
   vec3 r = reflect(-s,norm);

   vec3 specular = vec3(0, 0, 0);
   if (dot(s, norm) > 0.0) {
	    specular = LightIntensity * Ks * pow(max(dot(r, v), 0.0),max(50.0, shininess));
   }
   return LightIntensity * (Ka + Kd * max(dot(s, norm), 0.0)) + specular;
}

// Interpolate animation
mat4 InterpolateBoneTransform(float currentFrame){
	float weight1 = currentFrame - floor(currentFrame);	
	float weight2 = 1.0 - weight1;
	
	int boneOffset1 = (animIndex * int(animMaxFrame) + int(floor(currentFrame))) * boneNum;
	int boneOffset2 = (animIndex * int(animMaxFrame) + int(min(floor(currentFrame) + 1, animMaxFrame - 1))) * boneNum;

	mat4 BoneTransform1 = gBones[boneOffset1 + boneID[0]] * weight[0];
	BoneTransform1 += gBones[boneOffset1 + boneID[1]] * weight[1];
	BoneTransform1 += gBones[boneOffset1 + boneID[2]] * weight[2];
	BoneTransform1 += gBones[boneOffset1 + boneID[3]] * weight[3];

	mat4 BoneTransform2 = gBones[boneOffset2 + boneID[0]] * weight[0];
	BoneTransform2 += gBones[boneOffset2 + boneID[1]] * weight[1];
	BoneTransform2 += gBones[boneOffset2 + boneID[2]] * weight[2];
	BoneTransform2 += gBones[boneOffset2 + boneID[3]] * weight[3];

	return (BoneTransform1 * weight2) + (BoneTransform2 * weight1);
}

void main()
{
	mat4 BoneOffset = InterpolateBoneTransform(currentAnimFrame[gl_InstanceID]);
	mat4 transformOffset = transform[gl_InstanceID * int(transMaxFrame) + int(currentTransFrame[gl_InstanceID])];

	vec3 bNorm = (BoneOffset * vec4(normal, 0.0)).xyz;
	vec3 eyeNorm = normalize(NormalMatrix * bNorm);
	vec4 eyePosition = view * transformOffset * model * BoneOffset * vec4(position,1.0);
	Color = ads(eyePosition, eyeNorm);

	TexCoords = texCoords;
    
	gl_Position = projection * view * transformOffset * model * BoneOffset * vec4(position, 1.0f);
}