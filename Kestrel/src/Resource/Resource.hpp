#pragma once

#include <kstpch.hpp>

#include <compare>
#include <filesystem>
#include <map>


namespace Kestrel {

	struct NoSharedResources {
		//Empty
	};

	template <
			typename Resource,
			typename Handle,
			typename SharedResources = NoSharedResources,
			typename LocationType = std::filesystem::path>
	struct ResourceRegistry {
		/*
		struct ID {
			uint32_t id;

			auto constexpr operator<=>( const ID& ) const = default;
		};
		*/

		public:
			// Needs to be implemented per instantiation
			template<typename... Args>
			static void initialize( Args... ) = delete;

		private:
			// Needs to be implemented per instantiation
			static std::shared_ptr<Resource> load_resource( const LocationType& loc );

		public:
			static Handle requestResourceHandle( const LocationType& loc ){
				auto it = resource_locations.find( loc );
				if( it != resource_locations.end() )
					return it->second;

				static Handle ID = 0;
				Handle curr_id = ++ID;

				resource_locations.emplace( loc, curr_id );
				resources.emplace( curr_id, load_resource( loc ));
				return curr_id;
			}

			static std::shared_ptr<Resource> requestResource( const Handle& id ){
				return resources.at( id );
			}

			static std::shared_ptr<Resource> requestResource( const LocationType& location ){
				return resources.at( requestResourceHandle( location ));
			}

			static bool exists( const LocationType& loc ){
				return resource_locations.contains( loc );
			}

			static void remove( const LocationType& loc ){
				resources.erase( resource_locations.at( loc ));
				resource_locations.erase( loc );
			}

			static void clear(){
				resources.clear();
				resource_locations.clear();
				shared_resources = {};
			}

			static inline SharedResources shared_resources{};
		private:
			static inline std::map<LocationType, Handle> resource_locations{};
			static inline std::unordered_map<Handle, std::shared_ptr<Resource>> resources{};
	};
}
