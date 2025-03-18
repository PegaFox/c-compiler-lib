#include "save_file.hpp"

#include <fstream>

extern std::string outputFilename;

void saveFile(const std::string& code)
{
  std::ofstream file(outputFilename);

  file << code;

  file.close();
}