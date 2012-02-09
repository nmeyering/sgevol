#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

#define PI 3.141592653589793

bool
outside_unit_cube(const vec3);

bool
outside_sphere(const vec3, const vec3, const float);

float
simplex_noise(const vec3);

const
vec3
polar(
	const in vec3 p)
{
	float r = length(p);
	float phi = atan(p.z,p.x);
	float theta = acos(p.y/r);
	return vec3(
		r,
		phi/(2.0 * PI) + 0.5,
		theta/PI);
}

// Haupt-Parameter fürs Raycasting
// selbsterklärend...
const float stepsize = 1e-3;
// Hauptdiagonale des Würfels ist die längste mögliche Strecke
const int steps = int(sqrt(3.0)/stepsize);
const vec3 center = vec3(0.5,0.5,0.5);
float radius_limit = 0.498 * radius;
float cloud_height = 1.0 - radius_limit;

void
main()
{
	float value;
	vec4 dst = vec4(0.0);
	vec3 direction = normalize(position_interp - camera);
	// scaling factor for uniform cloud data
	float factor = opacity * (stepsize / 2e-2);

	vec3 position = (position_interp + 1.0) * 0.5;

	//stochastic jittering
	//position -= stepsize * simplex_noise(vec3(gl_FragCoord.xy,0.0));

	for(int i = 0; i < steps; i++)
	{
		vec3 polar_pos = polar(position * 2.0 - 1.0);
		value =
			texture(
				tex,
				vec3(
					polar_pos.x * cloud_height + radius_limit,
					polar_pos.yz
				)).r;
			/*
			texture(
				clouds,
				polar_pos.yz).r;
				*/
		float cloudiness = texture(clouds, polar_pos.yz).r;
		vec3 col = vec3(cloudiness, 0.0, 1.0 - cloudiness);
		dst += (1.0 - dst.a) * factor * value * vec4(col, 1.0);

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
