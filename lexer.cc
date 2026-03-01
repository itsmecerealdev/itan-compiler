#include "lexer.h"
#include "symbol.h"
#include <iostream>

using namespace std;

void Lexer::generateKeywordMap() {
	typeKeywords["int32"] = VariableType::int32;
	typeKeywords["int64"] = VariableType::int64;
}

vector<Token> Lexer::tokenizeBuffer() {
	vector<Token> result;
	generateKeywordMap();
	int bufferIndex = 0;
	while(buffer.length() > bufferIndex) {
		if(isspace(buffer.at(bufferIndex))) {
			bufferIndex++;
			continue;
		}
		if(!isdigit(buffer.at(bufferIndex)) && !issymbol(buffer.at(bufferIndex)) && !ischar(buffer.at(bufferIndex))) {
			throw logic_error("Invalid symbol: " + string(1, buffer.at(bufferIndex)) + "\na-z, A-Z, 0-9, +, -, *, /, (, ), or ; expected.\n");
			exit(EXIT_FAILURE);
		}
		else if(issymbol(buffer.at(bufferIndex))) {
			switch(buffer[bufferIndex]) {
				case '+':
					result.push_back(Token{TokenType::Plus, VariableType::LITERAL, "", 0});
					break;
				case '-':
					result.push_back(Token{TokenType::Minus, VariableType::LITERAL, "", 0});
					break;
				case '/':
					result.push_back(Token{TokenType::Slash, VariableType::LITERAL, "", 0});
					break;
				case '*':
					result.push_back(Token{TokenType::Star, VariableType::LITERAL, "", 0});
					break;
				case '(':
					result.push_back(Token{TokenType::LParen, VariableType::LITERAL, "", 0});
					break;
				case ')':
					result.push_back(Token{TokenType::RParen, VariableType::LITERAL, "", 0});
					break;
				case '=':
					result.push_back(Token{TokenType::Assignment, VariableType::LITERAL, "", 0});
					break;
				case ';':
					result.push_back(Token{TokenType::StatementEnd, VariableType::LITERAL, "", 0});
					break;
				case '^':
					result.push_back(Token{TokenType::Exponent, VariableType::LITERAL, "", 0});
					break;
			}
			bufferIndex++;
			continue;
		}
		else if(ischar(buffer.at(bufferIndex))) {//variables
			int startIndex = bufferIndex;
			while(bufferIndex < buffer.length() && !isspace(buffer.at(bufferIndex)) && !issymbol(buffer.at(bufferIndex))) {
				bufferIndex++;
			}
			string str = buffer.substr(startIndex, bufferIndex - startIndex);
			if(typeKeywords.contains(str)) {
				result.push_back(Token{TokenType::Type, typeKeywords.at(str), str, 0});	
			}
			else {
				result.push_back(Token{TokenType::Identifier, VariableType::LITERAL, str, 0});	
			}
		}
		else {
			int startIndex = bufferIndex;
			while(bufferIndex < buffer.length() && isdigit(buffer.at(bufferIndex)) && !issymbol(buffer.at(bufferIndex))) {
				bufferIndex++;
			}
			int i = stoi(buffer.substr(startIndex, bufferIndex - startIndex));
			result.push_back(Token{TokenType::Number, VariableType::LITERAL, "", i});
		}
	}
	result.push_back(Token{TokenType::End, VariableType::LITERAL, "", 0});
	return result;
}

void Lexer::printTokens(const vector<Token> &tokens) {
	for (const Token &t : tokens) {
		switch(t.tokentype) {
			case TokenType::Number:
				cout << "Number:";
				break;
			default:
				cout << "Symbol";
				break;
		}
		cout << " " << (t.tokentype == TokenType::Number ? t.value : 0) << ", ";
	}
}
