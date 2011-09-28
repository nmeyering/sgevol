#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

const float stepsize = 0.001;
const int steps = int(sqrt(3.0)/stepsize);
const vec3 center = vec3(0.5,0.5,0.5);

void
main()
{
  vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize(position_interp - camera);
  vec3 position;
	// scaling factor for uniform cloud data
	float factor = stepsize * 4000;
	float value;

	// TODO: rendering from the inside?

	position = (position_interp + 1.0) * 0.5;

	float jitter = texture(noise, 90 * position).r;
	position -= stepsize * 1.0 * jitter;

  for(int i = 0; i < steps; i++)
  {
		value = texture(tex, position).r;

    // ray termination - sphere
		if (distance(position, center) < 0.475)
			break;
		if (distance(position, center) > 0.51)
			break;

		dst += (1.0 - dst.a) * factor * vec4(1.0, 1.0, 1.0, value);

		/*
		if(dst.a > 0.95)
			break;
		*/

    position = position + direction * stepsize;


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
