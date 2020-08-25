/*
 * =====================================================================================
 *
 *       Filename:  Layer.hpp
 *
 *    Description:  A simple layer
 *
 *        Version:  1.0
 *        Created:  08/25/2020 04:29:42 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#pragma once

#include "kstpch.hpp"
#include "Event/Event.hpp"

namespace Kestrel {
	struct Layer {
		public:
			Layer( const std::string& name );
			virtual ~Layer() = default;

			virtual void onAttach();
			virtual void onDetach();
			virtual void onUpdate();
			virtual void onGui();
			virtual void onEvent( Event& e );

			const std::string getName();

		private:
			std::string debug_name;
	};
}
