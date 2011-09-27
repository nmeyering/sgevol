#version 140

$$$HEADER$$$

in vec3 star_color;
out vec4 frag_color;

const vec2 center = vec2(0.5,0.5);

void
main()
{
	frag_color = vec4(star_color,1.0 - 2.0 * distance(gl_PointCoord, center));
}
