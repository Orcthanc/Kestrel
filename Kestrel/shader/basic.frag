#version 450
#extension GL_ARB_separate_shader_objects : enable

layout( constant_id = 0 ) const bool logarithmic = false;

layout( location = 0 ) in frag{
	vec3 color;
	float z;
} u_frag;
layout( location = 0 ) out vec4 outColor;

void main() {
	//gl_FragDepth = u_frag.z;
	outColor = vec4( u_frag.color, 1.0 );
}
