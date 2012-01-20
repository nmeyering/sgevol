#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

const float stepsize = 1e-3;
const int steps = int(sqrt(3.0)/stepsize);
const vec3 center = vec3(0.5,0.5,0.5);
const float PI = 3.1415926;

float simplex_noise(vec3);

vec3
polar(
			 vec3 p)
{
	float r = length(p);
	float phi = atan(p.y,p.x);
	float theta = acos(p.z/r);
	// radius is defined externally
	return vec3(r,phi/(2.0 * PI) + 0.5,theta/PI);
}


void
main()
{
	vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize(position_interp - camera);
	vec3 position;
	// scaling factor for uniform cloud data
	float factor = stepsize * opacity;
	float value;

	// cutoff - surface is reached here
	float radius_limit = 0.498 * radius;

	// TODO: rendering from the inside?

	position = (position_interp + 1.0) * 0.5;

	position -= stepsize * simplex_noise(vec3(gl_FragCoord.xy,0.0));

	for(int i = 0; i < steps; i++)
	{
		// value = texture(tex, polar(position * 2.0 - 1.0)).r;
		value = texture(tex, position).r;

		//float xxx = polar(position * 2.0 - 1.0).y;
		//vec3 color = vec3(1.0 - xxx, xxx, 0.0);
		dst += (1.0 - dst.a) * factor * vec4(1.0,1.0,1.0, value);

		if(dst.a > 0.99)
			break;

		position = position + direction * stepsize;

		// ray termination - sphere
		if (distance(position, center) < radius_limit)
			break;
		if (distance(position, center) > 0.51)
			break;

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
