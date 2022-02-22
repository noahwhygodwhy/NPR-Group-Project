#version 460

in vec3 fragNormal;

//uniform vec3 eyePos;

in vec2 fragUV;
out vec4 FragColor;

uniform sampler2D diffuseSampler;
uniform sampler2D specularSampler;

void main() 
{

    /*float value = distance(gl_FragCoord.zyx, eyePos);
    
    float scaledValue = (value/1500.0f);
    
    float near = 0.1; 
    float far  = 10.0; 

    //float depth = gl_FragCoord.z;
    float ndc = gl_FragCoord.z * 2.0 - 1.0; // back to NDC 
    float depth = (2.0 * near * far) / (far + near - ndc * (far - near));	
    float ratio = 1.0f-(depth/far)
    */
    FragColor = texture(diffuseSampler, fragUV);//vec4(1.0f, 0.0f, 1.0f, 1.0f);//vec4(vec3(ratio), 1.0f);
}