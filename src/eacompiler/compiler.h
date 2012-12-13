#ifndef _ADRENO_EA_COMPILER_H
#define _ADRENO_EA_COMPILER_H

#include <iostream>

class AdrenoEACompiler {
public:
	AdrenoEACompiler() {}
	~AdrenoEACompiler() {}

	bool Compile( std::istream& in, std::ostream& out );

private:
};

#endif

