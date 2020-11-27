#pragma once

#include <kstpch.hpp>

namespace Kestrel {

	struct VariableRegistry;

	enum VM_Type {
		float1,
		float2,
		float3,
		float4,
		string,
		composite,
	};

	struct TypeInformation {
		size_t size{ 0 };
		VM_Type type;
		std::unique_ptr<VariableRegistry> members{ nullptr }; //nullptr if not class
	};

	struct Variable {
		std::string name{};
		size_t offset{ 0 };
		TypeInformation type{};
	};
	
	struct VariableRegistry {
		std::unordered_map<std::string, Variable> variable{};
	};

	struct VariableHeap {
		public:
			VariableHeap();
			VariableHeap( size_t size_hint );

			VariableHeap( const VariableHeap& ) = delete;
			VariableHeap( VariableHeap&& );

			VariableHeap& operator=( const VariableHeap& ) = delete;
			VariableHeap& operator=( VariableHeap&& );

			~VariableHeap();

			size_t alloc_size( size_t alloc_size ); //Returns offset

			template<typename T>
			T& accessVal( const Variable& v ){
				KST_CORE_ASSERT( v.type.size == sizeof( T ), "Size {} of typeinformation does not match size {} of returned type", v.type.size, sizeof( T ));
				KST_CORE_ASSERT( v.offset + v.type.size <= size, "Trying to access memory range {} - {} out of bounds {} - {}", v.offset, v.offset + v.type.size, 0, size );

				return reinterpret_cast<T&>( *( data + v.offset ));
			}

			template<typename T>
			const T& accessVal( const Variable& v ) const {
				KST_CORE_ASSERT( v.type.size == sizeof( T ), "Size {} of typeinformation does not match size {} of returned type", v.type.size, sizeof( T ));
				KST_CORE_ASSERT( v.offset + v.type.size <= size, "Trying to access memory range {} - {} out of bounds {} - {}", v.offset, v.offset + v.type.size, 0, size );

				return reinterpret_cast<T&>( *( data + v.offset ));
			}


			template<typename T>
			T& accessVal( size_t offset ){
				KST_CORE_ASSERT( offset + sizeof( T ) <= size, "Trying to access memory range {} - {} out of bounds {} - {}", offset, offset + sizeof( T ), 0, size );

				return reinterpret_cast<T&>( *( data + offset ));
			}

			template<typename T>
			const T& accessVal( size_t offset ){
				KST_CORE_ASSERT( offset + sizeof( T ) <= size, "Trying to access memory range {} - {} out of bounds {} - {}", offset, offset + sizeof( T ), 0, size );

				return reinterpret_cast<T&>( *( data + offset ));
			}

			size_t size{};

		private:
			char* data{ nullptr };
			size_t reserved{};
	};
}
