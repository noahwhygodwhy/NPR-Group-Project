#version 460

layout (location = 0) in vec2 aPos;

uniform mat4 projection;

out vec2 fraguv;
void main()
{
    fraguv = aPos;
    gl_Position = projection*vec4(aPos, 0.0f, 1.0f);
}