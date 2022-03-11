#version 460



//in vec2 fraguv;

out vec4 FragColor;

uniform sampler2D stage4Texture1;
uniform sampler2D stage4Texture2;
uniform sampler2D stage4Texture3;
uniform sampler2D colorTexture;

void main() 
{    
    vec2 pointA = texelFetch(stage4Texture1, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 pointB = texelFetch(stage4Texture2, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 pointC = texelFetch(stage4Texture3, ivec2(gl_FragCoord.xy), 0).xy;
    vec2 centro = (pointA+pointB+pointC)/3.0;

    FragColor = texture(colorTexture, centro);

    FragColor = vec4(centro, 0.0, 1.0);

}