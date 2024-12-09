#version 460 core

out vec4 FragColor;

in vec4 vVertexColor;
in vec2 vTexturePosition;
flat in int vTextureIndex;

//uniform sampler2D uTextures[32];
uniform sampler2DArray uTextures;

void main() {


//	vec4 col;
//	int ind;
//if (vTextureIndex == 0) {
//	ind = 0;
//		col = vec4(1.0, 1.0, 1.0, 1.0);
//	}
//else if (vTextureIndex == 1) {
//	ind = 1;
//	col = vec4(0.0, 1.0, 0.0, 1.0);
//}
//else if (vTextureIndex == 2) {
//	ind = 2;
//		col = vec4(1.0, 0.0, 0.0, 1.0);
//}
//
//vec2 safeTexCoords = clamp(vTexturePosition, 0.0, 1.0);
//
//
//FragColor = texture(uTextures[ind], safeTexCoords) * vVertexColor;


	vec3 texCoords = vec3(vTexturePosition, vTextureIndex); // Encode TexIndex as the z-coordinate
    FragColor = texture(uTextures, texCoords) * vVertexColor;
	
	//FragColor = vec4(vVertexColor.r, vVertexColor.g, vVertexColor.b, 1.0f);
}