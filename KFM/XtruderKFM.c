#include	<exec/types.h>
#include	<dos/dos.h>
#include	<dos/dosextens.h>
#include	<libraries/reqtools.h>
#include	<own/mw.h>

#include	<string.h>
#include	<stdlib.h>
#include	<ctype.h>

#include	<clib/dos_protos.h>
#include	<clib/exec_protos.h>
#include	<clib/intuition_protos.h>
#include	<clib/reqtools_protos.h>

extern struct ExecBase *	SysBase;
extern struct Library *		DOSBase;

struct Library *			ReqToolsBase;
struct Library *			IntuitionBase;

ULONG				ASM	xaGetRandomNumber( REGD0 ULONG, REGD1 ULONG, REGD2 ULONG );
ULONG					DoChecksum( STRPTR name );

/*
 * First some random numbers eor'ed with random numbers 256 bytes.
 * Then the name of the user.
 * Then some more random numbers...
 * Finally a checksum of the first 500 bytes...
 */
main( UWORD argc, UBYTE *argv[] )
{
	BPTR	file;
	UBYTE	key[512];
	UWORD	a = 0;
	
	
	if( IntuitionBase = OpenLibrary( "intuition.library", 37 ) )
		{
		if( argc == 2 )
			{
			ULONG	len;
			
			
			Printf( "Xtruder Keyfilemaker - Copyright 1997 Martin Wulffeld\nCreating key...\n" );
			
			len = strlen( argv[1] );
			
				/// Make keyfile...
			
			if( file = Open( "RAM:Xtruder.key", MODE_NEWFILE ) )
				{
				UBYTE	q = 1;
				
				
				for( a = 0; a < 256; a++ )
					{
					ULONG	sec,mic;
					
					
					CurrentTime( &sec, &mic );
					
					q = xaGetRandomNumber( a * q, mic, 255 );
					
					key[a] = q;
					}
				
				Write( file, key, 256 );
				
				strcpy( &key[256], argv[1] );
				Write( file, argv[1], len );
				Write( file, "\0", 1 );
				
				for( a = 256 + len + 1; a < 496; a++ )
					{
					ULONG	sec,mic;
					
					
					CurrentTime( &sec, &mic );
					
					q = xaGetRandomNumber( a * q, mic, 255 );
					
					key[a] = q;
					Write( file, &key[a], 1 );
					}
				
				len = DoChecksum( &key[0] );
				
				//Printf( "%ld\n", len);
				
				Write( file, &len, 4 );
				
				Printf( "Keyfile created! Location: RAM:Xtruder.key\n" );
				
				Close( file );
				}
			}
		else
			Printf( "Wrong arguments! - XtruderKFM \"#<number> <name>\"   Example: XtruderKFM \"#1 Martin Wulffeld\"\n" );
		
		/*
		if( file = Open( "RAM:Xtruder.key", MODE_OLDFILE ) )
			{
			ULONG	cs;
			
			
			Read( file, key, 500 );
			
			cs = ((ULONG *)key)[496/4];
			
			if( k = DoChecksum( key ) )
				{
				Printf( "%ld %ld\n",k,cs );
				
				if( k == cs )
					Printf( "Match\n" );
				}
			
			Close( file );
			}
		*/
		
		CloseLibrary( IntuitionBase );
		}
}

ULONG DoChecksum( UBYTE *key )
{
	UWORD	a;
	ULONG	c = 0;
	
	
	for( a = 0; a < 490; a++ )
		c += key[a] * key[a] + 5;
	
	return( c );
}
