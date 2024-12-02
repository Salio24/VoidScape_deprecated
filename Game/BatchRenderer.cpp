#include "BatchRenderer.hpp"
#include "App.hpp"
#include <cstddef>

// Basically static memory allocation and value init for batch rendering

BatchRenderer::BatchRenderer() : app_(nullptr) {

}

BatchRenderer::~BatchRenderer() {

}

App& BatchRenderer::app()  {
	if (app_ == nullptr) {
		app_ = &App::getInstance();
	}
	return *app_;
}
void BatchRenderer::StartUp(const GLuint& PipelineProgram) {
	if (QuadBuffer != nullptr) {
		std::cout << "BatchRenderer has been initialized twice. ERROR!" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	QuadBuffer = new Box[MaxVertexCount];


	glCreateVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glCreateBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Box), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(mVAO, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Position));

	glEnableVertexArrayAttrib(mVAO, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Color));

	glEnableVertexArrayAttrib(mVAO, 2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, TexturePosition));

	glEnableVertexArrayAttrib(mVAO, 3);
	glVertexAttribPointer(3, 1, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, TextureIndex));

	uint32_t indices[MaxIndexCount];
	uint32_t offset = 0;
	for (int i = 0; i < MaxIndexCount; i += 6) {
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;

		offset += 4;
	}
	glCreateBuffers(1, &mIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	TextureSlots[0] = WhiteTexture;
	for (size_t i = 1; i < MaxTextureSlots; i++) {
		TextureSlots[i] = 0;
	}
}

void BatchRenderer::ShutDown() {
	glDeleteVertexArrays(1, &mVAO); 
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mIBO);

	glDeleteTextures(1, &WhiteTexture);

	delete[] QuadBuffer;
}

void BatchRenderer::BeginBatch(const glm::mat4& ProjectionMatrix, const glm::mat4& test) {
	QuadBufferPtr = QuadBuffer;
	mod = test;
	CurrentProjectionMatrix = ProjectionMatrix;
	glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&CurrentPipeline);
}

void BatchRenderer::EndBatch() {
	GLsizeiptr size = (uint8_t*)QuadBufferPtr - (uint8_t*)QuadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, QuadBuffer);
}

void BatchRenderer::DrawInBatch(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	if (IndexCount >= MaxIndexCount) {
		EndBatch();
		Flush();
		BeginBatch(CurrentProjectionMatrix);
	}

	QuadBufferPtr->Position = {position.x, position.y};
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.0f, 0.0f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y };
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 1.0f, 0.0f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y + size.y};
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 1.0f, 1.0f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x, position.y + size.y };
	QuadBufferPtr->Color = color;
	QuadBufferPtr->TexturePosition = { 0.0f, 1.0f };
	QuadBufferPtr->TextureIndex = 0;
	QuadBufferPtr++;

	IndexCount += 6;
}

void BatchRenderer::DrawInBatch(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec2& textureSize, const glm::vec2& texturePosition, const bool& drawFliped) {
	if (IndexCount >= MaxIndexCount || TextureSlotIndex > 1024) {
		EndBatch();
		Flush();
		BeginBatch(CurrentProjectionMatrix);
	}

	constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	//int CurrentTextureIndex = 0;
	//for (uint32_t i = 1; i < TextureSlotIndex; i++) {
	//	if (TextureSlots[i] == textureID) {
	//		CurrentTextureIndex = i;
	//		break;
	//	}
	//}

	//if (CurrentTextureIndex == 0.0f) {
	//	CurrentTextureIndex = (float)TextureSlotIndex;
	//	TextureSlots[TextureSlotIndex] = textureID;
	//	TextureSlotIndex++;
	//}

	if (drawFliped == true) {
		QuadBufferPtr->Position = { position.x, position.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x + size.x, position.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y + textureSize.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x, position.y + size.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y + textureSize.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;
	}
	else if (drawFliped == false) {
		QuadBufferPtr->Position = { position.x, position.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x + size.x, position.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x + size.x, position.y + size.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x + textureSize.x, texturePosition.y + textureSize.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;

		QuadBufferPtr->Position = { position.x, position.y + size.y };
		QuadBufferPtr->Color = color;
		QuadBufferPtr->TexturePosition = { texturePosition.x, texturePosition.y + textureSize.y };
		QuadBufferPtr->TextureIndex = textureID;
		QuadBufferPtr++;
	}

	IndexCount += 6;
}


void BatchRenderer::DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::vec4& color, const glm::mat4& ProjectionMatrix, const glm::mat4& ModelMatrix) {
	BeginBatch(ProjectionMatrix);
	DrawInBatch(position, size, color);
	EndBatch();
	Flush(ModelMatrix);
}

void BatchRenderer::DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::mat4& ProjectionMatrix, uint32_t textureID, const glm::vec2& textureSize, const glm::vec2& texturePosition, const glm::mat4& ModelMatrix, const bool& drawFliped) {
	BeginBatch(ProjectionMatrix);
	DrawInBatch(position, size, textureID, textureSize, texturePosition, drawFliped);
	EndBatch();
	Flush(ModelMatrix);
}

void BatchRenderer::Flush(const glm::mat4 ModelMatrix) {
	//glBindTexture(GL_TEXTURE_2D_ARRAY, app().textureArray);

	UniformVariableLinkageAndPopulatingWithMatrix("uModelMatrix", ModelMatrix, app().mGraphicsPipelineShaderProgram);
	UniformVariableLinkageAndPopulatingWithMatrix("uProjectionMatrix", app().mCamera.GetProjectionMatrix(), app().mGraphicsPipelineShaderProgram);
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, nullptr);
	IndexCount = 0;

	TextureSlotIndex = 1;

	for (size_t i = 1; i < MaxTextureSlots; i++) {
		TextureSlots[i] = 0;
	}
}

void BatchRenderer::UniformVariableLinkageAndPopulatingWithMatrix(const GLchar* uniformLocation, glm::mat4 matrix, const GLuint& PipelineProgram) {
	GLint MatrixLocation = glGetUniformLocation(PipelineProgram, uniformLocation);
	if (MatrixLocation >= 0) {
		glUniformMatrix4fv(MatrixLocation, 1, GL_FALSE, &matrix[0][0]);
	}
	else {
		std::cerr << "Failed to get uniform location of: " << uniformLocation << std::endl << "Exiting now" << std::endl;
		exit(EXIT_FAILURE);
	}
}