#include "node.h"
#include "visitor.h"

void ProgramNode::accept(Visitor &v) { v.visit(*this); }
void ScopeNode::accept(Visitor &v) { v.visit(*this); }
void NumberNode::accept(Visitor &v) { v.visit(*this); }
void AssignmentNode::accept(Visitor &v) { v.visit(*this); }
void DeclarationNode::accept(Visitor &v) { v.visit(*this); }
void OperandNode::accept(Visitor &v) { v.visit(*this); }
void VariableNode::accept(Visitor &v) { v.visit(*this); }
void COCNode::accept(Visitor &v) { v.visit(*this); }
void PrintNode::accept(Visitor &v) { v.visit(*this); }
