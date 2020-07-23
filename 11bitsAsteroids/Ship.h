#ifndef SHIP_H
#define SHIP_H

#include "Actor.h"
#include "Physics.h"
#include "ICircleContactReport.h"

const float SHIP_SPEED = 5.0f;

class Shader;

//Used as abstraction to stay away from window-system specific input methods
enum Ship_Movement {
	L,
	R
};

class Ship: public Actor, ICircleContactReport
{
public:

	Ship(const glm::vec3 &pos, const glm::vec3 &scale);
	~Ship();

	void Init();

	void Render(Shader shader) override;

	void Update(float deltaTime) override;

	void OnContact(Physics::PhysicActor* other) override;

	void Reset() override;

public:

	// accerelated horizontal movement
	float m_thrust;
	float m_mass;

	bool m_alive;
};

#endif