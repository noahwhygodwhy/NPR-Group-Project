#version 460

out vec4 FragColor;
in vec2 fraguv;
uniform sampler2D colorTexture;

void main()
{
	FragColor = vec4(texture(colorTexture, fraguv*vec2(1, -1)).xyz, 1.0f);
}	