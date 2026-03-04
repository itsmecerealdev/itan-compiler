#include "visitor.h"
#include "lexer.h"
#include "node.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>

//Helper functions
uint32_t typeRank(ValueType t) {
	switch(t) {
		case ValueType::LITERAL:
		case ValueType::int64:
			return 0;
		case ValueType::int32:
			return 1;
		default:
			throw runtime_error("typeRank() called on non-numeric type.");
	}
}

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

void Visitor::visit(COCNode& node) {
	if(node.expressions.size() <= 0) return;
	for(const auto &n : node.expressions) {
		n->accept(*this);
	}
}

void Visitor::visit(PrintNode& node) {
	node.expression->accept(*this);
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

void PrintVisitor::visit(COCNode& node) {
	tabHelper();
	std::cout << node.name << " : params ->";
	for(int i = 0; i < node.expressions.size(); i++) {
		node.expressions.at(i)->accept(*this);
		if(i + 1 < node.expressions.size()) cout << ", ";
	}
}

void PrintVisitor::visit(VariableNode& node) {
	tabHelper();
	std::cout << "Variable " << node.name << "\n";
}

void PrintVisitor::visit(PrintNode& node) {
	tabHelper();
	std::cout << "print: ";
	node.expression->accept(*this);
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

void SemanticsVisitor::visit(COCNode& node){
	Visitor::visit(node);
	if(typeKeywords.contains(node.name) && node.expressions.size() == 1) {
		node.resolution = COCResolution::Cast; 
		node.type = typeKeywords.at(node.name);
	}
	else if(typeKeywords.contains(node.name)) {
		throw runtime_error("Cast: " + node.name + " has too many parameters."); 
	}
	else {
		node.resolution = COCResolution::Call; 
	}
}

//EvaluatorVisitor

void EvaluatorVisitor::visit(ProgramNode& node) {
	Visitor::visit(node);
	// std::cout << stack.back() << std::endl;
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

void EvaluatorVisitor::visit(COCNode& node) {
	if(node.expressions.size() == 1) {
		node.expressions[0]->accept(*this);
		int64_t val = stack.back();
		stack.pop_back();
		switch(node.type) {
			case ValueType::int64:
				stack.push_back(val);
				break;
			case ValueType::int32:
				int32_t narrow = static_cast<int32_t>(val);;
				stack.push_back(static_cast<int64_t>(narrow));
				break;
		}
	}
	else {
		//temporary
		throw runtime_error("Currently, only casting is supported. Can only contain one expression.");
	}
}

void EvaluatorVisitor::visit(PrintNode& node) {
	Visitor::visit(node);
	std::cout << stack.back() << std::endl;
}

//TypeVisitor

void TypeVisitor::visit(COCNode& node) {
	Visitor::visit(node);
	for(int i = 0; i < node.expressions.size(); i++) stack.pop_back();
	if(node.resolution == COCResolution::Cast) {
		stack.push_back(node.type);
	}
	else {
		throw runtime_error("Only casts are supported for now.");
	}
}

void TypeVisitor::visit(NumberNode& node) {
	stack.push_back(ValueType::int64);
}

void TypeVisitor::visit(VariableNode& node) {
	stack.push_back(Context.at(node.name)->varType);	
}

void TypeVisitor::visit(AssignmentNode& node) {
	node.expression->accept(*this);
	ValueType vt = stack.back();
	stack.pop_back();
	uint32_t left = typeRank(Context.at(node.name)->varType); 
	uint32_t right = typeRank(vt);
	if(left < right) {
		throw runtime_error("Implicit narrowing requires explicit cast.");	
	}
}

void TypeVisitor::visit(DeclarationNode& node) {
	node.expression->accept(*this);
	ValueType vt = stack.back();
	stack.pop_back();
	uint32_t left = typeRank(Context.at(node.name)->varType); 
	uint32_t right = typeRank(vt);
	if(right < left) {
		throw runtime_error("Implicit narrowing requires explicit cast.");	
	}
}

void TypeVisitor::visit(OperandNode& node) {
	node.lhs->accept(*this);
	node.rhs->accept(*this);
	ValueType right = stack.back();
	stack.pop_back();
	ValueType left = stack.back();
	stack.pop_back();
	ValueType temp = typeRank(left) < typeRank(right) ? left : right;
	stack.push_back(temp);
}

void TypeVisitor::visit(ProgramNode& node) {
	Visitor::visit(node);	
}
