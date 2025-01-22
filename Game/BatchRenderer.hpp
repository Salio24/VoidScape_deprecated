#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>
#include <array>
#include <iostream>
#include "glm/gtx/string_cast.hpp"
#include <glm/mat4x4.hpp>
#include "ShaderProgram.hpp"
#include "HelperStructs.hpp"

// usage On StartUp BatchRenderer::Startup -> in update 1st, BatchRenderer::BeginBatch, 2nd, BatchRenderer::Draw,
// 3rd BatchRenderer::EndBatch, 4th BatchRenderer::Flush, On ShutDown BatchRenderer::Shutdown
class App;

class BatchRenderer {
public:
	BatchRenderer();

	~BatchRenderer();

	void StartUp(ShaderProgram* program, GLuint& PipelineProgramID);
	void ShutDown();

	void BeginBatch(const glm::mat4& ProjectionMatrix);
	void EndBatch();
	void Flush(const glm::mat4& ModelMatrix = glm::mat4(1.0f));

	void DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::vec4& color, const glm::mat4& ProjectionMatrix, const glm::mat4& ModelMatrix = glm::mat4(1.0f));
	void DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::mat4& ProjectionMatrix, uint32_t textureID, const glm::vec2& textureSize = glm::vec2(1.0f, 1.0f), const glm::vec2& texturePosition = glm::vec2(0.0f, 0.0f), const glm::mat4& ModelMatrix = glm::mat4(1.0f), const bool& drawFliped = false);

	void DrawInBatch(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

	void DrawInBatch(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec2& textureSize = glm::vec2(1.0f, 1.0f), const glm::vec2& texturePosition = glm::vec2(0.0f, 0.0f), const bool& drawFliped = false, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	void Test();

	void DrawInBatch(const glm::vec2& position, const glm::vec2& size, uint32_t textureID, const glm::vec2& textureSize, const glm::vec2& texturePosition, float flyAngle, float flyOrientation);

	void FlushFly();

private:
	App* app_;

	App& app();

	uint32_t indexCount{ 0 };

	Box* QuadBuffer{ nullptr };
	Box* QuadBufferPtr{ nullptr };

	glm::mat4 currentProjectionMatrix;

	ShaderProgram* currentProgram;
	GLuint currentPipelineProgramID;

	GLuint VAO{ 0 };
	GLuint VBO{ 0 };
	GLuint IBO{ 0 };

	static const size_t maxQuadCount{ 1000 };
	static const size_t maxVertexCount{ maxQuadCount * 4 };
	static const size_t maxIndexCount{ maxQuadCount * 6 };

	//deprecated
	void UniformVariableLinkageAndPopulatingWithMatrix(const GLchar* uniformLocation, glm::mat4 matrix, const GLuint& PipelineProgram);
};