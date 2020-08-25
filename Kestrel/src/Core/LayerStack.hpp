/*
 * =====================================================================================
 *
 *       Filename:  LayerStack.hpp
 *
 *    Description:  LayerStack class
 *
 *        Version:  1.0
 *        Created:  08/25/2020 04:40:01 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#pragma once

#include "kstpch.hpp"
#include "Layer.hpp"

namespace Kestrel {
	struct LayerStack {
		public:
			~LayerStack();

			void pushLayer( Layer* );
			void PopLayer( Layer* );

			auto begin(){ return layers.begin(); }
			auto end(){ return layers.end(); }
			auto rbegin(){ return layers.rbegin(); }
			auto rend(){ return layers.rend(); }

			auto begin() const { return layers.begin(); }
			auto end() const { return layers.end(); }
			auto rbegin() const { return layers.rbegin(); }
			auto rend() const { return layers.rend(); }


		private:
			std::vector<Layer*> layers;
	};
}
