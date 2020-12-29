%include{
	#include <stdio.h>
	#include <assert.h>
	#include <string.h>
	#include <stdlib.h>

	#include "SyntaxTree.h"
}

%syntax_error{ fprintf( stderr, "Syntax error\n" ); }

%right ASSIGN.
%left PLUS MINUS.
%left TIMES DIVIDE MODULO.
%right UMINUS.

%token_type { const char* }
%default_type { ast_node* }
%type floatl { float }
%default_destructor { ast_node_free( $$ ); }
%extra_argument { ast_node** root }

program(A) ::= decls(B) functions(C). {
	*root = A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_program;
	A->val.program.decls = B;
	A->val.program.functions = C;
}

decls(A) ::= decls(B) decl(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_declaration_list;
	A->val.list.next = B;
	A->val.list.val = C;
}

decls(A) ::= . {
	A = NULL;
}

decl(A) ::= identifier(B) LEFT_SQUARE_BRACKET complist(C) RIGHT_SQUARE_BRACKET. {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_declaration;
	A->val.decl.identifier = B;
	A->val.decl.components = C;
}

decl(A) ::= identifier(B) LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET. {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_declaration;
	A->val.decl.identifier = B;
	A->val.decl.components = NULL;
}

identifier(A) ::= IDENTIFIER(B). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_identifier;
	A->val.identifier.name = malloc( strlen( B ) + 1 );
	strcpy( A->val.identifier.name, B );
}

complist(A) ::= complist(B) COMMA comp(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_component_list;
	A->val.list.next = B;
	A->val.list.val = C;
}

complist(A) ::= comp(B). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_component_list;
	A->val.list.next = NULL;
	A->val.list.val = B;
}

comp(A) ::= identifier(B). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_component;
	A->val.comp.name = B;
	A->val.comp.value = NULL;
}

comp(A) ::= identifier(B) ASSIGN rvalue(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_component;
	A->val.comp.name = B;
	A->val.comp.value = C;
}

functions(A) ::= functions(B) function(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_function_list;
	A->val.list.next = B;
	A->val.list.val = C;
}

functions(A) ::= . {
	A = NULL;
}

function(A) ::= VOID identifier(B) PARENTHESIS_LEFT PARENTHESIS_RIGHT CURLY_BRACKET_LEFT statements(C) CURLY_BRACKET_RIGHT. {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_function;
	A->val.function.name = B;
	A->val.function.args = NULL;
	A->val.function.statements = C;
}

statements(A) ::= statements(B) statement(C). {
	A = B;
	if( A == NULL ){
		A = malloc( sizeof( ast_node ));
		A->type = AST_NODE_statement_list;
		A->val.list.next = NULL;
		A->val.list.val = C;
	} else {
		ast_node* temp = B;
		while( temp->val.list.next != NULL ){
			temp = temp->val.list.next;
		}
		temp->val.list.next = malloc( sizeof( ast_node ));
		temp->val.list.next->type = AST_NODE_statement_list;
		temp->val.list.next->val.list.next = NULL;
		temp->val.list.next->val.list.val = C;
	}
}

statements(A) ::= . {
	A = NULL;
}

statement(A) ::= operator(B) SEMICOLON. {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_statement;
	A->val.statement.statement = B;
}

operator(A) ::= rvalue(B) PLUS rvalue(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_plus;
	A->val.op.lhs = B;
	A->val.op.rhs = C;
}

operator(A) ::= rvalue(B) MINUS rvalue(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_minus;
	A->val.op.lhs = B;
	A->val.op.rhs = C;
}

operator(A) ::= rvalue(B) TIMES rvalue(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_times;
	A->val.op.lhs = B;
	A->val.op.rhs = C;
}

operator(A) ::= rvalue(B) DIVIDE rvalue(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_divide;
	A->val.op.lhs = B;
	A->val.op.rhs = C;
}

operator(A) ::= rvalue(B) MODULO rvalue(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_modulo;
	A->val.op.lhs = B;
	A->val.op.rhs = C;
}

operator(A) ::= lvalue(B) ASSIGN rvalue(C). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_assign;
	A->val.op.lhs = B;
	A->val.op.rhs = C;
}

lvalue(A) ::= identifier(B). {
	A = B;
}

lvalue(A) ::= lvalue(B) DOT identifier (C) . {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_dot;
	A->val.op.lhs = B;
	A->val.op.rhs = C;
}

rvalue(A) ::= lvalue(B). {
	A = B;
}

rvalue(A) ::= PARENTHESIS_LEFT rvalue(B) PARENTHESIS_RIGHT. {
	A = B;
}

rvalue(A) ::= STRING(B). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_string;
	size_t temp = strlen( B );
	A->val.identifier.name = malloc( temp - 1 );
	strncpy( A->val.identifier.name, B + 1, temp - 2 );
	A->val.identifier.name[temp - 2] = 0;
}

rvalue(A) ::= float1(B). {
	A = B;
}

rvalue(A) ::= PARENTHESIS_LEFT floatl(B) COMMA floatl(C) PARENTHESIS_RIGHT. {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_float2;
	A->val.float2.x = B;
	A->val.float2.y = C;
}

rvalue(A) ::= PARENTHESIS_LEFT floatl(B) COMMA floatl(C) COMMA floatl(D) PARENTHESIS_RIGHT. {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_float3;
	A->val.float3.x = B;
	A->val.float3.y = C;
	A->val.float3.z = D;
}

rvalue(A) ::= PARENTHESIS_LEFT floatl(B) COMMA floatl(C) COMMA floatl(D) COMMA floatl(E) PARENTHESIS_RIGHT. {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_float4;
	A->val.float4.x = B;
	A->val.float4.y = C;
	A->val.float4.z = D;
	A->val.float4.w = E;
}

floatl(A) ::= FLOAT(B). {
	A = (float)atof( B );
}

floatl(A) ::= MINUS floatl(B). [UMINUS] {
	A = -B;
}

rvalue(A) ::= MINUS rvalue(B). [UMINUS] {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_uminus;
	A->val.u_op.rhs = B;
}

float1(A) ::= FLOAT(B). {
	A = malloc( sizeof( ast_node ));
	A->type = AST_NODE_float1;
	A->val.float1 = (float)atof( B );
}

rvalue(A) ::= operator(B). {
	A = B;
}
