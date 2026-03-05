#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include "lexer.h"

class DeclarationNode;

using namespace std;
using ContextMap = std::unordered_map<string, DeclarationNode*>;

enum class COCResolution : uint8_t {
	Unresolved,
	Call,
	Cast
};

class Visitor;

class Scope {
	public:
		Scope* parent;
		ContextMap context;	
};

class Node {
	public:
		virtual void accept(Visitor &v) = 0;	
		virtual ~Node() = default;
};

class ScopeNode: public Node {
	public:
		vector<Node*> statements;
		Scope* scope;
		void accept(Visitor &v) override;
		ScopeNode(vector<Node*> &nodes) : statements(nodes), scope(new Scope()) {}
		~ScopeNode() override { for (auto &c : statements) delete c; delete scope; }
};

class ProgramNode : public Node {
	public:
		ScopeNode* global;
		void accept(Visitor &v) override;
		ProgramNode(ScopeNode* scope) : global(scope) {}
		~ProgramNode() override { delete global; }
};

class NumberNode : public Node {
	public:
		int val;
		void accept(Visitor &v) override;
		NumberNode(int v) :val(v) {}
		~NumberNode() override = default;
};

class OperandNode : public Node	{
	public:
		Node *lhs, *rhs;
		TokenType type;
		void accept(Visitor &v) override;
		OperandNode(Node *l, Node *r, TokenType t) : lhs(l), rhs(r), type(t) {}
		~OperandNode() override { delete(lhs); delete(rhs); }
};

class VariableNode : public Node {
	public: 
		string name;

		void accept(Visitor &v) override;
		VariableNode(const string &inname) : name(inname) {}
		~VariableNode() override = default;
};

class DeclarationNode : public Node {
	public:
		ValueType varType;
		string name;
		Node* expression;

		void accept(Visitor &v) override;
		DeclarationNode(ValueType intype, string inname, Node* inexpression) : varType(intype), name(inname), expression(inexpression) {
			if(expression == nullptr) throw runtime_error("No expression provided in " + name + "variable."); 
		}
		~DeclarationNode() override { delete expression; }
};

class AssignmentNode : public Node {
	public:
		string name;
		Node* expression;

		void accept(Visitor &v) override;
		AssignmentNode(const string &inname, Node* inexpression) : name(inname), expression(inexpression) {
			if(expression == nullptr) throw runtime_error("No expression provided in " + name + "variable."); 
		}
		~AssignmentNode() override { delete expression; }
};

class COCNode : public Node {
	public:
		string name;
		COCResolution resolution = COCResolution::Unresolved;
		vector<Node*> expressions;
		ValueType type;

		void accept(Visitor &v) override;
		//Checking for a lack of expression here is premature. Function calls can exist with no parameter.
		COCNode(const string &str, vector<Node*> &inexpressions) : name(str), expressions(inexpressions) {}
		~COCNode() override { for (auto e : expressions) delete e; }
};

class PrintNode : public Node {
	public:
	Node* expression;

	void accept(Visitor &v) override;
	PrintNode(Node* inexpression) : expression(inexpression) { 
		if(expression == nullptr) throw runtime_error ("Print() requires exactly one expression."); 
	}
	~PrintNode() override { delete expression; }
};
