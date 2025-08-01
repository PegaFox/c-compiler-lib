#ifndef PF_PARSER_PROGRAM_HPP
#define PF_PARSER_PROGRAM_HPP

#include <vector>
#include <list>
#include <map>
#include <set>
#include <memory>

#include "../lexer.hpp"
#include "AST_node.hpp"

struct Statement;
struct DataType;
struct Declaration;

typedef uint32_t ENUM_TYPE;
//typedef uint8_t PFCC_SIZE_T;
//typedef uint8_t PFCC_PTR;
//typedef int16_t PFCC_PTRDIFF_T;

class Program: public ASTnode
{
  public:
    friend Statement;
    friend DataType;
    friend Declaration;

    std::vector<std::unique_ptr<ASTnode>> nodes;

    Program();

    Program(const std::list<Token>& code);

    void parse(std::list<Token> code);

  private:
    std::map<std::string, ENUM_TYPE> enums;
    std::set<std::string> enumTypes;

    std::map<std::string, std::unique_ptr<DataType>> typedefs;

    void parseEnum(std::list<Token>& code);
};

#endif // PF_PARSER_PROGRAM_HPP
