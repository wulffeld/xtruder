/*
**	$VER: XtruderCheckDecrunch.c (30.7.96)
**
**	Xtruder check decrunch module
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Used ONLY in conjunction with xDoFile().
 *
 * Decrunches an executable or date file and then checks it.
 */
VOID xCHK_DecrunchExeOrDataAndCheck( struct CurrentCheck *cc, struct CurrentFile *cf, struct CurrentFile *archivecf, UBYTE library_determine )
{
	struct CurrentFile *	check_cf = archivecf ? archivecf : cf;
	UBYTE *					password = NULL;
	STRPTR					filename;
	STRPTR					filename_parted;
	ULONG					filesize;
	
	
	filename = check_cf->cf_FileName;
	filesize = check_cf->cf_Size;
	filename_parted = FilePart( filename );
	
		/// Should we ask the user first?
	
	if( XSettings->xs_FC_UNP_AskBeforeDecrunching )
		if( !xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FILECRUNCHEDDECRUNCHIT ), GCS( MSG_GF_YESNO ), filename_parted, filesize, check_cf->cf_CruncherName ) )
			return;
	
		/// Write out that we are beginning to decrunch the file...
	
	xAddFileCheckHistoryNodes( 1, GCS( MSG_DECRUNCHINGFILE ), filename_parted, filesize, check_cf->cf_CruncherName );
	
	switch( library_determine )
		{
		case LCRU_UNPACK:
				/// File determined to be crunched by Unpack.library
			
			if( UInfo->ui_CrunchType & 0x80 )
				{
					/// Should we check encrypted files?
				
				if( XSettings->xs_FC_UNP_DecrunchEncryptedFiles == FALSE )
					{
						/// Copy the file to recheck path so the user can check them later.
					
					xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_FILEENCRYPTEDCOPYING ), filename_parted, XSettings->xs_PAT_RecheckPath );
					
					if( !xCopyFile( filename, XSettings->xs_PAT_RecheckPath ) )
						xAddFileCheckHistoryNodes( 3, GCS( MSG_FC_COULDNOTCOPYFILETO ), filename_parted, XSettings->xs_PAT_RecheckPath );
					
					return;
					}
				}
			
				/// We will free the decrunched data ourselves
			
			UInfo->ui_Flag |= UFN_NoFree;
			UInfo->ui_CrunchAdr = check_cf->cf_Buffer;
			UInfo->ui_CrunchLen = check_cf->cf_BufferSize;
			
				/// Decrunch it...
			
			if( upUnpack( UInfo ) )
				{
				cc->cc_FilesDecrunched++;
				
				xAddFileCheckHistoryNodes( 1, GCS( MSG_FILEDECRUNCHED ), filename_parted, filesize, UInfo->ui_DecrunchLen );
				
					/// Check buffer for vira
				
				if( xCHK_CheckBuffer( UInfo->ui_DecrunchAdr, UInfo->ui_DecrunchLen ) )
					{
						/// VIRUS FOUND! Fill in virus name etc.
					
					cc->cc_ViraFound++;
					strncpy( check_cf->cf_VirusName, FilevirusNode->fv_FileInfection->fi_VirusName, DEFAULTBUFFERLEN );
		
					if( XSettings->xs_HIS_RecFileCheck )
						XtruderHistory->his_ViraFound++;
					
						/// Deal with virus according to preferences
					
					if( archivecf )
						xCHK_HandleVirusInArchive( cc, cf, archivecf );
					else
						xCHK_HandleVirus( cc, cf );
					}
				
					/// Free the decrunched file
				
				upFreeFile( UInfo );
				}
			else
				{
				switch( UInfo->ui_ErrorNum )
					{
					case UERR_Corrupt:
						xCopyFile( filename, XSettings->xs_PAT_RecheckPath );
					case UERR_Open:
					case UERR_ReadWrite:
					case UERR_Memory:
					case UERR_Determine:
					case UERR_Password:
					case UERR_Hunk:
					case UERR_Extern:
					case UERR_Device:
					case UERR_DevOpen:
					case UERR_CRC:
					case UERR_CheckSum:
					case UERR_Old:
					case UERR_DevErr:
						xAddFileCheckHistoryNodes( 4, GCS( MSG_DECRUNCHERROR ), UInfo->ui_ErrorMsg );
						break;
					}
				}
			break;
		
		case LCRU_XFDMASTER:
				/// File determined to be crunched by xfdmaster.library ?
			
			//Printf( "%lx\n", XFDInfo->xfdbi_PackerFlags);
			
			if( (XFDInfo->xfdbi_PackerFlags & XFDPFF_PASSWORD) ||
				(XFDInfo->xfdbi_PackerFlags & XFDPFF_KEY16) ||
				(XFDInfo->xfdbi_PackerFlags & XFDPFF_KEY32) )
				{
					/// Should we check encrypted files ?
				
				if( XSettings->xs_FC_UNP_DecrunchEncryptedFiles == FALSE )
					{
						/// No, so copy the file to recheck path so the user can check them later.
					
					xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_FILEENCRYPTEDCOPYING ), filename_parted, XSettings->xs_PAT_RecheckPath );
					
					if( !xCopyFile( filename, XSettings->xs_PAT_RecheckPath ) )
						xAddFileCheckHistoryNodes( 3, GCS( MSG_FC_COULDNOTCOPYFILETO ), filename_parted, XSettings->xs_PAT_RecheckPath );
					
					return;
					}
				
				if( XFDInfo->xfdbi_PackerFlags & XFDPFF_PASSWORD )
					{
						/// Request password from user
					
					if( password = AllocVec( 1000, MEMF_CLEAR ) )
						{
						if( !rtGetString( password, 999, GCS( MSG_TL_ENTERPASSWORD ), NULL,
							RT_ReqPos,		REQPOS_CENTERSCR,
							RT_Window,		Window,
							RT_Underscore,	'_',
							RT_TextAttr,	&RequesterFontAttr,
							TAG_END ) )
							{
							FreeVec( password );
							
							return;
							}
						else
							XFDInfo->xfdbi_Special = password;
						}
					else
						return;
					}
				else
					return;
				}
			
				/// Decrunch file
			
			XFDInfo->xfdbi_TargetBufMemType = MEMF_ANY;
			
			if( xfdDecrunchBuffer( XFDInfo ) )
				{
				cc->cc_FilesDecrunched++;
				
				xAddFileCheckHistoryNodes( 1, GCS( MSG_FILEDECRUNCHED ), filename_parted, filesize, XFDInfo->xfdbi_TargetBufSaveLen );
				
					/// Check buffer for vira
				
				if( xCHK_CheckBuffer( XFDInfo->xfdbi_TargetBuffer, XFDInfo->xfdbi_TargetBufSaveLen ) )
					{
						/// VIRUS FOUND! Fill in virus name etc.
					
					cc->cc_ViraFound++;
					strncpy( check_cf->cf_VirusName, FilevirusNode->fv_FileInfection->fi_VirusName, DEFAULTBUFFERLEN );
		
					if( XSettings->xs_HIS_RecFileCheck )
						XtruderHistory->his_ViraFound++;
					
						/// Deal with virus according to preferences
					
					if( archivecf )
						xCHK_HandleVirusInArchive( cc, cf, archivecf );
					else
						xCHK_HandleVirus( cc, cf );
					}
				
				FreeMem( XFDInfo->xfdbi_TargetBuffer, XFDInfo->xfdbi_TargetBufLen );
				}
			else
				{
				switch( XFDInfo->xfdbi_Error )
					{
					case XFDERR_CORRUPTEDDATA:
						xCopyFile( filename, XSettings->xs_PAT_RecheckPath );
					case XFDERR_NOMEMORY:
					case XFDERR_NOSLAVE:
					case XFDERR_NOTSUPPORTED:
					case XFDERR_UNKNOWN:
					case XFDERR_NOSOURCE:
					case XFDERR_WRONGPASSWORD:
					case XFDERR_BADHUNK:
					case XFDERR_MISSINGRESOURCE:
					case XFDERR_UNDEFINEDHUNK:
					case XFDERR_NOHUNKHEADER:
					case XFDERR_BADEXTTYPE:
					case XFDERR_BUFFERTRUNCATED:
					case XFDERR_WRONGHUNKAMOUNT:
					case XFDERR_UNSUPPORTEDHUNK:
					case XFDERR_BADRELMODE:
						xAddFileCheckHistoryNodes( 4, GCS( MSG_DECRUNCHERROR ), xfdGetErrorText( XFDInfo->xfdbi_Error ) );
						break;
					}
				}
			
			if( password )
				FreeVec( password );
			break;
		}
}
