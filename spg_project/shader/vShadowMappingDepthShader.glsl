#version 330 core 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec4 v_position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
		gl_Position = lightSpaceMatrix * model * vec4(aPos,1);
		v_position =  lightSpaceMatrix * model * vec4(aPos,1);
}