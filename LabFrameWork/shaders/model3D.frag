#version 430 core
in vec2 TexCoords;
in vec3 Color;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	vec4 diffuse = texture(texture_diffuse1, TexCoords);
	FragColor = vec4(Color, 1.0) * diffuse;
}