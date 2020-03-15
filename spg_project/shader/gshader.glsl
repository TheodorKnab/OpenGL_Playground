#version 410 core
layout (points) in;
layout (triangle_strip, max_vertices = 15) out;

in VS_OUT {
    vec3 wsCoord;
    vec3 uvw;
    vec4 f0123;
    vec4 f4567;
    uint mc_case;
} gs_in[]; 


out vec3 fColor;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform sampler3D densityTexture;

void build_house(vec4 position)
{    
    float scale = 0.5;
    vec4 worldPos = inverse(model) * inverse(view) * inverse(projection) * position;
    fColor = vec3(texture(densityTexture, vec3(worldPos.x / 96, worldPos.y / 96, worldPos.z / 256)).rrr); 
    gl_Position = (position * scale + projection * view * model * vec4(-0.5 * scale, -0.5 * scale, 0.0, 0.0)); // 1:bottom-left   
    EmitVertex();   
    gl_Position = (position * scale + projection * view * model * vec4( 0.5 * scale, -0.5 * scale, 0.0, 0.0)); // 2:bottom-right
    EmitVertex();
    gl_Position = (position * scale + projection * view * model * vec4(-0.5 * scale , 0.5  * scale, 0.0, 0.0)); // 3:top-left
    EmitVertex();
    gl_Position = (position * scale + projection * view * model * vec4( 0.5 * scale, 0.5 * scale, 0.0, 0.0)); // 4:top-right
    EmitVertex();
    EndPrimitive();

}

void build_house_s(vec4 position)
{    
    float scale = 0.5;
    vec4 worldPos = inverse(model) * inverse(view) * inverse(projection) * position;
    fColor = vec3(texture(densityTexture, vec3(worldPos.x / 96, worldPos.y / 96, worldPos.z / 256)).rrr); 
    gl_Position = (position * scale + projection * view * model * vec4(-0.1 * scale, -0.1 * scale, 0.0, 0.0)); // 1:bottom-left   
    EmitVertex();   
    gl_Position = (position * scale + projection * view * model * vec4( 0.1 * scale, -0.1 * scale, 0.0, 0.0)); // 2:bottom-right
    EmitVertex();
    gl_Position = (position * scale + projection * view * model * vec4(-0.1 * scale, 0.1  * scale, 0.0, 0.0)); // 3:top-left
    EmitVertex();
    gl_Position = (position * scale + projection * view * model * vec4( 0.1 * scale, 0.1 * scale, 0.0, 0.0)); // 4:top-right
    EmitVertex();
    EndPrimitive();

}

void main() {       
    if(gs_in[0].mc_case != 0){        
        build_house_s(gl_in[0].gl_Position);
    }
}

