#pragma once

#include <kstpch.hpp>

#include "SyntaxTree.h"
#include "VM/Variables.hpp"

#include "glm/glm.hpp"

namespace Kestrel {



	struct VM_Value {
		VM_Type type;
		union {
			float float1;
			glm::vec2 float2;
			glm::vec3 float3;
			glm::vec4 float4;
			const char* string;
		} val;
	};


	struct VM {
		public:
			void execute( ast_node* statement, VariableRegistry& reg, VariableHeap& heap );

		private:
			Variable& getLvalue( ast_node* node, VariableRegistry& reg, VariableHeap& heap );
			VM_Value getRvalue( ast_node* node, VariableRegistry& reg, VariableHeap& heap );
	};
}
