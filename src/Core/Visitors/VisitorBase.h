#pragma once
#include <typeinfo>
#include "spdlog/spdlog.h"

class ASTVisitorBase {
public:
    virtual ~ASTVisitorBase() = default;   
    void defaultVisit(auto&& node) {
        spdlog::error("No visitor {} found for node type: {}",typeid(this).name(), typeid(node).name());
    }
};

template<typename Derived>
class VisitImplCRTP : public ASTVisitorBase {
public:
    template<class Node>
    void visit(Node& n)
    {
        if constexpr (requires (Derived& dv, Node& nn) { dv.visit(nn); })
            static_cast<Derived&>(*this).visit(n);
        else
            this->defaultVisit(n);
    }
};

