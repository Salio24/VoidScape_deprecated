#version 460 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 texturePosition;
layout(location = 3) in int textureIndex;

out vec4 vVertexColor;
out highp vec2 vTexturePosition;
flat out int vTextureIndex;


uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;

void main() {

	vVertexColor = vertexColor;
	vTexturePosition = texturePosition;
	vTextureIndex = textureIndex;

	vec4 newPosition = uProjectionMatrix * uModelMatrix * vec4(position, 0.0f, 1.0f);
	gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
	
}