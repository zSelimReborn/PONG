#include "GameActor.h"

#include <glm/gtc/matrix_transform.hpp>

Transform::Transform()
	: Location(0.f), Size(0.f)
{
}

Transform::Transform(const glm::vec3& _Location, const glm::vec3& _Size)
	: Location(_Location), Size(_Size)
{
}

BoundingBox::BoundingBox(const Transform& _Transform)
	: mTransform(_Transform)
{
	ScaleOffset.x = mTransform.Size.x / 2.0f;
	ScaleOffset.y = mTransform.Size.y / 2.0f;
}

float BoundingBox::Top() const
{
	return mTransform.Location.y + ScaleOffset.y;
}

float BoundingBox::Bottom() const
{
	return mTransform.Location.y - ScaleOffset.y;
}

float BoundingBox::Right() const
{
	return mTransform.Location.x + ScaleOffset.x;
}

float BoundingBox::Left() const
{
	return mTransform.Location.x - ScaleOffset.x;
}

GameActor::GameActor(const glm::vec3& _Location, const glm::vec3 _Size)
	: Transform(_Location, _Size), Velocity(0.f)
{
}

Transform GameActor::GetTransform() const
{
	return Transform;
}

glm::vec3 GameActor::GetLocation() const
{
	return Transform.Location;
}

glm::vec3 GameActor::GetSize() const
{
	return Transform.Size;
}

glm::vec3 GameActor::GetVelocity() const
{
	return Velocity;
}

glm::mat4 GameActor::GetRenderModel() const
{
	const glm::mat4 Identity(1.f);
	glm::mat4 RenderModel = glm::translate(Identity, Transform.Location);
	RenderModel = glm::scale(RenderModel, Transform.Size);

	return RenderModel;
}

BoundingBox GameActor::GetBoundingBox() const
{
	BoundingBox Box(Transform);
	return Box;
}

void GameActor::Begin()
{
}

void GameActor::Update(const float Delta)
{
}
