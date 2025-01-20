#pragma once

#include <string>

namespace Assets
{
	const std::string BasePath = "Assets/";

	const std::string FontPath = BasePath + "Fonts/Exan.ttf";

	const std::string FirstPaddleSprite = BasePath + "Sprites/first_paddle.png";
	const std::string SecondPaddleSprite = BasePath + "Sprites/second_paddle.png";
	const std::string BallSprite = BasePath + "Sprites/ball.png";
	const std::string BrickSprite = BasePath + "Sprites/brick.png";

	const std::string MainVertexShader = BasePath + "Shaders/main.vert";
	const std::string MainFragmentShader = BasePath + "Shaders/main.frag";

	const std::string TextVertexShader = BasePath + "Shaders/text.vert";
	const std::string TextFragmentShader = BasePath + "Shaders/text.frag";

	const std::string ParticleVertexShader = BasePath + "Shaders/particle.vert";
	const std::string ParticleFragmentShader = BasePath + "Shaders/particle.frag";

	const std::string PongSound = BasePath + "Sounds/pong.wav";
	const std::string GoalSound = BasePath + "Sounds/goal.wav";
	const std::string WinSound = BasePath + "Sounds/win_sound.wav";
}