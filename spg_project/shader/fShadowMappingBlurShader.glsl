#version 420 core 
layout (location = 0) out vec4 FragColor;

in vec2 texCoords;


/////////////////////////////////////////////////
// 7x1 gaussian blur fragment shader
/////////////////////////////////////////////////


uniform vec3 direction;
uniform vec3 resolution;
layout(binding = 0) uniform sampler2D textureSource;

vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction) {
	vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.411764705882353) * direction;
  vec2 off2 = vec2(3.2941176470588234) * direction;
  vec2 off3 = vec2(5.176470588235294) * direction;
  color += texture2D(image, uv) * 0.1964825501511404;
  color += texture2D(image, uv + (off1 / resolution)) * 0.2969069646728344;
  color += texture2D(image, uv - (off1 / resolution)) * 0.2969069646728344;
  color += texture2D(image, uv + (off2 / resolution)) * 0.09447039785044732;
  color += texture2D(image, uv - (off2 / resolution)) * 0.09447039785044732;
  color += texture2D(image, uv + (off3 / resolution)) * 0.010381362401148057;
  color += texture2D(image, uv - (off3 / resolution)) * 0.010381362401148057;
  return color;
}

void main()
{
	vec4 color = vec4(0.0);

	// color += texture2D( textureSource, texCoords.st + vec2( -3.0*ScaleU.x, -3.0*ScaleU.y ) ) * 0.015625;
	// color += texture2D( textureSource, texCoords.st + vec2( -2.0*ScaleU.x, -2.0*ScaleU.y ) ) * 0.09375;
	// color += texture2D( textureSource, texCoords.st + vec2( -1.0*ScaleU.x, -1.0*ScaleU.y ) ) * 0.234375;
	// color += texture2D( textureSource, texCoords.st + vec2( 0.0 , 0.0) )*0.3125;
	// color += texture2D( textureSource, texCoords.st + vec2( 1.0*ScaleU.x,  1.0*ScaleU.y ) ) * 0.234375;
	// color += texture2D( textureSource, texCoords.st + vec2( 2.0*ScaleU.x,  2.0*ScaleU.y ) ) * 0.09375;
	// color += texture2D( textureSource, texCoords.st + vec2( 3.0*ScaleU.x, -3.0*ScaleU.y ) ) * 0.015625;

	FragColor = blur13(textureSource, texCoords, resolution.xy, direction.xy);
}

