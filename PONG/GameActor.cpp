#include "GameActor.h"
#include "Game.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Assets.h"
#include "pk/AssetManager.h"
#include "pk/Renderer.h"
#include "pk/Texture.h"

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
	: mTransform(_Transform), Color(1.f, 1.f, 1.f)
{
}

GameActor::GameActor(const glm::vec3& _Location, const glm::vec3 _Size)
	: mTransform(_Location, _Size), Color(1.f, 1.f, 1.f)
{
}

void GameActor::SetGame(Game* _Game)
{
	mGame = _Game;
}

void GameActor::SetTexture(const Texture::SharedPtr& NewTexture)
{
	mTexture = NewTexture;
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

void GameActor::SetColor(const glm::vec3& _Color)
{
	Color = _Color;
}

glm::vec3 GameActor::GetColor() const
{
	return Color;
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

void GameActor::BindTexture() const
{
	if (mTexture)
	{
		mTexture->Bind();
	}
}

void GameActor::UnBindTexture() const
{
	if (mTexture)
	{
		mTexture->UnBind();
	}
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

void GameActor::Render() const
{
	Renderer::Get().RenderSprite(
		AssetManager::Get().GetShader(Assets::MainShaderName),
		mTexture,
		GetRenderModel(),
		Color
	);
}

Game* GameActor::GetGame() const
{
	return mGame;
}

void GameActor::SetLocation(const glm::vec3 NewLocation)
{
	mTransform.Location = NewLocation;
}
