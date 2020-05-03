#version 440 core
layout (points) in;
layout (points, max_vertices = 30) out;

in VS_OUT {
    vec3 velocity; 
    float lifeTime;
    float type; 
} gs_in[]; 


out vec3 outPosition;
out vec3 outVelocity; 
out float outLifeTime;
out float outType;


void spawnParticle(vec4 position)
{    
	outPosition = position.xyz + vec3(1,0,1);
	outVelocity = vec3(0,0,0);
	outLifeTime = 0;
	outType = 1;
    EmitVertex();   
    EndPrimitive();
}

void main() {    
    //calculate
    float deltaTime = 0.001;
    //emit self
    outPosition = gl_in[0].gl_Position.xyz;
	outVelocity = gs_in[0].velocity;
	outLifeTime = gs_in[0].lifeTime + deltaTime;
	outType = gs_in[0].type;    
    EmitVertex();   
    EndPrimitive();
    if (gs_in[0].type < 0.5)
        spawnParticle(gl_in[0].gl_Position);
}  