#ifndef CURVE_INCLUDED
#define CURVE_INCLUDED

#define LOWLANDS vec2(0.0, 0.0), vec2(0.25, 0.15), vec2(1.0, 0.0), vec2(1.0, 1.0)
#define MIDLANDS vec2(0.0, 0.0), vec2(0.25, 0.375), vec2(0.85, 0.15), vec2(1.0, 1.0)

float CubicCurve(vec2 P1, vec2 P2, vec2 P3, vec2 P4, float t)
{
	vec2 r1 = pow(1.0 - t, 3) * P1;
	vec2 r2 = 3 * pow(1.0 - t, 2) * t * P2;
	vec2 r3 = 3 * (1.0 - t) * pow(t, 2) * P3;
	vec2 r4 = pow(t, 3) * P4;

	vec2 r = r1 + r2 + r3 + r4;

	return (r.y);
}

#endif