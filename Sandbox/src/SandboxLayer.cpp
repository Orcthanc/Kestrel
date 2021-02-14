#include "SandboxLayer.hpp"

#include "Renderer/Context.hpp"

#include "Platform/Vulkan/VKCameraRenderer.hpp"
#include "Platform/Vulkan/VKMaterial.hpp"
#include "Platform/Vulkan/VKMesh.hpp"

#include "Scene/Components.hpp"
#include "Renderer/CameraModes.hpp"

#include "glm/gtc/quaternion.hpp"
#include "imgui.h"

#include <numbers>

using namespace Kestrel;

//static float run_distances[100] = { 0.001, 0.007999999999999997, 0.026999999999999986, 0.06399999999999996, 0.12499999999999992, 0.21599999999999983, 0.3429999999999997, 0.5119999999999996, 0.7289999999999993, 0.999999999999999, 1.3309999999999986, 1.7279999999999982, 2.1969999999999974, 2.7439999999999967, 3.374999999999996, 4.095999999999995, 4.912999999999994, 5.831999999999993, 6.858999999999991, 7.999999999999989, 9.260999999999987, 10.647999999999985, 12.166999999999984, 13.82399999999998, 15.624999999999979, 17.575999999999976, 19.68299999999997, 21.951999999999966, 24.388999999999964, 26.99999999999996, 29.790999999999958, 32.76799999999995, 35.93699999999994, 39.303999999999945, 42.874999999999936, 46.65599999999993, 50.65299999999992, 54.871999999999915, 59.3189999999999, 63.9999999999999, 68.92099999999988, 74.08799999999988, 79.50699999999986, 85.18399999999986, 91.12499999999984, 97.33599999999984, 103.82299999999982, 110.59199999999981, 117.6489999999998, 124.99999999999979, 132.65099999999975, 140.60799999999978, 148.87699999999973, 157.46399999999971, 166.37499999999972, 175.61599999999967, 185.19299999999967, 195.11199999999965, 205.37899999999962, 215.99999999999963, 226.9809999999996, 238.32799999999958, 250.04699999999954, 262.1439999999995, 274.6249999999995, 287.49599999999947, 300.7629999999994, 314.4319999999994, 328.50899999999933, 342.9999999999994, 357.9109999999993, 373.2479999999993, 389.01699999999926, 405.22399999999925, 421.8749999999992, 438.97599999999915, 456.5329999999991, 474.55199999999905, 493.0389999999991, 511.99999999999903, 531.440999999999, 551.3679999999989, 571.7869999999988, 592.7039999999988, 614.1249999999989, 636.0559999999987, 658.5029999999987, 681.4719999999986, 704.9689999999986, 728.9999999999985, 753.5709999999984, 778.6879999999985, 804.3569999999984, 830.5839999999984, 857.3749999999983, 884.7359999999983, 912.6729999999982, 941.1919999999982, 970.298999999998, 999.999999999998 };

