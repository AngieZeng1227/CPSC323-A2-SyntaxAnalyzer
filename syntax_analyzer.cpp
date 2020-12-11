// zhi zeng
// compilers cpsc323
// lexical analyzer

//#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;
class record {
private:
	string token, lexeme;
public:
	string getToken() { return token; }
	string getLexeme() { return lexeme; }

	void setLexeme(string lex) { lexeme = lex; }
	void setToken(string tok) { token = tok; }
};

int realTable[4][3] = {
	//d  .  other
	/* null*/		{ 0, 0, 0 },
	/* 1 start*/	{ 2, 0, 0 },		//f = 3
	/* 2 */			{ 2, 3, 0 },
	/* 3 */			{ 3, 0, 0 }
};

int intTable[3][3] = {
	//d  .	other
	/* 0 */			{ 0, 0, 0 },
	/* 1 start */	{ 2, 0, 0 },		//f = 2
	/* 2 */			{ 2, 0, 0 }
};

int idTable[4][4] = {
	//l  d	_  other
	/* 0 */			{ 0, 0, 0, 0},
	/* 1 start */	{ 2, 0, 0, 0 },
	/* 2 */			{ 3, 3, 3, 0 },		// f = 23
	/* 3 */			{ 3, 3, 3, 0 }
};

// list of keywords, separators, and operators
const int opSize = 14;
const int keywordSize = 20;
string separators = "(){}[],:;";
string operators[opSize] = { "=", "<", ">", "<=", "==", "!=", ">=", "+", "-", "*", "/", "|", "++", "--" };
string keywords[keywordSize] = { "if", "function", "int", "else", "fi", "while", "return", "get", "put", "bool", "boolean", "real", "true", "false" };
string comment[2] = { "/*", "*/" };
int state;
vector<string> lexeme;
vector<string> input;
ifstream fin;
ofstream output;
vector<record> l;



bool printRules = true;
record currentRecord;


// function declarationssssss
record lexer();
record returnState(const string cbuffer, char ch);
bool  isInt(const string str);
bool  isReal(const string str);
bool  isId(const string str);
int ichar_to_col(char ch);
int char_to_col(char ch);
bool isSeparator(char ch);
bool isOperator(const string str);
bool isKeyword(const string str);

void pError(string error);
void printCurrent();
record callLexer();

// production declarations

void rat20f();
void optFunctionDeclarations();
void functionDefinitions();
void function();
void identifier();
void empty();
void functionDefinitionsp();
void parameterList();
void optParameterList();
void optDeclarationList();
void body();
void ids();
void idsp();
void parameter();
void parameterListp();
void qualifiers();
void statementList();
void statementListp();
void statement();
void declarationList();
void declaration();
void declarationListp();

void ifFunc();
void compound();
void assign();
void returnFunc();
void scan();
void print();
void expression();
void expressionp();
void condition();

void whileFunc();
void relop();
void term();
void termp();
void factor();
void primary();
void isreal();
void isinteger();
void isbool();

int main() {
	// change file to input file
	ifstream input;
	string fileName;
	cout << "Enter the file name: ";
	cin >> fileName;
	input.open(fileName.c_str());

	while (input.fail())
	{
		input.clear();
		cout << "Incorrect filename, please enter again: ";
		cin >> fileName;
		input.open(fileName.c_str());

	}
	string eofcheck = " ";			//eofcheck to stop lexer function after end of file.

	// open file
	fin.open(fileName);
	output.open("out.txt");
	rat20f();

	output << "Rat20F has executed successfully.\n";
	output.flush();

	//if (fin.is_open()) {
	//	while (eofcheck != "stop") {

	//		
	//		record latestRecord = lexer();
	//		eofcheck = latestRecord.getToken();
	//		if (!(latestRecord.getToken() == "stop" || latestRecord.getToken() == "space")) {
	//			cout << latestRecord.getLexeme() << "  " << latestRecord.getToken() << endl;
	//			l.push_back(latestRecord);
	//		}
	//	}
	//}
	//else perror("file");

	fin.close();
	system("Pause");
	return 0;
}

