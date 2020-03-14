#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec2 texCoords;

void main()
{
    
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, 0.0, 1.0); 
    texCoords = aTexCoords;
}