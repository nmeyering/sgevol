#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

const float stepsize = 0.0005;
const int steps = int(sqrt(3.0)/stepsize);
const vec3 center = vec3(0.5,0.5,0.5);
const float PI = 3.1415926;

float simplex_noise(vec3);

void
main()
{
	vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize(position_interp - camera);
	vec3 position;
	// scaling factor for uniform cloud data
	float factor = (stepsize * 10000) * opacity;
	float value;

	// cutoff - surface is reached here
	float radius_limit = 0.5 * radius;

	// TODO: rendering from the inside?

	position = (position_interp + 1.0) * 0.5;

	// jitter
	// position += 0.0002 * 0.25 * simplex_noise(vec3(gl_FragCoord.xy,0.0));
	vec3 starting_pos = position;

	for(int i = 0; i < 1; i++)
	{
		float radius = distance(position, center);
		// ray termination - sphere
		if (radius < radius_limit)
			break;
		if (radius > 0.51)
			break;

		value = 0.5 * simplex_noise(10 * position)
					+ 0.25 * simplex_noise(20 * position)
					+ 0.125 * simplex_noise(40 * position);
		value *= 0.1;

		// smooth falloff towards surface?
		// value *= clamp(5 * (radius - radius_limit) / (0.51 - radius_limit), 0.0, 1.0);

		//float xxx = polar(position * 2.0 - 1.0).y;
		//vec3 color = vec3(1.0 - xxx, xxx, 0.0);
		dst += (1.0 - dst.a) * factor * vec4(1.0,1.0,1.0, value);

		/*
		if(dst.a > 0.95)
			break;
		*/

		// float-Ungenauigkeit?!
		// position = position + direction * stepsize;
		position = starting_pos + i * direction * stepsize;


		/*
		// ray termination - cube
		vec3 temp1 = sign(position);
		vec3 temp2 = sign(vec3( 1.0, 1.0, 1.0 ) - position);
		float inside = dot(temp1, temp2);

		// outside
		if (inside < 3.0)
			break;
		*/
	}
	float m = max(max(dst.r, dst.g), max(dst.b, dst.a));
	vec4 result = dst;
	if(m > 1.0)
		result = vec4(dst.r / m, dst.g / m, dst.b / m, dst.a);
	frag_color = result;
}
