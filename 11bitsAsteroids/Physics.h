#ifndef PHYSICS_H
#define PHYSICS_H

#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MAX_DYNAMICS 1000

class ICircleContactReport
{
public:
	virtual void OnContact() = 0;
};

class Physics
{
public:
	
	struct PhysicActor
	{
		bool active;

		glm::vec3 vel;
		glm::vec3 pos;

		float mass;
		glm::vec3 accelerationForce;

		ICircleContactReport* report = 0;
	};

	// physics sphere geometry
	struct sDynamicGeometryCircle
	{
		float radius;
		PhysicActor actorInfo;
	};

	Physics(const glm::vec3 &gravity);
	~Physics();

	void Update(float deltaTime);
	void UpdateDymanicPos(sDynamicGeometryCircle &geom, float deltaTime);

	// sphere collision
	bool CheckCircleCircleCollision(const glm::vec3 &circle1Pos, float circle1Radius, const glm::vec3 &circle2Pos, float circle2Radius,
		glm::vec3 &col, glm::vec3 &normal);

	PhysicActor* AddDynamicActor(const glm::vec3 &pos, const glm::vec3 &vel, float radius, glm::vec3 force = glm::vec3(0.0f), float mass = 1.0f);

private:

	glm::vec3 m_gravityForce;

	unsigned int m_numDynamic;
	sDynamicGeometryCircle *m_dynamicActors[MAX_DYNAMICS];
};

#endif // !PHYSICS_H
