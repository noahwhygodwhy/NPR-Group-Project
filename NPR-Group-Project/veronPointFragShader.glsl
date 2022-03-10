#version 460

//in vec2 fraguv;

uniform int width;
uniform int height;

out vec4 FragColor;

uniform sampler2D veronoiTexture;

void main() 
{


    vec2 p = texelFetch(veronoiTexture, ivec2(gl_FragCoord), 0).xy;
    vec2 pX = texelFetch(veronoiTexture, ivec2(gl_FragCoord) + ivec2(1, 0), 0).xy;
    vec2 pY = texelFetch(veronoiTexture, ivec2(gl_FragCoord) + ivec2(0, 1), 0).xy;
    vec2 pXY = texelFetch(veronoiTexture, ivec2(gl_FragCoord)+ivec2(1, 1), 0).xy;
    
    bool top = gl_FragCoord.y == height-1;
    bool right = gl_FragCoord.x == width-1;
    
    int dups = 0;
    dups += int(p == pXY || pX == pY);
    dups *= 5;

    dups += int(!right && (p == pX));
    dups += int(!top && p == pY); 
    dups += int(!right && !top && p == pXY); 

    dups += int(!top && pX == pY); 
    dups += int(!right && !top && pX == pXY); 

    dups += int(!right && !top && pY == pXY); 

    int f = int(dups<2);




    //FragColor = vec4(1.0);

    /*if(int(gl_FragCoord.x)%2 == 0) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }*/


    FragColor = vec4(f, f, f, 1.0);
}