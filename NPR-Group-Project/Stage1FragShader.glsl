#version 460

in vec2 fraguv;
in vec2 thePoint;
out vec4 FragColor;

uniform sampler2D pointTexture;

void main() 
{
    FragColor = vec4(thePoint, 0.0, 1.0);
    gl_FragDepth = distance(fraguv, thePoint);
}