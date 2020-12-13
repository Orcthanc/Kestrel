#include "Renderer/CameraModes.hpp"

std::string Kestrel::to_string( RenderModeFlags rmf ){
	std::string res;

	if( any_flag( rmf & RenderModeFlags::eInverse ))
		res += "Inverse db | ";
	if( any_flag( rmf & RenderModeFlags::eLogarithmic ))
		res += "Logarithmic db | ";
	if( any_flag( rmf & RenderModeFlags::eIntegerDepth ))
		res += "Integer db | ";
	if( any_flag( rmf & RenderModeFlags::eWireframe ))
		res += "Wireframe rendering | ";

	if( res.empty() )
		return "{ None }";
	else
		return "{ " + res.substr( 0, res.size() - 3 ) + " }";
}
