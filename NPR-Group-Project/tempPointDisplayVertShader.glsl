#version 460

uniform sampler2D pointTexture;
uniform int width;
uniform int height;
uniform mat4 projection;

void main()
{

    vec2 bias = vec2(1.0/(float(width*2.0)), 1.0/(float(height*2.0)));

    float u = fract(float(gl_VertexID)/float(width));
    float v = (gl_VertexID%width)/height;

    vec2 aPos = texture(pointTexture, vec2(u, v)+bias).xy;

    gl_Position = projection*vec4(aPos, 0.0f, 1.0f);
}