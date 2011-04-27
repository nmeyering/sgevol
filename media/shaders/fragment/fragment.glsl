#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

bool 
grid_point(
	float foo)
{
	return fract(10*foo) < 0.05;
}

void
main()
{
	vec3 p = (position_interp + 1.0) * 0.5;
	vec3 dir = normalize(position_interp - camera);
	int steps = 3;
	float stepsize = 0.05;

	vec4 acc = vec4(0.0);

	for (int i = 0; i < steps; ++i)
	{
		if (
			grid_point(p.x) ||
			grid_point(p.y) ||
			grid_point(p.z)
			)
		{
			acc += (1.0 - acc.a) * vec4(
					abs(p.x - 0.5),
					abs(p.y - 0.5),
					abs(p.z - 0.5),
					1.0 / steps);
		}

		if( acc.a > 0.95 )
			break;

		p += dir * stepsize;
	}
	frag_color = acc;
}