static float run_distances[100] = { 0.0011481536214968829, 0.0013182567385564073, 0.0015135612484362085, 0.001737800828749376, 0.0019952623149688802, 0.002290867652767774, 0.002630267991895383, 0.003019951720402018, 0.0034673685045253184, 0.003981071705534975, 0.004570881896148753, 0.00524807460249773, 0.006025595860743582, 0.006918309709189371, 0.007943282347242822, 0.009120108393559107, 0.010471285480509006, 0.012022644346174144, 0.013803842646028866, 0.015848931924611155, 0.01819700858609986, 0.020892961308540424, 0.02398832919019494, 0.02754228703338171, 0.03162277660168385, 0.0363078054770102, 0.04168693834703361, 0.04786300923226392, 0.05495408738576255, 0.06309573444801946, 0.07244359600749915, 0.08317637711026726, 0.0954992586021438, 0.10964781961431874, 0.125892541179417, 0.1445439770745931, 0.16595869074375644, 0.19054607179632518, 0.21877616239495581, 0.2511886431509587, 0.28840315031266134, 0.331131121482592, 0.38018939632056226, 0.4365158322401672, 0.5011872336272738, 0.5754399373371587, 0.660693448007598, 0.7585775750291862, 0.8709635899560834, 1.000000000000003, 1.1481536214968864, 1.3182567385564117, 1.5135612484362133, 1.7378008287493816, 1.9952623149688866, 2.290867652767781, 2.630267991895392, 3.0199517204020276, 3.4673685045253295, 3.981071705534988, 4.5708818961487685, 5.248074602497747, 6.025595860743603, 6.918309709189393, 7.943282347242848, 9.120108393559136, 10.47128548050904, 12.022644346174182, 13.80384264602891, 15.848931924611206, 18.197008586099916, 20.892961308540492, 23.988329190195017, 27.542287033381797, 31.622776601683945, 36.30780547701031, 41.686938347033745, 47.863009232264076, 54.95408738576274, 63.09573444801965, 72.4435960074994, 83.17637711026754, 95.49925860214411, 109.6478196143191, 125.89254117941742, 144.54397707459356, 165.95869074375702, 190.5460717963258, 218.77616239495651, 251.18864315095948, 288.4031503126623, 331.1311214825931, 380.1893963205635, 436.51583224016866, 501.18723362727536, 575.4399373371605, 660.6934480076002, 758.5775750291886, 870.9635899560863, 1000.0000000000065 };

static size_t run_num = 0;
glm::quat rot = glm::quat( glm::vec3( 0, 1, 0 ));

SandboxLayer::SandboxLayer( const std::string& s ): Layer{ s }{
#ifndef SB_KST_TERRAIN
	auto& scene = Application::getInstance()->current_scene;

	plane1 = scene->createEntity( "Plane1" );
	plane1.addComponent<MaterialComponent>( VK_Materials::getInstance().loadMaterial( "../res/Kestrel/shader/basic" ));
	plane1.addComponent<MeshComponent>( VK_MeshRegistry::requestOrLoadMesh( "../res/Kestrel/res/models/Terrain4x4.obj" ));
	plane1.addComponent<TransformComponent>( glm::vec3{ 0, 0, 0 }, glm::conjugate( rot ), glm::vec3{ 1, 1, 1 });
	plane1.addComponent<ColorComponent>( glm::vec3{ 1, 0, 0 });

	plane2 = scene->createEntity( "Plane2" );
	plane2.addComponent<MaterialComponent>( VK_Materials::getInstance().loadMaterial( "../res/Kestrel/shader/basic" ));
	plane2.addComponent<MeshComponent>( VK_MeshRegistry::requestOrLoadMesh( "../res/Kestrel/res/models/Terrain4x4.obj" ));
	plane2.addComponent<TransformComponent>( glm::conjugate( rot ) * glm::vec3{ 0, 0, -run_distances[run_num] } * rot, glm::conjugate( rot ), glm::vec3{ 1, 1, 1 });
	plane2.addComponent<ColorComponent>( glm::vec3{ 0, 1, 0 });

#endif

	auto cam = Application::getInstance()->current_scene->createEntity( "Camera" );
	cam.addComponent<TransformComponent>();
	camera = std::make_shared<NaiveCamera>( glm::radians( 90.0 ), 960.0/1080.0, 0.1, 1000000.0 );

	camera->rotate( rot );

	cam.addComponent<CameraComponent>( camera );
}

