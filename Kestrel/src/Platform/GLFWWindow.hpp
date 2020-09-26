/*
 * =====================================================================================
 *
 *       Filename:  GLFWWindow.hpp
 *
 *    Description:  GLFW window implementation
 *
 *        Version:  1.0
 *        Created:  09/22/2020 04:46:52 PM
 *       Revision:  none
 *
 *         Author:  Samuel Knoethig (), samuel@knoethig.net
 *
 * =====================================================================================
 */

#include <kstpch.hpp>

#include "Core/Window.hpp"

namespace Kestrel {
	class KST_GLFWWindow: public Window {
		public:
			KST_GLFWWindow( WindowSettings settings = {} );
			~KST_GLFWWindow();

			std::pair<unsigned int, unsigned int> getResolution() override;

			void onUpdate() override;

			void setCallback(const EventCallback &e) override;
			void setCursor(const CursorMode &cm) override;

		private:
			WindowSettings w_settings;
			GLFWwindow* window;
	};
}
