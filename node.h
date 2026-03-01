#pragma once

#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include "lexer.h"

using namespace std;

class Visitor;

class Node {
	public:
		virtual void accept(Visitor &v) = 0;	
		virtual ~Node() = default;
};

class ProgramNode : public Node {
	public:
		vector<Node*> statements;
		void accept(Visitor &v) override;
		ProgramNode(vector<Node*> &nodes) : statements(nodes) {}
		~ProgramNode() override { for (auto &c : statements) delete c; }
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
		VariableType varType;
		string name;
		Node* expression;

		void accept(Visitor &v) override;
		DeclarationNode(VariableType intype, string inname, Node* inexpression) : varType(intype), name(inname), expression(inexpression) {
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

		// int eval() override {
		// 	int left = lhs->eval();
		// 	int right = rhs->eval();
		// 	switch(type) {
		// 		case TokenType::Plus:
		// 			return left + right;
		// 		case TokenType::Minus:
		// 			return left - right;
		// 		case TokenType::Star: 
		// 			return left * right;
		// 		case TokenType::Slash:
		// 			if(right == 0) throw runtime_error("Division by zero.");
		// 			return left / right;
		//
		// 	}
		// 	throw logic_error("Invalid operand\n");
		// }
