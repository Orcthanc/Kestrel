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

void main() {
	outdata[gl_InvocationID].position = indata[gl_InvocationID].position;
	outdata[gl_InvocationID].color = indata[gl_InvocationID].color;

	if( gl_InvocationID == 0 ){
		gl_TessLevelOuter[0] = 3.0;
		gl_TessLevelOuter[1] = 3.0;
		gl_TessLevelOuter[2] = 3.0;
		gl_TessLevelInner[0] = 3.0;
	}
}
