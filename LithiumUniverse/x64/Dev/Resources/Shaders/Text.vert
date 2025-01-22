#version 330 core
layout (location = 0) in vec3 PointPosition;
layout (location = 1) in vec2 TextureUV;
layout (location = 2) in int PointID;

out vec2 TextureCoord;

uniform vec2 Position;
uniform vec2 Size;
uniform float Layer;
uniform int ID;
uniform vec2 CameraPosition;
uniform float CameraZoom;
uniform bool Interface;
uniform bool Resize;
uniform vec2 ScreenStartSize;
uniform vec2 ScreenSize;

uniform vec2 TextureSize;
uniform int TextCharPosition;
uniform int TextCharSize;
uniform vec2 CharPosition;
uniform vec2 CharSize;
uniform vec2 ErrorCharSize;

mat4 Scale(mat4 mat, vec2 vec){
	return mat4(
		mat[0] * vec[0],
		mat[1] * vec[1],
		mat[2],
		mat[3]);
}

mat4 Translate(mat4 mat, vec3 vec)
{
	return mat4(
		mat[0],
        mat[1],
        mat[2],
		mat[0] * vec[0] + mat[1] * vec[1] + mat[2] * vec[2] + mat[3]
	);
}

mat4 Translate(mat4 mat, vec2 vec){
	return Translate(mat, vec3(vec, 0));
}

mat4 Ortho(float L, float R, float B, float T){
	float ZN = -1000;
	float ZF = 1000;
	mat4 Result;
	Result[0] = vec4(2/(R-L),0,0,0);
	Result[1] = vec4(0,2/(T-B),0,0);
	Result[2] = vec4(0,0,-(1/(ZF-ZN)),0);
	Result[3] = vec4(-((R+L)/(R-L)),-((T+B)/(T-B)),-(ZN/(ZF-ZN)),1);
	return Result;
}

const vec2 ScreenScale = vec2(10.0/3, 7.5/3);

void main()
{
	float SW = Resize ? ScreenStartSize.x : ScreenSize.x;
	float SH = Resize ? ScreenStartSize.y : ScreenSize.y;

	float Z = Interface ? 1 : 1/CameraZoom;

	float PLeft  = -SW / 240;
	float PRight =  SW / 240;
	float PDown  = -SH / 240;
	float PUp    =  SH / 240;
	mat4 Projection = Ortho(PLeft/Z, PRight/Z, PDown/Z, PUp/Z);

	vec2 ScreenDifference = vec2(ScreenSize.x/ScreenStartSize.x, ScreenSize.y/ScreenStartSize.y);

	mat4 RPosition;
	RPosition[0] = vec4(1,0,0,0);
	RPosition[1] = vec4(0,1,0,0);
	RPosition[2] = vec4(0,0,1,0);
	RPosition[3] = vec4(0,0,0,1);
	
	float Indent = (float(TextCharSize)/ErrorCharSize.x) * 0.4;
	RPosition = Translate(RPosition, vec3(vec2(Position.x + Indent, Position.y) * (Interface ? (Resize ? ScreenScale : ScreenScale * ScreenDifference) : vec2(1,1)), (Layer + (float(ID)/100))/100));
	
	if(!Interface){
		RPosition = Translate(RPosition, CameraPosition);
	}
	
	RPosition = Scale(RPosition, vec2(Size.x * (CharSize.x/ErrorCharSize.x), Size.y));
	
    gl_Position = Projection * RPosition * vec4(PointPosition, 1.0f);
    TextureCoord = vec2(TextureUV.x * (CharSize.x / TextureSize.x) + (CharPosition.x / TextureSize.x), 1.0 - (TextureUV.y * (CharSize.y / TextureSize.y) + (CharPosition.y / TextureSize.y)));
}