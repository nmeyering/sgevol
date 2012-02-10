#version 140

$$$HEADER$$$

in vec2 texcoord_interp;
in vec3 normal_interp;
in vec3 position_interp;
out vec4 frag_color;

const float ambient = 0.0;
vec3 sun_direction = normalize(vec3(sin(offset),0.0,cos(offset)));

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
			mix(
				texture(
					day_tex,
					texcoord_interp).xyz,
				texture(
					night_tex,
					texcoord_interp).xyz,
				intensity),
			1.0);
	#else
	frag_color = vec4(normal_interp,1.0);
	#endif
}
