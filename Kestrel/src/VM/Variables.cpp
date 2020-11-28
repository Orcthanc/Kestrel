#include "VM/Variables.hpp"

using namespace Kestrel;

VariableHeap::VariableHeap(): VariableHeap{ 8 }{}

VariableHeap::VariableHeap( size_t initial_size ){
	data = ( char* )malloc( initial_size );
	reserved = initial_size;
}

VariableHeap::VariableHeap( VariableHeap&& o ){
	data = o.data;
	size = o.size;
	reserved = o.reserved;

	o.data = nullptr;
	o.size = 0;
	o.reserved = 0;
}

VariableHeap& VariableHeap::operator=( VariableHeap&& o ){
	if( this == &o )
		return *this;

	data = o.data;
	size = o.size;
	reserved = o.reserved;

	o.data = nullptr;
	o.size = 0;
	o.reserved = 0;

	return *this;
}

VariableHeap::~VariableHeap(){
	free( data );
}

size_t VariableHeap::alloc_size( size_t alloc_size ){
	while( alloc_size + size > reserved ){
		reserved *= 2;
		data = ( char* )realloc( data, reserved * 2 );
	}
	size_t temp = size;
	size += alloc_size;
	return temp;
}
