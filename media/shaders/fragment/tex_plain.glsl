#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

void main()
{
	frag_color = texture(tex, position_interp);
}
