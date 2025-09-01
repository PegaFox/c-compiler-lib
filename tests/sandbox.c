volatile char* welcomeString = "Hello, World!\n";

int main()
{
  *welcomeString = 'b';
  char helloString[] = "Hello, World!\n";
}
