#version 140

$$$HEADER$$$

out vec4 frag_color;

// const vec2 center = vec2(0.5,0.5);

void
main()
{
	frag_color = 2.0 * distance(gl_PointCoord, center);
}