void SandboxLayer::onUpdate(){
#ifdef SB_KST_TERRAIN
#ifdef SB_MANUAL
	float x = 0, y = 0, z = 0;
	Application& temp = *Application::getInstance();

	float speed = 0.1f;
	if( temp.getKeyState( GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS ){
		speed *= 50;
	}
	if( temp.getKeyState( GLFW_KEY_W ) == GLFW_PRESS ){
		z -= speed;
	}
	if( temp.getKeyState( GLFW_KEY_S ) == GLFW_PRESS ){
		z += speed;
	}
	if( temp.getKeyState( GLFW_KEY_A ) == GLFW_PRESS ){
		x -= speed;
	}
	if( temp.getKeyState( GLFW_KEY_D ) == GLFW_PRESS ){
		x += speed;
	}
	if( temp.getKeyState( GLFW_KEY_LEFT_CONTROL ) == GLFW_PRESS ){
		y -= speed;
	}
	if( temp.getKeyState( GLFW_KEY_SPACE ) == GLFW_PRESS ){
		y += speed;
	}
	if( x || y || z )
		camera->move( x, y, z );
#else

	static size_t frame = 0;
	if( frame == 9999 )
		Kestrel::Application::getInstance()->running = false;

	camera->pos.y = std::max( 10.0, -790.0 + frame * 0.2 );

	if( frame < 4000 ){
		camera->rot = glm::quat( glm::vec3(( 1 - std::abs( std::cos( 0.00025 * std::numbers::pi * frame ))) * -1.7, 0, 0.01 ));
	} else {
		camera->rot = glm::quat( glm::vec3( std::log(( frame - 3999 )) / 20, 0, 0.01 ));
	}

	camera->recalc_view();

	++frame;

#endif
#endif

#ifdef KST_COLOR_STATS
	static glm::vec3 pos = { 0, 0, 0.1 };

	static size_t steps = 0;
	static bool forward = true;
	static float last_val;
	static size_t iteration = 1;
	static bool reset = true;
	static size_t s = 1;

	if( reset ){
		pos = { 0, 0, 0.1 };
		steps = 0;
		forward = true;
		iteration = 1;
		reset = false;
		render_feed_back = {};
		render_feed_back.distance = run_distances[run_num] /* run_distances[run_num]*/ * 64 * 16 * 0.00001;
		s = 1;

		plane2.getComponent<TransformComponent>().loc = glm::conjugate( rot ) * glm::vec3{ 0, 0, -run_distances[run_num] * 0.00001 } * rot;
		//plane2.getComponent<TransformComponent>().loc.z = -run_distances[run_num] * 0.00001;

		KST_INFO( "Starting run {:2} with dz = {:.12}", run_num, render_feed_back.distance );
	}

	if( forward ){
		if( render_feed_back.is_hit ){
			render_feed_back.last_hit = pos.z;
			render_feed_back.time_since_hit = 0;
			render_feed_back.distance /= 1.777777777777;
			forward = false;
			KST_INFO( "Iteration  0 hit at {:.12} Starting next iteration with dz = {:.12}", render_feed_back.last_hit, render_feed_back.distance );
		} else {
			pos.z = 0.1 + render_feed_back.distance * s;
			if( !( ++s % 20000 )){
				color_stat_file << std::fixed << std::setprecision( 12 ) << run_distances[run_num] << ":" << -1 << std::endl;
				reset = true;
				++run_num;
				if( run_num > 99 )
					Application::getInstance()->running = false;
				return;

			}
		}
	}
	else {
		if( render_feed_back.is_hit ){
			render_feed_back.time_since_hit = 0;
			render_feed_back.last_hit = pos.z;
		} else {
			render_feed_back.time_since_hit += 1;
		}

		last_val = render_feed_back.last_hit - steps * render_feed_back.distance;

		++steps;

		if( last_val == render_feed_back.last_hit - steps * render_feed_back.distance ){
			KST_INFO( "Found first hit" );
		}

		pos.z = render_feed_back.last_hit - steps * render_feed_back.distance;

		if( pos.z <= 0.1 || render_feed_back.time_since_hit > 256 ){
			pos.z = render_feed_back.last_hit;
			render_feed_back.distance /= 1.777777777777;
			render_feed_back.time_since_hit = 0;
			steps = 0;

			if( iteration == 30 ){
				color_stat_file << std::fixed << std::setprecision( 12 ) << run_distances[run_num] << ":" << pos.z << std::endl;
				reset = true;
				++run_num;
				if( run_num > 99 )
					Application::getInstance()->running = false;
				return;
			}

			KST_INFO( "Iteration {:2} hit at {:.12} Starting next iteration with dz = {:.12}", iteration, pos.z, render_feed_back.distance );
			iteration++;
		}
	}

	pos.y = 0;
	//pos.x = 5 * pos.z;
	pos.x = 0;
	//glm::quat rot2( glm::vec3{ 0, -1, 0 });
	camera->pos = glm::inverse( glm::mat4_cast( rot )) * glm::vec4( pos, 1 );
	//camera->pos = glm::conjugate( rot2 ) * pos * rot2;
	//camera->pos = pos;
	camera->recalc_view();

	float size = 1.2 * (pos.z + 0.1);

	plane1.getComponent<TransformComponent>().scale = { size, size, size };
	plane2.getComponent<TransformComponent>().scale = { size, size, size };
#endif
}


void SandboxLayer::onEvent( Kestrel::Event& e ){
	Kestrel::EventDispatcher d{ e };

	d.dispatch<Kestrel::KeyPushEvent>( [this]( Kestrel::KeyPushEvent& e ){
			KST_INFO( "Key {} pushed", e.getKeyName() );
			KST_INFO( "{}", e.scancode );
			// F1
			static bool cursor_visible = true;
			if( e.scancode == 67 ){
				if( cursor_visible )
					Kestrel::Application::getInstance()->setCursorMode( Kestrel::CursorMode::Disabled );
				else
					Kestrel::Application::getInstance()->setCursorMode( Kestrel::CursorMode::Normal );
				cursor_visible = !cursor_visible;
			} else if( e.scancode == 68 ){
				camera->updateRenderMode( camera->camera_render_mode ^ RenderModeFlags::eWireframe );
			}

			return true;
		});

	d.dispatch<Kestrel::KeyReleaseEvent>( []( Kestrel::KeyReleaseEvent& e ){
			KST_INFO( "Key {} released", e.getKeyName() );
			return true;
		});

	d.dispatch<Kestrel::MousePressedEvent>( []( Kestrel::MousePressedEvent& e ){
			KST_INFO( "Key {} pressed", e.getButtonName() );
			return true;
		});

	d.dispatch<Kestrel::MouseReleasedEvent>( []( Kestrel::MouseReleasedEvent& e ){
			KST_INFO( "Key {} released", e.getButtonName() );
			return true;
		});

	d.dispatch<Kestrel::MouseMovedEvent>( [&]( Kestrel::MouseMovedEvent& e ){
			//KST_INFO( "Mouse moved to {}, {}", e.x, e.y );
#ifdef SB_KST_TERRAIN
#ifdef SB_MANUAL
			const double rot_speed = 0.01;
			static double last_x = e.x;
			static double last_y = e.y;
			camera->rotate_global( glm::quat( glm::vec3{ 0, ( e.x - last_x ) * rot_speed, 0 }));
			camera->rotate( glm::quat( glm::vec3{ ( e.y - last_y ) * rot_speed, 0, 0 }));
			last_x = e.x;
			last_y = e.y;
#endif
#endif
			return true;
		});

	d.dispatch<Kestrel::MouseScrollEvent>( []( Kestrel::MouseScrollEvent& e ){
			KST_INFO( "Mouse scrolled {}, {}", e.x, e.y );
			return true;
		});

	d.dispatch<Kestrel::WindowResizeEvent>( []( Kestrel::WindowResizeEvent& e ){
			auto cams = Application::getInstance()->current_scene->getView<CameraComponent>();
			for( auto& i: cams ){
				auto cam = cams.get( i );

				cam.camera->onSizeChange( e );
			}

			return false;
		});
}

