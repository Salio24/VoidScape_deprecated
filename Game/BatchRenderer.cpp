#include "BatchRenderer.hpp"
#include "App.hpp"

static const size_t MaxQuadCount = 1000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount = MaxQuadCount * 6;

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
void BatchRenderer::StartUp() {
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
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Box), (const void*)offsetof(Box, Color));

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
}

void BatchRenderer::ShutDown() {
	glDeleteVertexArrays(1, &mVAO); 
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mIBO);

	delete[] QuadBuffer;
}

void BatchRenderer::BeginBatch() {
	QuadBufferPtr = QuadBuffer;
}

void BatchRenderer::EndBatch() {
	GLsizeiptr size = (uint8_t*)QuadBufferPtr - (uint8_t*)QuadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, QuadBuffer);
}

void BatchRenderer::DrawInBatch(const glm::vec2& position, glm::vec2 size, const glm::vec3& color) {
	if (IndexCount >= MaxIndexCount) {
		EndBatch();
		Flush();
		BeginBatch();
	}

	QuadBufferPtr->Position = {position.x, position.y};
	QuadBufferPtr->Color = color;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y };
	QuadBufferPtr->Color = color;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x + size.x, position.y + size.y};
	QuadBufferPtr->Color = color;
	QuadBufferPtr++;

	QuadBufferPtr->Position = { position.x, position.y + size.y };
	QuadBufferPtr->Color = color;
	QuadBufferPtr++;

	IndexCount += 6;
}

void BatchRenderer::DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::vec3& color, const glm::mat4& ModelMatrix) {
	BeginBatch();
	DrawInBatch(position, size, color);
	EndBatch();
	Flush(ModelMatrix);
}

void BatchRenderer::Flush(const glm::mat4 ModelMatrix) {
	//glUseProgram(app().mGraphicsPipelineShaderProgram);

	UniformVariableLinkageAndPopulatingWithMatrix("uModelMatrix", ModelMatrix);
	UniformVariableLinkageAndPopulatingWithMatrix("uProjectionMatrix", app().mCamera.GetProjectionMatrix());
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, nullptr);
	IndexCount = 0;
	//glUseProgram(0);
}

void BatchRenderer::UniformVariableLinkageAndPopulatingWithMatrix(const GLchar* uniformLocation, glm::mat4 matrix) {
	GLint MatrixLocation = glGetUniformLocation(app().mGraphicsPipelineShaderProgram, uniformLocation);
	if (MatrixLocation >= 0) {
		glUniformMatrix4fv(MatrixLocation, 1, GL_FALSE, &matrix[0][0]);
	}
	else {
		std::cerr << "Failed to get uniform location of: " << uniformLocation << std::endl << "Exiting now" << std::endl;
		exit(EXIT_FAILURE);
	}
}