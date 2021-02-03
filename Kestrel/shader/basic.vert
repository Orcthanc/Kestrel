#version 450

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 color;

layout( location = 0 ) out VertData {
	vec3 position;
	vec3 fragColor;
} outdata;

void main() {
	outdata.position = position;
	outdata.fragColor = color;
}
