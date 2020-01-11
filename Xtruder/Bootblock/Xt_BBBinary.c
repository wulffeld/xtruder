/*
**	$VER: XtruderBBBinary.c (1.8.96)
**
**	Xtruder save as binary functions
**
**  Copyright (C) 1993-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Save a bootblock to a file from either a disk or a DMS archive
 */
VOID xSaveAsBinary( VOID )
{
	BPTR	file;
	BOOL	overwrite = TRUE;
	UBYTE	strbuf1[ DEFAULTBUFFERLEN ] = "";
	
	
		/// Request save filename
	
	if( !xSelectFile( Window,
		SELFP_ReqTitle,	GCS( MSG_BB_TL_SAVEASBINARY ),
		SELFP_Dir,		RPATH,
		SELFP_DestFile,	strbuf1,
		TAG_END ) )
		{
		return;
		}
	
		/// Check if file already exist and notify user if so
	
	if( xExists( strbuf1 ) )
		overwrite = (BOOL)xReqMsg( Window, GCS( MSG_REQ_WARNING ), GCS( MSG_FC_FILEALREADYEXISTS ), GCS( MSG_GF_CONTINUEABORT ), FilePart( strbuf1 ) );
	
	if( overwrite )
		{
			/// Write the data to the file
		
		if( file = Open( strbuf1, MODE_NEWFILE ) )
			{
			if( Write( file, XStrings->xs_Bootblock, 1024 ) == 1024 )
				xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_BB_SAV_BBSAVEDASBINARY ), GCS( MSG_GF_OKAY ), FilePart( strbuf1 ) );
			else
				xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_BB_SAV_COULDNOTSAVEBBASBINARY ), GCS( MSG_GF_OKAY ), FilePart( strbuf1 ) );
			
			Close( file );
			}
		}
}
