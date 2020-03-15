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


out vec3 fColor;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform sampler3D densityTexture;
uniform isamplerBuffer mcTableTexture;



void buildHouse(vec4 position)
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

void buildHouseS(vec4 position)
{    
    float scale = 0.5;
    vec4 worldPos = inverse(model) * inverse(view) * inverse(projection) * position;
    //fColor = (texelFetch(mcTableTexture, 34)).xxx; 
    //fColor = vec3(1,1,1);
    //fColor = vec3(texture(densityTexture, vec3(worldPos.x / 96, worldPos.y / 96, worldPos.z / 256)).rrr); 
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

void placeVertOnEdge(uint edgeNum)
{
    // Along this cell edge, where does the density value hit zero?
    // float str0= dot(cornerAmask0123[edgeNum], input.field0123) + dot(cornerAmask4567[edgeNum], input.field4567);
    // float str1= dot(cornerBmask0123[edgeNum], input.field0123) + dot(cornerBmask4567[edgeNum], input.field4567);
    // float t= saturate( str0/(str0 -str1) ); //0..1
    // use that to get wsCoordand uvwcoords

    vec3 pos_within_cell = vec3(0,0,0); //vec_start[edgeNum] + t * vec_dir[edgeNum]; //[0..1]
    vec3 vecWsCoord= gs_in[0].wsCoord.xyz + pos_within_cell.xyz;// * wsVoxelSize;
    gl_Position = vec4(vecWsCoord, 1);
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
    
    uint tablePos = gs_in[0].mc_case * 16;
    
    if(texelFetch(mcTableTexture, int(tablePos)).x != -1){
    
        placeVertOnEdge(uint(texelFetch(mcTableTexture, int(tablePos)).x));
        tablePos++;
        placeVertOnEdge(uint(texelFetch(mcTableTexture, int(tablePos)).x));
        tablePos++;
        placeVertOnEdge(uint(texelFetch(mcTableTexture, int(tablePos)).x));
        tablePos++;    
        EndPrimitive();        
    }
}


void main() {     
    buildMarchingCube();   
    if(gs_in[0].mc_case != 0){        
       buildHouseS(gl_in[0].gl_Position);
    }
}

