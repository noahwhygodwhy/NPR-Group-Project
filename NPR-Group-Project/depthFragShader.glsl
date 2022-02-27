#version 460

//uniform vec3 color;

//uniform mat4 model;

//uniform sampler2D texture_diffuse;
//uniform sampler2D texture_specular;

in vec3 fragNormal;

uniform vec3 eyePos;


//uniform float near;
//uniform float far;

out vec4 FragColor;

uniform float near;
uniform float far;
uniform float modelNear;
uniform float modelFar;




float linearlizeDepth(float depth) { 

    //float depth = z * 2.0 - 1.0; // back to NDC 
    return (2.0f*near*far) / (far+near-(depth*2.0f-1.0f)*(far-near));
}


//float linearlizeDepth(float depth) {
//    return (2.0*near*far)/(far+near-(depth*2.0f-1.0f)*far-near));
//}


void main() 
{
    float camDepth = linearlizeDepth(gl_FragCoord.z)/far;
    
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0f);
    FragColor = vec4(vec3(camDepth), 1.0f);


}