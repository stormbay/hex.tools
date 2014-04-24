#ifndef	__HEXTOOLS_DEFINE_H__
#define	__HEXTOOLS_DEFINE_H__


#define	ASCII_LOWER_UPPER_OFFSET			( 'a' - 'A' )

#define	DEFAULT_OUTPUT_SUFFIX				".out"

#define	FILE_NAME_PATH_LENGTH				256
#define	PROCESS_BLOCK_SIZE					( 2 * 1024 )

#define	IS_HEX_STR( str )					((( str[0] == '0' ) && (( str[1] == 'x' ) || ( str[1] == 'X' ))) ? 1 : 0 )


void inline
str_tolower( char *str )
{
	if( !str )
		return;

	while( *str )
	{
		if(( *str >= 'A' ) && ( *str <= 'Z' ))
			*str += ASCII_LOWER_UPPER_OFFSET;
		str++;
	}
}

void inline
str_toupper( char *str )
{
	if( !str )
		return;

	while( *str )
	{
		if(( *str >= 'a' ) && ( *str >= 'z' ))
			*str -= ASCII_LOWER_UPPER_OFFSET;
		str++;
	}
}


#endif	// __HEXTOOLS_DEFINE_H__
