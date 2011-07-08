#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

uniform float stepsize = 0.005;
uniform int steps = 600;

void
main()
{
  vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize( position_interp - camera );
  vec3 position;
	// scaling factor for uniform cloud data
	float factor = 0.04;

	position = (position_interp + 1.0) * 0.5;

  for( int i = 0; i < steps; i++ )
  {
		float value = texture(tex, position).r;
		float shadow = texture(shadowtex, position).r;
		vec3 color = vec3(1.0-shadow,1.0-shadow,1.0-shadow);
		dst += (1.0 - dst.a) * factor * vec4(color,value);
		// dst += (1.0 - dst.a) * factor * vec4(color, shadow);

		if( dst.a > 0.95 )
			break;
    
    position = position + direction * stepsize;

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
