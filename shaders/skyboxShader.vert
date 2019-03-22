#version 410 core

layout (location = 0) in vec3 vertexPosition;
out vec3 textureCoordinates;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 tempPos = projection * view * vec4(vertexPosition, 1.0);
    gl_Position = tempPos.xyww;//z e de la 0 la 1 definita in frag shader
	//x,y,z se impart la w cand ajung in frag shader
    textureCoordinates = vertexPosition;
}
