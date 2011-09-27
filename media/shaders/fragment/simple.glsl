#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

const float stepsize = 0.01;
const int steps = int(sqrt(3.0)/stepsize);
const vec3 center = vec3(0.5,0.5,0.5);
const float pi = 3.1415925;
const float twopi = 2.0 * pi;

void
main()
{
  vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize(position_interp - camera);
  vec3 position;
	// scaling factor for uniform cloud data
	float factor = stepsize * 4000;
	float value;

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
	position = (position_interp + 1.0) * 0.5;

  for(int i = 0; i < steps; i++)
  {
		value = texture(tex, position).r;

		if (distance(position, center) > 0.5)
			value = 0.0;

		dst += (1.0 - dst.a) * factor * vec4(1.0, 1.0, 1.0, value);

		if(dst.a > 0.95)
			break;

    position = position + direction * stepsize;

    // ray termination - sphere
		if (distance(position, center) < 0.475)
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
