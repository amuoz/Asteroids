#ifndef SHIP_H
#define SHIP_H

#include "Mesh.h"

const float SHIP_SPEED = 5.0f;

//Used as abstraction to stay away from window-system specific input methods
enum Ship_Movement {
	L,
	R
};

class Ship
{
public:

	glm::vec3 Position;

	Ship(const glm::vec3 & pos)
	{
		Position = pos;

		Init();
	}

	~Ship()
	{
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

	void Render(Shader &shader)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position);
		model = glm::scale(model, glm::vec3(1.5f));
		
		shader.setMat4("model", model);

		m_mesh->Draw(shader);
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Ship_Movement direction, float deltaTime)
	{
		float velocity = SHIP_SPEED * deltaTime;
		if (direction == L)
			Position.x -= velocity;
		if (direction == R)
			Position.x += velocity;
	}

private:

	Mesh* m_mesh;
};

#endif