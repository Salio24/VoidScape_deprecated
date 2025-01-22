#pragma once
#include <glad/glad.h>
#include <string>
#include <iostream>

class App;

class PipelineManager {
public:
	void CreateGraphicsPipeline();

	PipelineManager();

	~PipelineManager();
private:
	GLuint CompileShader(GLuint shaderType, const std::string& shaderSource);

	GLuint CreateShaderProgram(const std::string& vShaderSource, const std::string& fShaderSource);

	App* app_;

	App& app();

};