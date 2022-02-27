#version 460

layout (location = 0) in vec2 aPos;

out vec2 fraguv;
out vec2 thePoint;

uniform mat4 projection;
uniform int width;
uniform int height;
uniform sampler2D pointTexture;

void main()
{


    vec2 bias = vec2(1.0/(float(width*2.0)), 1.0/(float(height*2.0)));
    float u = fract(float(gl_InstanceID)/float(width));
    float v = (gl_InstanceID%width)/height;
    thePoint = texture(pointTexture, vec2(u, v)+bias).xy;
    fraguv = aPos;



    gl_Position = projection*vec4(aPos, 0.0f, 1.0f);
}



