#include "Ship.h"

#include "Common.h"
#include "Mesh.h"

#define HORIZONTAL_BOUND 12.0f

Ship::Ship(const glm::vec3 &pos, const glm::vec3 &scale, const float thrust, const float mass)
{
	Init();

	m_alive = true;

	m_position = pos;
	m_scale = scale;
	m_radius = m_scale.x / 2.0f;
	m_velocity = glm::vec3(0.0f);
	m_thrust = thrust;
	m_mass = mass;
	m_color = glm::vec3(1.0f);

	m_physicsActor = g_PhysicsPtr->AddDynamicActor(m_position, m_velocity, m_radius, glm::vec3(0.0f), mass);
	m_physicsActor->active = true;
	m_physicsActor->report = this;
}

Ship::~Ship()
{

}

void Ship::Init()
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	Vertex vertex;
	vertex.Position = glm::vec3(-0.5f, -0.5f, 0.2f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(-0.1f, -0.5f, 0.2f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.0f, 0.5f, 0.0f);
	vertices.push_back(vertex);

	vertex.Position = glm::vec3(-0.1f, -0.5f, 0.2f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.0f, -0.4f, 0.0f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.0f, 0.5f, 0.0f);
	vertices.push_back(vertex);

	vertex.Position = glm::vec3(0.0f, -0.4f, 0.0f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.1f, -0.5f, 0.2f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.0f, 0.5f, 0.0f);
	vertices.push_back(vertex);

	vertex.Position = glm::vec3(0.1f, -0.5f, 0.2f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.5f, -0.5f, 0.2f);
	vertices.push_back(vertex);
	vertex.Position = glm::vec3(0.0f, 0.5f, 0.0f);
	vertices.push_back(vertex);

	m_mesh = new Mesh(vertices, indices, textures);
}

void Ship::Render(Shader shader)
{
	m_position = m_physicsActor->pos;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, m_position);
	model = glm::scale(model, m_scale);
	shader.SetMatrix4("model", model);

	shader.SetVector4f("color", glm::vec4(m_color, 1.0f));
	shader.SetFloat("time", 0.0f);

	m_mesh->Draw(shader);
}

void Ship::Update(float deltaTime)
{
	m_physicsActor->accelerationForce = glm::vec3(0.0f);

	if (m_position.x < -HORIZONTAL_BOUND)
	{
		m_position.x = HORIZONTAL_BOUND;
		m_physicsActor->pos = m_position;
	}
	else if (m_position.x > HORIZONTAL_BOUND)
	{
		m_position.x = -HORIZONTAL_BOUND;
		m_physicsActor->pos = m_position;
	}
}

void Ship::OnContact(Physics::PhysicActor* other)
{
	m_alive = false;
}

void Ship::Reset(const glm::vec3 &pos)
{
	m_position = pos;
	m_physicsActor->pos = m_position;
	m_physicsActor->vel = glm::vec3(0.0f);
	m_alive = true;
}