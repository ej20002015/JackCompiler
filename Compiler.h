#pragma once

#include <vector>
#include <string>

namespace JackCompiler
{
	class Compiler
	{
	public:
		int run(int argc, char** argv);

	private:
		void compileFile(std::string& filePath);
		std::vector<std::string> filePaths;
	};
}
