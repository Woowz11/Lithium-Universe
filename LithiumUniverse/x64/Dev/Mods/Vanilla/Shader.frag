#version 330 core

in vec2 UV;
out vec4 PixelColor;

uniform sampler2D Texture;
uniform vec4 Color;

uniform float LocalRandom;

void main()
{
    vec4 TextureColor = texture(Texture, UV) * Color;
    if(TextureColor.a == 0){ discard; }
	TextureColor = vec4(UV.x,TextureColor.r,UV.y,TextureColor.a);
	TextureColor = vec4(TextureColor.r,LocalRandom-TextureColor.g,(UV.x>0.9||UV.y<0.3?LocalRandom*(UV.x+UV.y):TextureColor.b),1);
    PixelColor = TextureColor;
}