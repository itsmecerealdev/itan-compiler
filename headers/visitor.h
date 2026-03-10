#pragma once

#include "lexer.h"
#include "node.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>

using Context = std::vector<std::unordered_map<string, DeclarationNode*>>;
using Runtime = std::vector<std::unordered_map<string, Value>>;

class Visitor {
	public:
		virtual void visit(ProgramNode &node);
		virtual void visit(FuncDeclNode &node);
		virtual void visit(ParamNode &node);
		virtual void visit(ReturnNode &node);
		virtual void visit(ScopeNode &node);
		virtual void visit(NumberNode& node);
		virtual void visit(OperandNode& node);
		virtual void visit(AssignmentNode& node);
		virtual void visit(DeclarationNode& node);
		virtual void visit(VariableNode& node);
		virtual void visit(COCNode& node);
		virtual void visit(PrintNode& node);

		virtual ~Visitor() = default;
};

class PrintVisitor : public Visitor {
	private:
		int depth = 0;
		void tabHelper();
	public:
		void visit(ProgramNode &node) override;
		void visit(FuncDeclNode &node) override; 
		void visit(ReturnNode &node) override;
		void visit(ParamNode &node) override;
		void visit(ScopeNode &node) override;
		void visit(NumberNode& node) override;
		void visit(OperandNode& node) override;
		void visit(AssignmentNode& node) override;
		void visit(DeclarationNode& node) override;
		void visit(VariableNode& node) override;
		void visit(COCNode& node) override;
		void visit(PrintNode& node) override;
};

class DeclarationVisitor : public Visitor {
	private:
		vector<Scope*> scopes;
	public:
		void visit(FuncDeclNode &node) override;
		void visit(ParamNode &node) override;
		void visit(ScopeNode& node) override;
		void visit(DeclarationNode& node) override;
		DeclarationVisitor() {}
};

class SemanticsVisitor : public Visitor {
	private:
		bool inFunc = false;
		bool returned = false;
		Scope* currscope;
		unordered_map<string, ValueType> typeKeywords;
		std::string currentInit;
		void populateKeywords() {
			typeKeywords["int32"] = ValueType::int32;
			typeKeywords["int64"] = ValueType::int64;
		}
	public:
		void visit(FuncDeclNode &node) override;
		void visit(ParamNode &node) override;
		void visit(ReturnNode &node) override;
		void visit(ScopeNode& node) override;
		void visit(VariableNode& node) override;
		void visit(AssignmentNode& node) override; 
		void visit(DeclarationNode& node) override; 
		void visit(COCNode& node) override;
		SemanticsVisitor() { populateKeywords(); }
};

class EvaluatorVisitor : public Visitor {
	private:
		Scope* currscope;
		Runtime runtime;
		bool returned = false;
		Value retval;
		vector<Value> stack;
		Value getRuntimeValUI64(const std::string &name); 
		Value &getRuntimeVal(const std::string &name); 
	public:
		void visit(ProgramNode& node) override;
		void visit(FuncDeclNode &node) override;
		void visit(ParamNode &node) override;
		void visit(ReturnNode &node) override;
		void visit(ScopeNode& node) override;
		void visit(NumberNode& node) override;
		void visit(OperandNode& node) override;
		void visit(AssignmentNode& node) override;
		void visit(DeclarationNode& node) override;
		void visit(VariableNode& node) override;
		void visit(COCNode& node) override;
		void visit(PrintNode& node) override;
};

class TypeVisitor : public Visitor {
	private:
		ValueType funcType = ValueType::none;
		Scope* currscope;
		vector<ValueType> stack;
	public:
		void visit(FuncDeclNode &node) override;
		void visit(ParamNode &node) override;
		void visit(ScopeNode &node) override;
		void visit(ReturnNode &node) override;
		void visit(COCNode& node) override;
		void visit(ProgramNode& node) override;
		void visit(NumberNode& node) override;
		void visit(OperandNode& node) override;
		void visit(AssignmentNode& node) override;
		void visit(DeclarationNode& node) override;
		void visit(VariableNode& node) override;
		TypeVisitor() {}
};
