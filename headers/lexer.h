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
	Type,
	StatementEnd,
	Identifier,
	FuncIdentifier,
	RBrace,
	LBrace,
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
	print
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
		void eatWhiteSpace();

		void generateKeywordMap();

	public:
		Lexer(const string &inbuf) : buffer(inbuf) { if(buffer.length() == 0) throw logic_error("0 length buffer."); }
		

		vector<Token> tokenizeBuffer();
		void printTokens(const vector<Token> &tokens);
};

