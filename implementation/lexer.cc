#include "../headers/lexer.h"
#include "../headers/symbol.h"
#include <iostream>

using namespace std;

string printToken(const Token &t) {
    string ret;
    switch(t.tokentype) {
        case TokenType::Number:
            ret += "Number: " + to_string(t.value) + " ";
            break;
        case TokenType::Plus:
            ret += "+";
            break;
        case TokenType::Minus:
            ret += "-";
            break;
        case TokenType::Star:
            ret += "=";
            break;
        case TokenType::Slash:
            ret += "/";
            break;
        case TokenType::Exponent:
            ret += "^";
            break;
        case TokenType::LParen:
            ret += "(";
            break;
        case TokenType::RParen:
            ret += ")";
            break;
        case TokenType::Comma:
            ret += ",";
            break;
        case TokenType::Assignment:
            ret += "=";
            break;
        case TokenType::Not:
            ret += "!";
            break;
        case TokenType::Equal:
            ret += "==";
            break;
        case TokenType::NotEqual:
            ret += "!=";
            break;
        case TokenType::LessThan:
            ret += "<";
            break;
        case TokenType::GreaterThan:
            ret += ">";
            break;
        case TokenType::LessEqual:
            ret += "<=";
            break;
        case TokenType::GreaterEqual:
            ret += ">=";
            break;
        case TokenType::Type:
            ret += "Type: " + to_string(int(t.vartype)) + " ";
            break;
        case TokenType::StatementEnd:
            ret += ";";
            break;
        case TokenType::Identifier:
            ret += "Identifier: " + t.name + " ";
            break;
        case TokenType::FuncIdentifier:
            ret += "FuncIdentifier";
            break;
        case TokenType::Return:
            ret += "=>";
            break;
        case TokenType::LBrace:
            ret += "{";
            break;
        case TokenType::RBrace:
            ret += "}";
            break;
        case TokenType::End:
            ret += "EOF";
            break;
    }
    return ret;
}

void Lexer::printTokens() {
    string s;
    for(const auto &t : tokens) {
        s += printToken(t);
        if(t.tokentype != TokenType::End) s += ", ";
    }
    std::cout << s << std::endl;
}

void Lexer::generateKeywordMap() {
	typeKeywords["int32"] = ValueType::int32;
	typeKeywords["int64"] = ValueType::int64;
	funcKeywords["print"] = BuiltInFuncNames::print;
    funcKeywords["if"] = BuiltInFuncNames::conditionalIf;
}

void Lexer::tokenizeBuffer() {
	generateKeywordMap();
	int bufferIndex = 0;
	while(buffer.length() > bufferIndex) {
		if(isspace(buffer.at(bufferIndex))) {
			bufferIndex++;
			continue;
		}
		if(!isdigit(buffer.at(bufferIndex)) && !issymbol(buffer.at(bufferIndex)) && !ischar(buffer.at(bufferIndex)) && buffer.at(bufferIndex) != '\\' ) {
			throw logic_error("Invalid symbol: " + string(1, buffer.at(bufferIndex)) + "\na-z, A-Z, 0-9, +, -, *, /, (, ), or ; expected.\n");
			exit(EXIT_FAILURE);
		}
		else if(issymbol(buffer.at(bufferIndex))) {
			switch(buffer[bufferIndex]) {
				case '+':
					tokens.push_back(Token{TokenType::Plus, ValueType::none, "", 0});
					break;
				case '-':
					tokens.push_back(Token{TokenType::Minus, ValueType::none, "", 0});
					break;
				case '/':
					tokens.push_back(Token{TokenType::Slash, ValueType::none, "", 0});
					break;
				case '*':
					tokens.push_back(Token{TokenType::Star, ValueType::none, "", 0});
					break;
				case '(':
					tokens.push_back(Token{TokenType::LParen, ValueType::none, "", 0});
					break;
				case ')':
					tokens.push_back(Token{TokenType::RParen, ValueType::none, "", 0});
					break;
				case ',':
					tokens.push_back(Token{TokenType::Comma, ValueType::none, "", 0});
					break;	
				case '=':
					tokens.push_back(Token{TokenType::Assignment, ValueType::none, "", 0});
					break;
				case ';':
					tokens.push_back(Token{TokenType::StatementEnd, ValueType::none, "", 0});
					break;
				case '^':
					tokens.push_back(Token{TokenType::Exponent, ValueType::none, "", 0});
					break;
				case '{':
					tokens.push_back(Token{TokenType::LBrace, ValueType::none, "", 0});
					break;
				case '}':
					tokens.push_back(Token{TokenType::RBrace, ValueType::none, "", 0});
					break;
				case '>':
					// if(result.back().tokentype == TokenType::Assignment) {
						// result.back().tokentype = TokenType::Return;
						// bufferIndex++;
					// }
                    tokens.push_back(Token{TokenType::GreaterThan, ValueType::none, "", 0});
					break;
                case '<':
                    tokens.push_back(Token{TokenType::LessThan, ValueType::none, "", 0});
					break;
                case '!':
                    tokens.push_back(Token{TokenType::Not, ValueType::none, "", 0});
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
				tokens.push_back(Token{TokenType::Type, typeKeywords.at(str), str, 0});	
			}
			else if(funcKeywords.contains(str)) {
				tokens.push_back(Token{TokenType::FuncIdentifier, ValueType::none, str, 0, funcKeywords.at(str)});	
			}
			else {
				tokens.push_back(Token{TokenType::Identifier, ValueType::LITERAL, str, 0});	
			}
		}
		else {
			int startIndex = bufferIndex;
			while(bufferIndex < buffer.length() && isdigit(buffer.at(bufferIndex)) && !issymbol(buffer.at(bufferIndex))) {
				bufferIndex++;
			}
			int i = stoi(buffer.substr(startIndex, bufferIndex - startIndex));
			tokens.push_back(Token{TokenType::Number, ValueType::LITERAL, "", i});
		}
	}
	tokens.push_back(Token{TokenType::End, ValueType::none, "", 0});
}
