#version 330 core

in vec2 UV;
out vec4 PixelColor;

uniform sampler2D Texture;

void main() {
    PixelColor = texture(Texture, UV);
	
	//PixelColor = vec4(vec3(1.0 - PixelColor), 1.0);
}