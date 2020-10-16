#include "Mesh.hpp"

using namespace Kestrel;

void Mesh::unload(){
	impl->unload();
}

bool Mesh::loaded(){
	return impl->loaded();
}
