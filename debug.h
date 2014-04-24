#ifndef	__HEXTOOLS_DEBUG_H__
#define	__HEXTOOLS_DEBUG_H__


//#define	HEXTOOLS_DEBUG

#ifdef HEXTOOLS_DEBUG

#define	DBG_ENTER()							printf( "ENTER %s()\n", __FUNCTION__ )
#define	DBG_LEAVE()							printf( "LEAVE %s()\n", __FUNCTION__ )

#define	DBG_PRINT( fmt... )										\
	do	{														\
			printf( "[%s] - %d: ", __FUNCTION__, __LINE__ );	\
			printf( fmt );										\
		} while( 0 )

#define	DBG_ERROR( fmt... )										\
	do	{														\
			printf( "[%s] - %d: ", __FUNCTION__, __LINE__ );	\
			printf( "* ERROR * " );								\
			printf( fmt );										\
		} while( 0 )

#else	// HEXTOOLS_DEBUG

#define	DBG_ENTER()
#define	DBG_LEAVE()
#define	DBG_PRINT( fmt... )

#define	DBG_ERROR( fmt... )										\
	do	{														\
			printf( "* ERROR * " );								\
			printf( fmt );										\
		} while( 0 )

#endif	// HEXTOOLS_DEBUG

#define	DBG_DUMP( fmt... )										\
	do	{														\
			printf( "\t" );										\
			printf( fmt );										\
		} while( 0 )


#endif	// __HEXTOOLS_DEBUG_H__
