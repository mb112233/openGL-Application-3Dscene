#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;//unde se lipeste de textura obiectul,face parte din obiect

out vec3 normal;
out vec4 fragPosEye;
out vec2 fragTexCoords;
out vec4 fragLight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



uniform mat4 lightSpaceTrMatrix;


void main() 
{
	//compute eye space coordinates
	fragPosEye = view * model * vec4(vPosition, 1.0f);
	normal = vNormal;
	fragLight=lightSpaceTrMatrix*model*vec4(vPosition, 1.0f);
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	fragTexCoords=vTexCoords;
}
