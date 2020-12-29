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
	mat4 model;
} u_push;

layout( binding = 0 ) uniform vp {
	mat4 view;
	mat4 projection;
	mat4 view_projection;
} u_vp;

void main() {
	outdata[gl_InvocationID].position = indata[gl_InvocationID].position;
	outdata[gl_InvocationID].color = indata[gl_InvocationID].color;

	if( gl_InvocationID == 0 ){
		float distances[3];

		for( int i = 0; i < 3; i++ ){
			vec4 pos = u_vp.view * u_push.model * vec4( indata[i].position, 1.0 );
			distances[i] = dot( pos, pos );
		}

		float tess_facs[3];

		tess_facs[0] = min( distances[1], distances[2] ) / 100000000.0;
		tess_facs[1] = min( distances[0], distances[2] ) / 100000000.0;
		tess_facs[2] = min( distances[0], distances[1] ) / 100000000.0;

		gl_TessLevelOuter[0] = max(1, 32 - tess_facs[0]);
		gl_TessLevelOuter[1] = max(1, 32 - tess_facs[1]);
		gl_TessLevelOuter[2] = max(1, 32 - tess_facs[2]);
		gl_TessLevelInner[0] = max(1, 32 - tess_facs[2]);
	}
}
