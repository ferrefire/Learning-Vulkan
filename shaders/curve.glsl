#ifndef CURVE_INCLUDED
#define CURVE_INCLUDED

struct Curve
{
	vec2 p0;
	vec2 p1;
	vec2 p2;
	vec2 p3;
};

struct Curve3D
{
	vec3 p0;
	vec3 p1;
	vec3 p2;
	vec3 p3;
};

#define LOWLANDS vec2(0.0, 0.0), vec2(0.25, 0.15), vec2(1.0, 0.0), vec2(1.0, 1.0)
#define MIDLANDS vec2(0.0, 0.0), vec2(0.25, 0.375), vec2(0.85, 0.15), vec2(1.0, 1.0)

vec3 CubicDerivative(vec3 P0, vec3 P1, vec3 P2, vec3 P3, float t)
{
	float t2 = pow(t, 2.0);

	vec3 r0 = P0 * (-3 * t2 + 6 * t - 3);
	vec3 r1 = P1 * (9 * t2 - 12 * t + 3);
	vec3 r2 = P2 * (-9 * t2 + 6 * t);
	vec3 r3 = P3 * (3 * t2);

	vec3 r = r0 + r1 + r2 + r3;

	return (r);
}

vec3 CubicCurve(vec3 P0, vec3 P1, vec3 P2, vec3 P3, float t)
{
	//vec3 r1 = pow(1.0 - t, 3) * P1;
	//vec3 r2 = 3 * pow(1.0 - t, 2) * t * P2;
	//vec3 r3 = 3 * (1.0 - t) * pow(t, 2) * P3;
	//vec3 r4 = pow(t, 3) * P4;

	float t2 = pow(t, 2.0);
	float t3 = pow(t, 3.0);

	vec3 r0 = P0 * (-t3 + 3 * t2 - 3 * t + 1);
	vec3 r1 = P1 * (3 * t3 - 6 * t2 + 3 * t);
	vec3 r2 = P2 * (-3 * t3 + 3 * t2);
	vec3 r3 = P3 * (t3);

	vec3 r = r0 + r1 + r2 + r3;

	return (r);
}

vec2 CubicCurve(vec2 P1, vec2 P2, vec2 P3, vec2 P4, float t)
{
	vec2 r1 = pow(1.0 - t, 3) * P1;
	vec2 r2 = 3 * pow(1.0 - t, 2) * t * P2;
	vec2 r3 = 3 * (1.0 - t) * pow(t, 2) * P3;
	vec2 r4 = pow(t, 3) * P4;

	vec2 r = r1 + r2 + r3 + r4;

	return (r);
}

vec2 CubicCurve(Curve c, float t)
{
	return (CubicCurve(c.p0, c.p1, c.p2, c.p3, t));
}

vec3 CubicCurve3D(Curve3D c, float t)
{
	return (CubicCurve(c.p0, c.p1, c.p2, c.p3, t));
}

vec3 CubicDerivative3D(Curve3D c, float t)
{
	return (CubicDerivative(c.p0, c.p1, c.p2, c.p3, t));
}

float GetCurveLength(Curve c, int samples)
{
	float curveLength = 0.0;
	vec2 lastPoint = c.p0;

	for (int i = 0; i < samples; i++)
	{
		float sampleInter = float(i + 1) / float(samples);
		vec2 samplePoint = CubicCurve(c, sampleInter);
		curveLength += distance(lastPoint, samplePoint);
		lastPoint = samplePoint;
	}

	return (curveLength);
}

float GetCurveLength3D(Curve3D c, int samples)
{
	float curveLength = 0.0;
	vec3 lastPoint = c.p0;

	for (int i = 0; i < samples; i++)
	{
		float sampleInter = float(i + 1) / float(samples);
		vec3 samplePoint = CubicCurve3D(c, sampleInter);
		curveLength += distance(lastPoint, samplePoint);
		lastPoint = samplePoint;
	}

	return (curveLength);
}

vec2 CubicCurveDistance(Curve c, float dis)
{
	float curveLength = GetCurveLength(c, 8);
	float t = dis / curveLength;

	return (CubicCurve(c, t));
}

vec3 CubicCurveDistance3D(Curve3D c, float dis)
{
	float curveLength = GetCurveLength3D(c, 8);
	float t = dis / curveLength;

	return (CubicCurve3D(c, t));
}

#endif