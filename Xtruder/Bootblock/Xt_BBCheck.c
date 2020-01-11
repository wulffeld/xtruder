/*
**	$VER: XtruderBBCheck.c (24.10.95)
**
**	Xtruder bootblock check functions
**
**	(C) Copyright 1993-1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Check a bootblock...
 */
VOID xCheckBootblockProcess( WORD unit, STRPTR dmsfilename )
{
	BOOL	suc = TRUE;
	UBYTE	strbuf[ DEFAULTBUFFERLEN ] = "";
	
	
	switch( unit )
		{
		case DRV_DF0:
		case DRV_DF1:
		case DRV_DF2:
		case DRV_DF3:
			xCheckDrive( unit, NULL );
			break;
		
		case DRV_DMS:
			if( !dmsfilename )
				{
				suc = xSelectFile( Window,
					SELFP_ReqTitle,	GCS( MSG_BB_TL_SELECTDMSARCHIVE ),
					SELFP_Dir,		RPATH,
					SELFP_DestFile,	strbuf,
					TAG_END );
				}
			
			if( suc )
				{
				if( xReadDMSBootblock( strbuf, TRUE ) )
					xCheckDrive( DRV_DMS, strbuf );
				}
			break;
		}
}

VOID xCheckDrive( WORD unit, STRPTR dmsfilename )
{
	struct Bootblock *	bb;
	LONG				ret;
	
	
	switch( unit )
		{
		case DRV_DF0:
		case DRV_DF1:
		case DRV_DF2:
		case DRV_DF3:
			if( !xReadBoot( unit ) )
				return;
			break;
		}
	
		/// Check bootblock
	
	bb = xAnalyzeAndUpdateBufferGad( unit, dmsfilename, &ret );
	
		/// Handle result of check
	
	xHandleBBCheckResult( bb, unit, ret, dmsfilename );
}

/*
 * This function updates the buffer gadget by analysing the contents of 'XStrings->xs_Bootblock'.
 */
struct Bootblock *xAnalyzeAndUpdateBufferGad( WORD unit, STRPTR dmsfilename, LONG *ret )
{
	struct Bootblock *	bb;
	
	
		/// Check bootblock
	
	bb = (struct Bootblock *)CheckBoot( (ULONG *)XStrings->xs_Bootblock, ret );
	
		/// Update buffer gadget
	
	switch( unit )
		{
		case DRV_DMS:
				/// The bootblock we just checked was from a DMS archive
			
			switch( *ret )
				{
				case BOOT_VIRUS:
					xBB_UpdateBufferGadget( FALSE, GCS( MSG_BB_BUF_DMSFILEINFECTED ), FilePart( dmsfilename ), bb->bootname );
					break;
				
				case BOOT_UNKNOWN:
					xBB_UpdateBufferGadget( FALSE, GCS( MSG_BB_BUF_UNKNOWNBBDMS ), FilePart( dmsfilename ) );
					break;
				
				case BOOT_NOT_BOOT:
					xBB_UpdateBufferGadget( FALSE, GCS( MSG_BB_BUF_NOTBOOTABLEBBDMS ), FilePart( dmsfilename ) );
					break;
				}
			break;
		
		case DRV_DF0:
		case DRV_DF1:
		case DRV_DF2:
		case DRV_DF3:
				/// The bootblock we just checked was from disk
			
			switch( *ret )
				{
				case BOOT_VIRUS:
					xBB_UpdateBufferGadget( FALSE, GCS( MSG_BB_BUF_DISKINFECTED ), unit, bb->bootname );
					break;
				
				case BOOT_UNKNOWN:
					xBB_UpdateBufferGadget( FALSE, GCS( MSG_BB_BUF_UNKNOWNBB ), unit );
					break;
				
				case BOOT_NOT_BOOT:
					xBB_UpdateBufferGadget( FALSE, GCS( MSG_BB_BUF_NOTBOOTABLEBB ), unit );
					break;
				}
			break;
		
		case DRV_CUSTOM:
				/// The bootblock we just checked was from a custom bootblock
			
			switch( *ret )
				{
				case BOOT_VIRUS:
					xBB_UpdateBufferGadget( FALSE, GCS( MSG_BB_BUF_CUSTOMBBINFECTED ), FilePart( dmsfilename ), bb->bootname );
					break;
				
				case BOOT_UNKNOWN:
					xBB_UpdateBufferGadget( FALSE, GCS( MSG_BB_BUF_UNKNOWNCUSTOMBB ), FilePart( dmsfilename ) );
					break;
				
				case BOOT_NOT_BOOT:
					xBB_UpdateBufferGadget( FALSE, GCS( MSG_BB_BUF_NOTBOOTABLECUSTOMBB ), FilePart( dmsfilename ) );
					break;
				}
			break;
		}
	
	return( bb );
}

VOID xHandleBBCheckResult( struct Bootblock *bb, WORD unit, LONG ret, STRPTR dmsfilename )
{
	LONG	c;
	
	
	switch( ret )
		{
		case BOOT_VIRUS:
			if( unit == DRV_DMS )
				c = xReqMsg( Window, GCS( MSG_REQ_WARNING ), GCS( MSG_BB_CHK_DMSFILEINFECTED ), GCS( MSG_BB_CHK_GF_DMSCURECHOICE ), FilePart( dmsfilename ), bb->bootname );
			else
				c = xReqMsg( Window, GCS( MSG_REQ_WARNING ), GCS( MSG_BB_CHK_DISKINFECTED ), GCS( MSG_BB_CHK_GF_BBCURECHOICE ), unit, bb->bootname );
			
			switch( c )
				{
					/// Install or Delete
				
				case 1:
					if( unit == DRV_DMS )
						DeleteFile( dmsfilename );
					else
						xInstallBootblockProcess( unit, -1 );
					break;
				
					/// Save as binary
				
				case 2:
					xSaveAsBinary();
					break;
				}
			break;
		}
}
