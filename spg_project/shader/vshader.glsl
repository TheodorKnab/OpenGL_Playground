#version 410 core
layout (location = 0) in vec2 aPos;
//layout (location = 1) in vec2 aTexCoords;

out int instanceID;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 densityTextureDimensions;

out vec2 texCoords;

out VS_OUT {
    vec3 wsCoord;
    vec3 uvw;
    vec4 f0123;
    vec4 f4567;
    uint mc_case;
} vs_out;

uniform sampler3D densityTexture;


void main()
{
    //instanceID = gl_InstanceID;   
    vs_out.wsCoord = vec3(aPos.x, aPos.y, gl_InstanceID);
    vs_out.uvw = vec3(vs_out.wsCoord.x / densityTextureDimensions.x, vs_out.wsCoord.y / densityTextureDimensions.y,  vs_out.wsCoord.z / densityTextureDimensions.z);

    vec3 step = vec3(1.0/densityTextureDimensions.x, 1.0/densityTextureDimensions.y, 1.0 / densityTextureDimensions.z);
    
    vs_out.f0123 = vec4( texture(densityTexture, vs_out.uvw + step.yyy).x,
    texture(densityTexture, vs_out.uvw + step.yyx).x,
    texture(densityTexture, vs_out.uvw + step.xyx).x,
    texture(densityTexture, vs_out.uvw + step.xyy).x );
    vs_out.f4567 = vec4( texture(densityTexture, vs_out.uvw + step.yxy).x,
    texture(densityTexture, vs_out.uvw + step.yxx).x,
    texture(densityTexture, vs_out.uvw + step.xxx).x,
    texture(densityTexture, vs_out.uvw + step.xxy).x );

    // determine which of the 256 marching cubes cases we have forthis cell:
    uvec4 n0123 = uvec4(clamp(vs_out.f0123*99999, 0.0, 1.0));
    uvec4 n4567 = uvec4(clamp(vs_out.f4567*99999, 0.0, 1.0));
    vs_out.mc_case = (n0123.x) | (n0123.y << 1) | (n0123.z << 2) | (n0123.w << 3) | (n4567.x << 4) | (n4567.y << 5) | (n4567.z << 6) | (n4567.w << 7) ;
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, gl_InstanceID, 1.0); 
    // fill out return structusing these values, then on to the Geometry Shader.
    //texCoords = aTexCoords;
}