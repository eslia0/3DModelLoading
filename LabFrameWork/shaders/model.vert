#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

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

   //return LightIntensity * (Ka + Kd * max(dot(s,norm),0.0)) + LightIntensity * Ks * pow(max(dot(r, v), 0.0), shininess);
}


void main()
{
   vec3 eyeNorm = normalize(NormalMatrix * normal);
   vec4 eyePosition = view * model * vec4(position,1.0);
   Color = ads(eyePosition,eyeNorm);

   TexCoords = texCoords;
   gl_Position = projection * view * model * vec4(position, 1.0f);

}