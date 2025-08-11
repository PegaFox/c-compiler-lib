class Program;

#ifndef PF_PARSER_PROGRAM_HPP
#define PF_PARSER_PROGRAM_HPP

#include <vector>
#include <map>
#include <set>
#include <memory>

#include "AST_node.hpp"

struct Statement;
struct Expression;
struct Constant;
struct DataType;
struct Declaration;

typedef int32_t ENUM_TYPE;
//typedef uint8_t PFCC_SIZE_T;
//typedef uint8_t PFCC_PTR;
//typedef int16_t PFCC_PTRDIFF_T;

class Program: public ASTnode
{
  public:
    friend Statement;
    friend Expression;
    friend Constant;
    friend DataType;
    friend Declaration;

    std::vector<std::unique_ptr<ASTnode>> nodes;

    Program();

    Program(const std::list<Token>& code, const Compiler::TypeSizes& typeSizes);

    void parse(std::list<Token> code, const Compiler::TypeSizes& typeSizes);

  private:
    std::map<std::string, ENUM_TYPE> enums;
    std::set<std::string> enumTypes;

    std::map<std::string, std::unique_ptr<DataType>> typedefs;

    void parseEnum(CommonParseData& data);
};

#endif // PF_PARSER_PROGRAM_HPP
