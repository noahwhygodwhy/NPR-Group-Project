#version 460

out vec4 FragColor;

in vec2 fraguv;

uniform sampler2D colorTexture;
uniform sampler2D derivativeTexture;



float radius = 2.0f;


//http://umsl.edu/cmpsci/about/People/Faculty/HenryKang/coon.pdf section 2.1

float ws(ivec2 x, ivec2 y) {
	return float(int(length(x - y) < radius));
}

float wm(vec2 gx, vec2 gy) {
	float weirdN = 1.0f;

	return 0.5f * (1.0f + tanh(weirdN * (length(gy) - length(gx))));
}
float wd(vec2 tx, vec2 ty) {
	return dot(tx, ty);
}


float phi(vec2 tx, vec2 ty) {
	if (dot(tx, ty) > 0) {
		return 1.0f;
	}
	return -1.0f;
}


void main()
{




	ivec2 maxSize = textureSize(colorTexture, 0);
	ivec2 currCoord = ivec2(gl_FragCoord.xy);

	int numberOfSamples = 0;
	vec2 result = vec2(0);


	for (int x = -int(radius); x <= int(radius); x++) {
		for (int y = -int(radius); y <= int(radius); y++) {

			ivec2 theCoord = clamp(ivec2(gl_FragCoord.xy) + ivec2(x, y), ivec2(0, 0), maxSize - ivec2(1.0f));

			vec2 gX = texelFetch(derivativeTexture, currCoord, 0).xy;
			vec2 gY = texelFetch(derivativeTexture, theCoord, 0).xy;

			vec2 tX = vec2(-gX.y, gX.x);
			vec2 tY = vec2(-gY.y, gY.x);

			float phiVal = phi(tX, tY);
			float wdVal = wd(tX, tY);
			float wsVal = ws(currCoord, theCoord);
			float wmVal = wm(gX, gY);

			if (wsVal > 0) {
				numberOfSamples++;
				result += phiVal * tY * wsVal * wmVal * wdVal;
			}
		}
	}



	FragColor = vec4(result/float(numberOfSamples), 0.0, 1.0f);
	//FragColor = texture(colorTexture, fraguv);
}