#version 410 core
layout (location = 0) out vec4 FragColor;

#define TEXTURE_SCALE 0.2

// in GS_Out{
// // Stream out to a VB & save for reuse!
// // .xyz = wsCoord, .w = occlusion
//     vec3 wsNormal;
// } fs_in;

in GS_Out{
// Stream out to a VB & save for reuse!
// .xyz = wsCoord, .w = occlusion
    vec3 wsCoord;
    vec3 wsNormal;
    vec3 fColor;
} gs_in;

uniform sampler3D densityTexture;

uniform sampler2D diffuseX;
uniform sampler2D diffuseY;
uniform sampler2D diffuseZ;
uniform sampler2D displacementX;
uniform sampler2D displacementY;
uniform sampler2D displacementZ;


void main()
{
    //blend weights for triplanar projection
    vec3 blendWeights= abs(gs_in.wsNormal) - 0.2;
    blendWeights *= 7;
    blendWeights = pow(blendWeights, vec3(3,3,3));
    blendWeights = max(blendWeights, 0);
    // and so they sum to 1.0:
    blendWeights /= dot(blendWeights, vec3(1,1,1));

    //triplanar pprojection coords
    vec2 xCoord = gs_in.wsCoord.yz * TEXTURE_SCALE;
    vec2 yCoord = gs_in.wsCoord.xz * TEXTURE_SCALE;
    vec2 zCoord = gs_in.wsCoord.xy * TEXTURE_SCALE;

    // //blend colors from textures
    // float4 xAxisColor = lichen1.Sample(LinearRepeatAnsio, xAxisCoord);
    // float4 yAxisColor = lichen2.Sample(LinearRepeatAnsio, yAxisCoord);
    // float4 zAxisColor = lichen3.Sample(LinearRepeatAnsio, zAxisCoord);

    // float4 blendedColor = xAxisColor * blend_weights.xxxx +
    //                       yAxisColor * blend_weights.yyyy +
    //                       zAxisColor * blend_weights.zzzz;

    FragColor = vec4(gs_in.wsNormal, 1.0);
}  