#include "visitor.h"
#include "lexer.h"
#include "node.h"
#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

//There is NO default visit pattern for terminal nodes, so these are stubbed.
//If a visitor concrete class doesn't need anything from these node types, then they are simply skipped
void Visitor::visit(NumberNode& node) { }
void Visitor::visit(VariableNode& node) { }

//If concrete visitors do not need information from these node types, then it simply traverses the children.
//These are overridden if the concrete visitor requires some functionality other than walking the tree
void Visitor::visit(ProgramNode &node) {
	if(node.statements.size() <= 0) return;
	for(const auto &n : node.statements) {
		n->accept(*this);
	}
}

void Visitor::visit(OperandNode& node) { 
	if(node.lhs) node.lhs->accept(*this);
	if(node.rhs) node.rhs->accept(*this);
}

void Visitor::visit(AssignmentNode& node) { 
	if(node.expression) node.expression->accept(*this); 
}

void Visitor::visit(DeclarationNode& node) { 
	if(node.expression) node.expression->accept(*this); 
}

//PrintVisitorOverrides

void PrintVisitor::tabHelper() {
	for(int i = 0; i < depth; i++) {
		std::cout << "\t";
	}
}

void PrintVisitor::visit(ProgramNode &node) {
	tabHelper();
	std::cout << "Program" << std::endl;
	depth++;
	for(const auto &c : node.statements) {
		c->accept(*this);
	}
}

void PrintVisitor::visit(NumberNode& node) {
	tabHelper();
	std::cout << "Number(" << node.val << ")\n";
}

void PrintVisitor::visit(OperandNode& node) {
	tabHelper();
	std::cout << "Operand " << static_cast<int>(node.type) << "\n";
	depth++;
	Visitor::visit(node);
	depth--;
}

void PrintVisitor::visit(AssignmentNode& node) {
	tabHelper();
	std::cout << "Assignment " << node.name << " =\n";
	depth++;
	node.expression->accept(*this);
	depth--;
}

void PrintVisitor::visit(DeclarationNode& node) {
	tabHelper();
	std::cout << "Declaration " << node.name << " =\n";
	depth++;
	node.expression->accept(*this);
	depth--;
}

void PrintVisitor::visit(VariableNode& node) {
	tabHelper();
	std::cout << "Variable " << node.name << "\n";
}

//DeclarationVisitor overrides
void DeclarationVisitor::visit(DeclarationNode& node) {
	if(Context.contains(node.name)) {
		throw runtime_error("Double declaration of: " + node.name + "\n");
	}
	
	Context[node.name] = &node;
}

//SemanticVisitor overrides
void SemanticsVisitor::visit(VariableNode& node) {
	if(!Context.contains(node.name)) {
		throw runtime_error("Use of undeclared variable: " + node.name + "\n");
	}
	if(currentInit != "" && node.name == currentInit) {
		throw runtime_error("Self-assignment in declaration of var: " + node.name + "\n");
	}
}

void SemanticsVisitor::visit(AssignmentNode& node) {
	if(!Context.contains(node.name)) {
		throw runtime_error("Use of undeclared variable: " + node.name + "\n");
	}
	node.expression->accept(*this);
}

void SemanticsVisitor::visit(DeclarationNode &node) {
	currentInit = node.name;
	node.expression->accept(*this);
	currentInit.erase();
}

//EvaluatorVisitor

void EvaluatorVisitor::visit(ProgramNode& node) {
	Visitor::visit(node);
	std::cout << stack.back() << std::endl;
}

void EvaluatorVisitor::visit(NumberNode& node) {
	stack.push_back(node.val);
}

void EvaluatorVisitor::visit(OperandNode& node) { 
	Visitor::visit(node);
	int64_t right = stack.back();
	stack.pop_back();
	int64_t left = stack.back();
	stack.pop_back();
	int64_t result = 0;
	switch(node.type) {
		case TokenType::Plus:
			result = left + right;
			break;
		case TokenType::Minus:
			result = left - right;
			break;
		case TokenType::Star:
			result = left * right;
			break;
		case TokenType::Slash:
			if(right == 0) throw runtime_error("Divide by zero: " + to_string(left)); 
			result = left / right;
			break;
		case TokenType::Exponent:
			result = 1;
			while(right > 0) {
				if(right % 2 == 1) {
					result *= left;
				}
				left *= left;
				right /= 2;
			}
			break;
	}
	stack.push_back(result);
}

void EvaluatorVisitor::visit(AssignmentNode& node) {
	node.expression->accept(*this);
	runtime[node.name] = stack.back();
	stack.pop_back();
}

void EvaluatorVisitor::visit(DeclarationNode& node) {
	node.expression->accept(*this);
	runtime[node.name] = stack.back();
	stack.pop_back();
}

void EvaluatorVisitor::visit(VariableNode& node) {
	stack.push_back(runtime[node.name]);
}
