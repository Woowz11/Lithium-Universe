#version 330 core

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec2 inSize;
layout (location = 2) in int inChar;
layout (location = 3) in int inPosition;
layout (location = 4) in vec2 inOffset;

out vec2 UV;

uniform vec2 Position;
uniform float Orientation;
uniform vec2 Size;
uniform float Layer;
uniform int ID;
uniform vec2 CameraPosition;
uniform float CameraOrientation;
uniform float CameraZoom;
uniform bool Interface;
uniform bool Resize;
uniform vec2 ScreenStartSize;
uniform vec2 ScreenSize;

uniform vec2 TextureSize;

mat4 Ortho(float L, float R, float B, float T){
	return mat4(
         2/(R - L), 0        , 0          , 0,
         0        , 2/(T - B), 0          , 0,
         0        , 0        , -1.0/2000.0, 0,
        -(R + L)/(R - L), -(T + B)/(T - B), -0.5, 1
    );
}

const vec2 ScreenScale = vec2(10.0/3, 7.5/3);

void main()
{
	float SW = Resize ? ScreenStartSize.x : ScreenSize.x;
	float SH = Resize ? ScreenStartSize.y : ScreenSize.y;

	float Z = Interface ? 1 : 1 / CameraZoom;

	mat4 Projection = Ortho(-SW / (240 * Z), SW / (240 * Z), -SH / (240 * Z), SH / (240 * Z));

	vec2 ScreenDifference = ScreenSize / ScreenStartSize;
    vec2 ScaleFactor = Interface ? (Resize ? ScreenScale : ScreenScale * ScreenDifference) : vec2(1.0);
	
	vec2 TextPosition = (Position+inOffset) * ScaleFactor;
	
	if (!Interface){
		TextPosition += CameraPosition;
	}
	
    gl_Position = Projection * vec4(TextPosition,1,1);
	
    UV = inUV/TextureSize;
}