#version 330 core

in vec2 UV;
out vec4 PixelColor;

uniform sampler2D Texture;
uniform vec4 Color;

void main()
{
    vec4 TextureColor = texture(Texture, UV) * Color;
    if(TextureColor.a == 0){
		if (true){
			TextureColor = vec4(1,0,0,1);//vec4((sin(TextCharPosition)+0.5)/2,(sin(TextCharPosition*2)+0.5)/2,(sin(TextCharPosition*3)+0.5)/2,1);
		}else{
			discard;
		}
	}
    PixelColor = TextureColor;
}