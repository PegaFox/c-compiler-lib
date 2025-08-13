#include "libminesweeper/include/minesweeper.h"

int main()
{
  uint8_t gameMemory[1024];

  struct minesweeper_game* game = minesweeper_init(10, 10, 0.15f, gameMemory);

  minesweeper_set_cursor(game, 5, 5);
  minesweeper_open_tile(game, game->selected_tile);
}
