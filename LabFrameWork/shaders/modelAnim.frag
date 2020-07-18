#version 440 core

in vec2 TexCoords;
in vec3 Color;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform int hasTextureDiffuse;
uniform int hasTextureSpecular;

void main()
{    
     color = vec4(Color, 1.0);
	
	 if (hasTextureDiffuse == 1) {
		 vec4 diffuse = texture(texture_diffuse1, TexCoords);
		 color = color * diffuse;
	 }

	 if (hasTextureSpecular == 1) {
		 vec4 specular = texture(texture_specular1, TexCoords).rgba;
		 color = color + specular * 0.2;
	 }
}