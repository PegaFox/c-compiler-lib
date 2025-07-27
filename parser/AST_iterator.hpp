#ifndef PF_PARSER_AST_ITERATOR_HPP
#define PF_PARSER_AST_ITERATOR_HPP

#include <vector>

#include "AST_node.hpp"

struct ASTiterator
{
  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = ASTnode;
    using pointer = ASTnode*;
    using reference = ASTnode&;

    ASTiterator(pointer ptr);

    pointer operator*() const;
    pointer operator->();

    std::size_t depth();

    ASTiterator& operator++();

    ASTiterator operator++(int);

    friend bool operator== (const ASTiterator& a, const ASTiterator& b) {return a.ptr == b.ptr;};
    friend bool operator!= (const ASTiterator& a, const ASTiterator& b) {return a.ptr != b.ptr;};

    // elements in path are arrays of 2 pointers, first is the intersection node, second is the path taken (represented by a pointer to a std::unique_ptr)
    std::vector<std::pair<pointer, void*>> path;

    pointer ptr;

  private:
    bool firstTime(pointer ptr);
};

#endif // PF_PARSER_AST_ITERATOR_HPP
