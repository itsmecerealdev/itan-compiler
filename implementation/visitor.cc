#include "../headers/visitor.h"
#include "../headers/lexer.h"
#include "../headers/node.h"
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
	if(node.global) node.global->accept(*this);
}

void Visitor::visit(ScopeNode &node) {
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
	if(node.global) node.global->accept(*this);
}

void PrintVisitor::visit(ScopeNode &node) {
	tabHelper();
	std::cout << "{" << std::endl;
	depth++;
	if(node.statements.size()) {
		Visitor::visit(node);
	}
	depth--;
	tabHelper();
	std::cout << "}" << std::endl;
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
	std::cout << node.name << " : params ->\n";
	depth++;
	for(int i = 0; i < node.expressions.size(); i++) {
		node.expressions.at(i)->accept(*this);
		if(i + 1 < node.expressions.size()) cout << ", ";
	}
	depth--;
}

void PrintVisitor::visit(VariableNode& node) {
	tabHelper();
	std::cout << "Variable " << node.name << "\n";
}

void PrintVisitor::visit(PrintNode& node) {
	tabHelper();
	std::cout << "print:\n";
	node.expression->accept(*this);
}

//DeclarationVisitor overrides

void DeclarationVisitor::visit(ScopeNode& node) {
	if(!scopes.empty()) {
		node.scope->parent = scopes.back();
	}
	scopes.push_back(node.scope);
	Visitor::visit(node);
	scopes.pop_back();
}

void DeclarationVisitor::visit(DeclarationNode& node) {
	if(scopes.back()->context.size() && scopes.back()->context.contains(node.name)) {
		throw runtime_error("Double declaration of: " + node.name + "\n");
	}
	
	scopes.back()->context[node.name] = &node;
}

bool stackContains(const string &name, Scope* scope) {
	Scope* p = scope;
	while(p) {
		if(p->context.contains(name)) return true;
		p = p->parent;
	}
	return false;
}

DeclarationNode* getDeclaration(const string &name, Scope* scope) {
	Scope* p = scope;
	while(p) {
		if(p->context.contains(name)) return p->context[name];
		p = p->parent;
	}
	return nullptr;
}

//SemanticVisitor overrides
void SemanticsVisitor::visit(ScopeNode &node) {
	currscope = node.scope;
	Visitor::visit(node);
	currscope = currscope->parent;
}

void SemanticsVisitor::visit(VariableNode& node) {
	if(!stackContains(node.name, currscope)) {
		throw runtime_error("Use of undeclared variable: " + node.name + "\n");
	}
	if(currentInit != "" && node.name == currentInit) {
		throw runtime_error("Self-assignment in declaration of var: " + node.name + "\n");
	}
}

