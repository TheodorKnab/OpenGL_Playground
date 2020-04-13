#version 440 core
layout (location = 0) out vec4 FragColor;

#define TEXTURE_SCALE 0.03
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
  
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPos;

uniform float heightScale;
uniform int primaryLayers;
uniform int secondaryLayers;


vec3 AddParallax(vec2 coord, sampler2D dispMap, vec3 toEyeVec, float dispStrength)
{
    float h = 1;
    vec2 uv = coord;
    float prev_hits;
    float ddh = 1.0 / float(primaryLayers);
    float dd_texels_max = length(-toEyeVec.xy * dispStrength);
    vec2 dduv_max = (1.0 / 2048.0 * dd_texels_max).xx;
    vec2 dduv = dduv_max / float(primaryLayers);

    // primary iterations
    prev_hits = 0;
    float hit_h = 0;
    for (int i = 0; i < primaryLayers; i++)
    {
        h -= ddh;
        uv += dduv;
        float h_tex = texture(dispMap, uv).x; 
        float is_first_hit = clamp((h_tex - h - prev_hits) * 4999999,0.0,1.0);
        hit_h += is_first_hit * h;
        prev_hits += is_first_hit;
    }

    // secondary iterations
    
    //go back one primary step
    h = hit_h + ddh;
    uv = coord + dduv_max * (1 - h);
    float x = 1.0 / float(primaryLayers * secondaryLayers);
    ddh *= x;
    dduv *= x;
  
    prev_hits = 0;
    hit_h = 0;
    for (int i = 0; i < secondaryLayers; i++)
    {
        h -= ddh;
        uv += dduv;
        float h_tex = texture(dispMap, uv).x; 
        float is_first_hit = clamp((h_tex - h - prev_hits) * 4999999, 0.0, 1.0); 
        hit_h += is_first_hit * h;
        prev_hits += is_first_hit;
    }

    vec2 new_coord = coord + dduv_max * (1 - hit_h);
    return vec3(new_coord, hit_h);
}


void main()
{


     //normal for shading
    vec3 pixelNormalW; 
    pixelNormalW = gs_in.wsNormal; 
    vec3 viewPosCorrection = -viewPos;	

    //blend weights for triplanar projection
    vec3 blendWeights= abs(gs_in.wsNormal) - 0.2;
    blendWeights *= 7;
    blendWeights = pow(blendWeights, vec3(3,3,3));
    blendWeights = max(blendWeights, 0);
    blendWeights.y *= 0.005;
    // and so they sum to 1.0:
    blendWeights /= dot(blendWeights, vec3(1,1,1));


    //triplanar projection coords
    vec2 xCoord = gs_in.wsCoord.yz * TEXTURE_SCALE;
    vec2 yCoord = gs_in.wsCoord.xz * TEXTURE_SCALE;
    vec2 zCoord = gs_in.wsCoord.xy * TEXTURE_SCALE;

    // view direction calculation
    
    //vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
    vec3 viewDir = normalize(viewPosCorrection - gs_in.wsCoord);


    //Displacement Mapping

    // xCoord = AddParallax(xCoord, displacementX, viewDir.yzx, heightScale).xy;
    // yCoord = AddParallax(yCoord, displacementY, viewDir.xzy, heightScale).xy;
    // zCoord = AddParallax(zCoord, displacementZ, viewDir.xyz, heightScale).xy;

    //blend colors from textures
    vec4 xColor = texture(diffuseX, xCoord);
    vec4 yColor = texture(diffuseY, yCoord);
    vec4 zColor = texture(diffuseZ, zCoord);

    vec4 blendedColor = xColor * blendWeights.xxxx +
                        yColor * blendWeights.yyyy +
                        zColor * blendWeights.zzzz;
   

    FragColor = vec4(blendedColor);
}  