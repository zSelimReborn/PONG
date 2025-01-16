#pragma once

#include <glm/glm.hpp>

namespace Console
{
	void Print(const glm::vec3& Vector);
}

namespace Math
{
	float Clamp(const float Value, const float Min, const float Max);
}

namespace Colors
{
	const float White[] = { 1.f, 1.f, 1.f };
	const float Black[] = { 0.f, 0.f, 0.f };
	const float BlackAlpha[] = { 0.f, 0.f, 0.f, 1.f };
	const float Red[] = { 1.f, 0.f, 0.f };
	const float Green[] = { 0.f, 1.f, 0.f };
	const float LightBlack[] = { 0.14f, 0.15f, 0.15f, 1.f };
}