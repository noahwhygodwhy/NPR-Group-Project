#version 460

out vec4 FragColor;

in vec2 fraguv;

uniform float gaussianKernel[49];

uniform sampler2D colorTexture;

//this takes a etxture, and a gauusian kernel, and outputs the blurred version

void main()
{

	vec3 color = vec3(0);
	ivec2 maxSize = textureSize(colorTexture, 0);

	for (int x = -3; x <= 3; x++) {
		for (int y = -3; y <= 3; y++) {
			ivec2 theCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(x, y), ivec2(0, 0), maxSize - ivec2(1.0f));
			color += gaussianKernel[((y+3) * 7) + (x+3)] * texelFetch(colorTexture, theCoord, 0).xyz;
		
		}
	}



	FragColor = vec4(color, 1.0f);
}