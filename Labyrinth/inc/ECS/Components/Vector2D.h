#pragma once

struct Vector2D
{
	float x;
	float y;

	static float lerpDur;

	Vector2D() : x(0.0f), y(0.0f) {};
	Vector2D(float x, float y) : x(x), y(y) {};
	Vector2D(float scalar) : x(scalar), y(scalar) {}
	Vector2D(int X, int Y) : x(static_cast<float>(X)), y(static_cast<float>(Y)) {}
	Vector2D(int scalar) : x(static_cast<float>(scalar)), y(static_cast<float>(scalar)) {}
	Vector2D(const Vector2D&) = default;

	Vector2D& Add(const Vector2D& vec);
	Vector2D& Subtract(const Vector2D& vec);
	Vector2D& Multiply(const Vector2D& vec);
	Vector2D& Divide(const Vector2D& vec);

	friend Vector2D& operator+(Vector2D& vec1, const Vector2D& vec2);
	friend Vector2D& operator-(Vector2D& vec1, const Vector2D& vec2);
	friend Vector2D& operator*(Vector2D& vec1, const Vector2D& vec2);
	friend Vector2D& operator/(Vector2D& vec1, const Vector2D& vec2);

	Vector2D& operator+=(const Vector2D& vec);
	Vector2D& operator-=(const Vector2D& vec);
	Vector2D& operator*=(const Vector2D& vec);
	Vector2D& operator/=(const Vector2D& vec);

	friend bool operator==(const Vector2D& a, const Vector2D& b);
	friend bool operator!=(const Vector2D& a, const Vector2D& b);

	bool isNull();
	static float lerp(float minVal, float maxVal, float interpolation);

	void normalise();
};