record lexer() {
	char ch;
	string cbuffer, state;
	int comment = 0;
	record h;
	do
	{
		if ((ch = fin.get()) != EOF)
		{
			// check for comments
			if (ch == '/') 
			{
				if (ch = fin.get() == '*') {
					comment = 1; // if /* is inputed, this is in comment state

					while (comment == 1) {
						if (ch == '*' && (ch = fin.get() == '/')) {  break;}	// close comment state if */ is inputted
						ch = fin.get();

					}
				} else fin.unget();

				if (comment == 1) {
					h.setToken("space");
					return h;
				}
				h.setLexeme("/");
				h.setToken("operator");
				return h;
			}
			
				

			
			if (!isspace(ch))
			{
				if (isSeparator(ch))
				{
					// if there are input before separater unget and break;

					if (cbuffer.size() >= 1) { fin.unget(); break; }

					else {
						
						string convertC(1, ch);
						h.setLexeme(convertC);
						h.setToken("separator");
						return h;
					}
				}
				cbuffer.push_back(ch);
			}
			else {
				h.setToken("space");
				break;
			}
		}
		else {
			h.setToken("stop");
			return h;
		}// return stop to lexer function
	} while (!isSeparator(ch) || !isspace(ch));

	if (comment == 1) {
		state = "comment";
	}
	return returnState(cbuffer, ch);
}

record returnState(const string cbuffer, char ch) {
	record h;
	if (isOperator(cbuffer))
	{
		h.setLexeme(cbuffer);
		h.setToken("operator");
		return h;
	}
	if (isId(cbuffer))
	{
		if (isKeyword(cbuffer)) {

			h.setLexeme(cbuffer);
			h.setToken("keyword");
			return h;
		}

		h.setLexeme(cbuffer);
		h.setToken("id");
		return h;
	}
	else if (isReal(cbuffer))
	{

		h.setLexeme(cbuffer);
		h.setToken("real");
		return h;
	}
	else if (isInt(cbuffer))
	{
		
		h.setLexeme(cbuffer);
		h.setToken("int");
		return h;
	}
	if (cbuffer == "$$")
	{
		
		h.setLexeme(cbuffer);
		h.setToken("separator");
		return h;
	}
	
	else if (isspace(ch)) {
		h.setToken("space");
		return h;
	}
	else (perror("is not a token"));
}

record callLexer() {
	//skip space
	record temp;
	do {
		temp = lexer();
		
	} while (temp.getToken() == "space");

	return temp;
}

// int dfsm
bool isInt(const string str)
{
	state = 1;
	int col;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] != NULL)
		{
			col = ichar_to_col(str[i]);
			state = intTable[state][col];
		}
	}
	if (state == 2) { return 1; }
	else { return 0; }
}

// real dfsm
bool isReal(const string str)
{
	state = 1;
	int col;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] != NULL)
		{
			col = ichar_to_col(str[i]);
			state = realTable[state][col];
		}
	}
	if (state == 3) { return 1; }
	else { return 0; }
}

// id dfsm
bool isId(const string str)
{
	state = 1;
	int col;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] != NULL)
		{
			col = char_to_col(str[i]);
			state = idTable[state][col];
		}
	}
	if (state == 3 || state == 2) { return 1; }
	else { return 0; }
}

// state transition for int and real
int ichar_to_col(char ch)
{
	if (isdigit(ch)) { return 0; }
	else if (ch == '.') { return 1; }
	else { return 2; }
}

// state transition for ID
int char_to_col(char ch)
{
	if (isalpha(ch)) { return 0; }
	else if (isdigit(ch)) { return 1; }
	else if (ch == '_') { return 2; }
	else { return 3; }
}

// checks if character is a separator
bool isSeparator(char ch)
{
	for (int i = 0; i < separators.size(); i++)
	{
		if (ch == separators[i]) { return true; }
	}
	return false;
}

bool isOperator(const string str)
{
	for (int i = 0; i < opSize; i++)
	{
		if (str == operators[i]) { return true; }
	}
	return false;
}

bool isKeyword(const string str)
{
	for (int i = 0; i < keywordSize; i++)
	{
		if (str == keywords[i]) { return true; }
	}
	return false;
}

//productions

