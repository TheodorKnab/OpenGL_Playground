// #version 430 core 
// layout(points) in; 
// layout(triangle_strip, max_vertices = 4) out; 
 
// in vec3 vs_color[]; 
// out vec3 fs_color; 
// out vec2 fs_txCoords; 
 
// layout(location = 1) uniform mat4 projection; 
// const vec2 offsets[4] = { 
// 	vec2(0.f, 0.f), vec2(1.f, 0.f), vec2(0.f, 1.f), vec2(1.f, 1.f) }; 
 
// void main() { 
// 	fs_color = vs_color[0]; 
// 	for (int i = 0; i < 4; ++i) { 
// 		fs_txCoords = offsets[i]; 
// 		vec4 pos = gl_in[0].gl_Position; 
// 		pos.xy += .5f * (offsets[i] - vec2(0.5f)); 
// 		gl_Position = projection * pos; 
// 		EmitVertex(); 
// 	} 
// }

#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec3 vs_color; 
} gs_in[]; 

out GS_Out{
// Stream out to a VB & save for reuse!
// .xyz = wsCoord, .w = occlusion
    vec3 fColor;
} gs_out;

void build_house(vec4 position)
{    
	gs_out.fColor = gs_in[0].vs_color;
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:bottom-left
    EmitVertex();   
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:bottom-right
    EmitVertex();
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:top-left
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:top-right
    EmitVertex();
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:top
    EmitVertex();
    EndPrimitive();
}

void main() {    
    build_house(gl_in[0].gl_Position);
}  