#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

// Haupt-Parameter fürs Raycasting
// selbsterklärend...
const float stepsize = 5e-3;
// Hauptdiagonale des Würfels ist die längste mögliche Strecke
const int steps = int(sqrt(3.0)/stepsize);

bool
outside_unit_cube(const vec3);

void
main()
{
	vec4 dst = vec4(0.0,0.0,0.0,0.0);
	vec3 direction = normalize(position_interp - camera);
	// scaling factor for uniform cloud data
	float factor = opacity;
	float value;

	vec3 position;
	if(any(greaterThan(abs(camera), vec3(1.0))))
		position = position_interp;
	else
		position = camera;
	position = (position + 1.0) * 0.5;

	for(int i = 0; i < steps; i++)
	{
		value = texture(tex, position).r;

		vec3 col = vec3(1.0);
		dst += (1.0 - dst.a) * factor * value * vec4(col, 1.0);

		position += direction * stepsize;

		// ray termination - threshold
		if(dst.a > 0.98)
			break;

		// ray termination - cube borders
		if (outside_unit_cube(position))
			break;
	}
	dst = vec4((1.0 / dst.a) * dst.rgb, dst.a);
	frag_color = dst;
}
