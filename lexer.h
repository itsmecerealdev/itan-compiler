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
	Assignment,
	Type,
	StatementEnd,
	Identifier,
	End
};

enum class  VariableType : unsigned char{
	LITERAL,
	int32,
	int64
};

class Token {
	public:
		TokenType tokentype{};
		VariableType vartype{};
		string name;
		int64_t value{};
};

class Lexer {
	private:
		string buffer;
		unordered_map<string, VariableType> typeKeywords;
		void eatWhiteSpace();

		void generateKeywordMap();

	public:
		Lexer(const string &inbuf) : buffer(inbuf) { if(buffer.length() == 0) throw logic_error("0 length buffer."); }
		

		vector<Token> tokenizeBuffer();
		void printTokens(const vector<Token> &tokens);
};

