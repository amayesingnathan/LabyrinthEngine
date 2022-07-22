#pragma once

struct LAB_API Vector2D
{
	f32 x;
	f32 y;

	static f32 lerpDur;

	Vector2D() : x(0.0f), y(0.0f) {};
	Vector2D(f32 x, f32 y) : x(x), y(y) {};
	Vector2D(f32 scalar) : x(scalar), y(scalar) {}
	Vector2D(i32 X, i32 Y) : x(static_cast<f32>(X)), y(static_cast<f32>(Y)) {}
	Vector2D(i32 scalar) : x(static_cast<f32>(scalar)), y(static_cast<f32>(scalar)) {}
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

	bool isNull() const;
	static f32 lerp(f32 minVal, f32 maxVal, f32 interpolation);

	void normalise();
};