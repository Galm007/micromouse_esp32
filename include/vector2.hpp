#pragma once

class Vector2 {
public:
	float x, y;

	Vector2(float x, float y);
	Vector2(float a);
	Vector2();
	~Vector2();

	Vector2 operator + (Vector2 p);
	void operator += (Vector2 p);
	Vector2 operator - (Vector2 p);
	void operator -= (Vector2 p);
	Vector2 operator * (Vector2 p);
	void operator *= (Vector2 p);
	Vector2 operator / (Vector2 p);
	void operator /= (Vector2 p);
	Vector2 operator * (float n);
	void operator *= (float n);
	Vector2 operator / (float n);
	void operator /= (float n);
	bool operator == (Vector2 p);
	bool operator != (Vector2 p);
};
