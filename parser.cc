#include "parser.h"
#include "lexer.h"
#include "node.h"
#include "symbol.h"
#include <stdexcept>
#include <string>


Node* Parser::parseFactor() {
	if(peek() == TokenType::Identifier) {
		Token t = consume();
		return new VariableNode(t.name);
	}
	else if(peek() == TokenType::LParen) {
		consume();
		Node* temp = parseExpression();
		expect(TokenType::RParen);
		return temp;
	}
	return new NumberNode(expect(TokenType::Number).value);
}

Node* Parser::parseExponent() {
	Node* left = parseFactor();
	if(peek() == TokenType::Exponent) {
		Token oper = consume();
		Node* right = parseExponent();
		return new OperandNode(left, right, oper.tokentype);
	}
	return left;
}

Node* Parser::parseTerm() {
	Node *left = parseExponent();
	Token oper; 

	while(true) {
		TokenType next = peek();
		if(next == TokenType::Slash || next == TokenType::Star) {
			oper = consume();
			Node *right = parseExponent();
			TokenType t = oper.tokentype;
			OperandNode *temp = new OperandNode(left, right, t);
			left = temp;
		}
		else {
			return left;
		}
	}
}

Node* Parser::parseExpression(){
	Node *left = parseTerm();
	Token oper{}; 
	TokenType next = peek();
	while(next == TokenType::Plus || next == TokenType::Minus) {
		oper = consume();
		Node* right = parseTerm();
		TokenType t = oper.tokentype;
		OperandNode *temp = new OperandNode(left, right, t);
		next = peek();
		left = temp;
	}
	return left;
}

Node* Parser::parseExprStatement() {
	Node* n = parseExpression();
	expect(TokenType::StatementEnd);
	return n;
}

Node* Parser::parseAssignment() {
	Token t = expect(TokenType::Identifier);
	string n = t.name;
	expect(TokenType::Assignment);
	Node* expression = parseExpression();
	expect(TokenType::StatementEnd);
	return new AssignmentNode(n, expression);
}

Node* Parser::parseDeclaration() {
	Token t = expect(TokenType::Type);
	VariableType vt = t.vartype;
	t = expect(TokenType::Identifier);
	string n = t.name;
	t = expect(TokenType::Assignment);
	Node* expression = parseExpression();
	expect(TokenType::StatementEnd);
	return new DeclarationNode(vt, n, expression);
}

Node* Parser::parseStatement() {
	TokenType t = peek();
	if(t == TokenType::Type) {
		return parseDeclaration();
	}
	else if(t == TokenType::Identifier && peekAhead() == TokenType::Assignment) {
		return parseAssignment();
	}
	else if (t != TokenType::End) {
		return parseExprStatement();
	}
	return nullptr;
}

Node* Parser::parseProgram() {
	vector<Node*> statements;
	while(peek() != TokenType::End) {
		 statements.push_back(parseStatement());
	}
	return new ProgramNode(statements);
}

TokenType Parser::peekAhead() {
	if(index < tokens.size() - 1) {
		return tokens.at(index + 1).tokentype;
	}
	else return TokenType::End;
}

TokenType Parser::peek() { 
	return tokens.at(index).tokentype; 
}

Token Parser::consume() { 
	Token t = tokens.at(index);
	index++;
	return t;
}

Token Parser::expect(TokenType t) {
	TokenType temp = peek();
	if(temp != t) throw logic_error("Token " + to_string(int(temp)) + " does not match expected qualifier: " + to_string(int(t)));
	return consume();
}
