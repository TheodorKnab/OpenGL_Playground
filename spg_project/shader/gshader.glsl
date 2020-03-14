#version 410 core
layout (points) in;
layout (triangle_strip, max_vertices = 15) out;

in VS_OUT {
    vec3 color;
} gs_in[];


out vec3 fColor;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform sampler3D densityTexture;

void build_house(vec4 position)
{    

    vec4 worldPos = inverse(model) * inverse(view) * inverse(projection) * position;
    fColor = vec3(texture(densityTexture, vec3(worldPos.x / 95, worldPos.y / 95, worldPos.z / 256)).rrr); 
    gl_Position = position + projection * view * model * vec4(-0.1, -0.1, 0.0, 0.0); // 1:bottom-left   
    EmitVertex();   
    gl_Position = position + projection * view * model * vec4( 0.1, -0.1, 0.0, 0.0); // 2:bottom-right
    EmitVertex();
    gl_Position = position + projection * view * model * vec4(-0.1, 0.1, 0.0, 0.0); // 3:top-left
    EmitVertex();
    gl_Position = position + projection * view * model * vec4( 0.1, 0.1, 0.0, 0.0); // 4:top-right
    EmitVertex();
    gl_Position = position + projection * view * model * vec4( 0.0, 0.2, 0.0, 0.0); // 5:top
    EmitVertex();
    EndPrimitive();
}

void main() {       

    build_house(gl_in[0].gl_Position);
}

