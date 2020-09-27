#pragma once

#include <kstpch.hpp>

#include <fstream>
#include <chrono>
#include <thread>
#include <algorithm>

namespace Kestrel {
	struct ProfileEvent {
		long long start, dur;
		const char* name;
		std::thread::id t_id;
	};

	class Profiler {
		public:
			void begin( const std::string& filename = "performance.json" ){
				KST_CORE_INFO( "Started profiling" );
				out_file.open( filename );
				out_file << "[";
			}
			void end(){
				out_file << "]";
				out_file.close();
				KST_CORE_INFO( "Stopped profiling" );
			}
			void write( const ProfileEvent& p ){
				if( !first ){
					out_file << ",";
				}
				first = false;

				std::string name = p.name;
				std::replace( name.begin(), name.end(), '"', '\'' );
				
				out_file << '{'
					<< "\"cat\":\"function\","
					<< "\"dur\":" << p.dur << ','
					<< "\"name\":\"" << name << "\","
					<< "\"ph\":\"X\","
					<< "\"pid\":0,"
					<< "\"tid\":" << p.t_id << ','
					<< "\"ts\":" << p.start
					<< '}';

				out_file.flush();
			}

			static Profiler& Get(){
				static Profiler p{};
				return p;
			}
		private:
			std::ofstream out_file;
			bool first = true;
	};

	struct ScopeTimer {
		ScopeTimer( const char* name ){
			this->name = name;
			start = std::chrono::high_resolution_clock::now();
		};
		~ScopeTimer(){
			auto end = std::chrono::high_resolution_clock::now();

			Profiler::Get().write( {
					std::chrono::time_point_cast<std::chrono::microseconds>( start ).time_since_epoch().count(),
					std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count(),
					name,
					std::this_thread::get_id(),
				});
		}

		private:
			const char* name;
			std::chrono::high_resolution_clock::time_point start;
	};
}

#ifdef KST_PROFILING
	#define PROFILE_SESSION_START( filename ) Profiler::Get().begin( filename )
	#define PROFILE_SESSION_END() Profiler::Get().end()
	#define PROFILE_SCOPE( name ) Kestrel::ScopeTimer timer##__LINE__( name )
	#ifdef __linux__
		#define PROFILE_FUNCTION() PROFILE_SCOPE( __PRETTY_FUNCTION__ )
	#elif defined _WIN32
		#define PROFILE_FUNCTION() PROFILE_SCOPE( __FUNCSIG__ )
	#else
		#define PROFILE_FUNCTION() PROFILE_SCOPE( __func__ )
	#endif
#else
	#define PROFILE_SESSION_START()
	#define PROFILE_SESSION_END()
	#define PROFILE_SCOPE( name )
	#define PROFILE_FUNCTION()
#endif
