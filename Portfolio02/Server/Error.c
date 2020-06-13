#include "Error.h"

void error_handling(const char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);

	exit(1);
}
