#pragma once

#include "DMLib.h"


#pragma region Math

float MathLerp(float a, float b, float t);

Vector2 GetDirection(Vector2 a, Vector2 b);
Vector2 GetDirectionN(Vector2 a, Vector2 b);
Vector2 Normalized(Vector2 a);

float angle_between(Vector2 p);

float GetDistance(Vector2 a, Vector2 b);

Vector2 GetNormal(Vector2 a);
#pragma endregion 
