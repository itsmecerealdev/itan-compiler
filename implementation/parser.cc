#include "../headers/parser.h"
#include "../headers/lexer.h"
#include "../headers/node.h"
#include "../headers/symbol.h"
#include <stdexcept>
#include <string>

Node* Parser::parseCastOrCall() {
	Token identifier = consume(); //Cannot do expect, as this can be either identifier or type. The next line will catch an error.
	expect(TokenType::LParen);
	vector<Node*> expressions;
	if(peek() != TokenType::RParen) expressions.push_back(parseExpression());
	while(peek() == TokenType::Comma) {
		consume();
		expressions.push_back(parseExpression());
	}
	expect(TokenType::RParen);
	return new COCNode(identifier.name, expressions);
}

Node* Parser::parseAssignment() {
	Token t = expect(TokenType::Identifier);
	string n = t.name;
	expect(TokenType::Assignment);
	Node* expression = parseExpression();
	expect(TokenType::StatementEnd);
	return new AssignmentNode(n, expression);
}

Node* Parser::parseFactor() {
	if((peek() == TokenType::Type || peek() == TokenType::Identifier) && peekAhead() == TokenType::LParen) {
		return parseCastOrCall();
	}
	else if(peek() == TokenType::Identifier) {
		Token t = consume();
		return new VariableNode(t.name);
	}
	else if(peek() == TokenType::LParen) {
		consume();
		Node* temp = parseExpression();
		expect(TokenType::RParen);
		return temp;
	}
	else if(peek() == TokenType::Number) {
		return new NumberNode(expect(TokenType::Number).value);
	}
	else return nullptr;
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
	if(peek() == TokenType::Identifier && peekAhead() == TokenType::Assignment) {
		return parseAssignment();
	}
	Node* n = parseExpression();
	expect(TokenType::StatementEnd);
	return n;
}

Node* Parser::parseDeclaration() {
	Token t = expect(TokenType::Type);
	ValueType vt = t.vartype;
	t = expect(TokenType::Identifier);
	string n = t.name;
	t = expect(TokenType::Assignment);
	Node* expression = parseExpression();
	expect(TokenType::StatementEnd);
	return new DeclarationNode(vt, n, expression);
}

Node* Parser::parseFunction() {
	Token t = expect(TokenType::FuncIdentifier);
	expect(TokenType::LParen);
	Node* exp = parseExpression();
	expect(TokenType::RParen);
	expect(TokenType::StatementEnd);
	return new PrintNode(exp);
}

Node* Parser::parseStatement() {
	TokenType t = peek();
	if(t == TokenType::Type && peekAhead() != TokenType::LParen) {
		return parseDeclaration();
	}
	else if(t == TokenType::FuncIdentifier) {
		return parseFunction();
	}
	// else if(peek() == TokenType::RBrace) {
		// return nullptr;
	// }
	else if(t != TokenType::End) {
		return parseExprStatement();
	}
	return nullptr;
}

Node* Parser::parseScope() {
	vector<Node*> statements;
	expect(TokenType::LBrace);
	// parseStatement();
	while(peek() != TokenType::RBrace) {
		if(peek() == TokenType::LBrace) statements.push_back(parseScope());
		else statements.push_back(parseStatement());	
	}
	expect(TokenType::RBrace);
	return new ScopeNode(statements);
}

Node* Parser::parseProgram() {
	vector<Node*> scopes;
	while(peek() != TokenType::End) {
		if(peek() == TokenType::LBrace) {
			scopes.push_back(parseScope());
		} 
		else {
			scopes.push_back(parseStatement());
		}
	}
	std::erase_if(scopes, [](Node* n) { return n == nullptr; }); 
	ScopeNode* global = new ScopeNode(scopes);
	return new ProgramNode(global);
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
	if(temp != t) throw logic_error("Token " + to_string(int(temp)) + " at index " + to_string(index) + " does not match expected qualifier: " + to_string(int(t)));
	return consume();
}
