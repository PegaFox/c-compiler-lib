
struct Bar
{
  char data[128];
};

int;

char;

struct Bar;

typedef unsigned short int foo;

int main()
{
  int halfArrayLen = 12;
    
  // this type of thing is only legal in local scope
  char testArray[halfArrayLen*2];

  (const int(*)[6])testArray;

  (volatile const int*)0;
  return 0;
}
