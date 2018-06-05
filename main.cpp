#include <iostream>
#include <string>

#include "bf.hpp"

int main(int argc, char* argv[]){
	int arch = 1; // default 8 bits
	for(int i=1; i<argc; i++){
		std::string arg = argv[i];
		if(arg=="-8")
			arch = 1;
		if(arg=="-16")
			arch = 2;
		if(arg=="-32")
			arch = 4;
		if(arg=="-64")
			arch = 8;
	}
	try{
		BFEngine *engine = new BFEngine(arch, 8); // 256 cells per block
		std::string str(std::istreambuf_iterator<char>(std::cin), {});
		engine->process(str.c_str(), str.length());
		delete engine;
	}catch(std::logic_error e){
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
