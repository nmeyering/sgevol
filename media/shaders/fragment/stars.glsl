#version 140

$$$HEADER$$$

in vec3 star_color;
out vec4 frag_color;

const vec2 center = vec2(0.5,0.5);

void
main()
{
	vec2 d = abs(2.0 * (gl_PointCoord - center));
	frag_color = vec4(star_color,sqrt(2.0) - sqrt(d.x) - sqrt(d.y));
}
