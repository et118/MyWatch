#pragma once
#include <string>

class Program {
	private:
		std::string name;
	public:
		Program(std::string name) {
			Program::name = name;
		};
		std::string getName();

		virtual int run(Program** programs, int numPrograms)=0;
};