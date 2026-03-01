#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "/public/read.h" // IWYU pragma: keep
#include "node.h"
#include "parser.h"
#include "lexer.h"
#include "visitor.h"

using namespace std;

using ContextMap = std::unordered_map<string, DeclarationNode*>;

string fileTarget(const string &filename) {
	ifstream ifs(filename + ".it");
	if(!ifs) {
		throw runtime_error("File " + filename + " is not found");
	}	
	stringstream ss; 
	ss << ifs.rdbuf();	

	return ss.str();
}

int main(int argc, char** argv) {
	vector<Token> tokens;
	string buffer;
	ContextMap Context;

	if(argc > 1) {
		buffer = fileTarget(string(argv[1]));
	} else {
		throw runtime_error("Provide a filename you'd like to \"compile\". Do not include the file extension.");
	}

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
