#ifndef PF_PREPROCESSOR_HPP
#define PF_PREPROCESSOR_HPP

#include <array>
#include <string>
#include <vector>
#include <list>

class Preprocessor
{
  public:
    Preprocessor();

    // like preprocess, but updates the original variable when run
    Preprocessor(std::string workingDir, std::string& code, const std::vector<std::string>& includeDirs);

    std::string preprocess(std::string workingDir, std::string code, const std::vector<std::string>& includeDirs);

  private:
    std::list<std::array<std::string, 2>> definitions;

    std::vector<std::size_t> conditionalDirectives;

    void removeSingleLineComments(std::string& code);

    void removeMultiLineComments(std::string& code);

    void removeUnwantedNewlines(std::string& code);

    void convertConstantTypes(std::string& code);

    std::string handleIncludeDirective(std::string& workingDir, std::string& directiveStr, const std::vector<std::string>& includeDirs);

    bool handleConditionalDirective(std::string& directiveStr);

    void handlePreprocessingDirectives(std::string& workingDir, std::string& code, const std::vector<std::string>& includeDirs);

    void resolveDefinitions(std::string& code);
};

#endif // PF_PREPROCESSOR_HPP
