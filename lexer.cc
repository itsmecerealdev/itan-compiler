#include "lexer.h"
#include "symbol.h"
#include <iostream>

using namespace std;

void Lexer::generateKeywordMap() {
	typeKeywords["int32"] = ValueType::int32;
	typeKeywords["int64"] = ValueType::int64;
	funcKeywords["print"] = BuiltInFuncNames::print;
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
					result.push_back(Token{TokenType::Plus, ValueType::none, "", 0});
					break;
				case '-':
					result.push_back(Token{TokenType::Minus, ValueType::none, "", 0});
					break;
				case '/':
					result.push_back(Token{TokenType::Slash, ValueType::none, "", 0});
					break;
				case '*':
					result.push_back(Token{TokenType::Star, ValueType::none, "", 0});
					break;
				case '(':
					result.push_back(Token{TokenType::LParen, ValueType::none, "", 0});
					break;
				case ')':
					result.push_back(Token{TokenType::RParen, ValueType::none, "", 0});
					break;
				case ',':
					result.push_back(Token{TokenType::Comma, ValueType::none, "", 0});
					break;	
				case '=':
					result.push_back(Token{TokenType::Assignment, ValueType::none, "", 0});
					break;
				case ';':
					result.push_back(Token{TokenType::StatementEnd, ValueType::none, "", 0});
					break;
				case '^':
					result.push_back(Token{TokenType::Exponent, ValueType::none, "", 0});
					break;
			}
			bufferIndex++;
			continue;
		}
		else if(ischar(buffer.at(bufferIndex))) {//ValueType
			int startIndex = bufferIndex;
			while(bufferIndex < buffer.length() && !isspace(buffer.at(bufferIndex)) && !issymbol(buffer.at(bufferIndex))) {
				bufferIndex++;
			}
			string str = buffer.substr(startIndex, bufferIndex - startIndex);
			if(typeKeywords.contains(str)) {
				result.push_back(Token{TokenType::Type, typeKeywords.at(str), str, 0});	
			}
			else if(funcKeywords.contains(str)) {
				result.push_back(Token{TokenType::FuncIdentifier, ValueType::none, str, 0, BuiltInFuncNames::print});	
			}
			else {
				result.push_back(Token{TokenType::Identifier, ValueType::LITERAL, str, 0});	
			}
		}
		else {
			int startIndex = bufferIndex;
			while(bufferIndex < buffer.length() && isdigit(buffer.at(bufferIndex)) && !issymbol(buffer.at(bufferIndex))) {
				bufferIndex++;
			}
			int i = stoi(buffer.substr(startIndex, bufferIndex - startIndex));
			result.push_back(Token{TokenType::Number, ValueType::LITERAL, "", i});
		}
	}
	result.push_back(Token{TokenType::End, ValueType::none, "", 0});
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
