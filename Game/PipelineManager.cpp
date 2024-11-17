#include "PipelineManager.hpp"
#include "IO.hpp"
#include "App.hpp"

PipelineManager::PipelineManager() : app_(nullptr) {

}

PipelineManager::~PipelineManager() {

}

App& PipelineManager::app() {
	if (app_ == nullptr) {
		app_ = &App::getInstance();
	}
	return *app_;
}

GLuint PipelineManager::CompileShader(GLuint shaderType, const std::string& shaderSource) {
	// Create shader
	GLuint shaderObject = glCreateShader(shaderType);
	// Compile shader
	const char* shaderSourceCStr = shaderSource.c_str();
	glShaderSource(shaderObject, 1, &shaderSourceCStr, nullptr);
	glCompileShader(shaderObject);

	int result;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);
	if (result == false) {
		int length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)malloc(length * sizeof(char));
		glGetShaderInfoLog(shaderObject, length, &length, message);
		std::cout << "Failed to compile " << (shaderType) << "!\n" << (message) << std::endl;
		free(message);
		glDeleteShader(shaderObject);
		return 0;
	}

	return shaderObject;
}

GLuint PipelineManager::CreateShaderProgram(const std::string& vShaderSource, const std::string& fShaderSource) {
	GLuint programObject = glCreateProgram();

	// Create vertex shader
	GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vShaderSource);
	GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fShaderSource);

	// Attach shaders
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	glLinkProgram(programObject);

	// Validate our program
	glValidateProgram(programObject);

	// Delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return programObject;

}

void PipelineManager::CreateGraphicsPipeline() {
	// Create graphics pipeline (this is done on GPU)

	std::string VertexShaderSource = ReadFileAsString("shaders/vertex.glsl");
	std::string FragmentShaderSource = ReadFileAsString("shaders/fragment.glsl");
	app().mGraphicsPipelineShaderProgram = CreateShaderProgram(VertexShaderSource, FragmentShaderSource);
}