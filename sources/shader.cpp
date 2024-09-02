#include "shader.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "utilities.hpp"

Shader::Shader()
{

}

Shader::~Shader()
{

}

VkShaderModule Shader::CreateShaderModule(const std::vector<char> &code)
{
	return nullptr;
}

void Shader::Create(std::string vertexPath, std::string fragmentPath)
{
	std::string currentPath = Utilities::GetPath();

	/*
	std::string currentPath = Utilities::GetPath();
	std::string shaderString = Utilities::FileToString((currentPath + "/shaders/" + vertexPath).c_str());
	const char *shaderSource = shaderString.c_str();
	const char *const *shaderPointer =  &shaderSource;
	int length = shaderString.size();

	glslang::TShader vertexShader(EShLanguage::EShLangVertex);

	vertexShader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetVulkan_1_3);
	vertexShader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3);
	vertexShader.setStringsWithLengths(shaderPointer, &length, 1);
	vertexShader.setSourceEntryPoint("main");
	vertexShader.setEntryPoint("main");
	*/

	system(("_deps/glslang-build/StandAlone/glslang -V ../shaders/" + vertexPath + " -o ../shaders/" + vertexPath + ".spv").c_str());
	system(("_deps/glslang-build/StandAlone/glslang -V ../shaders/" + fragmentPath + " -o ../shaders/" + fragmentPath + ".spv").c_str());

	auto vertexCode = Utilities::FileToBinary((currentPath + "/shaders/" + vertexPath + ".spv").c_str());
	auto fragmentCode = Utilities::FileToBinary((currentPath + "/shaders/" + fragmentPath + ".spv").c_str());
}