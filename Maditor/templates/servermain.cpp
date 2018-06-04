
#include "serverlib.h"
#include "%1"

int main() {

	%3::%2 server("%4", "scripts");
	
	return server.run();

}