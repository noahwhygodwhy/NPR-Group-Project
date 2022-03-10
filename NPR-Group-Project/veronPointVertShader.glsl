#version 460

layout (location = 0) in vec2 aPos;

//out vec2 fraguv;
uniform mat4 projection;


void main()
{

   // fraguv = aPos;

    gl_Position = projection*vec4(aPos, 0.0f, 1.0f);
}



