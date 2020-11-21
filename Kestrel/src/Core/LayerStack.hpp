#pragma once

#include "kstpch.hpp"
#include "Layer.hpp"

namespace Kestrel {
	struct LayerStack {
		public:
			~LayerStack();

			void pushLayer( std::shared_ptr<Layer> );
			void PopLayer( std::shared_ptr<Layer> );

			auto begin(){ return layers.begin(); }
			auto end(){ return layers.end(); }
			auto rbegin(){ return layers.rbegin(); }
			auto rend(){ return layers.rend(); }

			auto begin() const { return layers.begin(); }
			auto end() const { return layers.end(); }
			auto rbegin() const { return layers.rbegin(); }
			auto rend() const { return layers.rend(); }


		private:
			std::vector<std::shared_ptr<Layer>> layers;
	};
}
