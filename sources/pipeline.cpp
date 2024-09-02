#include "pipeline.hpp"

#include <cstdlib>
#include "shader.hpp"

Pipeline::Pipeline()
{

}

Pipeline::~Pipeline()
{

}

void Pipeline::Create()
{
	Shader simpleShader = Shader();
	simpleShader.Create("simple.vert", "simple.frag");
}

void Pipeline::Destroy()
{
	
}