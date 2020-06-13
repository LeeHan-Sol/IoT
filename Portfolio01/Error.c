#include "Error.h"

void error_handling(const char * str)
{
	fputs(str, stderr);
	fputc('\n', stderr);

	exit(EXIT_FAILURE);
}
