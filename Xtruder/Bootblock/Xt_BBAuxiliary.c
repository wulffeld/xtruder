/*
**	$VER: XtruderBBAuxiliary.c (1.8.96)
**
**	Xtruder miscellaneous bootblock auxiliary functions
**
**	Copyright (C) 1993-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Read from a disk
 */
BOOL xReadBoot( WORD unit )
{
	ULONG	ret;
	
	
	if( ret = ReadBoot( unit, XStrings->xs_Bootblock ) )
		{
		xTDError( ret );
		
		return( FALSE );
		}
	
	return( TRUE );
}

BOOL xReadBBBrainFile( VOID )
{
	UBYTE	strbuf[ 100 ];
	
	
	switch( ReadBBBF( "L:Bootblock.brainfile" ) )
		{
		case BBBF_ALREADY_LOADED:
			GetBBBFInfo( &Virus, strbuf );
			break;
		
		case BBBF_LOADED:
			GetBBBFInfo( &Virus, strbuf );
			break;
		
		default:
			return( FALSE );
			break;
		}
	
	return( TRUE );
}
