#version 460

in vec2 fraguv;
//out vec4 FragColor;
out vec4 pointA;
out vec4 pointB;
out vec4 pointC;

in vec2 possiblePoint;

uniform sampler2D stage1Texture;
uniform sampler2D stage2Texture;
uniform sampler2D stage3Texture;

//uniform sampler2D colorTexture;

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


float signage (vec2 p1, vec2 p2, vec2 p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool inTriangle (vec2 pt, vec2 v1, vec2 v2, vec2 v3)
{
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = signage(pt, v1, v2);
    d2 = signage(pt, v2, v3);
    d3 = signage(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

bool isObtuse(vec2 considered, vec2 otherA, vec2 otherB){
    vec2 vectorA = otherA-considered;
    vec2 vectorB = otherB-considered;
    return dot(vectorA, vectorB) >= 0.0;
}

void perp(vec2 p1, vec2 p2, out vec2 p, out vec2 v) {
    v = p2 - p1;
    v = vec2(-v.y, v.x);
    p = (p1+p2)*0.5;
}
vec2 getCenter(vec2 q1, vec2 q2, vec2 q3) {
    vec2 p1, p2;
    vec2 v1, v2;
    perp(q1, q2, p1, v1);
    perp(q1, q3, p2, v2);
    float delta = v1.x * v2.y - v1.y * v2.x;
    float t1 = (v2.y * (p2.x - p1.x) - v2.x * (p2.y - p1.y)) / delta;
    return p1 + t1 * v1;
}
bool sameSide(vec2 lineA, vec2 lineB, vec2 p1, vec2 p2) {
    return ((lineA.y-lineB.y)*(p1.x-lineA.x)+(lineB.x-lineA.x)*(p1.y-lineA.y)) *
    ((lineA.y-lineB.y)*(p2.x-lineA.x) + (lineB.x - lineA.x)*(p2.y-lineA.y)) >0;
}

void main() 
{    
    vec2 stage1Point = texelFetch(stage1Texture, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 stage2Point = texelFetch(stage2Texture, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 stage3Point = texelFetch(stage3Texture, ivec2(gl_FragCoord.xy), 0).xy;


    /*if(stage1Point==stage2Point || stage2Point==stage3Point || stage3Point==stage1Point) {
        //FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        return;
    }*/

    vec2 centro = (stage1Point+stage2Point+stage3Point)/3.0;

    vec3 color = vec3(rand(centro), rand(centro+0.23), rand(centro+0.62));
    
    //FragColor = vec4(color, 1.0);

    bool isIn = inTriangle(fraguv, stage1Point, stage2Point, stage3Point);

    
    pointA = vec4(stage1Point, 0.0, 1.0);
    pointB = vec4(stage2Point, 0.0, 1.0);
    pointC = vec4(stage3Point, 0.0, 1.0);

    if(isIn) {        
        pointA = vec4(stage1Point, 0.0, 1.0);
        pointB = vec4(stage2Point, 0.0, 1.0);
        pointC = vec4(stage3Point, 0.0, 1.0);
        return;
    }


    vec2 newPointA;
    vec2 newPointB;





    if(isObtuse(stage1Point, stage2Point, stage3Point)) {
        newPointA = stage2Point;
        newPointB = stage3Point;
    }
    else if(isObtuse(stage2Point, stage1Point, stage3Point)) {
        newPointA = stage1Point;
        newPointB = stage3Point;
    } 
    else if(isObtuse(stage3Point, stage2Point, stage1Point)) {
        newPointA = stage2Point;
        newPointB = stage1Point;
    } 

    
    vec2 circumCenter = getCenter(newPointA, newPointB, possiblePoint);
    bool samsies = sameSide(newPointA, newPointB, fraguv, circumCenter);
    if(!samsies){
        discard;
    }
    float dist = distance(possiblePoint, circumCenter);
    gl_FragDepth = dist;
        
    pointA = vec4(newPointA, 0.0, 1.0);
    pointB = vec4(newPointB, 0.0, 1.0);
    pointC = vec4(possiblePoint, 0.0, 1.0);
    return;

    //vec2 cent = (stage1Point+stage2Point+stage3Point)/3.0;


    //FragColor = texture(colorTexture, cent);//vec4(cent, 0.0, 1.0);
}