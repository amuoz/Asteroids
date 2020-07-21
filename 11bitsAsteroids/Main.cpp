#include "Common.h"
#include "Game.h"

#define PI 3.1415926535897932

Game* g_game;

int main()
{
	// ..:: config file ::..
	// process config file

	// cannon shooting feq (shots / sec)
	float bulletFrequency = 0.5f;

	// cannon bullet velocity (m / sec)
	float bulletVelocity = 15.0f;

	// explosion duration (sec)
	float explosionDuration = 1.0f;

	// spaceship forward velocity (m / sec)
	float forwardVelocity = 4.0f;

	// asteroid angular velocity range (rad / sec)
	float angularVelocity = 2 * PI;

	// manoeuvering engines thrust (newtons)
	float thrust = 9999.0f;

	// spaceship mass (kg)
	float mass = 500.0f;

	// initial asteroid appearance freq (n / sec)
	float freq = 1.0f;	// f = 1 / T , T=0.5

	// asteroid appearance freq increase (n / sec)
	float freqIncrease = 0.02f;

	g_game = new Game(forwardVelocity, angularVelocity, thrust, mass, freq, freqIncrease, bulletVelocity, bulletFrequency, explosionDuration);
	std::cout << "==================== GAME INITIALIZED =======================" << std::endl;

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
