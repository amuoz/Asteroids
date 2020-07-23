/*****************************************************************
** Asteroids is a simple spaceship in asteroid field game.
** Developed with custom C++ engine.
** OpenGL3,	GLFW, GLAD, GLM, FreeType, VS2017
** Custom physics and collisions
** Generic entity pool, abstract Actor class
** Custom vertex shader, geometry shader and fragment shader
******************************************************************/

#ifndef	GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <list>
#include <map>

// Forwards
class Physics;
class Camera;
class Ship;
class AsteroidMgr;
class Actor;
class TextRenderer;

// Represents the current state of the game
enum GameState {
	GAME_ACTIVE,
	GAME_RESTART
};

class Game
{
public:
	Game();
	~Game();

	void Execute();

	// Have we finished?
	inline bool IsFinished() const { return m_demoFinished; }

	// callback
	static void _framebuffer_size_callback(GLFWwindow* window, int width, int height);

	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	GameState m_state;
	int m_score;

private:
	
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	
	void InitContext();
	void InitGame();

	void ProcessInput(GLFWwindow* window, float deltaTime);
	void Update(float deltaTime);	// logic and physics
	void Render();
	
	void RenderUI();

	void Restart();
	void Finalize();

	bool m_demoFinished;

	GLFWwindow* window;
	
	Camera* camera;	// camera
	Ship* ship;	// ship
	AsteroidMgr* m_AsteroidMgr;
	TextRenderer* m_text;

	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	float m_physicsTimeStepAccum = 0.0f;
	float gameTime = 0.0f;

	float currentBulletFreq;

	// scene actors
	std::list<Actor*> m_scene;
};

#endif