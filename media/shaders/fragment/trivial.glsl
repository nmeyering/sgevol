#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

// Haupt-Parameter fürs Raycasting
// selbsterklärend...
const float stepsize = 5e-3;
// Hauptdiagonale des Würfels ist die längste mögliche Strecke
const int steps = int(sqrt(3.0)/stepsize);

// höhere Werte ~= höhere Dichte
// const float opacity = 25.0;

void
main()
{
	vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize(position_interp - camera);
	// scaling density for uniform cloud data
	float density = stepsize * opacity;
	float value;

	// cube_scale = 64.0 oder so muss von außen reinkommen
	// vec3 position = position_interp / cube_scale;
	vec3 position = 0.5 * (position_interp + 1.0);

	vec3 starting_position = position;

	for(int i = 0; i < steps; i++)
	{
		value = texture(tex, position).r;

		dst += (1.0 - dst.a) * density * vec4(1.0,0.2,0.1, value);

		// position = starting_position + stepsize * direction * i;
		position += direction * stepsize;

		// ray termination - threshold
		if(dst.a > 0.98)
			break;

		// ray termination - cube borders
		vec3 temp1 = sign(position);
		vec3 temp2 = sign(1.0 - position);
		if (dot(temp1, temp2) < 3.0)
			break;
	}

	frag_color = dst;
}
