#version 330 core

layout (location = 0) in vec2 PointPosition;
layout (location = 1) in vec2 TextureUV;
layout (location = 2) in int PointID;

out vec2 UV;

uniform vec2 Position;
uniform float Orientation;
uniform vec2 Size;
uniform vec2 Center;
uniform float Layer;
uniform int ID;
uniform vec2 CameraPosition;
uniform float CameraOrientation;
uniform float CameraZoom;
uniform bool Interface;
uniform bool Resize;
uniform vec2 ScreenStartSize;
uniform vec2 ScreenSize;

mat4 Scale(mat4 mat, vec2 vec){
	return mat4(
		mat[0] * vec[0],
		mat[1] * vec[1],
		mat[2],
		mat[3]);
}

mat4 Rotate(mat4 mat, float angl){
	float c = cos(angl);
	float s = sin(angl);
	
	vec3 axis = normalize(vec3(0,0,1));
	vec3 temp = (1 - c) * axis;
	
	mat4 Rotate;
	Rotate[0][0] = c + temp[0] * axis[0];
	Rotate[0][1] = temp[0] * axis[1] + s * axis[2];
	Rotate[0][2] = temp[0] * axis[2] - s * axis[1];

	Rotate[1][0] = temp[1] * axis[0] - s * axis[2];
	Rotate[1][1] = c + temp[1] * axis[1];
	Rotate[1][2] = temp[1] * axis[2] + s * axis[0];

	Rotate[2][0] = temp[2] * axis[0] + s * axis[1];
	Rotate[2][1] = temp[2] * axis[1] - s * axis[0];
	Rotate[2][2] = c + temp[2] * axis[2];
	
	mat4 Result;
	Result[0] = mat[0] * Rotate[0][0] + mat[1] * Rotate[0][1] + mat[2] * Rotate[0][2];
	Result[1] = mat[0] * Rotate[1][0] + mat[1] * Rotate[1][1] + mat[2] * Rotate[1][2];
	Result[2] = mat[0] * Rotate[2][0] + mat[1] * Rotate[2][1] + mat[2] * Rotate[2][2];
	Result[3] = mat[3];
	return Result;
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

	mat4 RPosition = mat4(1);
	
	if(!Interface){
		RPosition = Rotate(RPosition, CameraOrientation);
	}
	
	RPosition = Translate(RPosition, vec3(Position * ScaleFactor, (Layer + (float(ID)/100))/100));
	
	if(!Interface){
		RPosition = Translate(RPosition, CameraPosition);
	}
	
	RPosition = Rotate(RPosition, Orientation);
	
	RPosition = Translate(RPosition, -Center * Size);
	
	RPosition = Scale(RPosition, Size);
	
    gl_Position = Projection * RPosition * vec4(PointPosition, 1.0f, 1.0f);
	
    UV = vec2(TextureUV.x, 1.0 - TextureUV.y);
}