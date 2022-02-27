#version 460

//uniform vec3 color;

//uniform mat4 model;

//uniform sampler2D texture_diffuse;
//uniform sampler2D texture_specular;

in vec3 fragNormal;

uniform vec3 eyePos;

out vec4 FragColor;

void main() 
{
    
    float value = distance(gl_FragCoord.zyx, eyePos);
    
    float scaledValue = (value/1500.0f);
    
    float near = 0.1; 
    float far  = 10.0; 

    //float depth = gl_FragCoord.z;
    float ndc = gl_FragCoord.z * 2.0 - 1.0; // back to NDC 
    float depth = (2.0 * near * far) / (far + near - ndc * (far - near));	
    float ratio = 1.0f-(depth/far);



    FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);//vec4(vec3(ratio), 1.0f);
}