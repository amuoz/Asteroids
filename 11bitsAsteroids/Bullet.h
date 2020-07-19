#ifndef BULLET_H
#define BULLET_H

#include "Actor.h"

class Bullet: public Actor, ICircleContactReport
{
public:

	Bullet(const glm::vec3 &pos, float scale, const glm::vec3 &vel);
	~Bullet();

	void Init();

	void Render(Shader &shader) override;
	void Update(float deltaTime) override;

	void OnContact() override;
};

#endif