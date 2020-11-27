#include "VM/VM.hpp"

using namespace Kestrel;

void VM::execute( ast_node* node, VariableRegistry& reg, VariableHeap& heap ){
	switch( node->type ){
		case AST_NODE_statement_list:
		{
			while( node ){
				execute( node->val.list.val, reg, heap );
				node = node->val.list.next;
			}
		} break;
		case AST_NODE_statement:
		{
			execute( node->val.statement.statement, reg, heap );
		}
		case AST_NODE_assign:
		{
			Variable& temp = getLvalue( node->val.op.lhs, reg, heap );
			switch( temp.type.type ){
				case VM_Type::float1:
				{
					heap.accessVal<float>( temp ) = getRvalue( node->val.op.rhs, reg, heap ).val.float1;
				} break;
				case VM_Type::float2:
				{
					heap.accessVal<glm::vec2>( temp ) = getRvalue( node->val.op.rhs, reg, heap ).val.float2;
				} break;
				case VM_Type::float3:
				{
					heap.accessVal<glm::vec3>( temp ) = getRvalue( node->val.op.rhs, reg, heap ).val.float3;
				} break;
				case VM_Type::float4:
				{
					heap.accessVal<glm::vec4>( temp ) = getRvalue( node->val.op.rhs, reg, heap ).val.float4;
				} break;
				default:
				{
					KST_CORE_VERIFY( false, "Can not assign value type {}", temp.type.type );
				}
			}
		}
		default:
		{
			KST_CORE_VERIFY( false, "Can not execute node type {}", ast_node_to_string( node->type ));
		}
	}
}

Variable& VM::getLvalue( ast_node* node, VariableRegistry& reg, VariableHeap& heap ){
	switch( node->type ){
		case AST_NODE_dot:
		{
			KST_CORE_ASSERT( node->val.op.rhs->type == AST_NODE_identifier, "Unexpected node type {}, expected identifier", node->val.op.rhs->type );
			return getLvalue( node->val.op.lhs, reg, heap ).type.members->variable.at( node->val.op.rhs->val.identifier.name );
		} break;
		case AST_NODE_identifier:
		{
			return reg.variable.at( node->val.identifier.name );
		} break;
		default:
		{
			KST_CORE_VERIFY( false, "Can not convert node {} to lvalue", ast_node_to_string( node->type ));
		}
	}
}

