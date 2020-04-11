#version 440 core
layout (location = 0) out vec4 FragColor;

#define TEXTURE_SCALE 0.02

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

layout(binding = 0) uniform sampler3D densityTexture;
layout(binding = 2) uniform sampler2D diffuseX;
layout(binding = 3) uniform sampler2D diffuseY;
layout(binding = 4) uniform sampler2D diffuseZ;
layout(binding = 5) uniform sampler2D displacementX;
layout(binding = 6) uniform sampler2D displacementY;
layout(binding = 7) uniform sampler2D displacementZ;

// uniform sampler3D densityTexture;
// uniform sampler2D diffuseX;
// uniform sampler2D diffuseY;
// uniform sampler2D diffuseZ;
// uniform sampler2D displacementX;
// uniform sampler2D displacementY;
// uniform sampler2D displacementZ;

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
    vec4 xColor = texture(diffuseX, xCoord);
    vec4 yColor = texture(diffuseY, yCoord);
    vec4 zColor = texture(diffuseZ, zCoord);

    vec4 blendedColor = xColor * blendWeights.xxxx +
                          yColor * blendWeights.yyyy +
                          zColor * blendWeights.zzzz;

    FragColor = vec4(blendedColor);
}  