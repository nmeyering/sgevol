#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

//steps * stepsize > volume_diagonal
//=> steps * stepsize > sqrt(3)
uniform float stepsize = 0.01;
uniform int steps = 200;

void
main()
{
  vec4 value;
  vec4 dst = vec4(0.0, 0.0, 0.0, 0.0);
  vec3 position = position_interp;
  vec3 direction = normalize( position - camera );

  position = (position + 1.0) * 0.5;
	//jeden Ray eine Stepsize in das Volume laufen lassen,
	//um Sampling-Artefakte an den Rändern zu vermeiden.
	position = position + direction * stepsize;

  for( int i = 0; i < steps; i++ )
  {
    value = texture( tex, clamp(position, 0.0, 1.0) );
    
    vec4 src = value;

    //dst = (1.0 - dst.a) * src + dst;
    dst = (1.0 - dst.a) * src + vec4(vec3(dst.a * dst), dst.a);
    //dst = (1.0 - dst.a) * src + dst.a * dst;
		//dst = dst + value;

		//if( dst.a >= 0.95 )
		//break;
    
    position = position + direction * stepsize;

    // early termination
    vec3 temp1 = sign( position - vec3( 0.0, 0.0, 0.0 ) );
    vec3 temp2 = sign( vec3( 1.0, 1.0, 1.0 ) - position );
    float inside = dot( temp1, temp2 );
    
    if ( inside < 3.0 )
      break;
  }
	frag_color = dst;

  //frag_color = vec4( position_interp, 0.5 );
  //frag_color = vec4(1.0,1.0,1.0,1.0);
}