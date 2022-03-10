#version 460

in vec2 fraguv;
in vec2 possiblePoint;

out vec4 FragColor;

uniform sampler2D stage1Texture;


void main() 
{    
    vec2 stage1Point = texelFetch(stage1Texture, ivec2(gl_FragCoord.xy), 0).xy;

    vec2 c = possiblePoint-stage1Point;
    vec2 v = normalize(fraguv-stage1Point);

    if(dot(c, v)<=0.0) {
        discard;
    }
    float dist = (dot(c, c)/dot(c, v));
    gl_FragDepth = dist;
    FragColor = vec4(possiblePoint, 0.0, 1.0);
}