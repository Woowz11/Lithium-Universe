#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;

uniform sampler2D Texture;
uniform vec4 Color;
uniform float Random;
uniform float Time;
uniform float DeltaTime;
uniform vec2 MousePosition;

uniform bool DebugRender;
uniform bool Sleeping;

void main()
{
    vec4 TextureColor = texture(Texture, TextureCoord) * Color;
    if(TextureColor.a == 0){ discard; }
    if(DebugRender){
        if(Sleeping){
            TextureColor *= vec4(1,0,0,1);
        }
    }
	
    FragColor = TextureColor;
	
	float x = gl_FragCoord.x;
	float y = gl_FragCoord.y;
	
	float size = 30;
	
	float dis = clamp(1-distance(gl_FragCoord.xy, MousePosition) / 300, 0, 1);
	FragColor = vec4(mix(TextureColor.r, 1-TextureColor.r, dis), mix(TextureColor.g, 1-TextureColor.g, dis), mix(TextureColor.b, 1-TextureColor.b, dis), TextureColor.a);
	
	//if((x-size) <= MousePosition.x && (x+size) >= MousePosition.x && (y+size) >= MousePosition.y && (y-size) <= MousePosition.y){
		
	//}
	
	
}