#version 460

//uniform vec3 color;

//uniform mat4 model;

//uniform sampler2D texture_diffuse;
//uniform sampler2D texture_specular;

in vec3 fragNormal;

uniform vec3 eyePos;

out vec4 FragColor;


int power = 2;
vec3 squareAndNormalize(vec3 vin) {
    return normalize(vec3(pow(vin.x, power), pow(vin.y, power), pow(vin.z, power)));
}

void main() 
{
    vec3 color = fragNormal;

    float maxColor = max(max(fragNormal.x, fragNormal.y), fragNormal.z);
    if(maxColor == fragNormal.x) {
        color = vec3(fragNormal.x, 0.0f, 0.0f);
    }
    if(maxColor == fragNormal.y) {
        color = vec3(0.0f, fragNormal.y, 0.0f);
    }
    if(maxColor == fragNormal.z) {
        color = vec3(0.0f, 0.0f, fragNormal.z);
    }

    //FragColor = vec4(color, 1.0f);
    vec3 mwn = squareAndNormalize(fragNormal);
    //FragColor = vec4(squareAndNormalize(fragNormal), 1.0f);
    
    FragColor = vec4(abs(mwn.x), abs(mwn.y), abs(mwn.z), 1.0f);
    //FragColor = vec4(abs(fragNormal.x), abs(fragNormal.y), abs(fragNormal.z), 1.0f);
}