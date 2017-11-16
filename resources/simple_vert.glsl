#version  330 core
layout(location = 2) in vec3 vertPos;
// layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform float Time;

//out vec3 fragNor;

void main()
{
	float t = vertPos.x + Time;
	float x,y,z;
	if (vertPos.z > 0) {
		// x = sin(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t/12),5));
		// y = cos(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t/12),5));
		// x = sin(t/10) * 100 + sin(t/5) * 20;
		
		// y = cos(t / 10) * 100;
		x = 2 * cos(t) + 5 * cos(t * 2/3);
		y = 2 * sin(t) - 5 * sin(t * 2/3);
		z = 0;
	}
	else {
		// x = -1 * sin(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t/12),5));
		// y = -1 * cos(t) * (exp(cos(t)) - 2 * cos(4 * t) - pow(sin(t/12),5));
		// x = sin(t/10) * 200 + sin(t) * 2;
		// y = cos(t/20) * 200 + cos(t/12) * 20;
		x = 2 * cos(t) + 3 * cos(t * 2/3);
		y = 2 * sin(t) - 3 * cos(t * 2/3);
		z = 0;
	}

	// gl_PointSize=5.0;
	vec4 pos = vec4(normalize(vec3(x, y, z)), 1.0);
	gl_Position =  P * V * M * pos;
	// fragNor = (V*M * vec4(vertNor, 0.0)).xyz;
}
