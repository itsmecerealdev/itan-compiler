#include <cstdlib>
#include <stdexcept>
#include <vector>
#include "/public/read.h" // IWYU pragma: keep
#include "node.h"
#include "parser.h"
#include "lexer.h"
#include "visitor.h"

using namespace std;

using ContextMap = std::unordered_map<string, DeclarationNode*>;

int main() {
	vector<Token> tokens;
	string buffer;
	ContextMap Context;
	getline(cin, buffer);
	Lexer l{buffer};
	tokens = l.tokenizeBuffer();

	// for(const auto &c : tokens) {
		// cout << int(c.tokentype) << " ";
	// }
	Parser p{0, tokens};	
	Node* AST = p.parseProgram();


	PrintVisitor pv;
	DeclarationVisitor dv(Context);
	SemanticsVisitor sv(Context);
	EvaluatorVisitor ev;
	AST->accept(pv);
	AST->accept(dv);
	AST->accept(sv);
	AST->accept(ev);
	delete(AST);
}
