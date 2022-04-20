#version 460

out vec4 FragColor;

uniform sampler2D colorTexture;

in vec2 fraguv;

float avgRGB(vec3 i) {
    return (i.r + i.g + i.b)/3.0;
}

mat3 ka = mat3(1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);
mat3 kb = mat3(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0);


//sobel operator to detect edges. uses abs() to disreguard image flow
void main() 
{

    ivec2 maxSize = textureSize(colorTexture, 0);


    float sA = 0;
    float sB = 0;

    int x = 0;
    int y = 0;
    

    ivec2 coord = ivec2(gl_FragCoord.xy);


    mat3 gradiants;



    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++)
        {
            ivec2 coord = clamp(ivec2(gl_FragCoord.xy)+ivec2(x, y), ivec2(0, 0), maxSize-ivec2(1.0));//maxSize-ivec2(1.0)

            float theColor = avgRGB(texelFetch(colorTexture, coord, 0).xyz);
            sA += theColor * ka[x+1][y+1];
            sB += theColor * kb[x+1][y+1];
        }
    }


    FragColor = vec4(vec3(abs(sA+sB)), 1.0);
    //FragColor = vec4(vec3(clamp(sA+sB, 0.0, 1.0)), 1.0);
}