#version 330 core
in vec2 TextureCoord;
out vec4 FragColor;

uniform sampler2D Texture;
uniform vec4 Color;
uniform float Random;
uniform float Time;
uniform float DeltaTime;

uniform int TextCharPosition;

void main()
{
    vec4 TextureColor = texture(Texture, TextureCoord) * Color;
    if(TextureColor.a == 0){
		if (true){
			TextureColor = vec4((sin(TextCharPosition)+0.5)/2,(sin(TextCharPosition*2)+0.5)/2,(sin(TextCharPosition*3)+0.5)/2,1);
		}else{
			discard;
		}
	}
    FragColor = vec4(1,1,0,1);//TextureColor;
}