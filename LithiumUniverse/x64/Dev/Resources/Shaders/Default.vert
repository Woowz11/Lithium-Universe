#version 330 core
layout (location = 0) in vec3 PolygonPosition;
layout (location = 1) in vec2 TextureUV;

out vec2 TextureCoord;

uniform mat4 Position;
uniform mat4 Projection;
uniform float Random;
uniform float Time;
uniform float DeltaTime;

void main()
{
    gl_Position = Projection * Position * vec4(PolygonPosition, 1.0f);
    TextureCoord = vec2(TextureUV.x, 1.0 - TextureUV.y);
}