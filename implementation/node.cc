#include "../headers/node.h"
#include "../headers/visitor.h"

void ProgramNode::accept(Visitor &v) { v.visit(*this); }
void FuncDeclNode::accept(Visitor &v) { v.visit(*this); }
void ParamNode::accept(Visitor &v) { v.visit(*this); }
void ScopeNode::accept(Visitor &v) { v.visit(*this); }
void ReturnNode::accept(Visitor &v) { v.visit(*this); }
void NumberNode::accept(Visitor &v) { v.visit(*this); }
void AssignmentNode::accept(Visitor &v) { v.visit(*this); }
void DeclarationNode::accept(Visitor &v) { v.visit(*this); }
void OperandNode::accept(Visitor &v) { v.visit(*this); }
void VariableNode::accept(Visitor &v) { v.visit(*this); }
void COCNode::accept(Visitor &v) { v.visit(*this); }
void PrintNode::accept(Visitor &v) { v.visit(*this); }
void Condition::accept(Visitor &v) { v.visit(*this); }
void ConditionBlock::accept(Visitor &v) { v.visit(*this); }
void ConditionStruct::accept(Visitor &v) { v.visit(*this); }
