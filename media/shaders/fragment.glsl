#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

uniform float stepsize = 0.05;
uniform int steps = 100;
//uniform vec3 sun = vec3(0.0,0.0,-1.0);

void
main()
{
  vec4 value;
  vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 direction = normalize( position_interp - camera );
  vec3 position;

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
	position += 0.1 * offset;
	//float light = abs( dot( sun, direction ) );

  for( int i = 0; i < steps; i++ )
  {
    value = texture( tex, clamp(position, 0.0, 1.0) );
    
		vec4 src = value;

    dst = (1.0 - dst.a) * src * 0.05 + dst;
    //dst = (1.0 - dst.a) * src + vec4(vec3(dst.a * dst), dst.a);
    //dst = (1.0 - dst.a) * src + dst.a * dst;
		//dst = dst + value;

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

  //frag_color = vec4( position_interp, 0.5 );
  //frag_color = vec4(1.0,1.0,1.0,1.0);
}
