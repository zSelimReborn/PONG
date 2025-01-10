#include "GameActor.h"
#include "Game.h"

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

GameActor::GameActor(const ::Transform& _Transform)
	: mTransform(_Transform)
{
}

GameActor::GameActor(const glm::vec3& _Location, const glm::vec3 _Size)
	: mTransform(_Location, _Size)
{
}

void GameActor::SetGame(Game* _Game)
{
	mGame = _Game;
}

Transform GameActor::GetTransform() const
{
	return mTransform;
}

glm::vec3 GameActor::GetLocation() const
{
	return mTransform.Location;
}

glm::vec3 GameActor::GetSize() const
{
	return mTransform.Size;
}

void GameActor::Move(const glm::vec3& Delta)
{
	mTransform.Location += Delta;
}

glm::mat4 GameActor::GetRenderModel() const
{
	const glm::mat4 Identity(1.f);
	glm::mat4 RenderModel = glm::translate(Identity, mTransform.Location);
	RenderModel = glm::scale(RenderModel, mTransform.Size);

	return RenderModel;
}

BoundingBox GameActor::GetBoundingBox() const
{
	BoundingBox Box(mTransform);
	return Box;
}

bool GameActor::Collide(const GameActor& Other) const
{
	const BoundingBox Me(GetBoundingBox());
	const BoundingBox OtherBox(Other.GetBoundingBox());

	bool bCollidedX = (Me.Right() >= OtherBox.Left()  && OtherBox.Right() >= Me.Left());
	bool bCollidedY = (Me.Bottom() <= OtherBox.Top()  && OtherBox.Bottom() <= Me.Top());

	return bCollidedX && bCollidedY;
}

void GameActor::Begin()
{
}

void GameActor::Update(const float Delta)
{
}

void GameActor::Input(const Window& Window, const float Delta)
{
}

Game* GameActor::GetGame() const
{
	return mGame;
}

void GameActor::SetLocation(const glm::vec3 NewLocation)
{
	mTransform.Location = NewLocation;
}
