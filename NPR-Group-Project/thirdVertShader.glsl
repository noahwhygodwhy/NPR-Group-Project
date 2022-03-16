#version 460

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 cent;

uniform mat4 projection;

out vec2 fraguv;

void main()
{
    fraguv = cent;
    gl_Position = projection * vec4(aPos, 0.0f, 1.0f);
}