//r1
void rat20f()
{

	if (printRules) { output << "<Rat20F> ::= <Opt Function Definitions> $$ <Opt Declaration List> <Statement List> $$\n"; 
	output.flush();
	}
	optFunctionDeclarations();

	if (currentRecord.getLexeme() != "$$")
	{

		pError("$$ expected (rat20f)");
	}
	else printCurrent();

	optDeclarationList();
	statementList();

	if (currentRecord.getLexeme() != "$$")
	{
		pError("$$ expected (rat20f)");
	}
	else printCurrent();
}

//r2
void optFunctionDeclarations()
{
	if (printRules) {
		output << "<Opt Function Definitions> ::= <Function Definitions> | <Empty>	\n";
		output.flush();
	}
	currentRecord = callLexer();
	if (currentRecord.getLexeme() != "function")
	{
		empty();
		return;
	}
	else {
		printCurrent();
		functionDefinitions();
	}
}

//
void empty() {

	if (printRules) {
		output << "<Empty> ::= e	\n";
		output.flush();
	}

}

//r3
void functionDefinitions()
{
	if (printRules) {
		output << "<Function Definitions> ::= <Function> <Function Definitions prime>	\n";
		output.flush();
	}

	function();
	functionDefinitionsp();
}

void functionDefinitionsp() {

	if (printRules) {
		output << "<Function Definitions prime> ::= e | <Function Definitions>	\n";
		output.flush();
	}

	currentRecord = callLexer();

	if (currentRecord.getLexeme() == "function")
	{
		printCurrent();
		functionDefinitions();

	}
	else {
		empty();
		return;
	}
}

//r4
void function() {
	if (printRules) {
		output << "<Function> ::= function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List><Body>	\n";
		output.flush();
	}

	currentRecord = callLexer();
	identifier();

	currentRecord = callLexer();
	if (currentRecord.getLexeme() != "(") {
		pError("( expected (function)");
		return;
	}
	else printCurrent();

	currentRecord = callLexer();
	optParameterList();

	if (currentRecord.getLexeme() != ")")
	{
		pError(") expected (function)");
		return;
	}
	else printCurrent();
	optDeclarationList();
	body();

}

//r5
void optParameterList() {
	if (printRules) {
		output << "<Opt Parameter List> ::= <Parameter List> | <Empty>	\n";
		output.flush();
	}

	if (currentRecord.getToken() != "id") {
		empty();
		return;
	}
	else {
		parameterList();
	}
}

// function thisisafunction (thisis real)
//r6
void parameterList() {
	if (printRules) {
		output << "<Parameter List> ::= <Parameter> <Parameter List prime>\n";
		output.flush();
	}
	parameter();
	parameterListp();
}

void parameterListp() {

	if (printRules) {
		output << "<Parameter List prime> ::= e | <Parameter List> \n";
		output.flush();
	}	
	currentRecord = callLexer();
	
	if (currentRecord.getLexeme() == ",") {
		parameterList();
	}
	else {
		empty();
		return;
	}
}

//r7
void parameter() { //OK 
	if (printRules) {
		output << "<Parameter> ::= <IDs > <Qualifier> \n";
		output.flush();
	}
	ids(); 
	qualifiers();
	
}

//r8
void qualifiers() { //OK
	if (printRules) {
		output << "<Qualifier> ::= int | boolean | real\n";
		output.flush();
	}
	if (currentRecord.getLexeme() == "int")
	{
		printCurrent();
	}
	else if (currentRecord.getLexeme() == "bool")
	{
		printCurrent();
	}
	else if (currentRecord.getLexeme() == "real")
	{
		printCurrent();
	}
	else pError("qualifier expected (qualifiers)");

}

// r9
void body() {
	if (printRules) {
		output << "<Body> ::= { < Statement List> }\n";
		output.flush();
	}

	if (currentRecord.getLexeme() == "{")
	{
		printCurrent();
	}
	else pError("{ expected (body)");


	currentRecord = callLexer();
	statementList();

	if (currentRecord.getLexeme() == "}")
	{
		printCurrent();
	}
	else pError("} expected (body)");
}

//r10
void optDeclarationList() 
{
	if (printRules) {
		output << "<Opt Declaration List> ::= <Declaration List> | <Empty>\n";
		output.flush();
	}

	currentRecord = callLexer();
	if (currentRecord.getLexeme() == "real")
	{
		declarationList();
	}
	else if (currentRecord.getLexeme() == "int")
	{
		declarationList();
	}
	else if (currentRecord.getLexeme() == "bool")
	{
		declarationList();
	}
	else {
		empty();
		return;

	}
}

