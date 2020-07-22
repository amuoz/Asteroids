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
//extern Game *g_game;
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
	//delete g_PhysicsPtr;
	//delete camera;
	//delete ship;
	//delete m_AsteroidMgr;
	//delete m_text;
	//delete g_Config;

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// build and compile our shader program
	// ------------------------------------
	//ourShader = new Shader("3.3.shader.vs", "3.3.shader.fs"); // you can name your shader files however you like
	ResourceManager::LoadShader("shaders/shader.vs", "shaders/shader.fs", "shaders/shader.gs", "base");

	srand(glfwGetTime()); // initialize random seed
}

void Game::InitGame()
{
	g_Config = new Config();
	g_Config->Load(CONFIG_FILE);

	g_PhysicsPtr = new Physics(glm::vec3(0.0f, 0.0f, 0.0f));
	camera = new Camera(glm::vec3(0.0f, 0.0f, 20.0f));
	ship = new Ship(glm::vec3(0.0f, -6.0f, 0.0f), glm::vec3(1.0f), g_Config->GetValue(Config::THRUST), g_Config->GetValue(Config::MASS));
	m_AsteroidMgr = new AsteroidMgr();
	m_text = new TextRenderer(SCR_WIDTH, SCR_HEIGHT);
	m_text->Load("fonts/arial.ttf", 24);

	gameTime = 0.0f;
	m_demoFinished = false;
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

	if (this->m_state == GAME_ACTIVE)
	{
		gameTime += deltaTime;

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

		if (!ship->m_alive) 
		{
			this->m_state = GAME_RESTART;
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
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 10.0f, 100.0f);
	//ourShader->SetMatrix4("projection", projection);
	ResourceManager::GetShader("base").SetMatrix4("projection", projection);

	// camera/view transformation
	glm::mat4 view = camera->GetViewMatrix();
	//ourShader->SetMatrix4("view", view);
	ResourceManager::GetShader("base").SetMatrix4("view", view);
	ResourceManager::GetShader("base").SetVector4f("color", glm::vec4(1.0f));

	ship->Render(ResourceManager::GetShader("base"));

	// render boxes
	m_AsteroidMgr->Render(ResourceManager::GetShader("base"));

	for (Actor* actor: m_scene)
	{
		if (actor->IsActive()) 
		{
			actor->Render(ResourceManager::GetShader("base"));
		}
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	std::stringstream stream;
	stream << std::fixed << std::setprecision(1) << gameTime;
	std::string s = stream.str();
	string scorePanel = "Score: " + s;
	m_text->RenderText(scorePanel, 50, SCR_HEIGHT / 10, 1.0, glm::vec3(1.0, 1.0, 1.0));
	if (this->m_state == GAME_RESTART)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//m_text->RenderText("Press R to retry", 250.0f, this->SCR_HEIGHT / 2.0f, 1.0f);
		m_text->RenderText("You LOSE!!!", 320.0, SCR_HEIGHT / 2 - 25.0, 1.0, glm::vec3(1.0, 0.0, 0.0));
		m_text->RenderText("Press R to restart or ESC to quit", 220.0, SCR_HEIGHT / 2, 1.0, glm::vec3(1.0, 1.0, 1.0));
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Game::Restart()
{
	m_AsteroidMgr->Reset();
	ship->Reset(glm::vec3(0.0f, -6.0f, 0.0f));

	for (std::list<Actor*>::iterator it = m_scene.begin(); it != m_scene.end();)
	{
		Actor* actor = (*it);
		actor->m_physicsActor->active = false;
		it = m_scene.erase(it);
		delete actor;
	}
	m_scene.clear();
	// deallocating the memory
	std::list<Actor*>().swap(m_scene);

	// hot-reload config file
	g_Config->Load(CONFIG_FILE);

	gameTime = 0.0f;
	m_demoFinished = false;
	this->m_state = GAME_ACTIVE;
}

void Game::Finalize()
{
	m_demoFinished = true;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Game::processInput(GLFWwindow* window, float deltaTime)
{
	// Quit game
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		Finalize();
		glfwSetWindowShouldClose(window, true);
	}

	// Active inputs
	if (this->m_state == GAME_ACTIVE)
	{
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
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			if (currentBulletFreq >= g_Config->GetValue(Config::BULLET_FREQUENCY))
			{
				glm::vec3 bulletPosition = glm::vec3(ship->m_position) + glm::vec3(0.0f, 0.8f, 0.0f);
				Bullet* bullet = new Bullet(bulletPosition, 0.2f, glm::vec3(0.0f, g_Config->GetValue(Config::BULLET_VELOCITY), 0.0f));
				m_scene.push_back(bullet);
				currentBulletFreq = 0;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		{
			if (g_Config->GetValue(Config::RAPID_FIRE) == 1.0f)
			{
				currentBulletFreq = g_Config->GetValue(Config::BULLET_FREQUENCY);
			}
			
		}
	}

	if (this->m_state == GAME_RESTART)
	{
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			// Restart game
			Restart();
		}
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