#pragma once

#include <glm/glm.hpp>
#include <memory>

class Window;
class Game;

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
	GameActor(const Transform& _Transform);
	GameActor(const glm::vec3& _Location, const glm::vec3 _Size);

	void SetGame(Game* _Game);

	Transform GetTransform() const;
	glm::vec3 GetLocation() const;
	glm::vec3 GetSize() const;
	void Move(const glm::vec3& Delta);

	glm::mat4 GetRenderModel() const;
	BoundingBox GetBoundingBox() const;

	bool Collide(const GameActor& Other) const;

	virtual void Begin();
	virtual void Update(const float Delta);
	virtual void Input(const Window& Window, const float Delta);

	virtual ~GameActor() = default;

protected:
	Game* GetGame() const;
	void SetLocation(const glm::vec3 NewLocation);

private:
	Transform mTransform;
	Game* mGame;
};

