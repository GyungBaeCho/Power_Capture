// stdafx.cpp : source file that includes just the standard includes
// WindowProject.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

VECTOR2 operator+(const VECTOR2& vec1, const VECTOR2& vec2) {
	return VECTOR2(vec1.x + vec2.x, vec1.y + vec2.y);
}
VECTOR2 operator-(const VECTOR2& vec1, const VECTOR2& vec2) {
	return VECTOR2(vec1.x - vec2.x, vec1.y - vec2.y);
}
VECTOR2 operator*(const VECTOR2& vec1, const VECTOR2& vec2) {
	return VECTOR2(vec1.x * vec2.x, vec1.y * vec2.y);
}
VECTOR2 operator/(const VECTOR2& vec1, const VECTOR2& vec2) {
	return VECTOR2(vec1.x / vec2.x, vec1.y / vec2.y);
}
VECTOR2 operator*(const VECTOR2& vec1, const float& fData) {
	return VECTOR2(vec1.x * fData, vec1.y * fData);
}
VECTOR2 operator/(const VECTOR2& vec1, const float& fData) {
	return VECTOR2(vec1.x / fData, vec1.y / fData);
}
void operator+=(VECTOR2& vec1, const VECTOR2& vec2) {
	vec1.x += vec2.x;
	vec1.y += vec2.y;
}
void operator-=(VECTOR2& vec1, const VECTOR2& vec2) {
	vec1.x -= vec2.x;
	vec1.y -= vec2.y;
}
void operator*=(VECTOR2& vec1, const int& nData) {
	vec1.x *= nData;
	vec1.y *= nData;
}
void operator*=(VECTOR2& vec1, const float& fData) {
	vec1.x *= fData;
	vec1.y *= fData;
}