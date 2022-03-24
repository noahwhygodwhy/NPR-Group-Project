#version 460


out vec4 FragColor;
in vec3 fragNormal;

uniform vec3 eyePos;



uniform float near;
uniform float far;



float linearlizeDepth(float depth) { 

    //float depth = z * 2.0 - 1.0; // back to NDC 
    return (2.0f*near*far) / (far+near-(depth*2.0f-1.0f)*(far-near));
}



void main() 
{
    float camDepth = linearlizeDepth(gl_FragCoord.z)/far;
    FragColor = vec4(vec3(camDepth), 1.0f);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);


}
