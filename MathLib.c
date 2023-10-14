#include "MathLib.h"
#include <math.h>

#define PI 3.14159265358979323;

float MathLerp(float a, float b, float t) {
	return ((b - a) * t) + a;
}

Vector2 GetDirection(Vector2 a, Vector2 b) {
	return CP_Vector_Set(b.x - a.x, b.y - a.y);
}

Vector2 GetDirectionN(Vector2 a, Vector2 b) {
	Vector2 dir = GetDirection(a, b);
	return Normalized(dir);
}

Vector2 Normalized(Vector2 a) {
	float magnitude = (float)sqrt((a.x * a.x) + (a.y * a.y));
	return CP_Vector_Set(a.x / magnitude, a.y / magnitude);
}

float angle_between(Vector2 p) {
	double angle = atan(p.y / p.x) * 180 / PI + 90;
	if (p.x < 0) {
		angle = 270 - atan(p.y / -p.x) * 180 / PI;
	}
	else {
		angle = 90 + atan(p.y / p.x) * 180 / PI;
	}
	return (float)angle;

}

float GetDistance(Vector2 a, Vector2 b) {
	Vector2 r = GetDirection(a, b);
	return (float)sqrt(r.x * r.x + r.y * r.y);
}

Vector2 GetNormal(Vector2 a) {
	return CP_Vector_Set(-a.y, a.x);
}