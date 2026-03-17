#include "../headers/symbol.h"

bool issymbol(const char a) {
	switch(a) {
		case '(':
		case ')':
		case '+':
		case '-':
		case '*':
		case '/':
		case '=':
		case ';':
		case '^':
		case ',':
		case '{':
		case '}':
		case '>':
        case '<':
        case '!':
			return true;
	}
	return false;
}


bool issymbol(const string &a) {
	if(a.size() != 1) return false;
	switch(a[0]) {
		case '(':
		case ')':
		case '+':
		case '-':
		case '*':
		case '/':
		case '=':
		case ';':
		case '^':
		case ',':
		case '{':
		case '}':
		case '>':
        case '<':
        case '!':
			return true;
	}
	return false;
}

bool isdigits(const string &s) {
	for(const char c : s) { if(!isdigit(c)) return false; }
	return true;
}

bool ischar(const char a) {
	return (a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z');
}
