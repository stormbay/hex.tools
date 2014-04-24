#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>

#include "debug.h"
#include "defines.h"
#include "common.h"


struct swap_options
{
	unsigned int bitswap;		// little-endian 《==》 big-endian
	unsigned int byteswap;		// 2 - 16-bit byte swap;
								// 4 - 32-bit byte swap
								// others - invalid;

	char srcfilepath[FILE_NAME_PATH_LENGTH];
	char dstfilepath[FILE_NAME_PATH_LENGTH];
};

const static char *short_options = "bB:i:h";
const static struct option long_options[] =
{
	{ "bitswap",    0, NULL, 'b' },
	{ "byteswap",   1, NULL, 'B' },
	{ "inputfile",  1, NULL, 'i' },

	{ "help",  0, NULL, 'h' },

	{ 0, 0, 0, 0 },
};


static void
swap_usage( void )
{
	printf("[%s] command format :\n", SYMLINK_NAME_HEXSWAP );
	printf( "\t" SYMLINK_NAME_HEXSWAP " [-b] [-B bytes] [-i infputfile]" "\n" );
}

static int
swap_opt_parser( int argc, char *argv[], struct swap_options *opt )
{
	int loop = 1, c = 0, ret = 0;

	DBG_ENTER();

	while( loop )
	{
		c = getopt_long( argc, argv, short_options, long_options, NULL );
		switch( c )
		{
		case 'b' :
			if( opt->byteswap )
			{
				DBG_ERROR( "bitswap and byteswap can NOT be processed simultaneously.\n" );
				ret = -EINVAL;
				goto parser_exit;
			}
			opt->bitswap = 1;
			DBG_PRINT( "option [PARSED]: bitswap mode [ON].\n" );
			break;

		case 'B' :
			if( opt->bitswap )
			{
				DBG_ERROR( "bitswap and byteswap can NOT be processed simultaneously.\n" );
				ret = -EINVAL;
				goto parser_exit;
			}
			sscanf( optarg, "%d", &( opt->byteswap ));
			if(( opt->byteswap != 2 ) && ( opt->byteswap != 4 ))
			{
				DBG_ERROR( "byteswap only support '2' or '4' bytes.\n" );
				ret = -EINVAL;
				goto parser_exit;
			}
			DBG_PRINT( "option [PARSED]: opt->byteswap is '%d'\n", opt->byteswap );
			break;

		case 'i' :
			strncpy( opt->srcfilepath, optarg, FILE_NAME_PATH_LENGTH );
			DBG_PRINT( "option [PARSED]: opt->srcfilepath is '%s'\n", opt->srcfilepath );
			break;

		case 'h' :
			swap_usage();
		default :
			loop = 0;
			break;
		}
	}

	if(( opt->bitswap == 0 ) && ( opt->byteswap == 0 ))
	{
		DBG_ERROR( "bitswap or byteswap, MUST choose one.\n" );
		ret = -EINVAL;
		goto parser_exit;
	}

	if(access( opt->srcfilepath, R_OK ))
	{
		DBG_ERROR( "inputfile '%s' is NOT readable.\n", opt->srcfilepath );
		ret = -EACCES;
		goto parser_exit;
	}

	strncpy( opt->dstfilepath, opt->srcfilepath, FILE_NAME_PATH_LENGTH );
	strcat( opt->dstfilepath, DEFAULT_OUTPUT_SUFFIX );

parser_exit :

	DBG_LEAVE();

	return ret;
}

static int
swap_bits( struct swap_options *opts )
{
	struct stat fstat;
	char *pbuf = NULL;
	FILE *fin = NULL, *fout = NULL;
	unsigned char tmpdata1 = 0, tmpdata2 = 0;
	unsigned int filesize = 0, totalsize = 0;
	int ret = 0, i = 0, j = 0, rsize = 0, wsize = 0;

	DBG_ENTER();

	ret = stat( opts->srcfilepath, &fstat );
	if( ret )
	{
		DBG_ERROR( "Fail to stat file '%s'. (ret = %d)\n", opts->srcfilepath, ret );
		ret = -EACCES;
		goto error_exit;
	}
	filesize = fstat.st_size;

	pbuf = (char *)malloc( PROCESS_BLOCK_SIZE );
	if( !pbuf )
	{
		DBG_ERROR( "Fail to malloc buffer. (size = %d)\n", PROCESS_BLOCK_SIZE );
		ret = -ENOMEM;
		goto error_exit;
	}

	fin = fopen( opts->srcfilepath, "rb" );
	if( fin == NULL )
	{
		DBG_ERROR( "fail to open input file '%s'.\n", opts->srcfilepath );
		ret = -EACCES;
		goto error_exit;
	}

	fout = fopen( opts->dstfilepath, "wb+" );
	if( fout == NULL )
	{
		DBG_ERROR( "fail to open output file '%s'.\n", opts->dstfilepath );
		ret = -EACCES;
		goto error_exit;
	}

	rewind( fin );

	while( totalsize < filesize )
	{
		rsize = fread( pbuf, 1, PROCESS_BLOCK_SIZE, fin );
		if( rsize <= 0 )
		{
			DBG_ERROR( "Fail to read data. (rdsz=%d, datasz=%d)\n", rsize, PROCESS_BLOCK_SIZE );
			ret = -EIO;
			goto error_exit;
		}

		for( i = 0; i < rsize; i++ )
		{
			tmpdata1 = pbuf[i];
			tmpdata2 = 0;
			for( j = 0; j < 8; j++ )
			{
				if( tmpdata1 & ( 0x01 << j ))
					tmpdata2 |= ( 0x80 >> j );
			}
			pbuf[i] = tmpdata2;
		}

		wsize = fwrite( pbuf, 1, rsize, fout );
		if( wsize != rsize )
		{
			DBG_ERROR( "Fail to read data. (rdsz=%d, datasz=%d)\n", wsize, rsize );
			ret = -EIO;
			goto error_exit;
		}

		totalsize += wsize;
	}

error_exit :

	if( pbuf )
		free( pbuf );

	if( fin )
		fclose( fin );

	if( fout )
		fclose( fout );

	DBG_LEAVE();

	return ret;
}

