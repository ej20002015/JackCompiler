// JackCompiler.cpp : Defines the entry point for the application.
//

#include "Compiler.h"
#include <fstream>

using namespace std;

int main(int argc, char** argv)
{
	JackCompiler::Compiler compiler;
	return compiler.run(argc, argv);
}