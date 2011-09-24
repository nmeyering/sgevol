#version 140

$$$HEADER$$$

out vec3 position_interp;
out vec2 texcoord_interp;
//out vec3 normal_interp;

void
main()
{
	gl_Position =
		mvp * vec4(0.8 * position,1.0);
	position_interp =
		vec3(mv * vec4(position,1.0));
    // position;
	texcoord_interp = texcoord;
	//normal_interp = normalize(vec3(normal_matrix * vec4(normal,1.0)));
}