void SemanticsVisitor::visit(AssignmentNode& node) {
	if(!stackContains(node.name, currscope)) {
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

Value makeUInt64(uint64_t val) {
	Value v;
	v.setType(ValueType::int64);
	v.setUInt(val);
	return v;
}

Value EvaluatorVisitor::getRuntimeValUI64(const std::string &name) {
	return makeUInt64(runtime.back()[name].getUInt<uint64_t>());
}

Value& EvaluatorVisitor::getRuntimeVal(const std::string &name) {
	for(auto iter = runtime.rbegin(); iter != runtime.rend(); iter++) {
		auto found = iter->find(name);
		if(found != iter->end())
			return found->second;
	}
	throw runtime_error("Variable not found: " + name);
}

void EvaluatorVisitor::visit(ProgramNode& node) {
	Visitor::visit(node);
	// std::cout << stack.back() << std::endl;
}

void EvaluatorVisitor::visit(ScopeNode& node) {
	runtime.push_back({});
	size_t stackStart = stack.size();
	for(const auto &d : node.scope->context) {
		Value v;
		d.second->expression->accept(*this);
		v = stack.back();
		runtime.back()[d.first] = v;
		stack.pop_back();
	}
	Visitor::visit(node);
	stack.resize(stackStart);
	runtime.pop_back();
}

void EvaluatorVisitor::visit(NumberNode& node) {
	stack.push_back(makeUInt64(node.val));
}

void promote(Value &v1, Value &v2) {
	ValueType t1 = v1.getType();
	ValueType t2 = v2.getType();

	if(typeRank(t1) <= typeRank(t2)) {
		v2.promoteTo(v1);
	}
	else {
		v1.promoteTo(v2);
	}
}

void EvaluatorVisitor::visit(OperandNode& node) { 
	Visitor::visit(node);
	Value right = stack.back();
	stack.pop_back();
	Value left = stack.back();
	stack.pop_back();
	Value result;
	promote(left, right);
	result.setType(left.getType());
	switch(node.type) {
		case TokenType::Plus:
			result.setUInt(left.getUInt<uint64_t>() + right.getUInt<uint64_t>());
			break;
		case TokenType::Minus:
			result.setUInt(left.getUInt<uint64_t>() - right.getUInt<uint64_t>());
			break;
		case TokenType::Star:
			result.setUInt(left.getUInt<uint64_t>() * right.getUInt<uint64_t>());
			break;
		case TokenType::Slash:
			if(right.getUInt<uint64_t>() == 0) throw runtime_error("Divide by zero: " + to_string(left.getUInt<uint64_t>())); 
			result.setUInt(left.getUInt<uint64_t>() / right.getUInt<uint64_t>());
			break;
		case TokenType::Exponent:
			uint64_t res = 1;
			uint64_t l = left.getUInt<uint64_t>();
			uint64_t r = right.getUInt<uint64_t>();
			while(r > 0) {
				if(r % 2 == 1) {
					res *= l;
				}
				l *= l;
				r /= 2;
			}
			result.setUInt(res);
			break;
	}
	stack.push_back(result);
}

void EvaluatorVisitor::visit(AssignmentNode& node) {
	node.expression->accept(*this);
	Value &v = getRuntimeVal(node.name);
	switch(v.getType()) {
		case ValueType::int64:
			stack.back().setType(ValueType::int64);
			v.setUInt(stack.back().getUInt<uint64_t>());
			break;
		case ValueType::int32:
			stack.back().setType(ValueType::int32);
			v.setUInt(stack.back().getUInt<uint32_t>());
			break;
	}
	stack.pop_back();
}

void EvaluatorVisitor::visit(DeclarationNode& node) {
	node.expression->accept(*this);
	switch(node.varType) {
		case ValueType::int64:
			stack.back().setType(ValueType::int64);
			runtime.back()[node.name].setUInt(stack.back().getUInt<uint64_t>());
			break;
		case ValueType::int32:
			stack.back().setType(ValueType::int32);
			runtime.back()[node.name].setUInt(stack.back().getUInt<uint32_t>());
			break;
	}
	stack.pop_back();
}

void EvaluatorVisitor::visit(VariableNode& node) {
	Value& v = getRuntimeVal(node.name);
	stack.push_back(v);
}

void EvaluatorVisitor::visit(COCNode& node) {
	if(node.expressions.size() == 1 && node.resolution == COCResolution::Cast) {
		node.expressions[0]->accept(*this);
		Value &v = stack.back();
		v.setType(ValueType::int64);
		switch(node.type) {
			case ValueType::int64:
				break; 
			case ValueType::int32:
				uint32_t n = v.getUInt<uint32_t>();
				v.setType(ValueType::int32);
				v.setUInt(n);
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
	string s = "\n";
	switch(stack.back().getType()) {
		case ValueType::int64:
			s += to_string(stack.back().getUInt<uint64_t>());
			break;
		case ValueType::int32:
			s += to_string(stack.back().getUInt<uint32_t>());
			break;
	}
	std::cout << s << std::endl;
}

//TypeVisitor

void TypeVisitor::visit(ScopeNode &node) {
	currscope = node.scope;
	Visitor::visit(node);
	currscope = currscope->parent;
}

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
	DeclarationNode* dec = getDeclaration(node.name, currscope);
	if(dec) {
		stack.push_back(dec->varType);	
	}
}

void TypeVisitor::visit(AssignmentNode& node) {
	node.expression->accept(*this);
	ValueType vt = stack.back();
	stack.pop_back();
	uint32_t left = typeRank(getDeclaration(node.name, currscope)->varType); 
	uint32_t right = typeRank(vt);
	if(left < right) {
		throw runtime_error("Implicit narrowing requires explicit cast.");	
	}
}

void TypeVisitor::visit(DeclarationNode& node) {
	node.expression->accept(*this);
	ValueType vt = stack.back();
	stack.pop_back();
	uint32_t left = typeRank(getDeclaration(node.name, currscope)->varType); 
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
