#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;

uniform sampler2D Texture;
uniform vec4 Color;
uniform float Random;
uniform float Time;
uniform float DeltaTime;

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
}