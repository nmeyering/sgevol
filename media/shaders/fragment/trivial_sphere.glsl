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
	vec2 sample = vec2(0.0);
	float value;
	vec4 dst = vec4(0.8,0.8,0.9,0.0);
	vec3 direction = normalize(position_interp - camera);
	// scaling factor for uniform cloud data
	float factor = opacity * (stepsize / 5e-3);

	vec3 position;
	if(any(greaterThan(abs(camera), vec3(1.0))))
		position = position_interp;
	else
		position = camera;
	position = (position + 1.0) * 0.5;

	position -= stepsize * simplex_noise(vec3(gl_FragCoord.xy,0.0));

	for(int i = 0; i < steps; i++)
	{
		value = texture(tex, position).r;
		sample.x = value;

		vec3 col = vec3(1.0);
		dst += (1.0 - dst.a) * factor * sample.y * vec4(col, 1.0);

		sample.y = 0.5 * (sample.x + sample.y);

		position += direction * stepsize;

		// ray termination - threshold
		if(dst.a > 0.98)
			break;

		// ray termination - cube borders
		if (outside_unit_cube(position))
			break;

		if (!outside_sphere(position, center, radius_limit))
			break;
	}
	dst = vec4((1.0 / dst.a) * dst.rgb, dst.a);
	frag_color = dst;
}
