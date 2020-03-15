#version 430
layout (local_size_x = 4, local_size_y = 4, local_size_z = 4) in;
layout (r8, binding = 0) uniform image3D tex_output;

void main()
{
    ivec3 dims = imageSize (tex_output);
    ivec3 pixel_coords = ivec3(gl_GlobalInvocationID.xyz);

    //vec4 pixel = vec4(float(pixel_coords.x)/dims.x, pixel_coords.y/dims.y, pixel_coords.y/dims.y, 1.0);
    vec3 pos = vec3(float(pixel_coords.x)/dims.x,float(pixel_coords.y)/dims.y, float(pixel_coords.z)/dims.z);
    
    vec2[] pillars = vec2[](vec2(0.333,0.33), vec2(0.66, 0.33), vec2(0.5,0.66));
    
    float density = 0;
    
    //Pillars
    for(int i = 0;i < pillars.length(); ++i)
    {
        density += 0.15 / length(pos.xy - pillars[i].xy) - 1.0;
        //density += clamp(((0.15 / length(pos.xy - vec2(pillars[i].x + 0.1 * sin(10 * pos.z), pillars[i].y + 0.1 * cos(10 * pos.z)))) - 1) * 499999.0, 0.0, 1.0);        
    }
    
    //Negative Pillar
    density -= 0.7 / length(pos.xy) - 1.0;
    
    //Outside
    density -= pow(length(pos.xy), 3);
    
    //Helix
    vec2 vec = vec2(cos(pos.z * 3.0), sin(pos.z * 3.0));
    density += dot(vec, pos.xy);
    
    //Shelfs
    density += cos(pos.z * 20.0);
    
    //Noise
    
    vec4 pixel = vec4(clamp(density * 499999.0, 0.0, 1.0), 0, 0, 1);

    imageStore (tex_output, pixel_coords, pixel);
}