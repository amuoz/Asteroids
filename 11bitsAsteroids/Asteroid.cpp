#include "Asteroid.h"
#include "Common.h"
#include "Mesh.h"

#include <GLFW/glfw3.h>

#define PI 3.1415926535897932
#define VERTICAL_BOUND 10.0f


//extern Game *g_game;
//extern Physics* g_PhysicsPtr;


Asteroid::Asteroid()
{
	Init();

	Reset();

	m_physicsActor = g_PhysicsPtr->AddDynamicActor(m_position, m_velocity, m_radius);
	m_physicsActor->report = this;
}

Asteroid::~Asteroid()
{
	//delete m_physicsActor;
	delete m_mesh;
}

void Asteroid::Init()
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	Vertex vertex;
	vertex.Position = glm::vec3(-0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, 0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, -0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, -0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, -0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, 0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, -0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, -0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, -0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, -0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, -0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, -0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, 0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, -0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, 0.5f, 0.5f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.5f, 0.5f, -0.5f);
	vertices.push_back(vertex);

	/*
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	*/

	m_mesh = new Mesh(vertices, indices, textures);
}

void Asteroid::Render(Shader shader)
{
	m_position = m_physicsActor->pos;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_position);
	model = glm::scale(model, m_scale);
	model = glm::rotate(model, (float)glfwGetTime() * m_rotAngle, m_rotAxis);

	shader.SetMatrix4("model", model);

	m_mesh->Draw(shader);
}

void Asteroid::Update(float deltaTime)
{

}

/*
void Asteroid::SetActive(bool newActive)
{
	m_physicsActor->active = newActive;
}
*/

float Asteroid::Randf(float min, float max)
{
	return (float)(((rand() & 32767)*(1.0 / 32767.0))*(max - min) + min);
}

bool Asteroid::OutOfBounds()
{
	if (m_position.y < VERTICAL_BOUND * -1.0f)
	{
		return true;
	}
	return false;
}

void Asteroid::OnContact()
{
	m_exploded = true;
}

bool Asteroid::HasExploded()
{
	return m_exploded;
}

void Asteroid::Reset()
{	
	float offset = 10.0f;
	// 1. translation: displace along circle with 'radius' in range [-offset, offset]
	float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
	float x = displacement;
	float y = (rand() % 20) + 10.0f; // keep height of field smaller compared to width of x and z

	// 2. scale: scale between 0.05 and 0.25f / 0.5 and 1
	float scale = (rand() % 100) / 100.0f + 0.5;

	// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
	float rotAngle = Randf(g_Config->m_angularVelocity / 2, g_Config->m_angularVelocity);

	m_position = glm::vec3(x, y, 0.0f);
	m_scale = glm::vec3(scale);
	m_rotAngle = rotAngle;
	m_rotAxis = glm::vec3((float)rand(), (float)rand(), (float)rand());
	m_radius = m_scale.x / 2.0f;
	m_velocity = glm::vec3(0.0f, -(g_Config->m_forwardVelocity), 0.0f);
	m_exploded = false;

	if (m_physicsActor)
	{
		SetActive(false);
		m_physicsActor->pos = m_position;
		m_physicsActor->report = this;
	}
}
