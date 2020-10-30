#version 330 core

in vec3 vPosition;
in vec4 vColor;
out vec4 color;

uniform mat4 modelViewMatrix;

void main() 
{
    gl_Position = modelViewMatrix * vec4(vPosition, 1.0);
	
     color = vColor;
}