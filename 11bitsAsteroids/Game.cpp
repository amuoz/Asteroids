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
#include "TextRenderer.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#define CONFIG_FILE "config/config.conf"

// externs
Physics* g_PhysicsPtr;
Config* g_Config;

__inline float Randf(float min, float max)
{
	return (float)(((rand() & 32767)*(1.0 / 32767.0))*(max - min) + min);
}

Game::Game() : m_state(GAME_ACTIVE)
{
	InitContext();
	
	InitGame();
}

Game::~Game()
{
	glfwTerminate();

	delete camera;
	//delete ship;
	//delete m_AsteroidMgr;
	delete m_text;
}

void Game::InitContext()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

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
	glfwSetFramebufferSizeCallback(window, _framebuffer_size_callback);  
	
	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------                                                                   
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))   
	{
		// Glad GLAD is working
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(-1);
	}

	// configure global opengl state: 
	// ------------------------------
	glEnable(GL_DEPTH_TEST);	// depth testing

	glEnable(GL_BLEND);			// for text render
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// wireframe mode

	// build and compile our base shader program
	// -----------------------------------------
	ResourceManager::LoadShader("shaders/shader.vs", "shaders/shader.fs", "shaders/shader.gs", "base");

	// initialize random seed
	srand(time(NULL)); 
}

void Game::InitGame()
{
	g_Config = new Config();
	// Load file config properties
	g_Config->Load(CONFIG_FILE);

	g_PhysicsPtr = new Physics(glm::vec3(0.0f, 0.0f, 0.0f));
	
	camera = new Camera(glm::vec3(0.0f, 0.0f, 20.0f));
	
	ship = new Ship(glm::vec3(0.0f, -6.0f, 0.0f), glm::vec3(1.0f));
	m_scene.push_back(ship);
	
	m_AsteroidMgr = new AsteroidMgr();
	m_scene.push_back(m_AsteroidMgr);
	
	// text renderer with freetype
	m_text = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
	m_text->Load("fonts/arial.ttf", 24);

	gameTime = 0.0f;
	m_score = 0;
	m_demoFinished = false;

	// Projection and camera is fixed no need to update in every tick
	ResourceManager::GetShader("base").Use();
	// pass projection matrix to shader 
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 10.0f, 100.0f);
	ResourceManager::GetShader("base").SetMatrix4("projection", projection);
	// camera/view transformation
	glm::mat4 view = camera->GetViewMatrix();
	ResourceManager::GetShader("base").SetMatrix4("view", view);
}

void Game::Execute()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// ..:: INPUT ::..
	this->ProcessInput(window, deltaTime);

	// ..:: LOGIC ::..
	this->Update(deltaTime);

	// ..:: RENDER ::..
	this->Render();
}

void Game::Update(float deltaTime)
{
	if (this->m_state == GAME_ACTIVE)
	{
		currentBulletFreq += deltaTime;
		gameTime += deltaTime;

		// ..:: LOGIC ::..
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

		// ..:: PHYSICS ::..
		float fixedUpdateFreq = 1.0f / 60.0f;	
		m_physicsTimeStepAccum += deltaTime;
		while (m_physicsTimeStepAccum > fixedUpdateFreq)
		{
			m_physicsTimeStepAccum -= fixedUpdateFreq;
			g_PhysicsPtr->Update(fixedUpdateFreq);	// fixed physic timestep 
		}
		
	}

}

void Game::Render()
{
	// ..:: RENDERING ::..
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ..:: Drawing code ::..
	// be sure to activate the shader
	ResourceManager::GetShader("base").Use();

	// pass projection matrix to shader 
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 10.0f, 100.0f);
	ResourceManager::GetShader("base").SetMatrix4("projection", projection);

	// camera/view transformation
	glm::mat4 view = camera->GetViewMatrix();
	ResourceManager::GetShader("base").SetMatrix4("view", view);

	// Render scene
	for (Actor* actor: m_scene)
	{
		if (actor->IsActive()) 
		{
			actor->Render(ResourceManager::GetShader("base"));
		}
	}

	// Render Timer and lose message
	RenderUI();

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Game::RenderUI()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << gameTime;
	string timePanel = "Time: " + stream.str();
	string scorePanel = "Score: " + to_string(m_score);
	m_text->RenderText(timePanel, 50, SCR_HEIGHT / 10, 1.0, glm::vec3(1.0, 1.0, 1.0));
	m_text->RenderText(scorePanel, 650, SCR_HEIGHT / 10, 1.0, glm::vec3(1.0, 1.0, 1.0));

	if (m_state == GAME_RESTART)
	{
		m_text->RenderText("You LOSE!!!", 320.0, SCR_HEIGHT / 2 - 25.0, 1.0, glm::vec3(1.0, 0.0, 0.0));
		m_text->RenderText("Press R to restart or ESC to quit", 220.0, SCR_HEIGHT / 2, 1.0, glm::vec3(1.0, 1.0, 1.0));
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Game::Restart()
{
	// hot-reload config file
	g_Config->Load(CONFIG_FILE);

	for (std::list<Actor*>::iterator it = m_scene.begin(); it != m_scene.end();)
	{
		Actor* actor = (*it);
		if (actor->m_delete)	// clean delete actors
		{
			actor->m_physicsActor->active = false;
			it = m_scene.erase(it);
			delete actor;
		}
		else
		{
			actor->Reset();
			it++;
		}
	}

	gameTime = 0.0f;
	m_score = 0;
	m_physicsTimeStepAccum = 0.0f;
	m_demoFinished = false;
	m_state = GAME_ACTIVE;
}

void Game::Finalize()
{
	m_demoFinished = true;
	glfwSetWindowShouldClose(window, true);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Game::ProcessInput(GLFWwindow* window, float deltaTime)
{
	// Quit game
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		Finalize();
	}

	// Active inputs
	if (this->m_state == GAME_ACTIVE)
	{
		ship->m_physicsActor->accelerationForce = glm::vec3(0.0f);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			ship->m_physicsActor->accelerationForce = glm::vec3(-(ship->m_thrust), 0.0f, 0.0f);
		}	
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			ship->m_physicsActor->accelerationForce = glm::vec3(ship->m_thrust, 0.0f, 0.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			if (currentBulletFreq >= g_Config->GetValue(Config::BULLET_FREQUENCY))
			{
				currentBulletFreq = 0.0f;

				glm::vec3 bulletPosition = glm::vec3(ship->m_position) + glm::vec3(0.0f, 0.8f, 0.0f);
				Bullet* bullet = new Bullet(bulletPosition, 0.2f, glm::vec3(0.0f, g_Config->GetValue(Config::BULLET_VELOCITY), 0.0f));
				bullet->SetDelete(true);	// delete from scene on restart
				m_scene.push_back(bullet);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		{
			if (g_Config->GetValue(Config::RAPID_FIRE))
			{
				currentBulletFreq = g_Config->GetValue(Config::BULLET_FREQUENCY);
			}
		}
	}

	if (this->m_state == GAME_RESTART)
	{
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			std::cout << "==================== RESTART GAME =======================" << std::endl;
			Restart();	// Restart game
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Game::_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	g_game->framebuffer_size_callback(window, width, height);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
