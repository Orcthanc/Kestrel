#pragma once

#include "kstpch.hpp"
#include "Event/Event.hpp"

#include "Core/Input.hpp"

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

		WindowSettings( const WindowSettings& ) = default;

		WindowSettings( WindowSettings&& ws ) = default;

		WindowSettings& operator=( const WindowSettings& ) = default;

		WindowSettings& operator=( WindowSettings&& ws ) = default;
	};

	class Window {
		public:
			using EventCallback = std::function<void(Event&)>;
			virtual ~Window() = default;

			virtual std::pair<unsigned int, unsigned int> getResolution() = 0;

			virtual void onUpdate() = 0;

			virtual void setCallback( const EventCallback& e ) = 0;
			virtual void setCursor( const CursorMode& cm ) = 0;
			virtual int getKeyState( int key ) = 0;
	};
}
