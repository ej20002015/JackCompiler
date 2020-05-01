// JackCompiler.cpp : Defines the entry point for the application.
//

#include "Compiler.h"
#include <fstream>

using namespace std;

int main(int argc, char** argv)
{
  //Create an object of the compiler class and run it, passing in the terminal arguments
	JackCompiler::Compiler compiler;
	return compiler.run(argc, argv);
}