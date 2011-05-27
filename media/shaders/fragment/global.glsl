#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

uniform float stepsize = 0.01;
uniform int steps = 600;
uniform float delta = 0.04;

vec3
gradient(vec3 point)
{
	vec3 sample1, sample2, res;

	sample1.x = texture(tex, point - vec3(delta,0.0,0.0)).a;
	sample2.x = texture(tex, point + vec3(delta,0.0,0.0)).a;
	sample1.y = texture(tex, point - vec3(0.0,delta,0.0)).a;
	sample2.y = texture(tex, point + vec3(0.0,delta,0.0)).a;
	sample1.z = texture(tex, point - vec3(0.0,0.0,delta)).a;
	sample2.z = texture(tex, point + vec3(0.0,0.0,delta)).a;

	res = sample2 - sample1;
	if (length(res) < 0.02)
		return vec3(0.,0.,0.);
	else
		return normalize(res);
}

void
main()
{
  vec4 value;
  vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize( position_interp - camera );
  vec3 position;
	// scaling factor for uniform cloud data
	float factor = 1.0;

	vec3 skycolor = vec3(0.8,0.85,1.0);
	vec3 suncolor = 0.5 * vec3(1.0,1.0,0.8);
	vec3 sun = normalize(vec3(sin(offset),0.0,cos(offset)));
	vec3 sky = vec3(0.0,-1.0,0.0);

	position = (position_interp + 1.0) * 0.5;

	// main raycasting loop
  for (int i = 0; i < steps; i++)
  {
		vec4 value = texture(tex, position);

		float light = 1.0;
		vec3 occ_pos = position;
		vec3 occ_dir = vec3(0.0,-1.0,0.0);

		// volume shadow loop
		for (int j = 0; j < steps; j++)
		{
			occ_pos += occ_dir * stepsize;
			light -= texture(tex, position).a;

			if (light < 0.05)
				break;

			vec3 temp1 = sign( occ_pos );
			vec3 temp2 = sign( vec3( 1.0, 1.0, 1.0 ) - occ_pos );
			float inside = dot( temp1, temp2 );
			
			// outside
			if ( inside < 3.0 )
				break;
		}

		light *= 0.050;
		// dst += (1.0 - dst.a) * factor * value * vec4(suncolor * (1.0 - occlusion),1.0);
		vec3 col = light * suncolor;
		dst += (1.0 - dst.a) * factor * value * vec4(col, 1.0);

		if( dst.a > 0.95 )
			break;
    
    position += direction * stepsize;

    // ray termination
    vec3 temp1 = sign( position );
    vec3 temp2 = sign( vec3( 1.0, 1.0, 1.0 ) - position );
    float inside = dot( temp1, temp2 );
    
		// outside
    if ( inside < 3.0 )
      break;
  }
	float m = max(max(dst.r, dst.g), max(dst.b, dst.a));
	vec4 result = dst;
	if(m > 1.0)
		result = vec4(dst.r / m, dst.g / m, dst.b / m, dst.a);
	frag_color = result;
}