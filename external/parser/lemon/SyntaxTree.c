#include "SyntaxTree.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

void ast_node_free( ast_node* node ){
	if( !node )
		return;
	switch( node->type ){
		case AST_NODE_float1:
		case AST_NODE_float2:
		case AST_NODE_float3:
		case AST_NODE_float4:
			break;
		case AST_NODE_plus:
		case AST_NODE_minus:
		case AST_NODE_times:
		case AST_NODE_divide:
		case AST_NODE_modulo:
		case AST_NODE_assign:
		{
			ast_node_free( node->val.op.lhs );
			ast_node_free( node->val.op.rhs );
			break;
		}
		case AST_NODE_arg_list:
		case AST_NODE_statement_list:
		case AST_NODE_declaration_list:
		{
			ast_node_free( node->val.list.next );
			ast_node_free( node->val.list.val );
			break;
		}
		case AST_NODE_declaration:
		{
			ast_node_free( node->val.decl.identifier );
			ast_node_free( node->val.decl.components );
			break;
		}
		case AST_NODE_identifier:
		{
			free( node->val.identifier.name );
			break;
		}
		case AST_NODE_statement:
		{
			ast_node_free( node->val.statement.statement );
			break;
		}
		case AST_NODE_function:
		{
			ast_node_free( node->val.function.name );
			ast_node_free( node->val.function.args );
			ast_node_free( node->val.function.statements );
			break;
		}
		case AST_NODE_program:
		{
			ast_node_free( node->val.program.decls );
			ast_node_free( node->val.program.functions );
			break;
		}
		default:
		{
			printf( "Encountered invalid type during free" );
		}
	}

	free( node );
}

static const char* ast_node_type_string( ast_node_type t ){
	switch( t ){
		case AST_NODE_float1: return "AST_NODE_float1";
		case AST_NODE_float2: return "AST_NODE_float2";
		case AST_NODE_float3: return "AST_NODE_float3";
		case AST_NODE_float4: return "AST_NODE_float4";
		case AST_NODE_plus: return "AST_NODE_plus";
		case AST_NODE_minus: return "AST_NODE_minus";
		case AST_NODE_times: return "AST_NODE_times";
		case AST_NODE_divide: return "AST_NODE_divide";
		case AST_NODE_modulo: return "AST_NODE_modulo";
		case AST_NODE_assign: return "AST_NODE_assign";
		case AST_NODE_identifier: return "AST_NODE_identifier";
		case AST_NODE_component_list: return "AST_NODE_component_list";
		case AST_NODE_component: return "AST_NODE_component";
		case AST_NODE_arg_list: return "AST_NODE_arg_list";
		case AST_NODE_declaration_list: return "AST_NODE_declaration_list";
		case AST_NODE_declaration: return "AST_NODE_declaration";
		case AST_NODE_statement_list: return "AST_NODE_statement_list";
		case AST_NODE_statement: return "AST_NODE_statement";
		case AST_NODE_function_list: return "AST_NODE_function_list";
		case AST_NODE_function: return "AST_NODE_function";
		case AST_NODE_program: return "AST_NODE_program";
		default: return "ERROR";
	}
}

static void ast_node_print_tree_int( ast_node* node, uint32_t offset ){
	if( !node ){
		printf( "NULL\n" );
		return;
	}
	printf( "%s:\n%*s", ast_node_type_string( node->type ), offset + 1, "" );
	switch( node->type ){
		case AST_NODE_float1:
		{
			printf( "%f\n", node->val.float1 );
			break;
		}
		case AST_NODE_float2:
		{
			printf( "%f %f\n", node->val.float2.x, node->val.float2.y );
			break;
		}
		case AST_NODE_float3:
		{
			printf( "%f %f %f\n", node->val.float3.x, node->val.float3.y, node->val.float3.z );
			break;

		}
		case AST_NODE_float4:
		{
			printf( "%f %f %f %f\n", node->val.float4.x, node->val.float4.y, node->val.float4.z, node->val.float4.w );
			break;

		}
		case AST_NODE_plus:
		case AST_NODE_minus:
		case AST_NODE_times:
		case AST_NODE_divide:
		case AST_NODE_modulo:
		case AST_NODE_assign:
		{
			printf( "lhs: " );
			ast_node_print_tree_int( node->val.op.lhs, offset + 1 );
			printf( "%*srhs: ", offset + 1, "" );
			ast_node_print_tree_int( node->val.op.rhs, offset + 1 );
			break;
		}
		case AST_NODE_identifier:
		{
			printf( "%s\n", node->val.identifier.name );
		}
		case AST_NODE_component_list:
		case AST_NODE_declaration_list:
		case AST_NODE_statement_list:
		case AST_NODE_function_list:
		case AST_NODE_arg_list:
		{
			ast_node* temp = node;
			do {
				ast_node_print_tree_int( temp->val.list.val, offset + 1 );
				printf( "%*s", temp->val.list.next ? offset + 1 : 0, "" );
				temp = temp->val.list.next;
			} while( temp != NULL );
			printf( "\n" );
		}
		case AST_NODE_component:
		{
			printf( "%s\n", node->val.identifier.name );
		}
		case AST_NODE_declaration:
		{
			ast_node_print_tree_int( node->val.decl.identifier, offset + 1 );
			printf( "%*s", offset + 1, "" );
			ast_node_print_tree_int( node->val.decl.components, offset + 1 );
		}
		case AST_NODE_statement:
		{
			ast_node_print_tree_int( node->val.statement.statement, offset + 1 );
		}
		case AST_NODE_function:
		{
			ast_node_print_tree_int( node->val.function.name, offset + 1 );
			printf( "%*s", offset + 1, "" );
			ast_node_print_tree_int( node->val.function.args, offset + 1 );
			printf( "%*s", offset + 1, "" );
			ast_node_print_tree_int( node->val.function.statements, offset + 1 );
		}
		case AST_NODE_program:
		{
			ast_node_print_tree_int( node->val.program.decls, offset + 1 );
		}
	}
}

void ast_node_print_tree( ast_node* node ){
	ast_node_print_tree_int( node, 0 );
}
