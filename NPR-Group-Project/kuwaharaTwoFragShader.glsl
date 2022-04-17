#version 460

out vec4 FragColor;

in vec2 fraguv;

uniform int radius;

#define SECTORS = 8;
#define M_PI 3.1415926535897932384626433832795

uniform sampler2D colorTexture;

float rand(inout vec2 co){
    float toReturn = fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
    co += vec2(toReturn);
    return toReturn;
}

//TODO: these might be bakcwards
mat3 kX = mat3(1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);
mat3 kY = mat3(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0);


float avgRGB(vec3 i) {
    return (i.r + i.g + i.b)/3.0;
}


////////gets the angle of the local flow as an angle between 0 and 2pi
vec2 getAngle(){
    ivec2 maxSize = textureSize(colorTexture, 0);

    float sX;
    float sY;

    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++)
        {
            ivec2 coord = clamp(ivec2(gl_FragCoord.xy)+ivec2(x, y), ivec2(0, 0), maxSize-ivec2(1.0));//maxSize-ivec2(1.0)

            float theColor = avgRGB(texelFetch(colorTexture, coord, 0).xyz);
            sX += theColor * kX[x+1][y+1];
            sY += theColor * kY[x+1][y+1];
        }
    }
    //float angle = atan(sX / sY);
    //if (angle<0) angle += 2*M_PI;
    return vec2(sX, sY);
}


void main(){
    vec3 sectorMeans[8];
    float sectorVariances[8];

    vec2 center = gl_FragCoord.xy;
    


    vec2 angle = getAngle();
    vec2 longBoi = angle;

    vec2 up = normalize(angle);
    up = vec2(-up.y, up.x);

    vec2 A = center + (longBoi*radius);
    vec2 B = center - (longBoi*radius);

    vec2 minBounding = min(A, B);
    vec2 maxBounding = max(A, B);

      








}