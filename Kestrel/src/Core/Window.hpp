/*
 * =====================================================================================
 *
 *       Filename:  Window.hpp
 *
 *    Description:  A standard window
 *
 *        Version:  1.0
 *        Created:  08/26/2020 02:26:32 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */
#pragma once

#include "kstpch.hpp"
#include "Event/Event.hpp"

#include <GLFW/glfw3.h>

namespace Kestrel {
	struct WindowSettings {
		using EventCallback = std::function<void(Event&)>;

		std::string name;
		unsigned int width;
		unsigned int height;
		EventCallback callback;

		WindowSettings(
				std::string name = "Kestrel application",
				unsigned int width = 1280,
				unsigned int height = 960,
				EventCallback callback = {}
			): name{ name }, width{ width }, height{ height }, callback{ callback }{}
	};

	class Window {
		public:
			using EventCallback = std::function<void(Event&)>;
			Window( WindowSettings settings = {} );
			~Window();

			std::pair<unsigned int, unsigned int> getResolution();

			void onUpdate();

			void setCallback( EventCallback e );

		private:
			WindowSettings s;
			EventCallback callback;
			GLFWwindow* window;
	};
}
