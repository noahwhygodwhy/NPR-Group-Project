#version 460

out vec4 FragColor;
in vec2 fraguv;
uniform sampler2D kuwaharaTexture;
uniform sampler2D laplacianTexture;

uniform float gaussianKernel[49];




vec3 gaussianSobel(){
	
	vec3 color = vec3(0);
	ivec2 maxSize = textureSize(laplacianTexture, 0);

	for (int x = -3; x <= 3; x++) {
		for (int y = -3; y <= 3; y++) {
			ivec2 theCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(x, y), ivec2(0, 0), maxSize - ivec2(1.0f));
			color += gaussianKernel[((y+3) * 7) + (x+3)] * texelFetch(laplacianTexture, theCoord, 0).xyz;
		
		}
	}
	return color;
}

void main()
{

	ivec2 coord = ivec2(gl_FragCoord.xy);




	vec3 kuwaColor = texelFetch(kuwaharaTexture, coord, 0).xyz;
	
	vec3 borderColor = gaussianSobel();

	vec3 color = (kuwaColor*0.9)+ (borderColor*kuwaColor);

	FragColor = vec4(color, 1.0);

//	FragColor = vec4(texture(colorTexture, fraguv*vec2(1, -1)).xyz, 1.0f);
//	
//	FragColor = vec4(texture(colorTexture, fraguv*vec2(1, -1)).xyz, 1.0f);
}	