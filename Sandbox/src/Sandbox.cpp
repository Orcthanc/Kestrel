#include "Sandbox.hpp"
#include "SandboxLayer.hpp"

#include "SyntaxTree.h"
#include "Lexer.hpp"
#include "test.h"

static void ParserTest(){
	std::string test_prog = R"asdf(
cam1[Camera]
plane1[Transform]
plane2[Camera,Transform]

void cam1(){
	Transform + ( 1.0, 0.5, -1.0 );
}
	)asdf";

	Lexer::Lexer lex;

	std::map<Lexer::token_id, int> convert_tok;

	convert_tok[0] = 0;
	convert_tok[lex.push_rule( "void", "void" )] = VOID;
	convert_tok[lex.push_rule( "Identifier", "\\w+\\d*" )] = IDENTIFIER;
	convert_tok[lex.push_rule( "Plus", "\\+" )] = PLUS;
	convert_tok[lex.push_rule( "Minus", "-" )] = MINUS;
	convert_tok[lex.push_rule( "Times", "\\*" )] = TIMES;
	convert_tok[lex.push_rule( "Divide", "/" )] = DIVIDE;
	convert_tok[lex.push_rule( "Modulo", "%" )] = MODULO;
	convert_tok[lex.push_rule( "Float", "\\d*.\\d+" )] = FLOAT;
	convert_tok[lex.push_rule( "Assignment", "=" )] = ASSIGN;
	convert_tok[lex.push_rule( "CurlyBracketOpen", "{" )] = CURLY_BRACKET_LEFT;
	convert_tok[lex.push_rule( "CurlyBracketClose", "}" )] = CURLY_BRACKET_RIGHT;
	convert_tok[lex.push_rule( "ParenthesisOpen", "(" )] = PARENTHESIS_LEFT;
	convert_tok[lex.push_rule( "ParenthesisClose", ")" )] = PARENTHESIS_RIGHT;
	convert_tok[lex.push_rule( "BracketOpen", "[" )] = LEFT_SQUARE_BRACKET;
	convert_tok[lex.push_rule( "BracketClose", "]" )] = RIGHT_SQUARE_BRACKET;
	convert_tok[lex.push_rule( "Comma", "," )] = COMMA;
//	convert_tok[lex.push_rule( "Dot", "." )] =
	convert_tok[lex.push_rule( "Semicolon", ";" )] = SEMICOLON;

	auto ws = lex.push_rule( "Whitespace", "\\s" );

	auto parser = ParseAlloc( malloc );

	lex.start( test_prog.c_str() );

	Lexer::Token t;
	ast_node* tree;
	FILE* f = fopen( "parser_debug.txt", "w" );
	char prefix[2] = { '>', ' ' };
	ParseTrace( f, prefix );
	std::unordered_set<std::string> strings; //TODO
	do {
		lex >> t;
		//std::cout << t << std::endl;
		if( t.id != ws ){
			if( convert_tok[ t.id ] == IDENTIFIER || convert_tok[ t.id ] == FLOAT ){
				Parse( parser, convert_tok.at( t.id ), strings.emplace( std::move( t.match_text )).first->c_str(), &tree );
			} else
				Parse( parser, convert_tok.at( t.id ), t.match_text.c_str(), &tree );
		}
	} while( t.id != 0 );

	fflush( f );
	fclose( f );

	ast_node_print_tree( tree );
	ast_node_free( tree );

	ParseFree( parser, free );
}

Sandbox::Sandbox(): Kestrel::Application(){
	//TODO lifetime management
	addLayer( std::make_shared<SandboxLayer>( "Sandbox" ));
	ParserTest();
}

std::unique_ptr<Kestrel::Application> createApplication(){
	return std::make_unique<Sandbox>();
}
