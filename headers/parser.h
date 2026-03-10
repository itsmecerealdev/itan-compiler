#pragma once
#include <functional>
#include <vector>
#include "node.h"
#include "symbol.h"
#include "lexer.h"

class Parser {
	private:
		int index{};
		vector<Token> tokens;

		Node* parseFactor();
		Node* parseExponent();
		Node* parseTerm();
		Node* parseExpression();
		Node* parseExprStatement(); 

		Node* parseCastOrCall();
		Node* parseAssignment();
		Node* parseDeclaration();
		Node* parseFunction();
		Node* parseReturn();

		Node* parseStatement();
		Node* parseScope();
		ParamNode* parseParam();
		Node* parseFuncDeclaration();

		Token consume();
		TokenType peek();
		TokenType peekAhead();
		Token expect(TokenType t); // qualifier is something like isdigits, issymbol (this is handwritten, and is used by the tokenizer. Only validates paranethesis, +, -, *, /.

	public:
		Parser(int ind, vector<Token> &intokens) : index(ind), tokens(intokens) {}

		Node* parseProgram();
};
