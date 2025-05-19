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

template<typename Tuple, typename Node>
inline bool dispatch_tuple(ASTVisitorBase& vb, Node& node)
{
    bool handled = false;

    auto tryCast = [&](auto* vis) -> bool {
        if (auto* real = dynamic_cast<std::decay_t<decltype(*vis)>*>(&vb)) {
            real->visit(node);
            return handled = true;
        }
        return false;
    };

    std::apply([&](auto&&... dummy) {
        ( ... || tryCast(static_cast<std::add_pointer_t<std::decay_t<decltype(dummy)>>>(nullptr)) );
    }, Tuple{});

    return handled;
}
