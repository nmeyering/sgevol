#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

const float stepsize = 5e-3;
const int steps = int(sqrt(3.0)/stepsize);
const float delta = 2e-2;
const float halfdelta = 0.5 * delta;
const vec3 center = vec3(0.5,0.5,0.5);
const vec3 skycolor = 0.2 * vec3(0.8,0.85,1.0);
const vec3 suncolor = vec3(1.0,1.0,0.95);
const vec3 ambient = vec3(0.1);

bool
outside_unit_cube(const vec3);

bool
outside_sphere(
	const vec3,
	const vec3,
	const float);

vec3 sun = normalize(vec3(sin(offset),0.0,cos(offset)));
// vec3 sun = normalize(vec3(0.0,-1.0,0.0));
const vec3 sky = vec3(0.0,1.0,0.0);

// float radius_limit = 0.498 * radius;
float factor = opacity;

vec3
gradient(vec3 point)
{
	vec3 sample1, sample2, res;

	sample1.x = texture(tex, point - vec3(delta,0.0,0.0)).r;
	sample2.x = texture(tex, point + vec3(delta,0.0,0.0)).r;
	sample1.y = texture(tex, point - vec3(0.0,delta,0.0)).r;
	sample2.y = texture(tex, point + vec3(0.0,delta,0.0)).r;
	sample1.z = texture(tex, point - vec3(0.0,0.0,delta)).r;
	sample2.z = texture(tex, point + vec3(0.0,0.0,delta)).r;

	res = sample2 - sample1;
	return normalize(res);
}

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

	for( int i = 0; i < steps; i++ )
	{
		float value = texture(tex, position).r;

		vec3 grad = gradient(position);
		float sunlight = max(0.0, dot(sun, grad));
		// float skylight = max(0.0, pow(dot(grad, sky), 0.1));
		// vec3 col = sunlight * suncolor + skylight * skycolor;
		vec3 col = sunlight * suncolor + ambient;

		dst += (1.0 - dst.a) * factor * value * vec4(col,1.0);

		if(dst.a > 0.99)
			break;

		position = position + direction * stepsize;

		/*
		if (outside_sphere(position, center, radius_limit))
			break;
		*/

		// ray termination
		if (outside_unit_cube(position))
			break;
	}

	dst = vec4((1.0 / dst.a) * dst.rgb, dst.a);
	frag_color = dst;
}
