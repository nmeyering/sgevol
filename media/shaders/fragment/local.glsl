#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

const float stepsize = 5e-3;
const int steps = int(sqrt(3.0)/stepsize);
const float delta = 0.080;
const vec3 center = vec3(0.5,0.5,0.5);
const vec3 skycolor = vec3(0.8,0.85,1.0);
const vec3 suncolor = 0.5 * vec3(1.0,1.0,0.8);
const vec3 sky = vec3(0.0,-1.0,0.0);
const vec3 ambient = vec3(stepsize * 0.1);

// vec3 sun = normalize(vec3(sin(offset),0.0,cos(offset)));
vec3 sun = normalize(vec3(0.0,-1.0,0.0));

// float radius_limit = 0.498 * radius;
float factor = stepsize * opacity;

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
	if (length(res) < 0.02)
		return vec3(0.,0.,0.);
	else
		return normalize(res);
}

void
main()
{
	vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize( position_interp - camera );

	/*
	if(
		abs( camera.x ) < 1.0 ||
		abs( camera.y ) < 1.0 ||
		abs( camera.z ) < 1.0
		)
	{
		position = camera;
	}
	else
	{
		position = position_interp;
	}
	position = (position + 1.0) * 0.5;
	*/
	vec3 position = (position_interp + 1.0) * 0.5;

	for( int i = 0; i < steps; i++ )
	{
		float value = texture(tex, position).r;

		float sunlight = 1e-2 * max(0.0, dot(gradient(position),sun));
		vec3 col = min(sunlight * suncolor, 1.0);
		dst += (1.0 - dst.a) * factor * vec4(col + ambient,value);

		if( dst.a > 0.99 )
			break;

		position = position + direction * stepsize;

		//if (distance(position, center) < radius_limit)
		//	break;

		// ray termination
		vec3 temp1 = sign( position );
		vec3 temp2 = sign( vec3( 1.0, 1.0, 1.0 ) - position );
		float inside = dot( temp1, temp2 );

		// outside
		if ( inside < 3.0 )
			break;
	}
	/*
	float m = max(max(dst.r, dst.g), max(dst.b, dst.a));
	vec4 result = dst;
	if(m > 1.0)
		result = vec4(dst.r / m, dst.g / m, dst.b / m, dst.a);
	*/
	frag_color = dst;
}
