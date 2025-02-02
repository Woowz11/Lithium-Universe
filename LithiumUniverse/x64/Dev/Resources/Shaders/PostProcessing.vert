#version 330 core

layout (location = 0) in vec2 PointPosition;
layout (location = 1) in vec2 TextureUV;
layout (location = 2) in int PointID;

out vec2 UV;

void main() {
    gl_Position = vec4(PointPosition, 0.0, 1.0);
	UV = TextureUV;
}