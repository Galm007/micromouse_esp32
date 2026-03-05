#include "vector2.hpp"

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

Vector2::Vector2(float a) {
	this->x = this->y = a;
}

Vector2::Vector2() { }

Vector2::~Vector2() { }

Vector2 Vector2::operator + (Vector2 p) {
	return Vector2(x + p.x, y + p.y);
}

void Vector2::operator += (Vector2 p) {
	x += p.x;
	y += p.y;
}

Vector2 Vector2::operator - (Vector2 p) {
	return Vector2(x - p.x, y - p.y);
}

void Vector2::operator -= (Vector2 p) {
	x -= p.x;
	y -= p.y;
}

Vector2 Vector2::operator * (Vector2 p) {
	return Vector2(x * p.x, y * p.y);
}

void Vector2::operator *= (Vector2 p) {
	x *= p.x;
	y *= p.y;
}

Vector2 Vector2::operator / (Vector2 p) {
	return Vector2(x / p.x, y / p.y);
}

void Vector2::operator /= (Vector2 p) {
	x /= p.x;
	y /= p.y;
}

Vector2 Vector2::operator * (float n) {
	return Vector2(x * n, y * n);
}

void Vector2::operator *= (float n) {
	x *= n;
	y *= n;
}

Vector2 Vector2::operator / (float n) {
	return Vector2(x / n, y / n);
}

void Vector2::operator /= (float n) {
	x /= n;
	y /= n;
}

bool Vector2::operator == (Vector2 p) {
	return x == p.x && y == p.y;
}

bool Vector2::operator != (Vector2 p) {
	return x != p.x || y != p.y;
}
