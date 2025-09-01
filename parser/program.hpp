class Program;

#ifndef PF_PARSER_PROGRAM_HPP
#define PF_PARSER_PROGRAM_HPP

#include <iostream>
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
struct ForLoop;

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
    friend ForLoop;

    std::vector<ASTnode*> nodes;

    Program();

    Program(const std::list<Token>& code, const Compiler::TypeSizes& typeSizes);

    void* arenaAlloc(std::size_t size);

    template<typename ObjType>
    ObjType* arenaAlloc(ObjType* object)
    {
      object = new((ObjType*)&dynamicData.first[dynamicData.second]) ObjType();
      dynamicData.second += sizeof(ObjType);

      return object;
    }

    std::string_view arenaAlloc(const std::string& sourceString);

    void parse(std::list<Token> code, const Compiler::TypeSizes& typeSizes);

  private:
    std::pair<std::unique_ptr<uint8_t[]>, std::size_t> dynamicData;

    std::map<std::string, ENUM_TYPE> enums;
    std::set<std::string> enumTypes;

    std::map<std::string, DataType*> typedefs;

    void parseEnum(CommonParseData& data);
};

#endif // PF_PARSER_PROGRAM_HPP
