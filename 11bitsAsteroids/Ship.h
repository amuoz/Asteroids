#ifndef SHIP_H
#define SHIP_H

#include "Common.h"
#include "Mesh.h"
#include "Physics.h"

#include "ICircleContactReport.h"

#define HORIZONTAL_BOUND 12.0f

const float SHIP_SPEED = 5.0f;

//extern Physics* g_PhysicsPtr;

//Used as abstraction to stay away from window-system specific input methods
enum Ship_Movement {
	L,
	R
};

class Ship: ICircleContactReport
{
public:

	glm::vec3 m_position;
	glm::vec3 m_scale;
	glm::vec3 m_velocity;
	float m_radius;
	float m_thrust;
	float m_mass;
	glm::vec3 m_color;

	bool m_alive;

	// physics pointer
	Physics::PhysicActor* m_physicsActor;


	Ship(const glm::vec3 &pos, const glm::vec3 &scale, const float thrust, const float mass)
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

	~Ship()
	{
		//delete m_physicsActor;
		delete m_mesh;
	}

	void Init()
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

		/*
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		*/

		m_mesh = new Mesh(vertices, indices, textures);
	}

	void Render(Shader shader)
	{
		m_position = m_physicsActor->pos;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_position);
		model = glm::scale(model, m_scale);
		shader.SetMatrix4("model", model);
		
		shader.SetVector3f("color", m_color);
		shader.SetFloat("time", 0.0f);

		m_mesh->Draw(shader);
	}

	void Update(float deltaTime)
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

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Ship_Movement direction, float deltaTime)
	{
		float velocity = SHIP_SPEED * deltaTime;
		if (direction == L) 
		{
			m_position.x -= velocity;
		}

		if (direction == R)
		{
			m_position.x += velocity;
		}
			
	}

	void OnContact(Physics::PhysicActor* other) override
	{
		m_alive = false;
	}

	void Reset(const glm::vec3 &pos)
	{
		m_position = pos;
		m_physicsActor->pos = m_position;
		m_physicsActor->vel = glm::vec3(0.0f);
		m_alive = true;
	}

private:

	Mesh* m_mesh;
};

#endif