#pragma once
#include "spdlog/spdlog.h"

#include "VisitorBase.h"

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




