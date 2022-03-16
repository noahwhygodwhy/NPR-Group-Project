#version 460

out vec4 FragColor;


//uniform sampler2D colorTexture;

in vec2 fraguv;

void main() 
{
    FragColor = vec4(fraguv, 0.0, 1.0);
    //FragColor = vec4(fraguv, 0.0, 1.0);//texture(colorTexture, fraguv);
}