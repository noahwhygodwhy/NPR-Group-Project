#version 460

out vec4 FragColor;

in vec2 fraguv;




uniform sampler2D colorTexture;


#define GAUS_STD 2.0f
#define M_PI 3.1415926535897932384626433832795

void main()
{

	ivec2 texMin = ivec2(0, 0);
	ivec2 texMax = textureSize(colorTexture, 0)-ivec2(1, 1);

	ivec2 topCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(0, 1), texMin, texMax);
	ivec2 botCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(0, -1), texMin, texMax);
	ivec2 rightCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(1, 0), texMin, texMax);
	ivec2 leftCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(-1, 0), texMin, texMax);

	vec3 topColor = texelFetch(colorTexture, topCoord, 0).xyz;
	vec3 botColor = texelFetch(colorTexture, botCoord, 0).xyz;
	vec3 rightColor = texelFetch(colorTexture, rightCoord, 0).xyz;
	vec3 leftColor = texelFetch(colorTexture, leftCoord, 0).xyz;

	

	vec3 vdX = rightColor - leftColor;
	vec3 vdY = topColor - botColor;

	float dX = (vdX.x + vdX.y + vdX.z) / 3.0f;
	float dY = (vdY.x + vdY.y + vdY.z) / 3.0f;



	FragColor = vec4(dX, dY, 0.0f, 1.0f);


}