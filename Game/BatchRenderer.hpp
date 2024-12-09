#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>
#include <array>
#include <iostream>
#include "glm/gtx/string_cast.hpp"
#include <glm/mat4x4.hpp>
#include "HelperStructs.hpp"

// usage On StartUp BatchRenderer::Startup -> in update 1st, BatchRenderer::BeginBatch, 2nd, BatchRenderer::Draw,
// 3rd BatchRenderer::EndBatch, 4th BatchRenderer::Flush, On ShutDown BatchRenderer::Shutdown
class App;

struct BatchRenderer {

	BatchRenderer();

	~BatchRenderer();

	void StartUp(const GLuint& PipelineProgram);
	void ShutDown();

	void BeginBatch(const glm::mat4& ProjectionMatrix);
	void EndBatch();
	void Flush(const glm::mat4 ModelMatrix = glm::mat4(1.0f));

	void DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::vec4& color, const glm::mat4& ProjectionMatrix, const glm::mat4& ModelMatrix = glm::mat4(1.0f));
	void DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::mat4& ProjectionMatrix, uint32_t textureID, const glm::vec2& textureSize = glm::vec2(1.0f, 1.0f), const glm::vec2& texturePosition = glm::vec2(0.0f, 0.0f), const glm::mat4& ModelMatrix = glm::mat4(1.0f), const bool& drawFliped = false);

	void DrawInBatch(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

	void DrawInBatch(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec2& textureSize = glm::vec2(1.0f, 1.0f), const glm::vec2& texturePosition = glm::vec2(0.0f, 0.0f), const bool& drawFliped = false);

	static const size_t MaxQuadCount{ 1000 };
	static const size_t MaxVertexCount{ MaxQuadCount * 4 };
	static const size_t MaxIndexCount{ MaxQuadCount * 6 };

	GLuint mVAO{ 0 };
	GLuint mVBO{ 0 };
	GLuint mIBO{ 0 };

	uint32_t IndexCount{ 0 };

	Box* QuadBuffer{ nullptr };
	Box* QuadBufferPtr{ nullptr };

	glm::mat4 CurrentProjectionMatrix;

	GLuint CurrentPipeline;

	void UniformVariableLinkageAndPopulatingWithMatrix(const GLchar* uniformLocation, glm::mat4 matrix, const GLuint& PipelineProgram);

private:
	App* app_;

	App& app();
};