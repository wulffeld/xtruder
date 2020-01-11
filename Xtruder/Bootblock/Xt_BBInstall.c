/*
**	$VER: XtruderBootblockInstall.c (18.8.96)
**
**	Xtruder bootblock install functions
**
**	Copyright (C) 1993-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


BOOL xInstallBootblockProcess( WORD unit, WORD type )
{
	UBYTE *	buffer;
	ULONG	size;
	UBYTE	strbuf[ DEFAULTBUFFERLEN ] = "";
	BOOL	success = FALSE;
	
	
	switch( unit )
		{
		case DRV_DF0:
		case DRV_DF1:
		case DRV_DF2:
		case DRV_DF3:
			
				/// Select the bootblock to install
			
			if( type == -1 )
				type = xOpenBootblockSelectWindow( Coords->xc_BootblockSelectWindow, unit - DRV_DF0 );
			
			switch( type )
				{
				case BB_1XOFS:
				case BB_13FFS:
				case BB_20OFS:
				case BB_20FFS:
				case BB_20OFSINT:
				case BB_20FFSINT:
				case BB_30OFSCACHE:
				case BB_30FFSCACHE:
					if( xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_BB_INS_INSTALLBOOTBLOCK ), GCS( MSG_GF_YESNO ), unit ) )
						{
						if( xInstallDrive( unit, type - 1, NULL ) )
							success = TRUE;
						}
					break;
				
				case BB_CUSTOM:
					if( XSettings->xs_BOO_CustomBBFile[0] )
						{
						size = xFileSize( XSettings->xs_BOO_CustomBBFile );
						
						if( size >= 1024 )
							{
							if( buffer = xStoreFile( XSettings->xs_BOO_CustomBBFile, size ) )
								{
								if( xInstallDrive( unit, 0, buffer ) )
									success = TRUE;
								}
							}
						
						break;
						}
				
				case BB_CUSTOMSELECT:
					if( xSelectFile( Window,
						SELFP_ReqTitle,	GCS( MSG_BB_TL_SELECTCUSTOMBOOTBLOCK ),
						SELFP_Dir,		RPATH,
						SELFP_DestFile,	strbuf,
						TAG_END ) )
						{
						size = xFileSize( strbuf );
						
						if( size >= 1024 )
							{
							if( buffer = xStoreFile( strbuf, size ) )
								{
								if( xInstallDrive( unit, 0, buffer ) )
									success = TRUE;
								
								FreeVec( buffer );
								}
							}
						}
					break;
				}
			break;
		}
	
	return( success );
}

/*
 * Install a bootblock to a drive using the bootblock library...
 */
BOOL xInstallDrive( WORD unit, WORD bbtype, UBYTE *buffer )
{
	LONG	a;
	BOOL	success = FALSE;
	
	
		/// Ask for confirmation on installing a bootblock
	
	xBB_UpdateStatusGadget( FALSE, GCS( MSG_BB_STA_INSTALLINGBOOTBLOCK ), unit );
	
		/// Install
	
	if( buffer )
		a = WriteBoot( unit, buffer );
	else
		a = InstallBoot( unit, bbtype );
	
		/// Inform user of the operation
	
	if( a )
		{
		xTDError( a );
		
		xBB_UpdateStatusGadget( FALSE, GCS( MSG_BB_STA_COULDNOTINSTALLBB ), unit );
		}
	else
		{
		xBB_UpdateStatusGadget( FALSE, GCS( MSG_BB_STA_BOOTBLOCKINSTALLED ), unit );
		
		success = TRUE;
		}
	
	return( success );
}
