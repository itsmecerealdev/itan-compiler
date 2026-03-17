#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

enum class TokenType : unsigned char {
	Number,
	Plus,
	Minus,
	Star,
	Slash,
	Exponent,
	LParen,
	RParen,
	Comma,
	Assignment,
    Not,
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    LessEqual,
    GreaterEqual,
	Type,
	StatementEnd,
	Identifier,
	FuncIdentifier,
	Return,
	LBrace,
	RBrace,
	End
};

enum class  ValueType : unsigned char{
	none,
	LITERAL,
	int32,
	int64
};
enum class BuiltInFuncNames : unsigned char {
	none,
	print,
    conditionalIf
};

class Token {
	public:
		TokenType tokentype{};
		ValueType vartype{};
		string name;
		int64_t value{};
		BuiltInFuncNames func = BuiltInFuncNames::none;
};

class Lexer {
	private:
		string buffer;
		unordered_map<string, ValueType> typeKeywords;
		unordered_map<string, BuiltInFuncNames> funcKeywords; 

		void generateKeywordMap();

	public:
		Lexer(const string &inbuf) : buffer(inbuf) { if(buffer.length() == 0) throw logic_error("0 length buffer."); }
		
        vector<Token> tokens;

        void tokenizeBuffer();
		void printTokens();
};

string printToken(const Token &t); //stupid c++ fix, otherwise parser cannot use this.
