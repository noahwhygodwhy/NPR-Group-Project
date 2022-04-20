#version 460

out vec4 FragColor;

in vec2 fraguv;

uniform int kernalWidth;
uniform int kernalHeight;




uniform sampler2D colorTexture;

float rand(inout vec2 co){
    float toReturn = fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
    co += vec2(toReturn);
    return toReturn;
}

mat3 kX = mat3(1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);
mat3 kY = mat3(1.0, 0.0, -1.0, 2.0, 0.0, -2.0, 1.0, 0.0, -1.0);



float avgRGB(vec3 i) {
    return (i.r + i.g + i.b)/3.0;
}

//gets the angle of the local flow of the image
float getAngle(){
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
    return atan(sX / sY);
}


//gets the mean and variance inside a quadrant of neighbors
void getMeanAndVariance(in ivec2 xRange, in ivec2 yRange, in mat2 rotMatrix, out vec3 mean, out float variance) {
    ivec2 maxSize = textureSize(colorTexture, 0);
    vec2 pixelSize = vec2(1.0)/vec2(maxSize);
    vec2 base = gl_FragCoord.xy;

    mean = vec3(0);
    vec3 channelvariance = vec3(0);
    int samples = 0;

    for(int x = xRange.x; x < xRange.y; x++) {
        for(int y = yRange.x; y < yRange.y; y++) {
            vec2 pixelCoord = vec2(fraguv+((pixelSize*vec2(x, y)*rotMatrix)));
            vec3 pixelColor = texture(colorTexture, pixelCoord).xyz;

            mean += pixelColor;
            channelvariance += (pixelColor*pixelColor);
            samples++;
        }
    }
    mean = mean/samples;
    channelvariance = (channelvariance/samples)-(mean*mean);
    variance = channelvariance.x + channelvariance.y + channelvariance.z;
}


void main() 
{

    //get the local image flow angle
    float localAngle = getAngle();

    //and create a rotation matrix for the kernel
    mat2 rotMat = mat2(cos(localAngle), -sin(localAngle), sin(localAngle), cos(localAngle));
   
    float minVariance = 1000000000000000000.0;//NO FLOAT_INF? :(
    vec3 minMean = vec3(0);
    float possibleVariance = 0.0;
    vec3 possibleMean = vec3(0);
    float differentAmmount = 0;


    //Applies the kuwahara kernel

    getMeanAndVariance(ivec2(-kernalWidth, 0), ivec2(-kernalHeight, 0),rotMat, possibleMean, possibleVariance);
    int itLess = int(possibleVariance<minVariance);
    int itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);
    differentAmmount += abs(possibleVariance-minVariance);

    getMeanAndVariance(ivec2(0, kernalWidth), ivec2(-kernalHeight, 0),rotMat, possibleMean, possibleVariance);
    itLess = int(possibleVariance<minVariance);
    itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);
    differentAmmount += abs(possibleVariance-minVariance);

    getMeanAndVariance(ivec2(-kernalWidth, 0), ivec2(0, kernalHeight),rotMat, possibleMean, possibleVariance);
    itLess = int(possibleVariance<minVariance);
    itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);
    differentAmmount += abs(possibleVariance-minVariance);
    
    getMeanAndVariance(ivec2(0, kernalWidth), ivec2(0, kernalHeight),rotMat, possibleMean, possibleVariance);
    itLess = int(possibleVariance<minVariance);
    itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);
    differentAmmount += abs(possibleVariance-minVariance);

    FragColor = vec4(minMean, 1.0);

    FragColor = vec4(minMean-vec3((sqrt(differentAmmount))/2.0f), 1.0);



}