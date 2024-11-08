#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

#ifndef SAMPLE_COUNT
#define SAMPLE_COUNT 1
#endif

#ifndef MAX_TRIANGLE_POINTS
#define MAX_TRIANGLE_POINTS 25
#endif

struct Line
{
	vec2 start;
	vec2 end;
};

struct Triangle
{
	vec2 p0;
	vec2 p1;
	vec2 p2;
};

struct Square
{
	vec2 p0;
	vec2 p1;
	vec2 p2;
	vec2 p3;
};

Line GetLine(Triangle triangle, int index)
{
	Line line;
	line.start = vec2(0);
	line.end = vec2(0);

	if (index == 0)
	{
		line.start = triangle.p0;
		line.end = triangle.p1;
	}
	else if (index == 1)
	{
		line.start = triangle.p1;
		line.end = triangle.p2;
	}
	else if (index == 2)
	{
		line.start = triangle.p2;
		line.end = triangle.p0;
	}
	return (line);
}

Line GetLine(Square square, int index)
{
	Line line;
	line.start = vec2(0);
	line.end = vec2(0);

	if (index == 0)
	{
		line.start = square.p0;
		line.end = square.p1;
	}
	else if (index == 1)
	{
		line.start = square.p1;
		line.end = square.p2;
	}
	else if (index == 2)
	{
		line.start = square.p2;
		line.end = square.p3;
	}
	else if (index == 3)
	{
		line.start = square.p3;
		line.end = square.p0;
	}
	return (line);
}

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

vec3[SAMPLE_COUNT] DivideLine(vec3 p1, vec3 p2)
{
	vec3[SAMPLE_COUNT] result;

	float iteration = 1.0 / (SAMPLE_COUNT + 1);

	for (int i = 1; i <= SAMPLE_COUNT; i++)
	{
		result[i - 1] = mix(p1, p2, iteration * i);
	}

	return (result);
}

double TriangleArea(vec2 p1, vec2 p2, vec2 p3)
{
	double a1 = p1.x * (p2.y - p3.y);
	double a2 = p2.x * (p3.y - p1.y);
	double a3 = p3.x * (p1.y - p2.y);
	double a = (a1 + a2 + a3) * 0.5;

	return (abs(a));
}

int PointInSquare(vec4 bounds, vec2 point)
{
	if (point.x < bounds.x) return (0);
	if (point.x > bounds.y) return (0);
	if (point.y < bounds.z) return (0);
	if (point.y > bounds.w) return (0);

	return (1);
}

int PointInTriangle(vec2 p1, vec2 p2, vec2 p3, vec2 point, float tolerance)
{
	//float minX = min(min(p1.x, p2.x), p3.x);
	//if (point.x < minX) return (0);
	//float maxX = max(max(p1.x, p2.x), p3.x);
	//if (point.x > maxX) return (0);
	//float minY = min(min(p1.y, p2.y), p3.y);
	//if (point.y < minY) return (0);
	//float maxY = max(max(p1.y, p2.y), p3.y);
	//if (point.y > maxY) return (0);

	double a = TriangleArea(p1, p2, p3);
	double a1 = TriangleArea(point, p2, p3);
	double a2 = TriangleArea(p1, point, p3);
	double a3 = TriangleArea(p1, p2, point);

	if (abs(a - (a1 + a2 + a3)) <= tolerance) return (1);
	else return (0);
}

int OnLine(vec2 p, vec2 q, vec2 r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y)) return (1);
	
	return (0);
}

int LineOrientation(Line line, vec2 point)
{
	double val = (line.end.y - line.start.y) * (point.x - line.end.x) - (line.end.x - line.start.x) * (point.y - line.end.y);

	if (val == 0.) return (0);

	return (val > 0) ? 1 : 2;
}

int Intersect(Line l1, Line l2)
{
	int o1 = LineOrientation(l1, l2.start);
	int o2 = LineOrientation(l1, l2.end);
	int o3 = LineOrientation(l2, l1.start);
	int o4 = LineOrientation(l2, l1.end);

	if (o1 != o2 && o3 != o4) return (1);

	if (o1 == 0 && OnLine(l1.start, l2.start, l1.end) == 1) return (1);
	if (o2 == 0 && OnLine(l1.start, l2.end, l1.end) == 1) return (1);
	if (o3 == 0 && OnLine(l2.start, l1.start, l2.end) == 1) return (1);
	if (o4 == 0 && OnLine(l2.start, l1.end, l2.end) == 1) return (1);

	return (0);
}

int LineIntersectsTriangle(Triangle triangle, Line line)
{
	if (Intersect(GetLine(triangle, 0), line) == 1) return (1);
	if (Intersect(GetLine(triangle, 1), line) == 1) return (1);
	if (Intersect(GetLine(triangle, 2), line) == 1) return (1);

	return (0);
}

#endif