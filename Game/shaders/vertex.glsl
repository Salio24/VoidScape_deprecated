#version 460 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 vertexColor;

out vec3 VertexColorOut;

uniform mat4 uModelMatrix;

uniform mat4 uProjectionMatrix;

uniform mat4 uViewMatrix;

void main() {

	VertexColorOut = vertexColor;

	vec4 newPosition = uProjectionMatrix * uModelMatrix * vec4(position, 0.0f, 1.0f);
	gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
	
}