#version 460 core

in vec3 VertexColorOut;

out vec4 FragColor;

uniform float uOffsetY;

void main() {
	FragColor = vec4(VertexColorOut.r, VertexColorOut.g, VertexColorOut.b, 1.0f);
	//FragColor = texture(uTexture, texturePositionOut);
}