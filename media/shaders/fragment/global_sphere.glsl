#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

const float stepsize = 1e-2;
const int steps = 300;
const float absorption_factor = 32.0 * stepsize;

const vec3 suncolor = 0.5 * vec3(2.0,2.0,1.90);
const vec3 ambient = vec3(0.25);

bool
outside_unit_cube(const vec3);

bool
outside_sphere(
	const vec3,
	const vec3,
	const float);

const vec3 center = vec3(0.5,0.5,0.5);
float radius_limit = 0.498 * radius;
float factor = opacity;

void
main()
{
	vec4 value;
	vec4 dst = vec4(0.0);
	vec3 sundir = normalize(vec3(sin(offset),0.0,cos(offset)));

	vec3 position = (position_interp + 1.0) * 0.5;
	vec3 direction = normalize(position_interp - camera);

	// main raycasting loop
	for (int i = 0; i < steps; i++)
	{
		float value = texture(tex, position).r;

		float light = 1.0;
		vec3 occ_pos = position;
		vec3 occ_dir = sundir;

		// volume shadow loop
		for (int j = 0; j < steps; j++)
		{
			occ_pos += occ_dir * stepsize;
			light -= texture(tex, occ_pos).r * absorption_factor;

			if (light < 0.01)
				break;

			if (!outside_sphere(occ_pos, center, radius_limit))
			{
				light = 0.f;
				break;
			}

			if (outside_unit_cube(occ_pos))
				break;
		}

		vec3 col = light * suncolor + ambient;
		dst += (1.0 - dst.a) * factor * value * vec4(col, 1.0);

		if( dst.a > 0.95 )
			break;

		position += direction * stepsize;

		if (!outside_sphere(position, center, radius_limit))
			break;

		if (outside_unit_cube(position))
			break;
	}
	dst = vec4((1.0 / dst.a) * dst.rgb, dst.a);
	frag_color = dst;
}
