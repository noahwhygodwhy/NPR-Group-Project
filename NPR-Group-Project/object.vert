#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec3 aBitangent;
layout (location = 4) in vec2 aUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

out vec3 fragNormal;
out vec2 fragUV;

//uniform sampler2D texture_normal;

void main()
{
fragUV = aUV;
fragNormal = normalize((normalMatrix*vec4(aNormal, 1.0f)).xyz);
gl_Position = projection*view*model*vec4(aPos, 1.0);
}