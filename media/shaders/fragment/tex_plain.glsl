#version 140

$$$HEADER$$$

in vec2 texcoord_interp;
in vec3 normal_interp;
in vec3 position_interp;
out vec4 frag_color;

const float ambient = 0.2;
//vec3 sun_direction = normalize(vec3(sin(offset),0.0,cos(offset)));
vec3 sun_direction = normalize(vec3(-1,0,-1));


void main()
{
	#if 1
	float
		intensity =
			ambient +
			max(
				0.0,
				dot(
					normal_interp,
					sun_direction));

	frag_color =
		vec4(
			intensity *
			texture(
				tex,
				texcoord_interp).xyz,
			1.0);
	#else
	frag_color = vec4(normal_interp,1.0);
	#endif
}
