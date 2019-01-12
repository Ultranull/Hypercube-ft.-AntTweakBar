#version 330 core
precision highp float;

layout(location = 0) in vec4 vertpos;
layout(location = 1) in vec3 vertcolor;

out vec3 fragcolor;

uniform int dfc;
uniform mat4 to3D;
uniform mat4 view;
uniform float time;
uniform float xzAngle;
uniform float xyAngle;
uniform float yzAngle;
uniform float xwAngle;
uniform float ywAngle;
uniform float zwAngle;

mat4 rotateXY(float angle);
mat4 rotateXZ(float angle);
mat4 rotateYZ(float angle);
mat4 rotateXW(float angle);
mat4 rotateYW(float angle);
mat4 rotateZW(float angle);

void main(){
	const float PI=3.14159265;
	float angle=time;

	
    vec4 pos=(		
					rotateXY(radians(xyAngle))*
					rotateXZ(radians(xzAngle))*
					rotateYZ(radians(yzAngle))*
					rotateXW(radians(xwAngle))*
					rotateYW(radians(ywAngle))*
					rotateZW(radians(zwAngle))*
					vertpos);
	if(dfc==1)
		gl_Position=vec4((to3D*pos).xyz,1);
	else
		gl_Position=vec4(to3D*view*pos);

	fragcolor=vertcolor;
}

mat4 rotateXY(float angle){
	float c=cos(angle),
		  s=sin(angle);
	return mat4(
			c,-s, 0, 0,
			s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	);
}
mat4 rotateXZ(float angle){
	float c=cos(angle),
		  s=sin(angle);
	return mat4(
			c, 0,-s, 0,
			0, 1, 0, 0,
			s, 0, c, 0,
			0, 0, 0, 1
	);
}
mat4 rotateYZ(float angle){
	float c=cos(angle),
		  s=sin(angle);
	return mat4(
			1, 0, 0, 0,
			0, c,-s, 0,
			0, s, c ,0,
			0, 0, 0, 1
	);
}

mat4 rotateXW(float angle){
	float c=cos(angle),
		  s=sin(angle);
	return mat4(
			c, 0, 0,-s,
			0, 1, 0, 0,
			0, 0, 1, 0,
			s, 0, 0, c
	);
}
mat4 rotateYW(float angle){
	float c=cos(angle),
		  s=sin(angle);
	return mat4(
			1, 0, 0, 0,
			0, c, 0,-s,
			0, 0, 1, 0,
			0, s, 0, c
	);
}
mat4 rotateZW(float angle){
	float c=cos(angle),
		  s=sin(angle);
	return mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, c,-s,
			0, 0, s, c
	);
}