VM_Value VM::getRvalue( ast_node* node, VariableRegistry& reg, VariableHeap& heap ){
	VM_Value ret;
	switch( node->type ){
		case AST_NODE_float1:
		{
			ret.type = VM_Type::float1;
			ret.val.float1 = node->val.float1;
		} break;
		case AST_NODE_float2:
		{
			ret.type = VM_Type::float2;
			ret.val.float2 = { node->val.float2.x, node->val.float2.y };
		} break;
		case AST_NODE_float3:
		{
			ret.type = VM_Type::float3;
			ret.val.float3 = { node->val.float3.x, node->val.float3.y, node->val.float3.z };
		} break;
		case AST_NODE_float4:
		{
			ret.type = VM_Type::float4;
			ret.val.float4 = { node->val.float4.x, node->val.float4.y, node->val.float4.z, node->val.float4.w };
		} break;
		case AST_NODE_dot:
		case AST_NODE_identifier:
		{
			auto& lv = getLvalue( node, reg, heap );
			switch( lv.type.type ){
				case VM_Type::float1:
				{
					ret.type = VM_Type::float1;
					ret.val.float1 = heap.accessVal<float>( lv );
					return ret;
				} break;
				case VM_Type::float2:
				{
					ret.type = VM_Type::float2;
					ret.val.float2 = heap.accessVal<glm::vec2>( lv );
					return ret;
				}
				case VM_Type::float3:
				{
					ret.type = VM_Type::float3;
					ret.val.float3 = heap.accessVal<glm::vec3>( lv );
					return ret;
				}
				case VM_Type::float4:
				{
					ret.type = VM_Type::float4;
					ret.val.float4 = heap.accessVal<glm::vec4>( lv );
					return ret;
				}
				default:
				{
					KST_CORE_VERIFY( false, "Can not assign value type {}", lv.type.type );
				}
			}
		} break;
		case AST_NODE_plus:
		{
			auto lhs = getRvalue( node->val.op.lhs, reg, heap );
			auto rhs = getRvalue( node->val.op.rhs, reg, heap );
			KST_CORE_ASSERT( lhs.type == rhs.type, "Can not add two different types {} and {}", lhs.type, rhs.type );
			ret.type = lhs.type;
			switch( ret.type ){
				case VM_Type::float1:
				{
					ret.val.float1 = lhs.val.float1 + rhs.val.float1;
				} break;
				case VM_Type::float2:
				{
					ret.val.float2 = lhs.val.float2 + rhs.val.float2;
				} break;
				case VM_Type::float3:
				{
					ret.val.float3 = lhs.val.float3 + rhs.val.float3;
				} break;
				case VM_Type::float4:
				{
					ret.val.float4 = lhs.val.float4 + rhs.val.float4;
				} break;
				default:
				{
					KST_CORE_VERIFY( false, "Invalid type {} in operator", ret.type );
				}
			}
		} break;
		case AST_NODE_minus:
		{
			auto lhs = getRvalue( node->val.op.lhs, reg, heap );
			auto rhs = getRvalue( node->val.op.rhs, reg, heap );
			KST_CORE_ASSERT( lhs.type == rhs.type, "Can not sub two different types {} and {}", lhs.type, rhs.type );
			ret.type = lhs.type;
			switch( ret.type ){
				case VM_Type::float1:
				{
					ret.val.float1 = lhs.val.float1 - rhs.val.float1;
				} break;
				case VM_Type::float2:
				{
					ret.val.float2 = lhs.val.float2 - rhs.val.float2;
				} break;
				case VM_Type::float3:
				{
					ret.val.float3 = lhs.val.float3 - rhs.val.float3;
				} break;
				case VM_Type::float4:
				{
					ret.val.float4 = lhs.val.float4 - rhs.val.float4;
				} break;
				default:
				{
					KST_CORE_VERIFY( false, "Invalid type {} in operator", ret.type );
				}
			}
		} break;
		case AST_NODE_times:
		{
			auto lhs = getRvalue( node->val.op.lhs, reg, heap );
			auto rhs = getRvalue( node->val.op.rhs, reg, heap );
			KST_CORE_ASSERT( lhs.type == rhs.type, "Can not mul two different types {} and {}", lhs.type, rhs.type );
			ret.type = lhs.type;
			switch( ret.type ){
				case VM_Type::float1:
				{
					ret.val.float1 = lhs.val.float1 * rhs.val.float1;
				} break;
				case VM_Type::float2:
				{
					ret.val.float2 = lhs.val.float2 * rhs.val.float2;
				} break;
				case VM_Type::float3:
				{
					ret.val.float3 = lhs.val.float3 * rhs.val.float3;
				} break;
				case VM_Type::float4:
				{
					ret.val.float4 = lhs.val.float4 * rhs.val.float4;
				} break;
				default:
				{
					KST_CORE_VERIFY( false, "Invalid type {} in operator", ret.type );
				}
			}
		} break;
		case AST_NODE_divide:
		{
			auto lhs = getRvalue( node->val.op.lhs, reg, heap );
			auto rhs = getRvalue( node->val.op.rhs, reg, heap );
			KST_CORE_ASSERT( lhs.type == rhs.type, "Can not div two different types {} and {}", lhs.type, rhs.type );
			ret.type = lhs.type;
			switch( ret.type ){
				case VM_Type::float1:
				{
					ret.val.float1 = lhs.val.float1 / rhs.val.float1;
				} break;
				case VM_Type::float2:
				{
					ret.val.float2 = lhs.val.float2 / rhs.val.float2;
				} break;
				case VM_Type::float3:
				{
					ret.val.float3 = lhs.val.float3 / rhs.val.float3;
				} break;
				case VM_Type::float4:
				{
					ret.val.float4 = lhs.val.float4 / rhs.val.float4;
				} break;
				default:
				{
					KST_CORE_VERIFY( false, "Invalid type {} in operator", ret.type );
				}
			}
		} break;
		case AST_NODE_modulo:
		{
			auto lhs = getRvalue( node->val.op.lhs, reg, heap );
			auto rhs = getRvalue( node->val.op.rhs, reg, heap );
			KST_CORE_ASSERT( lhs.type == rhs.type, "Can not div two different types {} and {}", lhs.type, rhs.type );
			ret.type = lhs.type;
			switch( ret.type ){
				case VM_Type::float1:
				{
					ret.val.float1 = (int)lhs.val.float1 % (int)rhs.val.float1;
				} break;
				default:
				{
					KST_CORE_VERIFY( false, "Invalid type {} in operator", ret.type );
				}
			}
		} break;
		case AST_NODE_uminus:
		{
			ret = getRvalue( node->val.u_op.rhs, reg, heap );
			switch( ret.type ){
				case VM_Type::float1:
				{
					ret.val.float1 = -ret.val.float1;
				} break;
				case VM_Type::float2:
				{
					ret.val.float2 = -ret.val.float2;
				} break;
				case VM_Type::float3:
				{
					ret.val.float3 = -ret.val.float3;
				} break;
				case VM_Type::float4:
				{
					ret.val.float4 = -ret.val.float4;
				} break;
				default:
				{
					KST_CORE_VERIFY( false, "Invalid type {} in negation operator", ret.type );
				}
			}
			return ret;
		} break;
		case AST_NODE_assign:
		{
			Variable& temp = getLvalue( node->val.op.lhs, reg, heap );
			switch( temp.type.type ){
				case VM_Type::float1:
				{
					auto rval = getRvalue( node->val.op.rhs, reg, heap ).val.float1;
					heap.accessVal<float>( temp ) = rval;
					ret.type = VM_Type::float1;
					ret.val.float1 = rval;
				} break;
				case VM_Type::float2:
				{
					auto rval = getRvalue( node->val.op.rhs, reg, heap ).val.float2;
					heap.accessVal<glm::vec2>( temp ) = rval;
					ret.type = VM_Type::float2;
					ret.val.float2 = rval;
				} break;
				case VM_Type::float3:
				{
					auto rval = getRvalue( node->val.op.rhs, reg, heap ).val.float3;
					heap.accessVal<glm::vec3>( temp ) = rval;
					ret.type = VM_Type::float3;
					ret.val.float3 = rval;
				} break;
				case VM_Type::float4:
				{
					auto rval = getRvalue( node->val.op.rhs, reg, heap ).val.float4;
					heap.accessVal<glm::vec4>( temp ) = rval;
					ret.type = VM_Type::float4;
					ret.val.float4 = rval;
				} break;
				default:
				{
					KST_CORE_VERIFY( false, "Can not assign value type {}", temp.type.type );
				}
			}
		} break;
		default:
		{
			KST_CORE_VERIFY( false, "Can not take rvalue of node type {}", node->type );
		} break;
	}
	return ret;
}
