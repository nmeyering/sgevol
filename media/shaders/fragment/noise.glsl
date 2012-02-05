#version 140
#define	NORMALIZE_GRADIENTS
#undef	USE_CIRCLE
#define	COLLAPSE_SORTNET

$$$HEADER$$$

in vec3 position_interp;
out vec4 frag_color;

float permute(float x0,vec3 p) {
	float x1 = mod(x0 * p.y, p.x);
	return floor(  mod( (x1 + p.z) *x0, p.x ));
}
vec2 permute(vec2 x0,vec3 p) {
	vec2 x1 = mod(x0 * p.y, p.x);
	return floor(  mod( (x1 + p.z) *x0, p.x ));
}
vec3 permute(vec3 x0,vec3 p) {
	vec3 x1 = mod(x0 * p.y, p.x);
	return floor(  mod( (x1 + p.z) *x0, p.x ));
}
vec4 permute(vec4 x0,vec3 p) {
	vec4 x1 = mod(x0 * p.y, p.x);
	return floor(  mod( (x1 + p.z) *x0, p.x ));
}

uniform vec4 pParam;
// Example constant with a 289 element permutation
//const vec4 pParam = vec4( 17.0*17.0, 34.0, 1.0, 7.0);

float taylorInvSqrt(float r)
{
	return ( 0.83666002653408 + 0.7*0.85373472095314 - 0.85373472095314 * r );
}

float simplexNoise3(vec3 v)
{
	const vec2	C = vec2(1./6. , 1./3. ) ;
	const vec4	D = vec4(0., 0.5, 1.0, 2.0);

// First corner
	vec3 i	= floor(v + dot(v, C.yyy) );
	vec3 x0 =		v - i + dot(i, C.xxx) ;

// Other corners
#ifdef COLLAPSE_SORTNET
	vec3 g = vec3( greaterThan(		x0.xyz, x0.yzx) );
	vec3 l = vec3( lessThanEqual( x0.xyz, x0.yzx) );

	vec3 i1 = g.xyz  * l.zxy;
	vec3 i2 = max( g.xyz, l.zxy);
#else
// Keeping this clean - let the compiler optimize.
	vec3 q1;
	q1.x = max(x0.x, x0.y);
	q1.y = min(x0.x, x0.y);
	q1.z = x0.z;

	vec3 q2;
	q2.x = max(q1.x,q1.z);
	q2.z = min(q1.x,q1.z);
	q2.y = q1.y;

	vec3 q3;
	q3.y = max(q2.y, q2.z);
	q3.z = min(q2.y, q2.z);
	q3.x = q2.x;

	vec3 i1 = vec3(equal(q3.xxx, x0));
	vec3 i2 = i1 + vec3(equal(q3.yyy, x0));
#endif

	 //  x0 = x0 - 0. + 0. * C
	vec3 x1 = x0 - i1 + 1. * C.xxx;
	vec3 x2 = x0 - i2 + 2. * C.xxx;
	vec3 x3 = x0 - 1. + 3. * C.xxx;

// Permutations
	i = mod(i, pParam.x );
	vec4 p = permute( permute( permute(
						 i.z + vec4(0., i1.z, i2.z, 1. ), pParam.xyz)
					 + i.y + vec4(0., i1.y, i2.y, 1. ), pParam.xyz)
					 + i.x + vec4(0., i1.x, i2.x, 1. ), pParam.xyz);

// Gradients
// ( N*N points uniformly over a square, mapped onto a octohedron.)
	float n_ = 1.0/pParam.w ;
	vec3	ns = n_ * D.wyz - D.xzx ;

	vec4 j = p - pParam.w*pParam.w*floor(p * ns.z *ns.z);  //  mod(p,N*N)

	vec4 x_ = floor(j * ns.z)  ;
	vec4 y_ = floor(j - pParam.w * x_ ) ;		 // mod(j,N)

	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1. - abs(x) - abs(y);

	vec4 b0 = vec4( x.xy, y.xy );
	vec4 b1 = vec4( x.zw, y.zw );

	vec4 s0 = vec4(lessThan(b0,D.xxxx)) *2. -1.;
	vec4 s1 = vec4(lessThan(b1,D.xxxx)) *2. -1.;
	vec4 sh = vec4(lessThan(h, D.xxxx));

	vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
	vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

	vec3 p0 = vec3(a0.xy,h.x);
	vec3 p1 = vec3(a0.zw,h.y);
	vec3 p2 = vec3(a1.xy,h.z);
	vec3 p3 = vec3(a1.zw,h.w);

#ifdef NORMALISE_GRADIENTS
	p0 *= taylorInvSqrt(dot(p0,p0));
	p1 *= taylorInvSqrt(dot(p1,p1));
	p2 *= taylorInvSqrt(dot(p2,p2));
	p3 *= taylorInvSqrt(dot(p3,p3));
#endif

// Mix
	vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.);
	m = m * m;
//used to be 64.
	return 48.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
																dot(p2,x2), dot(p3,x3) ) );
}

void main(void)
{
	//vec3 v = vec3(2.0 * v_texCoord3D.xyz * (2.0 + sin(0.5*time)));
	float n = simplexNoise3(position_interp*5.0);
	//frag_color = vec4(0.5 + 0.5 * vec3(n, n, n), 1.0);
	frag_color = vec4(n, n, n, 1.0);
}
