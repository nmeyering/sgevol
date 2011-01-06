#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

void
main()
{
  float gray = 1.0;
  float stepsize = 0.10;
  vec4 value;
  vec4 dst = vec4(0,0,0,0);
  vec3 position = position_interp;
  position = (position + 1.0) * 0.5;
  vec3 direction = normalize( position - camera );

  for( int i = 0; i < 200; i++ )
  {
    value = texture( tex, position );
    
    vec4 src = value;

    dst = (1.0 - dst.a) * src * 0.05 + dst;
    
    position = position + direction * stepsize;

    // early termination
    vec3 temp1 = sign( position - vec3( -1.0, -1.0, -1.0 ) );
    vec3 temp2 = sign( vec3( 1.0, 1.0, 1.0 ) - position );
    float inside = dot( temp1, temp2 );
    
    if ( inside < 3.0 )
      break;
  }
	frag_color = dst;

}
