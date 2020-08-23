/*
 * =====================================================================================
 *
 *       Filename:  Application.h
 *
 *    Description:  Main application to inherit
 *
 *        Version:  1.0
 *        Created:  08/23/2020 03:06:43 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#pragma once

#include "kstpch.hpp"

int main( int, char** );

namespace Kestrel {
	class Application {
		public:
			Application() = default;
			virtual ~Application() = default;

		private:
			void operator()();
			friend int ::main( int, char** );
	};
}