//r11
void declarationList() {

	if (printRules) {
		output << "<Declaration List> := <Declaration>; <Declaration List Prime>\n";
		output.flush();
	}

	declaration();

	if (currentRecord.getLexeme() == ";") {
		printCurrent();
		declarationListp();
	}
	else {
		pError("; expected (declarationList)");
		empty();
		return;
	}
	

}

void declarationListp() {
	if (printRules) {
		output << "<Declaration List Prime > := e | <Declaration List>\n";
		output.flush();
	}
	currentRecord = callLexer();
	if (currentRecord.getLexeme() == "real")
	{
		declarationList();
	}
	else if (currentRecord.getLexeme() == "int")
	{
		declarationList();
	}
	else if (currentRecord.getLexeme() == "bool")
	{
		declarationList();
	}
	else {
		return;
	}
}

//r12
void declaration() {
	if (printRules) {
		output << "<Declaration> ::= <Qualifier> <IDs>	\n";
		output.flush();
	}

	qualifiers();
	currentRecord = callLexer();
	ids();
}

//r14
void statementList() {
	if (printRules) {
		output << "<Statement List> ::= <Statement> <Statement List prime>	\n";
		output.flush();
	}
	statement();
	statementListp();
	
}

void statementListp() {
	if (printRules) {
		output << "<Statement List prime> ::= e | <Statement List>	\n";
		output.flush();
	}

	currentRecord = callLexer();
	if (currentRecord.getLexeme() == "if") {
		statementList();
	}
	else if (currentRecord.getLexeme() == "{") {
		statementList();
	}
	else if (currentRecord.getToken() == "id") {
		statementList();
	}
	else if (currentRecord.getLexeme() == "return") {
		statementList();
	}
	else if (currentRecord.getLexeme() == "get") {
		statementList();
	}
	else if (currentRecord.getLexeme() == "put") {
		statementList();
	}
	else if (currentRecord.getLexeme() == "while") {
		statementList();
	}
	else
	{
		empty();
		return;
	}
}

//r15
void statement() {
	if (printRules) {
		output << "<Statement> ::= <Compound> | <Assign> | <If> | <Return> | <Print> | <Scan> | <While>	\n";
		output.flush();
	}

	if (currentRecord.getLexeme() == "if") {
		ifFunc();
	}
	else if (currentRecord.getLexeme() == "{") {
		compound();
	}
	else if (currentRecord.getToken() == "id") {
		assign();
	}
	else if (currentRecord.getLexeme() == "return") {
		returnFunc();
	}
	else if (currentRecord.getLexeme() == "get") {
		scan();
	}
	else if (currentRecord.getLexeme() == "put") {
		print();
	}
	else if (currentRecord.getLexeme() == "while") {
		whileFunc();
	}
}

// r16 
void compound() {
	if (printRules) {
		output << "<Compound> ::= { <Statement List> }  \n";
		output.flush();
	}

	if (currentRecord.getLexeme() == "{")
	{
		printCurrent();
	}
	statementListp();

	if (currentRecord.getLexeme() == "}")
	{
		printCurrent();
	}
	else pError("} expected (compound)");

	
}

//r17
void assign() {
	if (printRules) {
		output << "<Assign> ::= <Identifier> = <Expression> ;  \n";
		output.flush();
	}
	identifier();

	currentRecord = callLexer();
	if (currentRecord.getLexeme() == "=")
	{
		printCurrent();
	}
	else pError("= expected (assign)");

	currentRecord = callLexer();
	expression();

	if (currentRecord.getLexeme() == ";") {
		printCurrent();
	}
	else pError("; expected (assign)");
}

