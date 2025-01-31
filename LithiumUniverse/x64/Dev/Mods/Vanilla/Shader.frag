#version 330 core

in vec2 UV;
out vec4 PixelColor;

uniform sampler2D Texture;
uniform vec4 Color;

void main()
{
    vec4 TextureColor = texture(Texture, UV) * Color;
    if(TextureColor.a == 0){ discard; }
	TextureColor = vec4(1-TextureColor.r,1-TextureColor.g,1-TextureColor.b,TextureColor.a);
    PixelColor = TextureColor;
}