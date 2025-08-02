#include "load_file.hpp"
#include <fstream>
#include <sstream>

std::string loadFile(const std::string& filename)
{
  std::ifstream inputFile(filename);

  std::stringstream fileText;
  fileText << inputFile.rdbuf();

  inputFile.close();

  return fileText.str();
}
