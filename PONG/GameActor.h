#pragma once

#include <glm/glm.hpp>

struct Transform
{
	glm::vec3 Location;
	glm::vec3 Size;

	Transform();
	Transform(const glm::vec3& _Location, const glm::vec3& _Size);
};

struct BoundingBox
{
	Transform mTransform;
	glm::vec2 ScaleOffset;

	BoundingBox(const Transform& _Transform);
	float Top() const;
	float Bottom() const;
	float Right() const;
	float Left() const;
};

class GameActor
{
public:
	GameActor(const glm::vec3& _Location, const glm::vec3 _Size);

	Transform GetTransform() const;
	glm::vec3 GetLocation() const;
	glm::vec3 GetSize() const;
	glm::vec3 GetVelocity() const;

	glm::mat4 GetRenderModel() const;
	BoundingBox GetBoundingBox() const;

	virtual void Begin();
	virtual void Update(const float Delta);

	virtual ~GameActor() = default;

private:
	Transform Transform;
	glm::vec3 Velocity;
};

