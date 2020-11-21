#include "LayerStack.hpp"

Kestrel::LayerStack::~LayerStack(){
	for( auto l: layers ){
		l->onDetach();
	}
}

void Kestrel::LayerStack::pushLayer( std::shared_ptr<Layer> l ){
	l->onAttach();
	layers.push_back( l );
}

void Kestrel::LayerStack::PopLayer( std::shared_ptr<Layer> l ){
	auto it = std::find( layers.begin(), layers.end(), l );
	if( it != layers.end() ){
		l->onDetach();
		layers.erase( it );
	}
}
