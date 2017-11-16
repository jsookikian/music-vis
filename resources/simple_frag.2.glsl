#version 330 core 
// in vec3 fragNor;
uniform float Time;
out vec3 color;

void main()
{	
	vec2 center = vec2(0.5, 0.5);
	float dist = distance(vec2(gl_PointCoord.x, gl_PointCoord.y), center);
	if (dist > 0.5) {
		discard;
	}
	color = vec3(0.6, 0.3, 0.2);

}
