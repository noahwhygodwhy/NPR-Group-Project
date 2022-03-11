#version 460

//in vec2 fraguv;
//in vec2 thePoint;
out vec4 FragColor;
in vec2 possiblePoint;
in vec2 fraguv;

uniform sampler2D stage1Texture;
uniform sampler2D stage2Texture;


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


/*vec2 getCenter(vec2 p1, vec2 p2, vec2 p3)
{
    bool debug = false;

    float middleX = -1.0;
    float middleY = -1.0;

    vec2 midAB = (p1 + p2) / 2.0;
    vec2 midBC = (p2 + p3) / 2.0;
    
    float slopeAB = (p1.x - p2.x);
    float tangentAB = -1;

    if (slopeAB == 0)
    {
        tangentAB = 0;
    }
    else
    {   
        tangentAB = -(p1.y - p2.y) / slopeAB;
    }
   
    float yIntTanAB = -1.0;

    if (slopeAB == 0)
    {
        middleX = (p1.x+p2.x)/2;
    }
    else if (tangentAB == 0)
    {
        middleY = (p1.y + p2.y) / 2.0;
    }
    else
    {
        yIntTanAB = midAB.y - (tangentAB * midAB.x);
    }

    float slopeBC =  (p2.x - p3.x);
    float tangentBC = -1;
    //if the slope is infinity, the tangent is 0, else, calculate the tangent
    if (slopeBC == 0)
    {
        tangentBC = 0;
    }
    else
    {
        tangentBC = -(p2.y - p3.y) / slopeBC;
    }
    
    float yIntTanBC = -1;
    if (slopeBC == 0)
    {
        middleX = (p2.x + p3.x) / 2.0;
    }
    else if (tangentBC == 0)
    {
        middleY = (p2.y + p3.y) / 2.0;
    }
    else
    {
        yIntTanBC = midBC.y - (tangentBC * midBC.x);
    }

    if (middleX<0 && middleY<0)
    {
        middleX = (yIntTanBC - yIntTanAB) / (tangentAB - tangentBC);
        middleY = (tangentAB * middleX) + yIntTanAB;
    }
    else if (middleX<0)
    { 
        if (yIntTanAB<0 || tangentAB<0)
        {
            middleX = (middleY - yIntTanBC) / tangentBC;
        }
        else
        {
            middleX = (middleY - yIntTanAB) / tangentAB;
        }
    }
    else if (middleY<0)
    {
        if (yIntTanAB<0 || tangentAB<0)
        {
            middleY = (tangentBC * middleX) + yIntTanBC;
        }
        else
        {
            middleY = (tangentAB * middleX) + yIntTanAB;
        }
    }
    return vec2(middleX, middleY);
}*/


//math adapted from https://github.com/alicevision/geogram/blob/master/src/lib/geogram/basic/geometry.cpp
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

    FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    //TODO: this isn't correct
    //vec2 thePoint = texelFetch(pointTexture, ivec2(gl_FragCoord.xy), 0).xy;

    
    //vec4 stage1Stuff = texelFetch(stage1Texture, ivec2(gl_FragCoord.xy), 0);

    vec2 stage1Point = texelFetch(stage1Texture, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 stage2Point = texelFetch(stage2Texture, ivec2(gl_FragCoord.xy), 0).xy;
    


    if(possiblePoint == stage1Point || possiblePoint == stage2Point || stage1Point == stage2Point) {
        discard;
    }


    vec2 circumCenter = getCenter(stage1Point, stage2Point, possiblePoint);


    bool samsies = sameSide(stage1Point, stage2Point, fraguv, circumCenter);
    //float ourSign = signage(fraguv, stage1Point, stage2Point);
    //float theirSign = signage(circumCenter, stage1Point, stage2Point);
    if(!samsies){
        discard;
    }
    float dist = distance(stage1Point, circumCenter);
    gl_FragDepth = dist;
    FragColor = vec4(possiblePoint, 0.0, 1.0);


    /*
    vec2 minPoint = vec2(0.0, 0.0);
    float minRad = 1001623562360.0;// 1.0/0.0;//should be infinity;

    for(int a = 0; a < pointWidth; a++) {
        for(int b = 0; b < pointHeight; b++) {
            vec2 possiblePoint = texelFetch(pointTexture, ivec2(a, b), 0).xy;
            if(possiblePoint != stage1Point && possiblePoint != stage2Point) {
                float possibleRadius = calculateRadius(stage1Point, stage2Point, possiblePoint);
                if(possibleRadius < 0) {
                    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                }
                if(possibleRadius<minRad) {
                    minPoint = possiblePoint;
                    minRad = possibleRadius;
                }
            }
            //float isLesser = float(possibleRadius<minRad);
            //float isGreater = 1.0-isLesser;
            //minPoint = (isLesser*possiblePoint)+(isGreater*minPoint);
            //minRad = (isLesser*possibleRadius)+(isGreater*minRad);
        }
    }

    //here the triangle is made of the stage1 point, stage2 point and stage3 point

    vec2 centroidOfTheMechanism = (stage1Point+stage2Point+minPoint)/3.0;
    


    FragColor = vec4(centroidOfTheMechanism, 0.0, 1.0);*/
    //FragColor = texture(colorTexture, centroidOfTheMechanism);

    //FragColor = vec4(minPoint, 0.2, 1.0);
    //gl_FragDepth = distance(fraguv, thePoint);

}