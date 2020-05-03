
#version 430 core 


layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 velocity; 
layout(location = 2) in float lifeTime; 
layout(location = 3) in float type; 

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT {
    vec3 vs_color; 
} vs_out;
 
void main() { 
	vs_out.vs_color = velocity; 
	gl_Position = projection * view * model * vec4(position, 1.f); 
}