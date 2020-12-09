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

// externs
Physics* g_PhysicsPtr;
//Config* g_Config;

__inline float Randf(float min, float max)
{
	return (float)(((rand() & 32767)*(1.0 / 32767.0))*(max - min) + min);
}

Game::Game() : m_state(GameState::GAME_ACTIVE)
{
	InitContext();
	
	InitGame();
}

Game::~Game()
{
	glfwTerminate();

	delete m_camera;
	//delete ship;
	//delete m_AsteroidMgr;
	delete m_text;
}

void Game::InitContext()
{
	// Load file config properties
	Config::GetInstance()->Load(CONFIG_FILE);

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);

	// glfw window creation
	// --------------------
	m_window = glfwCreateWindow(Config::GetInstance()->GetValue(Config::SRC_WIDTH),
		Config::GetInstance()->GetValue(Config::SRC_HEIGHT), "11bits~Asteroids", NULL, NULL);
	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(m_window);
	glfwSetFramebufferSizeCallback(m_window, _framebuffer_size_callback);
	
	// tell GLFW to capture our mouse
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	ResourceManager::GetInstance()->LoadShader("shaders/shader.vs", "shaders/shader.fs", "shaders/shader.gs", "base");

	// initialize random seed
	srand(time(NULL)); 
}

void Game::InitGame()
{
	g_PhysicsPtr = new Physics(glm::vec3(0.0f, 0.0f, 0.0f));
	
	m_camera = new Camera(glm::vec3(0.0f, 0.0f, 20.0f));
	
	m_ship = new Ship(glm::vec3(0.0f, -6.0f, 0.0f), glm::vec3(1.0f));
	m_scene.push_back(m_ship);
	
	m_AsteroidMgr = new AsteroidMgr();
	m_scene.push_back(m_AsteroidMgr);
	
	// text renderer with freetype
	m_text = new TextRenderer(Config::GetInstance()->GetValue(Config::SRC_WIDTH), 
		Config::GetInstance()->GetValue(Config::SRC_HEIGHT));
	m_text->Load("fonts/arial.ttf", 24);

	m_gameTime = 0.0f;
	m_score = 0;
	m_demoFinished = false;

	// Projection and camera is fixed no need to update in every tick
	ResourceManager::GetInstance()->GetShader("base").Use();
	// pass projection matrix to shader 
	glm::mat4 projection = glm::perspective(glm::radians(m_camera->GetZoom()),
		(float)Config::GetInstance()->GetValue(Config::SRC_WIDTH) /
		(float)Config::GetInstance()->GetValue(Config::SRC_HEIGHT),
		10.0f, 
		100.0f);
	ResourceManager::GetInstance()->GetShader("base").SetMatrix4("projection", projection);
	// camera/view transformation
	glm::mat4 view = m_camera->GetViewMatrix();
	ResourceManager::GetInstance()->GetShader("base").SetMatrix4("view", view);
}

void Game::Execute()
{
	float currentFrame = glfwGetTime();
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	// ..:: INPUT ::..
	this->ProcessInput(m_window, m_deltaTime);

	// ..:: LOGIC ::..
	this->Update(m_deltaTime);

	// ..:: RENDER ::..
	this->Render();
}

void Game::Update(float deltaTime)
{
	if (this->m_state == GameState::GAME_ACTIVE)
	{
		m_currentBulletFreq += deltaTime;
		m_gameTime += deltaTime;

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
				actor->GetPhysicsActor()->active = false;
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
	ResourceManager::GetInstance()->GetShader("base").Use();

	// pass projection matrix to shader 
	glm::mat4 projection = glm::perspective(glm::radians(m_camera->GetZoom()),
		(float)Config::GetInstance()->GetValue(Config::SRC_WIDTH) /
		(float)Config::GetInstance()->GetValue(Config::SRC_HEIGHT),
		10.0f, 
		100.0f);
	ResourceManager::GetInstance()->GetShader("base").SetMatrix4("projection", projection);

	// camera/view transformation
	glm::mat4 view = m_camera->GetViewMatrix();
	ResourceManager::GetInstance()->GetShader("base").SetMatrix4("view", view);

	// Render scene
	for (Actor* actor: m_scene)
	{
		if (actor->IsActive()) 
		{
			actor->Render(ResourceManager::GetInstance()->GetShader("base"));
		}
	}

	// Render Timer and lose message
	RenderUI();

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Game::RenderUI()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << m_gameTime;
	string timePanel = "Time: " + stream.str();
	string scorePanel = "Score: " + to_string(m_score);
	m_text->RenderText(timePanel, 50, Config::GetInstance()->GetValue(Config::SRC_WIDTH) / 10, 1.0, glm::vec3(1.0, 1.0, 1.0));
	m_text->RenderText(scorePanel, 650, Config::GetInstance()->GetValue(Config::SRC_HEIGHT) / 10, 1.0, glm::vec3(1.0, 1.0, 1.0));

	if (m_state == GameState::GAME_RESTART)
	{
		m_text->RenderText("You LOSE!!!", 320.0, Config::GetInstance()->GetValue(Config::SRC_HEIGHT) / 2 - 25.0, 1.0, glm::vec3(1.0, 0.0, 0.0));
		m_text->RenderText("Press R to restart or ESC to quit", 220.0, Config::GetInstance()->GetValue(Config::SRC_HEIGHT) / 2, 1.0, glm::vec3(1.0, 1.0, 1.0));
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Game::Restart()
{
	// hot-reload config file
	Config::GetInstance()->Load(CONFIG_FILE);

	for (std::list<Actor*>::iterator it = m_scene.begin(); it != m_scene.end();)
	{
		Actor* actor = (*it);
		if (actor->IsDelete())	// clean runtime deleteable actors
		{
			actor->GetPhysicsActor()->active = false;
			it = m_scene.erase(it);
			delete actor;
		}
		else
		{
			actor->Reset();
			it++;
		}
	}

	m_gameTime = 0.0f;
	m_score = 0;
	m_physicsTimeStepAccum = 0.0f;
	m_demoFinished = false;
	m_state = GameState::GAME_ACTIVE;
}

void Game::Finalize()
{
	m_demoFinished = true;
	glfwSetWindowShouldClose(m_window, true);
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
	if (m_state == GameState::GAME_ACTIVE)
	{
		m_ship->GetPhysicsActor()->accelerationForce = glm::vec3(0.0f);

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			m_ship->SetForceDirection(-1.0f);
		}	
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			m_ship->SetForceDirection(1.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			if (m_currentBulletFreq >= Config::GetInstance()->GetValue(Config::BULLET_FREQUENCY))
			{
				m_currentBulletFreq = 0.0f;

				Bullet* bullet = m_ship->Fire();
				m_scene.push_back(bullet);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		{
			if (Config::GetInstance()->GetValue(Config::RAPID_FIRE))
			{
				m_currentBulletFreq = Config::GetInstance()->GetValue(Config::BULLET_FREQUENCY);
			}
		}
	}

	if (m_state == GameState::GAME_RESTART)
	{
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			std::cout << "==================== RESTART GAME =======================" << std::endl;
			Restart();	// Restart game
		}
	}
}

void Game::SetGameState(GameState newState)
{
	m_state = newState;
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
