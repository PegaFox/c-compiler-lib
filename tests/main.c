
struct Game
{
  int tiles[4];
  int width;
}* game;

int main()
{
  int x, y;

  //(game->(width * y)) + x; Current state
  //game->(width * (y + x)); With no binary reformatting
  //game->(width * (y + x)); With only associativity changing
  //game->(width * y) + x; With less-or-equal on precedence checking
  //game->(width * (y + x)); With greater-or-equal on precedence checking
  //game->(width * (y + x)); With greater on precedence checking
  //
  /*

  ->
 g  *
   w +
    y x

  -> 
 g  +
   * x
  w y

    +
  -> x
 g  *
   w y

    +
   * x
 -> y
g  w
  */
  &game->tiles[game->width * y + x];
}
