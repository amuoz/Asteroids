#ifndef	GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader;
class Camera;
class Ship;
//class Asteroid;
class AsteroidMgr;
class Actor;

#include <iostream>
#include <cmath>
#include <list>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// defines
#define MAX_DYNAMICS 1000

// Forwards
class Physics;

// FreeType fonts
struct Character {
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

class Game
{
public:
	Game(float forwardVelocity, float angularVelocity, float thrust, float mass, float freq, float freqIncrease, float bulletVelocity, float bulletFrequency);
	
	~Game();

	void InitContext();
	void InitGame();

	void Update();
	void Render();

	void Restart();
	void Finalize();

	// Have we finish demo?
	inline bool IsFinished() const { return m_demoFinished; }

	// callbacks
	static void _framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void _mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void _scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void processInput(GLFWwindow* window, float deltaTime);

	void RandomizeAsteroids();

	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	// camera
	Camera* camera;

	// ship
	Ship* ship;

	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;
	bool firstMouse = true;

	// timing
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame

	unsigned int amount = 10;

	//Asteroid *m_asteroids[MAX_DYNAMICS];
	//unsigned int m_numAsteroids;

	GLFWwindow* window;

	AsteroidMgr* m_AsteroidMgr;
	Shader* ourShader;

	bool m_demoFinished;

	std::list<Actor*> m_scene;

	float currentBulletFreq;

	std::map<char, Character> Characters;
};

#endif