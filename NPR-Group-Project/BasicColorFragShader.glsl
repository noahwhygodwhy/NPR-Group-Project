#version 460


//this just outputs the texture without any modification

out vec4 FragColor;
in vec2 fraguv;
uniform sampler2D colorTexture;

void main()
{
	FragColor = vec4(texture(colorTexture, fraguv).xyz, 1.0f);
}	