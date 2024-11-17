#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>
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

	void StartUp();
	void ShutDown();

	void BeginBatch();
	void EndBatch();
	void Flush(const glm::mat4 ModelMatrix = glm::mat4(1.0f));

	void DrawSeperatly(const glm::vec2& position, glm::vec2 size, const glm::vec3& color, const glm::mat4& ModelMatrix = glm::mat4(1.0f));

	void DrawInBatch(const glm::vec2& position, glm::vec2 size, const glm::vec3& color);

	GLuint mVAO{ 0 };
	GLuint mVBO{ 0 };
	GLuint mIBO{ 0 };

	uint32_t IndexCount{ 0 };

	Box* QuadBuffer{ nullptr };
	Box* QuadBufferPtr{ nullptr };

	void UniformVariableLinkageAndPopulatingWithMatrix(const GLchar* uniformLocation, glm::mat4 matrix);

private:
	App* app_;

	App& app();
};