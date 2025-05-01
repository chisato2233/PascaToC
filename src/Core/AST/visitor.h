#pragma once
#include <memory>
#include <sstream>


template<typename Derived>
class ASTVisitor {
public:
    void visit(auto node) {
        static_cast<Derived*>(this)->visitImpl(node);
    }
};


class CCodeGenVisitor : public ASTVisitor<CCodeGenVisitor>{
    public:
    std::ostringstream output;

    template<typename NodeType>
    void visitImpl(std::shared_ptr<NodeType>){
    }  
};