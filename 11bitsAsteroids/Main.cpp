#include "Common.h"
#include "Game.h"

Game* g_game;

int main()
{
	g_game = new Game();
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
