#version 440 core
layout (points) in;
layout (triangle_strip, max_vertices = 15) out;

in VS_OUT {
    vec3 wsCoord;
    vec3 uvw;
    vec4 f0123;
    vec4 f4567;
    uint mc_case;
} gs_in[]; 

vec3 wsVoxelSize= vec3(1.0/95.0, 1.0/95.0,1.0/256.0);

out vec3 fColor;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


uniform sampler3D densityTexture;
uniform isamplerBuffer mcTableTexture;
uniform vec3 densityTextureDimensions;

int edge_start[72] = {
    // 0
    0,0,0,
    0,0,1,
    // 1
    0,0,1,
    1,0,1,
    // 2
    1,0,1,
    1,0,0,
    // 3
    1,0,0,
    0,0,0,
    // 4
    0,1,0,  
    0,1,1,
    // 5
    0,1,1,
    1,1,1,
    // 6
    1,1,1,
    1,1,0,
    // 7
    1,1,0,
    0,1,0,
    // 8
    0,0,0,
    0,1,0,
    // 9
    0,0,1,
    0,1,1,
    // 10
    1,0,1,
    1,1,1,
    // 11
    1,0,0,
    1,1,0
};



// __constant int4 cubeOffsets[8] = {
// 		{0, 0, 0, 0},
// 		{1, 0, 0, 0},
// 		{0, 0, 1, 0},
// 		{1, 0, 1, 0},
// 		{0, 1, 0, 0},
// 		{1, 1, 0, 0},
// 		{0, 1, 1, 0},
// 		{1, 1, 1, 0},
// 	}; 

vec4 cornerAmask0123[12] = {
    vec4(1, 0, 0, 0),
    vec4(0, 1, 0, 0),
    vec4(0, 0, 1, 0),
    vec4(0, 0, 0, 1),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(1, 0, 0, 0),
    vec4(0, 1, 0, 0),
    vec4(0, 0, 1, 0),
    vec4(0, 0, 0, 1),
    
}; 

vec4 cornerAmask4567[12] = {
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(1, 0, 0, 0),
    vec4(0, 1, 0, 0),
    vec4(0, 0, 1, 0),
    vec4(0, 0, 0, 1),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    
}; 

vec4 cornerBmask0123[12] = {
    vec4(0, 1, 0, 0),
    vec4(0, 0, 1, 0),
    vec4(0, 0, 0, 1),
    vec4(1, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    
}; 

vec4 cornerBmask4567[12] = {
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 0, 0, 0),
    vec4(0, 1, 0, 0),
    vec4(0, 0, 1, 0),
    vec4(0, 0, 0, 1),
    vec4(1, 0, 0, 0),
    vec4(1, 0, 0, 0),
    vec4(0, 1, 0, 0),
    vec4(0, 0, 1, 0),
    vec4(0, 0, 0, 1),    
}; 


void placeVertOnEdge(uint edgeNum)
{
    // Along this cell edge, where does the density value hit zero?
    float str0= dot(cornerAmask0123[edgeNum], gs_in[0].f0123) + dot(cornerAmask4567[edgeNum], gs_in[0].f4567);
    float str1= dot(cornerBmask0123[edgeNum], gs_in[0].f0123) + dot(cornerBmask4567[edgeNum], gs_in[0].f4567);
    float t= clamp((str0/(str0 - str1)), 0.0,1.0 ); //0..1
    // use that to get wsCoordand uvwcoords
     
    vec3 step = vec3(1.0 / densityTextureDimensions.x, 0, 1.0 / densityTextureDimensions.z);

    vec3 point0 = vec3(edge_start[edgeNum * 6], edge_start[edgeNum * 6 + 1], edge_start[edgeNum * 6 + 2]);  
    vec3 point1 = vec3(edge_start[edgeNum * 6 + 3], edge_start[edgeNum * 6 + 4], edge_start[edgeNum * 6 + 5]);
    
    //float str0 = 0.5;
    //str0 = texture(densityTexture, vec3(0.5,0.5,0.5)).r;

    //float str1 = texture(gs_in[0].tex, (gs_in[0].uvw + step.zxx*point1.zyx).xyz).x;

    //float t = clamp( str0, 0.0,1.0 ); 

    //t = 0;
    vec3 pos_within_cell = mix(point0, point1, t); //[0..1]

    //fColor = vec3(1, 1, str0);
    

    vec3 vecWsCoord= gs_in[0].wsCoord.xyz + pos_within_cell.xyz;// * wsVoxelSize;
    gl_Position = projection * view * model * vec4(vecWsCoord, 1);
    EmitVertex();
    //float3 uvw= input.uvw + ( pos_within_cell*inv_voxelDimMinusOne).xzy;

    //GSOutputoutput;
    //output.wsCoord_Ambo.xyz= wsCoord;
    //output.wsCoord_Ambo.w= grad_ambo_tex.SampleLevel(s, uvw, 0).w;
    //output.wsNormal= ComputeNormal(tex, s, uvw);
    //return output;
}

void buildMarchingCube()
{
    
    fColor = gs_in[0].f0123.xyz;
    int tablePos = int(gs_in[0].mc_case) * 16;

    while(texelFetch(mcTableTexture, int(tablePos)).x != -1){
        
        placeVertOnEdge(texelFetch(mcTableTexture, int(tablePos)).x);
        tablePos++;
        placeVertOnEdge(texelFetch(mcTableTexture, int(tablePos)).x);
        tablePos++;
        placeVertOnEdge(texelFetch(mcTableTexture, int(tablePos)).x);
        tablePos++;    
        EndPrimitive();        
    }
}


void main() {     
    buildMarchingCube();   
}

