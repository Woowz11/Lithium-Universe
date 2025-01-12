#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform float random;
uniform float random2;

void main()
{
    FragColor = texture(texture1, TexCoord);
}