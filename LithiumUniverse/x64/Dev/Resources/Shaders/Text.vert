#version 330 core

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec2 inSize;
layout (location = 2) in int inChar;
layout (location = 3) in int inPosition;
layout (location = 4) in vec2 inOffset;

uniform vec2 ScreenSize;

out vec2 UV;

const vec2 ScreenScale = vec2(10.0/3, 7.5/3);

void main() {
    int Scale = 10;

    vec2 Position = vec2(100, 300);
    float xpos = Position.x + float(inPosition) * (inSize.x * Scale) + inOffset.x * Scale;
    float ypos = Position.y + inOffset.y * Scale;

    vec2 screenPos = vec2(
        (xpos / ScreenSize.x) * 2.0 - 1.0,
        1.0 - (ypos / ScreenSize.y) * 2.0
    );

    gl_Position = vec4(inOffset*vec2(0.5,0.5)*ScreenScale,0,1);//vec4(screenPos, 0.0, 1.0);
    UV = inUV;
}
