#include "Game.h"

#include "Common.h"
#include "Physics.h"
#include "Shader.h"
#include "Camera.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Config.h"
#include "AsteroidMgr.h"
#include "Bullet.h"

#include "ResourceManager.h"

#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H 


// externs
//extern Game *g_game;
Physics* g_PhysicsPtr;
Config* g_Config;

__inline float Randf(float min, float max)
{
	return (float)(((rand() & 32767)*(1.0 / 32767.0))*(max - min) + min);
}

Game::Game(float forwardVelocity, float angularVelocity, float thrust, float mass, float freq, float freqIncrease, float bulletVelocity, float bulletFrequency)
{
	g_Config = new Config();
	g_Config->m_forwardVelocity = forwardVelocity;
	g_Config->m_angularVelocity = angularVelocity;
	g_Config->m_thrust = thrust;
	g_Config->m_mass = mass;
	g_Config->m_freq = freq;
	g_Config->m_freqIncrease = freqIncrease;
	g_Config->m_bulletVelocity = bulletVelocity;
	g_Config->m_bulletFrequency = bulletFrequency;

	InitContext();

	InitGame();
}

Game::~Game()
{
	delete ship;
	delete g_PhysicsPtr;
	glfwTerminate();
}

void Game::InitContext()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "11bits~Asteroids", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, _framebuffer_size_callback);   // resize callback    
	glfwSetCursorPosCallback(window, _mouse_callback);
	glfwSetScrollCallback(window, _scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------                                                                   
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))    // Glad GLAD is working
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}

	// configure global opengl state: depth testing
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  

	// build and compile our shader program
	// ------------------------------------
	//ourShader = new Shader("3.3.shader.vs", "3.3.shader.fs"); // you can name your shader files however you like
	ResourceManager::LoadShader("3.3.shader.vs", "3.3.shader.fs", nullptr, "base");

	srand(glfwGetTime()); // initialize random seed

	// FreeType
	// --------
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		exit(-1);
	}

	// load font as face
	FT_Face face;
	if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		exit(-1);
	}
	else {
		// set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, 48);

		// disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// load first 128 characters of ASCII set
		for (unsigned char c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// generate texture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// now store character for later use
			Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<char, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void Game::InitGame()
{
	g_PhysicsPtr = new Physics(glm::vec3(0.0f, 0.0f, 0.0f));
	camera = new Camera(glm::vec3(0.0f, 0.0f, 20.0f));
	ship = new Ship(glm::vec3(0.0f, -6.0f, 0.0f), glm::vec3(1.0f), g_Config->m_thrust, g_Config->m_mass);
	m_AsteroidMgr = new AsteroidMgr();

	//RandomizeAsteroids();

	m_demoFinished = false;
}

void Game::RandomizeAsteroids()
{
	float radius = 10.0f;
	float offset = 10.0f;
	for (unsigned int i = 0; i < amount; ++i)
	{
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		//float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
		float y = displacement; // keep height of field smaller compared to width of x and z
		//float y = sin(angle) * radius + displacement;

		// 2. scale: scale between 0.05 and 0.25f / 0.5 and 1
		float scale = (rand() % 100) / 100.0f + 0.5;

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		//float rotAngle = (rand() % 360);
		//float rotAngle = (rand() % 10 + 1);
		float rotAngle = Randf(g_Config->m_angularVelocity / 2, g_Config->m_angularVelocity);

		//m_asteroids[m_numAsteroids++] = new Asteroid(glm::vec3(x, y, 0.0f), glm::vec3(scale), rotAngle, glm::vec3(0.0f, -m_forwardVelocity, 0.0f));
	}
}

void Game::Update()
{
	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	currentBulletFreq += deltaTime;

	// ..:: INPUT ::..
	processInput(window, deltaTime);

	if (ship->m_alive)
	{
		// ..:: PHYSICS ::..
		g_PhysicsPtr->Update(deltaTime);

		// ..:: LOGIC ::..
		ship->Update(deltaTime);
		m_AsteroidMgr->Update(deltaTime);

		
		for (std::list<Actor*>::iterator it = m_scene.begin(); it != m_scene.end();)
		{
			Actor* actor = (*it);
			if (actor->IsActive())
			{
				actor->Update(deltaTime);
				it++;
			}
			else
			{
				actor->m_physicsActor->active = false;
				it = m_scene.erase(it);
				delete actor;
			}
						
		}

	}



}

void Game::Render()
{
	// ..:: RENDERING ::..
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ..:: Drawing code ::..
	// be sure to activate the shader
	//ourShader->Use();
	ResourceManager::GetShader("base").Use();

	// pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
	// -----------------------------------------------------------------------------------------------------------
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	//ourShader->SetMatrix4("projection", projection);
	ResourceManager::GetShader("base").SetMatrix4("projection", projection);

	// camera/view transformation
	glm::mat4 view = camera->GetViewMatrix();
	//ourShader->SetMatrix4("view", view);
	ResourceManager::GetShader("base").SetMatrix4("view", view);

	ship->Render(ResourceManager::GetShader("base"));

	// render boxes
	/*
	for (unsigned int i = 0; i < m_numAsteroids; ++i) 
	{
		m_asteroids[i]->Render(*ourShader);
	}
	*/
	m_AsteroidMgr->Render(ResourceManager::GetShader("base"));

	for (Actor* actor: m_scene)
	{
		if (actor->IsActive()) 
		{
			actor->Render(ResourceManager::GetShader("base"));
		}
	}

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Game::Restart()
{
	delete camera;
	delete ship;
	delete m_AsteroidMgr;
	delete g_PhysicsPtr;

	InitGame();
}

void Game::Finalize()
{
	m_demoFinished = true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Game::processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		Finalize();
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		//ship->ProcessKeyboard(Ship_Movement::L, deltaTime);
		//ship->m_physicsActor->vel = glm::vec3(-SHIP_SPEED, 0.0f, 0.0f);
		ship->m_physicsActor->accelerationForce = glm::vec3(-(ship->m_thrust), 0.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//ship->ProcessKeyboard(Ship_Movement::R, deltaTime);
		//ship->m_physicsActor->vel = glm::vec3(SHIP_SPEED, 0.0f, 0.0f);
		ship->m_physicsActor->accelerationForce = glm::vec3(ship->m_thrust, 0.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		// Restart game
		if (!ship->m_alive)
		{
			Restart();
		}
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (currentBulletFreq >= g_Config->m_bulletFrequency)
		{
			glm::vec3 bulletPosition = glm::vec3(ship->m_position) + glm::vec3(0.0f, 0.8f, 0.0f);
			Bullet* bullet = new Bullet(bulletPosition, 0.2f, glm::vec3(0.0f, g_Config->m_bulletVelocity, 0.0f));
			m_scene.push_back(bullet);
			currentBulletFreq = 0;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		currentBulletFreq = g_Config->m_bulletFrequency;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Game::_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	g_game->framebuffer_size_callback(window, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Game::_mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	g_game->mouse_callback(window, xpos, ypos);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Game::_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	g_game->scroll_callback(window, xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Game::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void Game::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll(yoffset);
}