#version 460

in vec2 fraguv;

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D colorTexture;

uniform vec2 screenResolution;

out vec4 FragColor;

float avgRGB(vec3 i) {
    return (i.r + i.g + i.b)/3.0;
}

mat3 ka = mat3(1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);
mat3 kb = mat3(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0);


vec2 offset = vec2(1.0f)/screenResolution;

vec2 numchar[8] = {
    vec2(-offset.x, -offset.y),
    vec2(0.0f, -offset.y),
    vec2(offset.x, -offset.y),
    vec2(-offset.x, 0.0f),
    vec2(offset.x, 0.0f),
    vec2(-offset.x, offset.y),
    vec2(0.0f, offset.y),
    vec2(offset.x, offset.y)
};

//vec2 numChar[8]

mat3 kernal = {
    vec3(1.0f, 1.0f, 1.0f),
    vec3(1.0f, 0.0f, 1.0f),
    vec3(1.0f, 1.0f, 1.0f)
    };

void main() 
{
    float normalVal = 0;
    float depthVal = 0;
    float sobelVal = 0;
    ivec2 maxSize = textureSize(normalTexture, 0);



    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
        
            ivec2 coord = clamp(ivec2(gl_FragCoord.xy)+ivec2(x, y), ivec2(0, 0), maxSize-ivec2(1.0));//maxSize-ivec2(1.0)
            float theColor = avgRGB(texelFetch(colorTexture, coord, 0).xyz);
            float sA = theColor * ka[x+1][y+1];
            float sB = theColor * kb[x+1][y+1];

            sobelVal += sA+sB;

            depthVal += abs(texelFetch(depthTexture, ivec2(gl_FragCoord.xy), 0).x-texelFetch(depthTexture, coord, 0).x);
            normalVal += distance(texelFetch(normalTexture, ivec2(gl_FragCoord.xy), 0).xyz, texelFetch(normalTexture, coord, 0).xyz);

        }
    }

    depthVal = depthVal / 8.0f;
    normalVal = normalVal / 8.0f;

    if(normalVal > 0.4f || depthVal > 0.02f || sobelVal > 0.1) {
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    } else {
    //FragColor = vec4(vec3(normalVal + (depthVal*75.0)), 1.0);
        discard;
    }

}