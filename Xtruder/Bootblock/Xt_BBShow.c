/*
**	$VER: XtruderBBShow.c (16.8.96)
**
**	Xtruder show bootblock functions
**
**	Copyright (C) 1993-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Show bootblock in listview
 */
BOOL xShowBootblock( WORD unit, STRPTR dmsfilename, BOOL readit )
{
	UBYTE	strbuf1[ DEFAULTBUFFERLEN ] = "";
	LONG	ret;
	
	
		/// Read the bootblock...
	
	if( readit )
		{
		switch( unit )
			{
			case DRV_DF0:
			case DRV_DF1:
			case DRV_DF2:
			case DRV_DF3:
				if( !xReadBoot( unit ) )
					return( FALSE );
				break;
			
			case DRV_DMS:
				if( xSelectFile( Window,
					SELFP_ReqTitle,	GCS( MSG_BB_TL_SELECTDMSARCHIVE ),
					SELFP_Dir,		RPATH,
					SELFP_DestFile,	strbuf1,
					TAG_END ) )
					{
					if( !xReadDMSBootblock( strbuf1, TRUE ) )
						return( FALSE );
					
					dmsfilename = strbuf1;
					}
				else
					return( FALSE );
				break;
			
			case DRV_CUSTOM:
				if( xSelectFile( Window,
					SELFP_ReqTitle,	GCS( MSG_BB_TL_SELECTCUSTOMBOOTBLOCK ),
					SELFP_Dir,		RPATH,
					SELFP_DestFile,	strbuf1,
					TAG_END ) )
					{
					BPTR	file;
					BOOL	success = FALSE;
					
					
					if( file = Open( strbuf1, MODE_OLDFILE ) )
						{
						if( 1024 == Read( file, XStrings->xs_Bootblock, 1024 ) )
							success = TRUE;
						
						Close( file );
						}
					
					if( !success )
						return( FALSE );
					
					dmsfilename = strbuf1;
					}
				else
					return( FALSE );
				break;
			}
		}
	
		/// Check bootblock
	
	xAnalyzeAndUpdateBufferGad( unit, dmsfilename, &ret );
	
		/// Clear status gadget
	
	xBB_UpdateStatusGadget( FALSE, "" );
	
		/// Update listview
	
	xCreateBootblockList();
	
	xUpdateLoadSaveGadgets();
	
	return( TRUE );
}
