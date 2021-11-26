#include "ECS\Vector2D.h"

Vector2D& Vector2D::Add(const Vector2D& vec)
{
	x += vec.x;
	y += vec.y;

	return *this;
}

Vector2D& Vector2D::Subtract(const Vector2D& vec)
{
	x -= vec.x;
	y -= vec.y;

	return *this;
}

Vector2D& Vector2D::Multiply(const Vector2D& vec)
{
	x *= vec.x;
	y *= vec.y;

	return *this;
}

Vector2D& Vector2D::Divide(const Vector2D& vec)
{
	x /= vec.x;
	y /= vec.y;

	return *this;
}

Vector2D& operator+(Vector2D& vec1, const Vector2D& vec2)
{
	return vec1.Add(vec2);
}

Vector2D& operator-(Vector2D& vec1, const Vector2D& vec2)
{
	return vec1.Subtract(vec2);
}

Vector2D& operator*(Vector2D& vec1, const Vector2D& vec2)
{
	return vec1.Multiply(vec2);
}

Vector2D& operator/(Vector2D& vec1, const Vector2D& vec2)
{
	return vec1.Divide(vec2);
}

Vector2D& Vector2D::operator+=(const Vector2D& vec)
{
	return Add(vec);
}

Vector2D& Vector2D::operator-=(const Vector2D& vec)
{
	return Subtract(vec);
}

Vector2D& Vector2D::operator*=(const Vector2D& vec)
{
	return Multiply(vec);
}

Vector2D& Vector2D::operator/=(const Vector2D& vec)
{
	return Divide(vec);
}