//r18
void ifFunc() {
	
	bool elseb = false;
	if (printRules) {
		output << "<If> ::= if ( <Condition> ) <Statement> fi | if (<Condition>) <Statement> else <Statement> fi		\n";
		output.flush();
	}

	if (currentRecord.getLexeme() == "if")
	{
		printCurrent();
	}

	currentRecord = callLexer();
	if (currentRecord.getLexeme() == "(")
	{
		printCurrent();
	}
	else pError("( expected (ifFunc)");

	currentRecord = callLexer();
	condition();

	if (currentRecord.getLexeme() == ")")
	{
		printCurrent();
	}
	else pError(") expected (ifFunc)");

	currentRecord = callLexer();
	statement();

	
	currentRecord = callLexer();
	if (currentRecord.getLexeme() == "else")
	{
		printCurrent();
		elseb = true;
	}
	if (elseb) { 
		currentRecord = callLexer();  
		statement();  
		currentRecord = callLexer();
	}


	if (currentRecord.getLexeme() == "fi")
	{
		printCurrent();
	}
	else pError("fi expected (ifFunc)");
}

//r23
void condition() {
	if (printRules) {
		output << "<Condition> ::= <Expression> <Relop> <Expression>		\n";
		output.flush();
	}
	expression();
	relop();
	currentRecord = callLexer();
	expression();
}

//r25
void expression() {

	if (printRules) {
		output << "<Expression > := <Term> <Expression prime>	\n";
		output.flush();
	}
	term();
	expressionp();
}

void expressionp() {
	if (printRules) {
		output << "<Expression prime> := + <Term> <Expression'> | - <Term> <Expression prime> | e	\n";
		output.flush();
	}

	if (currentRecord.getLexeme() == "+" || (currentRecord.getLexeme() == "-")) {
		printCurrent();
		currentRecord = callLexer();
		term();
		expressionp();
		
	}
	else {
		empty();
		return;
	}
}

void term() {
	if (printRules) {
		output << "<Term> := <Factor> <Term prime >	\n";
		output.flush();
	}
	factor();
	termp();
}

void termp() {
	if (printRules) {
		output << "<Term prime> := * <Factor> <Term prime> | / <Factor> <Term prime> | 	\n";
		output.flush();
	}
	
	if (currentRecord.getLexeme() == "*" || (currentRecord.getLexeme() == "/")) {
		printCurrent();
		currentRecord = callLexer();
		factor();
		termp();
	}
	else {
		empty();
		return;
	}
}

//r27
void factor() {
	if (printRules) {
		output << "<Factor> ::= - <Primary> | <Primary>	\n";
		output.flush();
	}

	if (currentRecord.getLexeme() == "-")
	{
		printCurrent();
	}
	primary();
	
}

void primary() {
	
	if (printRules) {
		output << "<Primary> :: = <Identifier> | <Integer> | <Identifier> (<IDs>) | (<Expression>) | <Real> | true | false\n";
		output.flush();
	}
	
	bool idsFlag = false;

	if (currentRecord.getToken() == "real") {
		isreal();
		currentRecord = callLexer();
	}
	else if (currentRecord.getToken() == "int") {
		isinteger();
		currentRecord = callLexer();
	}
	else if (currentRecord.getLexeme() == "true" || currentRecord.getLexeme() == "false") {
		isbool();
		currentRecord = callLexer();
	}
	else if (currentRecord.getToken() == "id") {
		identifier();
		currentRecord = callLexer();
		

		if (currentRecord.getLexeme() == "(") {
			printCurrent();
			idsFlag = true;
		}

		if (idsFlag) {
		
			currentRecord = callLexer();
			ids();
			if (currentRecord.getLexeme() == ")") {
				printCurrent();
				
			}
			else pError(") expected (primary)");
			currentRecord = callLexer();
		}
	}
	else if (currentRecord.getLexeme() == "(") {
		printCurrent();
		currentRecord = callLexer();
		expression();

		
		if (currentRecord.getLexeme() == ")") {
			printCurrent();
			currentRecord = callLexer();
		}
		else pError(") expected (primary)");

	}
	else pError("<primary> expected (primary)");

}
//r24
void relop() {

	if (printRules) {
		output << "<Relop> ::= == | != | > | < | <= | =>\n";
		output.flush();
	}

	if (currentRecord.getLexeme() == "==")
	{
		printCurrent();
	}
	else if (currentRecord.getLexeme() == "!=")
	{
		printCurrent();
	}
	else if (currentRecord.getLexeme() == ">")
	{
		printCurrent();
	}
	else if (currentRecord.getLexeme() == "<")
	{
		printCurrent();
	}
	else if (currentRecord.getLexeme() == "<=")
	{
		printCurrent();
	}
	else if (currentRecord.getLexeme() == "=>")
	{
		printCurrent();
	}
	else pError("<relop> expected (relop)");	
	

}

