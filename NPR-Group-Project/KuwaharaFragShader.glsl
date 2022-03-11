#version 460

out vec4 FragColor;

uniform int kernalSize;

uniform int width;
uniform int height;

uniform sampler2D colorTexture;





vec2 pixelToUV(vec2 fragCoord){
    return fragCoord/vec2(width, height);
}

void getMeanAndVariance(in ivec2 xRange, in ivec2 yRange, out vec3 mean, out float variance) {

    vec2 base = gl_FragCoord.xy;
    mean = vec3(0);
    vec3 channelvariance = vec3(0);
    int samples = 0;
    for(int x = xRange.x; x < xRange.y; x++) {
        for(int y = yRange.x; y < yRange.y; y++) {
            vec2 pixelCoord = gl_FragCoord.xy+vec2(x, y);
            pixelCoord = min(pixelCoord, vec2(width-1, height-1));
            pixelCoord = max(pixelCoord, vec2(0,0));
            vec3 pixelColor = texture(colorTexture, pixelToUV(pixelCoord)).xyz;

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


    float minVariance = 1000000000000000000.0;
    vec3 minMean = vec3(0);
    float possibleVariance = 0.0;
    vec3 possibleMean = vec3(0);

    getMeanAndVariance(ivec2(-kernalSize, 0), ivec2(-kernalSize, 0), possibleMean, possibleVariance);
    int itLess = int(possibleVariance<minVariance);
    int itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);

    getMeanAndVariance(ivec2(0, kernalSize), ivec2(-kernalSize, 0), possibleMean, possibleVariance);
    itLess = int(possibleVariance<minVariance);
    itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);

    getMeanAndVariance(ivec2(-kernalSize, 0), ivec2(0, kernalSize), possibleMean, possibleVariance);
    itLess = int(possibleVariance<minVariance);
    itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);
    
    getMeanAndVariance(ivec2(0, kernalSize), ivec2(0, kernalSize), possibleMean, possibleVariance);
    itLess = int(possibleVariance<minVariance);
    itGreater = 1-itLess;
    minVariance = (itLess*possibleVariance)+(itGreater*minVariance);
    minMean = (itLess*possibleMean)+(itGreater*minMean);

    FragColor = vec4(minMean, 1.0);



}