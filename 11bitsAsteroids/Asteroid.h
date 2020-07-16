#ifndef ASTEROID_H
#define ASTEROID_H

#include "Mesh.h"
#include "Physics.h"

extern Physics* g_PhysicsPtr;

class Asteroid
{
public:
	glm::vec3 m_position;
	glm::vec3 m_scale;
	float m_rotAngle;
	float m_radius;
	glm::vec3 m_velocity;
	
	// physics pointer
	Physics::PhysicActor* m_physicsActor;

	Asteroid(glm::vec3 pos, glm::vec3 scale, float rotAngle, glm::vec3 vel)
	{
		Init();

		m_position = pos;
		m_scale = scale;
		m_rotAngle = rotAngle;
		m_radius = scale.x / 2.0f;
		m_velocity = vel;

		m_physicsActor = g_PhysicsPtr->AddDynamicActor(m_position, m_velocity, m_radius);
	}

	~Asteroid()
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

	void Render(Shader &shader)
	{
		m_position = m_physicsActor->pos;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_position);
		model = glm::scale(model, m_scale);
		model = glm::rotate(model, (float)glfwGetTime() * m_rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		shader.setMat4("model", model);

		m_mesh->Draw(shader);
	}

private:
	Mesh* m_mesh;
};

#endif