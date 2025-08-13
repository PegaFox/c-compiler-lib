
#define RAND_MAX 255

static unsigned char seed = 0;

int rand()
{
  return seed++;
}

