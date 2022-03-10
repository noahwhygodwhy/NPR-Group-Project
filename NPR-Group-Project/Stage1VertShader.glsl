#version 460

layout (location = 0) in vec2 aPos;

//out vec2 fraguv;
uniform mat4 projection;
out vec2 fraguv;
out vec2 thePoint;
uniform int width;
uniform sampler2D pointTexture;


void main()
{
    int x = gl_InstanceID % width;
    int y = gl_InstanceID / width;

    thePoint = texelFetch(pointTexture, ivec2(x, y), 0).xy;
    fraguv = aPos;
    gl_Position = projection*vec4(aPos, 0.0f, 1.0f);
}