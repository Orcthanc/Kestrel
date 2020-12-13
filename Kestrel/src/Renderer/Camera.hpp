#pragma once

#include <kstpch.hpp>

#include "Scene/Entity.hpp"

#include "Renderer/CameraModes.hpp"
#include "Event/Events.hpp"

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Kestrel {
	struct Camera;

	struct CameraRenderer {
		virtual ~CameraRenderer() = default;

		virtual void begin_scene( Camera&, size_t window_index ) = 0;
		virtual void draw( Entity e ) = 0;
		virtual void endScene() = 0;
	};

	struct Camera {
		public:
			virtual ~Camera() = default;

			glm::vec3 pos;
			glm::quat rot;

			void move( float x, float y, float z );
			void rotate( float x, float y, float z, float angle );
			void rotate( const glm::quat& quaternion );
			void recalc_view();
			virtual bool onSizeChange( Kestrel::WindowResizeEvent& e );
			virtual void updateRenderMode( RenderModeFlags new_mode );

			void set_renderer( std::unique_ptr<CameraRenderer>&& );

			template <typename Renderer, typename... Args>
			auto emplace_renderer( Args... args )
					-> std::enable_if_t<std::is_constructible_v<Renderer, Args...>>{

				renderer = std::make_unique<Renderer>( std::forward<Args>( args )... );
			}

			void begin_scene( size_t window_index );
			void draw( Entity e );
			void endScene();

			glm::mat4 view{};
			glm::mat4 proj{};

			RenderModeFlags camera_render_mode{ RenderModeFlags::eInverse };

		private:
			std::unique_ptr<CameraRenderer> renderer;
	};
}
