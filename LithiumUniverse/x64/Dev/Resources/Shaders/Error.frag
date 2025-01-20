#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;
uniform vec4 Color;

uniform sampler2D Texture;

void main()
{
    FragColor = vec4(1.0f,0.0f,1.0f,1.0f) * Color;
}