#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

const float stepsize = 5e-3;
const int steps = int(sqrt(3.0)/stepsize);
const vec3 center = vec3(0.5,0.5,0.5);
const vec3 suncolor = vec3(1.0,1.0,0.95);
//const vec3 sun = normalize(vec3(0.0,1.0,1.0));
const vec3 ambient = vec3(0.0);

#define PI 3.141592653589793

bool
outside_unit_cube(const vec3);

bool
outside_sphere(
	const vec3,
	const vec3,
	const float);

/*
float
phase(
	const float theta,
	const float g)
{
	return
	clamp(
		(1.0 - g * g) /
		pow(1 + g * g - 2.0 * g * theta, 1.5),
		0.0,
		1.0);
}
*/

vec3 sun = normalize(vec3(sin(offset),0.0,cos(offset)));

float factor = opacity;

void
main()
{
	vec4 dst = vec4(0.0);
	vec3 direction = normalize( position_interp - camera );

	vec3 position;

	if(any(greaterThan(abs(camera), vec3(1.0))))
		position = position_interp;
	else
		position = camera;

	position = (position + 1.0) * 0.5;
	vec3 startpos = position;

	vec2 sample = vec2(0.0);

	for( int i = 0; i < steps; i++ )
	{
		float value = clamp(5.0 * texture(tex, position).r,0.0,1.0);

		sample.x = value;

		float light =
			texture(phasetex,vec2(
					0.5 * dot(sun, -direction) + 0.5,
					value)).r;

		vec3 col = light * suncolor + ambient;

		dst += (1.0 - dst.a) * factor * factor * sample.y * vec4(col, 1.0);

		sample.y = 0.5 * (sample.x + sample.y);

		if(dst.a > 0.95)
			break;

		// position = position + direction * stepsize;
		position = startpos + i * direction * stepsize;

		/*
		if (outside_sphere(position, center, radius_limit))
			break;
		*/

		// ray termination
		if (outside_unit_cube(position))
			break;
	}
	// homogenize color values
	dst = vec4((1.0 / dst.a) * dst.rgb, dst.a);
	frag_color = dst;
}
