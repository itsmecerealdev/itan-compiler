#pragma once

#include "lexer.h"
#include "node.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>

using ContextMap = std::unordered_map<string, DeclarationNode*>;

class Visitor {
	public:
		virtual void visit(ProgramNode &node);
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
		//Stores the context of variables. Declarations and assignments.
		//This is used to do a semantic pass through the file and ensure variables are declared.
		//This only stores declarations, and is referenced during the pass whenever a variable node occurs.
		ContextMap &Context; 
	public:
		//Declaration visitor ONLY cares about Declaration nodes. It doesn't need to traverse them, it just stores a ptr to it in
		//the ContextMap.
		void visit(DeclarationNode& node) override;
		DeclarationVisitor(ContextMap &context) : Context(context) {}
};

class SemanticsVisitor : public Visitor {
	private:
		const ContextMap &Context;
		unordered_map<string, ValueType> typeKeywords;
		std::string currentInit;
		void populateKeywords() {
			typeKeywords["int32"] = ValueType::int32;
			typeKeywords["int64"] = ValueType::int64;
		}
	public:
		void visit(VariableNode& node) override;
		void visit(AssignmentNode& node) override; 
		void visit(DeclarationNode& node) override; 
		void visit(COCNode& node) override;
		SemanticsVisitor(ContextMap &context) : Context(context) { populateKeywords(); }
};

class EvaluatorVisitor : public Visitor {
	private:
		unordered_map<string, uint64_t> runtime;
		vector<int64_t> stack;
	public:
		void visit(ProgramNode& node) override;
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
		const ContextMap &Context;
		vector<ValueType> stack;
	public:
		void visit(COCNode& node) override;
		void visit(ProgramNode& node) override;
		void visit(NumberNode& node) override;
		void visit(OperandNode& node) override;
		void visit(AssignmentNode& node) override;
		void visit(DeclarationNode& node) override;
		void visit(VariableNode& node) override;
		TypeVisitor(ContextMap &context) : Context(context) {}
};
