
#version 330 core 
// in vec3 fs_color; 
// in vec2 fs_txCoords; 
// out vec4 color; 
// layout(location = 2) uniform sampler2D txSampler; 
 
// void main() { 
// 	color = texture(txSampler, fs_txCoords) * vec4(fs_color, 1.f); 
// }


in GS_Out{
// Stream out to a VB & save for reuse!
// .xyz = wsCoord, .w = occlusion
    vec3 fColor;
} gs_in;
out vec4 FragColor;

void main()
{
    FragColor = vec4(gs_in.fColor, 1.0);   
}  