#version 460 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 texturePosition;
layout(location = 3) in int textureIndex;
layout(location = 4) in float flyAngle;
layout(location = 5) in float flyOrientation;
layout(location = 6) in vec2 center;


out vec4 vVertexColor;
out highp vec2 vTexturePosition;
flat out int vTextureIndex;


uniform mat4 uModelMatrix;
uniform mat4 uProjectionMatrix;
uniform float flyBlockBool;


out gl_PerVertex {
    vec4 gl_Position;
};

void main() {

	vVertexColor = vertexColor;
	vTexturePosition = texturePosition;
	vTextureIndex = textureIndex;



	if (flyBlockBool == 1.0) {
    mat4 final = mat4(1.0);

		// Step 1: Identity matrix
    mat4 model = mat4(1.0);

    // Step 2: Translate to the center
    model = model * mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        center.x, center.y, 0.0, 1.0
    );

    // Step 3: Rotate around the z-axis
	float angil = -0.78;
    float c = cos(flyAngle * flyOrientation);
    float s = sin(flyAngle * flyOrientation);
    mat4 rotation = mat4(
        c,   s,   0.0, 0.0,
        -s,  c,   0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    model = model * rotation;

    // Step 4: Translate back from the center
    model = model * mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        -center.x, -center.y, 0.0, 1.0
    );

        vec4 newPosition = uProjectionMatrix * model * vec4(position, 0.0f, 1.0f);
	    gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);

	} else {
	    vec4 newPosition = uProjectionMatrix * uModelMatrix * vec4(position, 0.0f, 1.0f);
	    gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
    
    }


	
}