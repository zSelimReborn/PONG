#include "Common.h"

#include <iostream>

void Console::Print(const glm::vec3& Vector)
{
	std::cout << "{x: " << Vector.x << ", y: " << Vector.y << ", z: " << Vector.z << "}\n";
}

float Math::Clamp(const float Value, const float Min, const float Max)
{
	return std::min(Max, std::max(Min, Value));
}
