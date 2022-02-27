#version 460

in vec2 fraguv;
in vec2 thePoint;

out vec4 FragColor;

uniform sampler2D colorTexture;

void main() 
{
    FragColor = texture(colorTexture, thePoint); 
    gl_FragDepth = distance(fraguv, thePoint);

}