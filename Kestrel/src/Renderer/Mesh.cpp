#include "Mesh.hpp"

using namespace Kestrel;

void Mesh::unload(){
	impl = nullptr;
}

bool Mesh::loaded(){
	return ( bool )impl;
}

const BufferView<float> Mesh::getVertices(){
	return impl->getVertices();
}

const BufferView<uint32_t> Mesh::getIndices(){
	return impl->getIndices();
}
