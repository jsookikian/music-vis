#version  330 core
layout(location = 2) in vec3 vertPos;
// layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform float Time;

//out vec3 fragNor;
float r(float theta, float a, float b, float m , float n1, float n2, float n3) {
	return pow(pow(abs(cos(m * theta / 4.0) / a), n2) + 
			   pow(abs(sin(m * theta / 4.0) / b), n3),
			    -1.0 / n1);
}


float r1(float t, float val) {
	return val * (1.0 - abs(-sin(6 * (t-1))) + 2*(cos(2*(t-1)))  ) ;
}
//butterfly
float r2(float t, float val) {
	return val * 3 * pow(sin(4 * t - 2),2) + 4 * pow(cos(2 * t -5), 2) ;
}

float r3(float t, float val) {
	return val * 3 * pow(sin(t-2), 2) + 4 * cos(2 * t -5);
}

float r4(float t, float val) {
	return val * cos( t + ((3.14*2)/6) * ceil( cos(t)*6 ) ) + sin( t * 36 ) * 0.2;
}

float lotus(float li, float lp, float lm, float lo, float lc, float lg, float lr, float theta, float pi ) {
	return  li +lm*(((abs(cos(theta*(lp/2)+lr/2))*lo)-(abs(cos(theta*(lp/2)+pi/2+lr/2)))*lg)/(2+abs(cos(theta*(lp/2)*2+pi/2+lr))*lc));
}

void main()
{
	// float t = vertPos.x + Time;
	float x,y,z;
	// if (vertPos.z > 0) {
	// x = sin(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t/12),5));
	// y = cos(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t/12),5));
	// 	// x = sin(t/10) * 100 + sin(t/5) * 20;
		
	// 	// y = cos(t / 10) * 100;
	// 	x = 2 * cos(t) + 5 * cos(t * 2/3);
	// 	y = 2 * sin(t) - 5 * sin(t * 2/3);
	// 	z = 0;
	// }
	// else {
	// 	// x = -1 * sin(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t/12),5));
	// 	// y = -1 * cos(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t/12),5));
	// 	// x = sin(t/10) * 200 + sin(t) * 2;
	// 	// y = cos(t/20) * 200 + cos(t/12) * 20;
	// 	x = 2 * cos(t) + 3 * cos(t * 2/3);
	// 	y = 2 * sin(t) - 3 * cos(t * 2/3);
	// 	z = 0;
	// }
	float theta = vertPos.x;
	//GOLD// -- r(theta, cos(Time) * 5 + 0.5, cos(Time), cos(Time)*10 +3, 1, 10, sin(Time) / 2);
	float rad = r(theta, cos(Time) * 5 + 0.5, cos(Time) * 5 +0.5, cos(Time)*10 +3, 1, 1, sin(Time) / 2);
	// float rad = r4(theta, cos(Time)* 10.0);
	// float rad = lotus(cos(Time), 6, 1, 1, 3, 1, 3.14159, theta, 3.14159);
	// if (Time  > 20) {
	// 	rad =r3(theta, cos(Time)* 10.0);
	// }
	x = rad * cos(theta);
	y = rad * sin(theta);
	z = 0;
		// if (vertPos.z > 0) {

	gl_PointSize=10.0;
	vec4 pos = normalize(vec4(vec3(x, y, z), 1.0));
	// vec4 pos = vec4(vertPos, 1.0);
	gl_Position =  P * V * M * pos;
	// fragNor = (V*M * vec4(vertNor, 0.0)).xyz;
}
