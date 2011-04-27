#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

uniform float stepsize = 0.02;
uniform int steps = 100;
uniform vec3 sun = vec3(0.0,1.0,0.0);
uniform float delta = 0.005;

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
	vec4 offset_value;
  vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize( position_interp - camera );
  vec3 position;
	float factor = 0.32;

	//jeden Ray eine Stepsize in das Volume laufen lassen,
	//um Sampling-Artefakte an den Rändern zu vermeiden.
	//position = position + direction * stepsize;

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
		//position = position;
	}
	position = (position + 1.0) * 0.5;
	//position += 0.1 * offset;

  for( int i = 0; i < steps; i++ )
  {
		vec4 value = texture( tex, position );
		// vec4 value = vec4(1.0,1.0,1.0,abs(dot(sun,gradient(position))));
		// float light = dot(gradient(position),sun);
		
		// dst += (1.0 - dst.a) * (factor * value + vec4(1.0,0.0,0.0,light * 0.1));
		dst += (1.0 - dst.a) * value * factor;
		if( dst.a >= 0.95 )
			break;
    
    position = position + direction * stepsize;

    // ray termination
		/*
    vec3 temp1 = sign( position );
    vec3 temp2 = sign( vec3( 1.0, 1.0, 1.0 ) - position );
    float inside = dot( temp1, temp2 );
    
		// outside
    if ( inside < 3.0 )
      break;
		*/
  }
	frag_color = dst;
}
