#version 450

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec2 texture;
layout( location = 3 ) in vec2 offset;
layout( location = 4 ) in float resolution;

layout( location = 0 ) out VertData {
	vec3 position;
	vec3 fragColor;
} outdata;

layout( push_constant ) uniform push_constants {
	layout( offset = 80 )vec2 off;
} u_push_constants;

void main() {
	vec3 npos = position;
	npos.x += offset.x + u_push_constants.off.x;
	npos.y += offset.y + u_push_constants.off.y;
	outdata.position = npos;
	if( resolution == 0 ){
		outdata.fragColor = vec3( 1, 0, 0 );
	} else if( resolution == 1 ){
		outdata.fragColor = vec3( 0, 1, 0 );
	} else {
		outdata.fragColor = vec3( 0, 0, 1 );
	}
}
