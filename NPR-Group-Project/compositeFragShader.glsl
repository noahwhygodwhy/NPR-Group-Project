#version 460

out vec4 FragColor;
in vec2 fraguv;
uniform sampler2D kuwaharaTexture;
uniform sampler2D sobelTexture;
uniform sampler2D canvasTexture;

uniform float gaussianKernel[49];




//just a basic gaussian blur that samples from the sobel texture
vec3 gaussianSobel(){
	
	vec3 color = vec3(0);
	ivec2 maxSize = textureSize(sobelTexture, 0);

	for (int x = -3; x <= 3; x++) {
		for (int y = -3; y <= 3; y++) {
			ivec2 theCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(x, y), ivec2(0, 0), maxSize - ivec2(1.0f));
			color += gaussianKernel[((y+3) * 7) + (x+3)] * texelFetch(sobelTexture, theCoord, 0).xyz;
		
		}
	}
	return color;
}

void main()
{

	ivec2 mainImageSize = textureSize(kuwaharaTexture, 0);
	ivec2 canvasImageSize = textureSize(canvasTexture, 0);
	vec2 ratio = vec2(mainImageSize)/vec2(canvasImageSize);


	//the sampled color of the kuwahara stage
	vec3 kuwaColor = texelFetch(kuwaharaTexture, ivec2(gl_FragCoord.xy), 0).xyz;

	//a blured sobel value
	vec3 borderColor = gaussianSobel();

	//adding in extra value around the edges to make it look like paint strokes
	vec3 color = (kuwaColor)+ (borderColor*kuwaColor*0.2);
	
	//the sampled color of the canvas image
	vec3 canvasColor = texture(canvasTexture, fraguv*ratio).xyz;

	//the final color
	FragColor = vec4(color*canvasColor, 1.0);

}	