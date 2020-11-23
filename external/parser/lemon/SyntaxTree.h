#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>

	typedef enum ast_node_type {
		//Constants
		AST_NODE_float1,
		AST_NODE_float2,
		AST_NODE_float3,
		AST_NODE_float4,
		//Operators
		AST_NODE_plus,
		AST_NODE_minus,
		AST_NODE_times,
		AST_NODE_divide,
		AST_NODE_modulo,
		AST_NODE_assign,
		//Identifier
		AST_NODE_identifier,
		AST_NODE_component_list,
		AST_NODE_component,
		//Function
		AST_NODE_arg_list, //Consists of declarations
		//Meta
		AST_NODE_declaration_list,
		AST_NODE_declaration,
		AST_NODE_statement_list,
		AST_NODE_statement,
		AST_NODE_function_list,
		AST_NODE_function,
		AST_NODE_program,
	} ast_node_type;

	typedef struct ast_node ast_node;

	typedef struct ast_node_declaration {
		ast_node* components;
		ast_node* identifier;
	} ast_node_declaration;

	typedef struct ast_node_operator {
		ast_node* lhs;
		ast_node* rhs;
	} ast_node_operator;

	typedef struct ast_node_statement {
		ast_node* statement;
	} ast_node_statement;

	typedef struct ast_node_list {
		ast_node* val;
		ast_node* next;
	} ast_node_list;

	typedef struct ast_node_function {
		ast_node* name;
		ast_node* args;
		ast_node* statements;
	} ast_node_function;

	typedef struct ast_node_prog {
		ast_node* decls;
		ast_node* functions;
	} ast_node_prog;

	typedef struct float2_s {
		float x, y;
	} float2_s;

	typedef struct float3_s {
		float x, y, z;
	} float3_s;

	typedef struct float4_s {
		float x, y, z, w;
	} float4_s;

	typedef struct string_s {
		char* name;
	} string_s;

	struct ast_node {
		ast_node_type type;
		union {
			ast_node_declaration decl;
			ast_node_operator op;
			ast_node_list list;
			ast_node_function function;
			ast_node_statement statement;
			ast_node_prog program;
			float float1;
			float2_s float2;
			float3_s float3;
			float4_s float4;
			string_s identifier;
		} val;
	};

	extern void ast_node_free( ast_node* );
	extern void ast_node_print_tree( ast_node* );


	//Parser
	extern void* ParseAlloc( void*( *memalloc )( size_t ));
	extern void ParseFree( void* parser, void( *memfree )( void* ));
	extern void Parse( void* parser, int token_code, const char* value, ast_node** tree );
	extern void ParseTrace( FILE* file, char* prefix );

#ifdef __cplusplus
}
#endif
