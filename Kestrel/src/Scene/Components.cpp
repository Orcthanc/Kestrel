#include "Scene/Components.hpp"

using namespace Kestrel;


TransformComponent::TransformComponent( glm::vec3 loc, glm::vec3 scale ):
	loc( std::move( loc )),
	scale( std::move( scale )){}


TransformComponent::TransformComponent( glm::vec3 loc, glm::quat rot, glm::vec3 scale ):
	loc( std::move( loc )),
	rot( std::move( rot )),
	scale( std::move( scale )){}


NameComponent::NameComponent(): name( "Unnamed" ){}

NameComponent::NameComponent( const char* name ): name( name ){}

NameComponent::NameComponent( const std::string& name ): name( name ){}

NameComponent::operator const std::string&() const {
	return name;
}

MeshComponent::MeshComponent( Mesh mesh ): mesh( mesh ){}

MeshComponent::operator const Kestrel::Mesh& (){
	return mesh;
}

MaterialComponent::MaterialComponent( const Material& mat ): mat( mat ){}

MaterialComponent::MaterialComponent( Material&& mat ): mat( std::move( mat )){}

MaterialComponent::operator const Material&() const {
	return mat;
}

CameraComponent::CameraComponent( const std::shared_ptr<Camera>& camera ): camera( camera ){}

CameraComponent::CameraComponent( std::shared_ptr<Camera>&& camera ): camera( std::move( camera )){}

CameraComponent::operator const Kestrel::Camera&(){
	return *camera;
}


ColorComponent::ColorComponent( const glm::vec3& rgb ): color( rgb ){}

ColorComponent::ColorComponent( glm::vec3&& rgb ): color( std::move( rgb )){}

ColorComponent::operator const glm::vec3&(){
	return color;
}
