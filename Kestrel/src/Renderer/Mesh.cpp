#include "Mesh.hpp"

using namespace Kestrel;

void Mesh::unload(){
	impl->unload();
}

bool Mesh::loaded(){
	return impl->loaded();
}

const BufferView<float> Mesh::getVertices(){
	return impl->getVertices();
}

const BufferView<uint32_t> Mesh::getIndices(){
	return impl->getIndices();
}
