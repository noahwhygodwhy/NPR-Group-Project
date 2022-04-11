#version 460

out vec4 FragColor;

in vec2 fraguv;

uniform int kernalWidth;
uniform int kernalHeight;

uniform int width;
uniform int height;



uniform sampler2D colorTexture;





//vec2 pixelToUV(vec2 fragCoord){
//    return fragCoord/vec2(width, height);
//}

void getMeanAndVariance(in ivec2 xRange, in ivec2 yRange, out vec3 mean, out float variance) {

    vec2 pixelSize = vec2(1.0)/vec2(width, height);

    vec2 base = gl_FragCoord.xy;
    mean = vec3(0);
    vec3 channelvariance = vec3(0);
    int samples = 0;
    for(int x = xRange.x; x < xRange.y; x++) {
        for(int y = yRange.x; y < yRange.y; y++) {
            vec2 pixelCoord = vec2(fraguv+(pixelSize*vec2(x, y)));

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

    /*FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    return;*/


    float minVariance = 1000000000000000000.0;
    vec3 minMean = vec3(0);
    float possibleVariance = 0.0;
    vec3 possibleMean = vec3(0);

    getMeanAndVariance(ivec2(-kernalWidth, 0), ivec2(-kernalHeight, 0), possibleMean, possibleVariance);
    int itLess = int(possibleVariance<minVariance);
    int itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);

    getMeanAndVariance(ivec2(0, kernalWidth), ivec2(-kernalHeight, 0), possibleMean, possibleVariance);
    itLess = int(possibleVariance<minVariance);
    itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);

    getMeanAndVariance(ivec2(-kernalWidth, 0), ivec2(0, kernalHeight), possibleMean, possibleVariance);
    itLess = int(possibleVariance<minVariance);
    itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);
    
    getMeanAndVariance(ivec2(0, kernalWidth), ivec2(0, kernalHeight), possibleMean, possibleVariance);
    itLess = int(possibleVariance<minVariance);
    itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);

    FragColor = vec4(minMean, 1.0);



}