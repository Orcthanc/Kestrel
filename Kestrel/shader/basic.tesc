#version 450

layout( vertices = 3 ) out;

layout( location = 0 ) in VertData {
	vec3 position;
	vec3 color;
} indata[];

layout( location = 0 ) out VertData {
	vec3 position;
	vec3 color;
} outdata[];

layout( push_constant ) uniform push_constants {
	float tess_level;
} u_push;

void main() {
	outdata[gl_InvocationID].position = indata[gl_InvocationID].position;
	outdata[gl_InvocationID].color = indata[gl_InvocationID].color;

	if( gl_InvocationID == 0 ){
		gl_TessLevelOuter[0] = u_push.tess_level;
		gl_TessLevelOuter[1] = u_push.tess_level;
		gl_TessLevelOuter[2] = u_push.tess_level;
		gl_TessLevelInner[0] = u_push.tess_level;
	}
}
