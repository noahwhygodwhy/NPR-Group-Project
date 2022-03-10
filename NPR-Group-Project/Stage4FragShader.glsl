#version 460

in vec2 fraguv;
out vec4 FragColor;
//out vec4 pointA;
//out vec4 pointB;
//out vec4 pointC;

uniform sampler2D stage1Texture;
uniform sampler2D stage2Texture;
uniform sampler2D stage3Texture;

uniform sampler2D colorTexture;

/*float signage(vec2 a, vec2 b, vec2 p)
{
    return (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
}

bool inTriangle(vec2 a, vec2 b, vec2 c, vec2 point)
{
    float x = signage(point, a, b);
    float y = signage(point, b, c);
    float z = signage(point, c, a);
    if (x == 0 || y == 0 || z == 0)
    {
        return false;
    }
    bool neg = x < 0 || y < 0 || z < 0;
    bool pos = x > 0 || y > 0 || z > 0;

    return !(pos && neg); //not all of them, and not none of them
}*/

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

void main() 
{    
    vec2 stage1Point = texelFetch(stage1Texture, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 stage2Point = texelFetch(stage2Texture, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 stage3Point = texelFetch(stage3Texture, ivec2(gl_FragCoord.xy), 0).xy;
    
    //bool isIn = inTriangle(fraguv, stage1Point, stage2Point, stage3Point);

    //if(isIn) {
        //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
        //return;
        //vec2 cent = (stage1Point+stage2Point+stage3Point)/3.0;


        //FragColor = texture(colorTexture, cent);
        
        //pointA = vec4(stage1Point, 0.0, 1.0);
        //pointB = vec4(stage2Point, 0.0, 1.0);
        //pointC = vec4(stage3Point, 0.0, 1.0);
        //return;
    //}
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    /*if(isObtuse(stage1Point, stage2Point, stage3Point)) {
    
        
    
    }*/

    
    vec2 cent = (stage1Point+stage2Point+stage3Point)/3.0;


    FragColor = texture(colorTexture, cent);//vec4(cent, 0.0, 1.0);
}