void whileFunc() {
	if (printRules) {
		output << "<While> ::= while ( <Condition> ) <Statement> \n";
		output.flush();
	}

	if (currentRecord.getToken() == "while")
	{
		printCurrent();

	}

	currentRecord = callLexer();
	if (currentRecord.getLexeme() == "(")
	{
		printCurrent();
	}
	else pError("( expected (whileFunc)");

	currentRecord = callLexer();
	condition();

	if (currentRecord.getLexeme() == ")")
	{
		printCurrent();
	}
	else pError(") expected (whileFunc)");

	statement();

}
//  r19
void returnFunc() {
	if (printRules) {
		output << "<Return> ::= return ; | return <Expression> ; \n";
		output.flush();
	}

	if (currentRecord.getLexeme() == "return")
	{
		printCurrent();
	}
	
	currentRecord = callLexer();
	if (currentRecord.getLexeme() != ";") {
		expression();
	}
	if (currentRecord.getLexeme() == ";") {
		printCurrent();
	}
	else pError("; expected (returnFunc)");
	
}

// r20
void print() {  

	if (printRules) {
		output << "<Print> ::= put ( <Expression>); \n";
		output.flush();
	}


	if (currentRecord.getLexeme() == "put")
	{
		printCurrent();
	}

	currentRecord = callLexer();
	if (currentRecord.getLexeme() == "(")
	{
		printCurrent();
	}
	else pError("( expected (print)");

	currentRecord = callLexer();
	expression();

	if (currentRecord.getLexeme() == ")")
	{
		printCurrent();
	}
	else pError(") expected (print)");

	currentRecord = callLexer();

	if (currentRecord.getLexeme() == ";") {
		printCurrent();
	}
	else pError("; expected (print)");
}

// r21
void scan() {  // OK

	if (printRules) {
		output << "<Scan> ::= get ( <IDs> ); \n";
		output.flush();
	}

	if (currentRecord.getLexeme() == "get")
	{
		printCurrent();

	}

	currentRecord = callLexer();
	if (currentRecord.getLexeme() == "(")
	{
		printCurrent();
	}
	else pError("( expected (scan)");
	currentRecord = callLexer();
	ids();

	if (currentRecord.getLexeme() == ")")
	{
		printCurrent();
	}
	else pError(") expected (scan)");

	currentRecord = callLexer();
	if (currentRecord.getLexeme() == ";") {
		printCurrent();
	}
	else pError("; expected (scan)");
}


//t
void identifier() {  // OK

	if (currentRecord.getToken() == "id")
	{
		printCurrent();

	}
	else pError("<id> expected (identifier)") ;
}

void isinteger() {  // OK

	if (currentRecord.getToken() == "int")
	{
		printCurrent();

	}
	else pError("<integer> expected (isinteger)");
}

void isreal() {  // OK

	if (currentRecord.getToken() == "real")
	{
		printCurrent();

	}
	else pError("<real> expected (isreal)");
}

void isbool() {  // OK

	if (currentRecord.getLexeme() == "true" || currentRecord.getLexeme() == "false")
	{
		printCurrent();

	}
	else pError("<bool> expected (isbool)");
}


//r13
void ids() { //OK

	if (printRules) {
		output << "<IDs> :: == <Identifier> <IDs prime>	\n";
		output.flush();
	}
	identifier();
	currentRecord = callLexer();
	idsp();
}

void idsp() { // OK

	if (printRules) {
		output << "<IDs prime> ::== |, <IDs>	\n";
		output.flush();
	}
	
	if (currentRecord.getLexeme() == ",") {
		printCurrent();
		currentRecord = callLexer();
		ids();
	}
	else {
		empty();
		return;
	}
}



void pError(string error) {
	output << error << "                Current Lexeme: " <<  currentRecord.getLexeme() << endl;
	output.flush();
	exit(0);
}

void printCurrent() {
	
	output << "Token: " << currentRecord.getToken() << "      Lexeme: " << currentRecord.getLexeme() << endl;
	output.flush();

}