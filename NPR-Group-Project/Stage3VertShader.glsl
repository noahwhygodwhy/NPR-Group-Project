#version 460

layout (location = 0) in vec2 aPos;


uniform sampler2D pointTexture;

out vec2 possiblePoint;
out vec2 fraguv;

uniform mat4 projection;

void main()
{
    possiblePoint = texelFetch(pointTexture, ivec2(gl_InstanceID, 0), 0).xy;
    fraguv = aPos;
    gl_Position = projection*vec4(aPos, 0.0f, 1.0f);
}