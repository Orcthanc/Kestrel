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

MeshComponent::MeshComponent( const std::shared_ptr<Mesh>& mesh ): mesh( mesh ){}

MeshComponent::MeshComponent( std::shared_ptr<Mesh>&& mesh ): mesh( std::move( mesh )){}

MeshComponent::operator const Kestrel::Mesh& (){
	return *mesh;
}

MaterialComponent::MaterialComponent( const Material& mat ): mat( mat ){}

MaterialComponent::MaterialComponent( Material&& mat ): mat( std::move( mat )){}

MaterialComponent::operator const Material&() const {
	return mat;
}
