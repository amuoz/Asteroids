#include "Game.h"

Game *g_game;
int main()
{
	g_game = new Game();

	// game loop
	while (!g_game->IsFinished())
	{
		g_game->Control();

		g_game->Render();
	}
	// free resources
	delete g_game;

	return 0;
}
