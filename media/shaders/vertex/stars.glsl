#version 140

$$$HEADER$$$
out vec3 star_color;

void
main()
{
	star_color =
		color;
	gl_PointSize =
		radius;
	gl_Position =
		mvp * vec4(position, 1.0);
}
