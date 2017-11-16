#version  330 core
layout(location = 2) in vec3 vertPos;
// layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform float Time;

//out vec3 fragNor;
float r(float theta) {
	return cos(theta * 3.14159265358979);
}



void main()
{
	float x,y,z;

	float theta = vertPos.x;

	float rad = r(cos(Time)/2 * theta);
	// if (Time  > 20) {
	// float rad =r3(theta, cos(Time)* 10.0);
	// }
	x = rad * cos(theta);
	y = rad * sin(theta);
	z = 0;
		// if (vertPos.z > 0) {

	gl_PointSize= 5.0;
	vec4 pos = normalize(vec4(vec3(x, y, z), 1.0));
	// vec4 pos = vec4(vertPos, 1.0);
	gl_Position =  P * V * M * pos;
	// fragNor = (V*M * vec4(vertNor, 0.0)).xyz;
}