static int
swap_bytes( struct swap_options *opts )
{
	struct stat fstat;
	char *pbuf = NULL;
	FILE *fin = NULL, *fout = NULL;
	unsigned char tmpdata = 0;
	int ret = 0, rsize = 0, wsize = 0;
	int i = 0, j = 0, cnt = 0, step = 0;
	unsigned int filesize = 0, totalsize = 0;

	DBG_ENTER();

	ret = stat( opts->srcfilepath, &fstat );
	if( ret )
	{
		DBG_ERROR( "Fail to stat file '%s'. (ret = %d)\n", opts->srcfilepath, ret );
		ret = -EACCES;
		goto error_exit;
	}
	filesize = fstat.st_size;

	pbuf = (char *)malloc( PROCESS_BLOCK_SIZE );
	if( !pbuf )
	{
		DBG_ERROR( "Fail to malloc buffer. (size = %d)\n", PROCESS_BLOCK_SIZE );
		ret = -ENOMEM;
		goto error_exit;
	}

	fin = fopen( opts->srcfilepath, "rb" );
	if( fin == NULL )
	{
		DBG_ERROR( "fail to open input file '%s'.\n", opts->srcfilepath );
		ret = -EACCES;
		goto error_exit;
	}

	fout = fopen( opts->dstfilepath, "wb+" );
	if( fout == NULL )
	{
		DBG_ERROR( "fail to open output file '%s'.\n", opts->dstfilepath );
		ret = -EACCES;
		goto error_exit;
	}

	rewind( fin );

	step = opts->byteswap;
	cnt  = step >> 1;

	while( totalsize < filesize )
	{
		rsize = fread( pbuf, 1, PROCESS_BLOCK_SIZE, fin );
		if( rsize <= 0 )
		{
			DBG_ERROR( "Fail to read data. (rdsz=%d, datasz=%d)\n", rsize, PROCESS_BLOCK_SIZE );
			ret = -EIO;
			goto error_exit;
		}

		for( i = 0; i < rsize; i += step )
		{
			for( j = 0; j < cnt; j++ )
			{
				tmpdata = pbuf[i + j];
				pbuf[i + j] = pbuf[i + (step - 1) - j];
				pbuf[i + (step - 1) - j] = tmpdata;
			}
		}

		wsize = fwrite( pbuf, 1, rsize, fout );
		if( wsize != rsize )
		{
			DBG_ERROR( "Fail to read data. (rdsz=%d, datasz=%d)\n", wsize, rsize );
			ret = -EIO;
			goto error_exit;
		}

		totalsize += wsize;
	}

error_exit :

	if( pbuf )
		free( pbuf );

	if( fin )
		fclose( fin );

	if( fout )
		fclose( fout );

	DBG_LEAVE();

	return ret;
}

int swap_main( int argc, char *argv[] )
{
	int ret = 0;
	struct swap_options options;

	DBG_ENTER();

	memset( &options, 0, sizeof(struct swap_options));

	ret = swap_opt_parser( argc, argv, &options );
	if( ret )
	{
		DBG_ERROR( "Failed to parse command line options. (ret = %d)\n", ret );
		swap_usage();
		goto swap_main_exit;
	}

	if( options.bitswap )
	{
		ret = swap_bits( &options );
		if( ret )
		{
			DBG_ERROR( "Failed to swap bits. (ret = %d)\n", ret );
			goto swap_main_exit;
		}
	}
	else if( options.byteswap )
	{
		ret = swap_bytes( &options );
		if( ret )
		{
			DBG_ERROR( "Failed to swap %d bytes. (ret = %d)\n", options.byteswap, ret );
			goto swap_main_exit;
		}
	}
	else
	{
		DBG_ERROR( "Nothing to do.\n" );
		swap_usage();
		ret = -EINVAL;
	}

swap_main_exit :

	DBG_LEAVE();

	return ret;
}
