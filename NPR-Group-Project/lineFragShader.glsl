#version 460

in vec2 fraguv;

uniform sampler2D normalTexture;
uniform sampler2D depthTexture;
uniform sampler2D colorTexture;

uniform vec2 screenResolution;

out vec4 FragColor;



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

mat3 kernal = {
    vec3(1.0f, 1.0f, 1.0f),
    vec3(1.0f, 0.0f, 1.0f),
    vec3(1.0f, 1.0f, 1.0f)
    };

void main() 
{
    float normalVal = 0;
    float depthVal = 0;

    /*for(int i = 0; i < 8; i++) {
        depthVal = max(abs(texture(depthTexture, fraguv).x-texture(depthTexture, fraguv+numchar[i]).x), depthVal);
        normalVal = max(distance(texture(normalTexture, fraguv).xyz, texture(normalTexture, fraguv+numchar[i]).xyz), normalVal);
    }
    depthVal = depthVal;
    normalVal = normalVal;*/

    
    for(int i = 0; i < 8; i++) {
        depthVal += abs(texture(depthTexture, fraguv).x-texture(depthTexture, fraguv+numchar[i]).x);
        normalVal += distance(texture(normalTexture, fraguv).xyz, texture(normalTexture, fraguv+numchar[i]).xyz);
    }
    depthVal = depthVal / 8.0f;
    normalVal = normalVal / 8.0f;

    //FragColor = vec4(1.0, 1.0f, 1.0f, 1.0f);
    
    if(normalVal > 0.3f || depthVal > 0.01f) {
        FragColor = vec4(1.0f, 1.0f, 1.0f,1.0f);
    } else {
        discard;
    }

}