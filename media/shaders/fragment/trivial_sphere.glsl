#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

bool
outside_unit_cube(const vec3);

bool
outside_sphere(const vec3, const vec3, const float);

float
simplex_noise(const vec3);

// Haupt-Parameter fürs Raycasting
// selbsterklärend...
const float stepsize = 1e-3;
// Hauptdiagonale des Würfels ist die längste mögliche Strecke
const int steps = int(sqrt(3.0)/stepsize);
const vec3 center = vec3(0.5,0.5,0.5);
float radius_limit = 0.498 * radius;

void
main()
{
	float value;
	vec4 dst = vec4(0.0);
	vec3 direction = normalize(position_interp - camera);
	// scaling factor for uniform cloud data
	float factor = opacity * (stepsize / 2e-2);

	#if 0
	if(any(greaterThan(abs(camera), vec3(1.0))))
		position = position_interp;
	else
		position = camera;
	#endif
	vec3 position = (position_interp + 1.0) * 0.5;

	//stochastic jittering
	position -= stepsize * direction * simplex_noise(vec3(gl_FragCoord.xy,0.0));
	// position -= 1.414 * stepsize * direction * texture(clouds, gl_FragCoord.xy).r;
	vec3 starting_position = position;

	for(int i = 0; i < steps; i++)
	{
		value = texture(tex, position).r;

		vec3 col = vec3(1.0);
		dst += (1.0 - dst.a) * factor * value * vec4(col, 1.0);

		position = starting_position + i * direction * stepsize;

		// ray termination - threshold
		/*
		if(dst.a > 0.98)
			value += 0.0001;

*/
		// ray termination - cube borders
		if (outside_sphere(position, center, radius))
			break;

		if (!outside_sphere(position, center, radius_limit))
			break;
	}
	dst = vec4((1.0 / dst.a) * dst.rgb, dst.a);
	frag_color = dst;
}
