/*
**	$VER: XtruderBBDMS.c (1.8.96)
**
**	Xtruder DMS bootblock support functions
**
**	Copyright (C) 1993-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Retrive the bootblock of a DMS archive.
 */
BOOL xReadDMSBootblock( STRPTR filename, BOOL verbose )
{
	BOOL	success = FALSE;
	
	
	UInfo->ui_Flag &= ~UFN_NoLoad;
	
	if( upDetermineFile( UInfo, filename ) )
		{
			/// Mask out encryption bit
		
		UInfo->ui_CrunchType &= 0x7F;
		
			/// Must be DMS and Track
		
		if( (UInfo->ui_CrunchType == CRU_Track) && (UInfo->ui_CrunchNum == CN_Dms) )
			{
			UInfo->ui_TrackJump = (void (*)(void *, void *))xGetDMSBootblockJumpRoutine;
			
			upUnpack( UInfo );
			
			if( UInfo->ui_ErrorNum == UERR_DevErr )
				success = TRUE;
			}
		}
	
	if( !success && verbose )
		xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_BB_FILEISNOTADMSARCHIVE ), GCS( MSG_GF_OKAY ), FilePart( filename ) );
	
	UInfo->ui_Flag |= UFN_NoLoad;
	
	return( success );
}

/*
 * Jump function for one purpose. To get the bootblock of a DMS file
 */
BOOL __saveds xGetDMSBootblockJumpRoutine( VOID )
{
	if( UInfo->ui_Offset != -1)          // Test to see if it's a banner
		{
		if( UInfo->ui_DecrunchLen >= 1024 )
			CopyMem( UInfo->ui_DecrunchAdr, XStrings->xs_Bootblock, 1024 );
		
			/// Return an error to make it stop
		
		return( 1 );
		}
	
		/// Skip the banners
	
	return( 0 );
}
