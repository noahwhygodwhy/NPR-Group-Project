#version 460

in vec2 fraguv;
in vec2 thePoint;

out vec4 FragColor;

uniform sampler2D colorTexture;

void main() 
{
    FragColor = texture(colorTexture, thePoint); 


    FragColor = vec4(thePoint, 0.2, 1.0);
    gl_FragDepth = distance(fraguv, thePoint);

}