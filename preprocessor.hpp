#ifndef PF_PREPROCESSOR_HPP
#define PF_PREPROCESSOR_HPP

#include <string>
#include <vector>
#include <list>

class Preprocessor
{
  public:
    Preprocessor();

    // like preprocess, but updates the original variable when run
    Preprocessor(std::string& code);

    std::string preprocess(std::string code);

  private:
    std::list<std::array<std::string, 2>> definitions;

    std::vector<std::size_t> conditionalDirectives;

    void removeSingleLineComments(std::string& code);

    void removeMultiLineComments(std::string& code);

    void removeUnwantedNewlines(std::string& code);

    void convertConstantTypes(std::string& code);

    std::string handleIncludeDirective(std::string& directiveStr);

    bool handleConditionalDirective(std::string& directiveStr);

    void handlePreprocessingDirectives(std::string& code);

    void resolveDefinitions(std::string& code);
};

#endif // PF_PREPROCESSOR_HPP
