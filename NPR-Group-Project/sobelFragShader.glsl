#version 460

out vec4 FragColor;

uniform sampler2D colorTexture;

in vec2 fraguv;

float avgRGB(vec3 i) {
    return (i.r + i.g + i.b)/3.0;
}

mat3 ka = mat3(1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);
mat3 kb = mat3(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0);

void main() 
{

    ivec2 maxSize = textureSize(colorTexture, 0) - ivec2(1.0);


    float sA = 0;
    float sB = 0;

    for(int channel = 0; channel < 3; channel++) {
    
    }
    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++)
        {
            ivec2 coord = clamp(ivec2(gl_FragCoord.xy)+ivec2(x, y), ivec2(0), maxSize-ivec2(1.0));

            float theColor = avgRGB(texelFetch(colorTexture, coord, 0).xyz);
            
            //FragColor = vec4(theColor, 1.0);

            sA = theColor * ka[x][y];
            sB = theColor * kb[x][y];

        }
    }
    FragColor = vec4(vec3(sA+sB)/2.0, 1.0);
}