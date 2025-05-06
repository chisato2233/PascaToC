#pragma once
#include <memory>
#include <sstream>
#include <typeinfo>

#include "VisitorBase.h"
#include "CCodeGenVisitor.h"
#include "LlvmVisitor.h"


using AllVisitors = std::tuple<
        CCodeGenVisitor, 
        LlvmVisitor
>;


template<std::size_t I = 0,
         typename Tuple,
         typename Node>
inline bool dispatch_tuple(ASTVisitorBase& vb, Node& node)
{
    if constexpr (I < std::tuple_size_v<Tuple>) {
        using Vis = std::tuple_element_t<I, Tuple>;

        if (auto *real = dynamic_cast<Vis*>(&vb)) {
            real->visit(node);
            return true;
        }
        return dispatch_tuple<I+1, Tuple>(vb, node);
    } else {
        return false;
    }
}