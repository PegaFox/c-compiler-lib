
int realFunction()
{
  return 42;
}

int (*proxyFunction)();

int (*otherProxy)();

int main()
{
  proxyFunction = realFunction;

  proxyFunction();

  int* otherPointer = (int*)proxyFunction;
  otherProxy = *proxyFunction;
}
