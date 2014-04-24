#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "debug.h"
#include "defines.h"
#include "common.h"



static void
usage( void )
{
	printf( "Please use symbol link command :\n" );
	printf( "\t" SYMLINK_NAME_HEXSWAP		"\n"	\
		  );
}

int main( int argc, char *argv[] )
{
	int ret = 0;
	char *pstr = NULL;

	DBG_ENTER();

	pstr = strrchr( argv[0], '/' );
	if( !pstr )
		pstr = argv[0];
	else
		pstr += 1;

	if(!strcmp( pstr, SYMLINK_NAME_HEXSWAP ))
	{
		ret = swap_main( argc, argv );
	}
	else
	{
		usage();
	}

	DBG_LEAVE();

	return ret;
}
