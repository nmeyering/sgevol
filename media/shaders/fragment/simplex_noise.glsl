#version 140

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

const int N = 3;
const int gradient_length = N * 2;
const int perm_length = 128;
uniform vec3 gradients[N * 2] = vec3[](
	vec3( 1.0,0.0,0.0),
	vec3(-1.0,0.0,0.0),
	vec3(0.0, 1.0,0.0),
	vec3(0.0,-1.0,0.0),
	vec3(0.0,0.0, 1.0),
	vec3(0.0,0.0,-1.0));
uniform int perm[128] = int[](53, 95, 14, 64, 54, 71, 52, 12, 55, 16, 21, 38, 10, 110, 74, 22, 18, 115, 29, 104, 125, 77, 60, 37, 6, 92, 25, 23, 9, 85, 112, 94, 27, 4, 34, 100, 69, 118, 111, 40, 75, 65, 126, 63, 13, 91, 19, 76, 67, 58, 11, 20, 97, 28, 83, 114, 72, 99, 59, 81, 47, 0, 50, 5, 89, 2, 31, 8, 116, 90, 36, 39, 123, 87, 122, 96, 107, 35, 61, 98, 68, 113, 49, 127, 119, 24, 30, 15, 102, 78, 79, 42, 117, 51, 46, 43, 41, 103, 105, 88, 80, 44, 109, 62, 3, 1, 101, 124, 121, 7, 66, 56, 82, 57, 45, 32, 26, 33, 70, 108, 86, 120, 106, 93, 84, 17, 48, 73);

vec3[N+1]
corners(
	vec3 point)
{
	vec3 res[N+1];
	vec3 cur;
	float max_f = -1.0;
	int max_i = 0;

	res[0] = cur;

	for (int j = 0; j < N; ++j)
	{
		for (int i = 0; i < N; ++i)
			if (point[i] > max_f)
			{
				max_f = point[i];
				max_i = i;
			}
		max_f = -1.0;
		point[max_i] = -2.0;
		cur[max_i] += 1.0;
		res[j+1] = cur;
	}

	return res;
}

float
stretch_factor()
{
	// N = 3
	return
		1.0/(1.0 + sqrt(1.0 + N));
}

float
inv_factor()
{
	// N = 3
	return
		-1.0/(1.0 + N + sqrt(1.0 + N));
}

mat3
stretch_m()
{
	mat3 tmp;
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			tmp[i][j] =
				stretch_factor() +
				((i == j)? 1 : 0 );
	return tmp;
}

mat3
inv_m()
{
	mat3 tmp;
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			tmp[i][j] =
				inv_factor() +
				((i == j)? 1 : 0 );
	return tmp;
}

int
index(
	const vec3 vec)
{
	ivec3 ivec = ivec3(vec);
	return
		perm[
			(perm[
				(perm[
					ivec[0] %
					perm_length
				]
				+ ivec[1]) %
				perm_length
			]
			+ ivec[2]) %
			perm_length
		];
}

float
contrib(
	vec3 v,
	vec3 intv)
{
	float t = 0.6;
	t -= dot(v,v);
	if (t < 0.0)
		return 0.0;

	t *= t;
	return t * t * dot(
		gradients[
			index(intv) %
			gradient_length
		],
		v);
}

float
sample(
	vec3 point)
{
	float res = 0.0;
	vec3 tmp = stretch_m() * point;
	tmp = floor(tmp);
	vec3 floored = tmp;
	tmp = inv_m() * tmp;
	tmp = point - tmp;
	vec3 offset = tmp;
	tmp = stretch_m() * tmp;

	vec3 c[] = corners(tmp);
	for (int i = 0; i < c.length(); ++i)
	{
		vec3 t = point - inv_m() * (floored + c[i]);
		res +=
			contrib(t, floored + c[i]);
	}

	return 48.f * res;
}

void
main()
{
	// two gradients per spacial dimension
	float n =
		0.5 * sample(position_interp * 4.0)
		+ 0.25 * sample(position_interp * 8.0)
		+ 0.125 * sample(position_interp * 16.0)
		+ 0.0625 * sample(position_interp * 32.0);
	n = 0.5 * n + 0.5;
	frag_color = vec4(1.0,1.0,1.0,n);
}
