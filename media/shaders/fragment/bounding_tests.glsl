bool
outside_unit_cube(
	const vec3 pos)
{
	return
		any(
			greaterThan(pos, vec3(1.0))) ||
		any(
			lessThan(pos, vec3(0.0)));
}

bool
outside_sphere(
	const vec3 pos,
	const vec3 center,
	const float radius)
{
	return
		distance(pos, center) > radius;
}
