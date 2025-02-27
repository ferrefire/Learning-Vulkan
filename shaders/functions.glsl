#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

#ifndef MAX_TRIANGLE_POINTS
#define MAX_TRIANGLE_POINTS 25
#endif

#define POW(x) (x * x)

struct Triangle
{
    vec2 p1;
    vec2 p2;
    vec2 p3;
};

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) 
{
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}



// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }



// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) 
{
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float Random01( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float Random01( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float Random01( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float Random01( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

float Random11( float x ) { return (Random01(x) - 0.5) * 2.0; }
float Random11( vec2  v ) { return (Random01(v) - 0.5) * 2.0; }
float Random11( vec3  v ) { return (Random01(v) - 0.5) * 2.0; }
float Random11( vec4  v ) { return (Random01(v) - 0.5) * 2.0; }

float SquaredDistance(vec3 p1, vec3 p2)
{
	vec3 difference = p2 - p1;
	return (dot(difference, difference));
}

float SquaredDistanceVec2(vec2 p1, vec2 p2)
{
	vec2 difference = p2 - p1;
	return (dot(difference, difference));
}

int ValidUV(vec2 uv)
{
	return (abs(uv.x - 0.5) <= 0.5 && abs(uv.y - 0.5) <= 0.5) ? 1 : 0;
}

bool InsideTriangle(Triangle t, vec2 p)
{
    float s1 = (t.p2.x - t.p1.x) * (p.y - t.p1.y) - (t.p2.y - t.p1.y) * (p.x - t.p1.x);
    float s2 = (t.p3.x - t.p2.x) * (p.y - t.p2.y) - (t.p3.y - t.p2.y) * (p.x - t.p2.x);
    float s3 = (t.p1.x - t.p3.x) * (p.y - t.p3.y) - (t.p1.y - t.p3.y) * (p.x - t.p3.x);

    if ((s1 > 0.0 && s2 > 0.0 && s3 > 0.0) || (s1 < 0.0 && s2 < 0.0 && s3 < 0.0)) return (true);
    else return (false);
}

vec3 NormalizeSum(vec3 vec)
{
    float sum = vec.x + vec.y + vec.z;
    vec /= sum;

    return (vec);
}


#endif