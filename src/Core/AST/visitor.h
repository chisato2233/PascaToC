#pragma once
#include <memory>
#include <sstream>
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


class CCodeGenVisitor : public VisitImplCRTP<CCodeGenVisitor> {
public:
    std::ostringstream output;
    inline static std::vector<std::string> GlobalFunctions;
    
    // 添加上下文信息
    bool isLeftSideOfAssignment = false; // 表示当前是否在赋值语句的左侧
    
    std::string current_function_name = "";
    
    CCodeGenVisitor() = default;
    explicit CCodeGenVisitor(std::ostringstream& os) : output(os.str()) {}
    
    std::string getCode() const {
        return output.str();
    }

    template<typename NodeType>
    void visit(NodeType& node) {
        spdlog::error("No CCodeGenVisitor method found for node type: {}", typeid(node).name());
    }
};






using AllVisitors = std::tuple<CCodeGenVisitor
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