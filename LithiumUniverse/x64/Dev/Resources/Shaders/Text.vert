#version 330 core

layout (location = 0) in vec2 PointPosition;
layout (location = 1) in vec2 TextureUV;
layout (location = 2) in int CharID;
layout (location = 3) in int CharPosition;

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

    vec2 ScaleFactor = Interface ? ScreenScale : vec2(1.0);
	
	vec2 ScaleFactorS = (Resize ? (Interface ? ScaleFactor : vec2(1,1) ) : vec2(1,1));
	vec2 ScaleFactorP = (Resize ? vec2(1,1) : (Interface ? ScreenScale * (ScreenSize/ScreenStartSize) : vec2(1,1) ));
	
	vec2 TextPosition = (Position * ScaleFactorP + (PointPosition * (Size * (Interface ? 0.192 : 1)))) * ScaleFactorS;
	
	if (!Interface){
		TextPosition += CameraPosition;
	}
	
    gl_Position = Projection * vec4(TextPosition,(Layer + (float(ID)/100))/100,1);
	
    UV = TextureUV;
}