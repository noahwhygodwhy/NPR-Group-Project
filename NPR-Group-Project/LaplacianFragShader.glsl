#version 460

out vec4 FragColor;

in vec2 fraguv;

uniform sampler2D colorTexture;



void main()
{

	vec4 asdf = texelFetch(colorTexture, ivec2(gl_FragCoord.xy), 0);

	float kernel[9] = float[9](
		-1.0f, -1.0f, -1.0f, 
		-1.0f, 8.0f, -1.0f, 
		-1.0f, -1.0f, -1.0f
	);

	vec3 color = vec3(0);
	ivec2 maxSize = textureSize(colorTexture, 0);

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			ivec2 theCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(x, y), ivec2(0, 0), maxSize - ivec2(1.0f));
			color += kernel[((y + 1) * 3) + (x + 1)] * texelFetch(colorTexture, theCoord, 0).xyz;

		}
	}



	FragColor = vec4(color, 1.0f);
	//FragColor = texture(colorTexture, fraguv);
}