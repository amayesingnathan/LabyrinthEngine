#pragma once

#include <Labyrinth/Core/System/Cast.h>

#include <glm/glm.hpp>

namespace Labyrinth
{
	namespace Math
	{
		constexpr f64 e = 2.71828182845904523536;
		constexpr f64 pi = 3.14159265358979323846;
		constexpr f64 pi_2 = 1.57079632679489661923;
		constexpr f64 pi_3 = 1.04719755119659774615;
		constexpr f64 pi_4 = 0.785398163397448309616;
		constexpr f64 one_pi = 0.318309886183790671538;
		constexpr f64 two_pi = 0.318309886183790671538;
		constexpr f64 sqrt2 = 1.41421356237309504880;
		constexpr f64 one_sqrt2 = 0.707106781186547524401;
	}

	namespace Angle
	{
		constexpr f64 Rad5 = 0.0872664625997164788461845;
		constexpr f64 Rad10 = 0.174532925199432957692369;
		constexpr f64 Rad20 = 0.261799387799149436538554;
		constexpr f64 Rad30 = 0.523598775598298873077107;
		constexpr f64 Rad45 = Math::pi_4;
		constexpr f64 Rad60 = Math::pi_3;
		constexpr f64 Rad90 = Math::pi_2;
		constexpr f64 Rad120 = 2.09343951023931954923084;
		constexpr f64 Rad180 = Math::pi;

		constexpr f32 Rad5f = Cast<f32>(Rad5);
		constexpr f32 Rad10f = Cast<f32>(Rad10);
		constexpr f32 Rad20f = Cast<f32>(Rad20);
		constexpr f32 Rad30f = Cast<f32>(Rad30);
		constexpr f32 Rad45f = Cast<f32>(Rad45);
		constexpr f32 Rad60f = Cast<f32>(Rad60);
		constexpr f32 Rad90f = Cast<f32>(Rad90);
		constexpr f32 Rad120f = Cast<f32>(Rad120);
		constexpr f32 Rad180f = Cast<f32>(Rad180);
	}

	namespace Zero
	{
		constexpr glm::vec2 Vec2 = glm::vec2{ 0.0f };
		constexpr glm::vec3 Vec3 = glm::vec3{ 0.0f };

		constexpr glm::mat2 Mat2 = glm::mat2{ 0.0f };
		constexpr glm::mat3 Mat3 = glm::mat3{ 0.0f };
		constexpr glm::mat4 Mat4 = glm::mat4{ 0.0f };
	}
}