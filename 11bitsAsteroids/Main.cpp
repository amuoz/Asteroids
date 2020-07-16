#include "Game.h"

#define PI 3.1415926535897932

Game *g_game;
int main()
{
	// ..:: config file ::..
	// process config file

	// cannon shooting feq (shots / sec)

	// cannon bullet velocity (m / sec)

	// explosion duration (sec)

	// spaceship forward velocity (m / sec)
	float forwardVelocity = 2.5f;

	// asteroid angular velocity range (rad / sec)
	float angularVelocity = 2 * PI;

	// manoeuvering engines thrust (newtons)
	float thrust = 9999.0f;

	// spaceship mass (kg)
	float mass = 500.0f;

	// initial asteroid appearance freq (n / sec)

	// asteroid appearance freq increase (n / sec)

	g_game = new Game(forwardVelocity, angularVelocity, thrust, mass);
	cout << "==================== GAME INITIALIZED =======================";

	// game loop
	while (!g_game->IsFinished())
	{
		g_game->Update();

		g_game->Render();
	}
	// free resources
	delete g_game;

	return 0;
}
