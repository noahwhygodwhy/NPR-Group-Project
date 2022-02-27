#version 460

in vec2 fraguv;
uniform vec3 eyePos;
uniform sampler2D lineTexture;
//uniform vec2 screenResolution;

out vec4 FragColor;

float rand(vec3 co){
    return fract(sin(dot(co, vec3(12.9898, 78.233, 0.0))) * 43758.5453);
}


//vec2 offset = vec2(1.0f)/screenResolution;

/*vec2 numchar[8] = {
    vec2(-offset.x, -offset.y),
    vec2(0.0f, -offset.y),
    vec2(offset.x, -offset.y),
    vec2(-offset.x, 0.0f),
    vec2(offset.x, 0.0f),
    vec2(-offset.x, offset.y),
    vec2(0.0f, offset.y),
    vec2(offset.x, offset.y)
};*/

/*mat3 kernal = {
    vec3(1.0f, 1.0f, 1.0f),
    vec3(1.0f, 0.0f, 1.0f),
    vec3(1.0f, 1.0f, 1.0f)
    };
    */


//noise function stolen from https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
/*  static.frag
    by Spatial
    05 July 2013
*/
// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}
// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }
// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}


// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }


void main() 
{   
    float val = 0.0;

    //vec2 coordsFromTexture = vec2(0.0, 0.1);
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    int i =0;

    vec2 randomUV = vec2(0.0);
    
    //randomUV.x = rand(eyePos + vec3(fraguv, 0) + vec3(val));
    //randomUV.y = rand(-vec3(eyePos.z, eyePos.x, eyePos.y) + vec3(fraguv, 0) + vec3(val+1.0));
    while(val < 2.0) {
        randomUV.x = rand(eyePos + vec3(fraguv, 0) + vec3(val));
        randomUV.y = rand(-vec3(eyePos.z, eyePos.x, eyePos.y) + vec3(fraguv, 0) + vec3(val+1.0));
        i++;
        vec3 color = texture(lineTexture, randomUV).xyz;
        val += color.x;
    }
    FragColor = vec4(randomUV, 0.0, 1.0);
}