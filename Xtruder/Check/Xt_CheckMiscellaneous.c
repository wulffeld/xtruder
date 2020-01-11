/*
**	$VER: XtruderCheckMiscellaneous.c (23.9.95)
**
**	Xtruder check miscellaneous module
**
**	Copyright (C) 1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


VOID xCHK_HandleVirus( struct CurrentCheck *cc, struct CurrentFile *cf )
{
	UBYTE	copiedname[ DEFAULTBUFFERLEN ];
	UWORD	act = XSettings->xs_FC_MIS_ActionWhenVirusFound;
	
	
		/// Add virus to the VirusList for the ARexx commands that needs this list.
	
	xCHK_AddVirusToVirusList( cf, NULL );
	
		/// Handle virus
		
	if( cf->cf_ArchiveType == AT_DMS )
		xAddFileCheckHistoryNodes( 3, GCS( MSG_DMSFILEINFECTED_SHORT ), FilePart( cf->cf_FileName ), cf->cf_VirusName );
	else
		xAddFileCheckHistoryNodes( 3, GCS( MSG_FC_VIRUSFOUND ), FilePart( cf->cf_FileName ), cf->cf_VirusName );
	
		/// File must not have been acted upon
	
	if( cf->cf_FileActedUpon == FALSE )
		{
			/// Request the user to take action...
		
		if( act == VIRUSFOUND_REQUEST )
			{
			act = xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_VIRUSFOUND_Q ), GCS( MSG_FC_GF_VIRUSFOUNDREQUEST ), FilePart( cf->cf_FileName ), cf->cf_Size, cf->cf_VirusName );
			
				/// VIRUSFOUND_NOTHING ?
			
			if( !act )
				return;
			
			act--;
			}
		
		switch( act )
			{
			case VIRUSFOUND_DISABLE:
				xCHK_DisableInfectedFile( cf );
				break;
			
			case VIRUSFOUND_DELETE:
				xCHK_AddFileToDelete( cc, cf );
				break;
			
			case VIRUSFOUND_RENAME:
				xCHK_RenameInfectedFile( cf );
				break;
			
			case VIRUSFOUND_COMMENT:
				xCHK_CommentInfectedFile( cf );
				break;
			
			case VIRUSFOUND_COPY:
				xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_COPYINGFILE ), FilePart( cf->cf_FileName ), XSettings->xs_PAT_VirusPath );
				
				if( !xCopyFile( cf->cf_FileName, XSettings->xs_PAT_VirusPath ) )
					xAddFileCheckHistoryNodes( 3, GCS( MSG_FC_COULDNOTCOPYFILETO ), FilePart( cf->cf_FileName ), XSettings->xs_PAT_VirusPath );
				else
					{
					strcpy( copiedname, XSettings->xs_PAT_VirusPath );
					AddPart( copiedname, cf->cf_FileName, DEFAULTBUFFERLEN );
					
						/// Exchange names
					
					strcpy( cf->cf_FileName, copiedname );
					
					xCHK_HandleVirusCopied( cc, cf );
					}
				break;
			}
		}
}

/*
 * Perform action on archive with virus as requested/specified
 */
VOID xCHK_HandleVirusInArchive( struct CurrentCheck *cc, struct CurrentFile *cf, struct CurrentFile *archivecf )
{
	UWORD	act = XSettings->xs_FC_MIS_ActionWhenVirusFound;
	UBYTE	copiedname[ DEFAULTBUFFERLEN ];
	BPTR	lock,oldcd;
	
	
		/// Add virus to the VirusList for the ARexx commands that needs this list.
	
	xCHK_AddVirusToVirusList( cf, archivecf );
	
	if( cf->cf_ArchiveType == AT_DMS )
		xAddFileCheckHistoryNodes( 3, GCS( MSG_DMSFILEINFECTED_SHORT ), FilePart( cf->cf_FileName ), archivecf->cf_VirusName );
	else
		xAddFileCheckHistoryNodes( 3, GCS( MSG_FC_VIRUSFOUND ), FilePart( cf->cf_FileName ), archivecf->cf_VirusName );
	
		/// File must not have been acted upon
	
	if( cf->cf_FileActedUpon == FALSE )
		{
			/// Request the user to take action...
		
		if( act == VIRUSFOUND_REQUEST )
			{
			act = xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_VIRUSINARCHIVEFOUND_Q ), GCS( MSG_FC_GF_VIRUSFOUNDREQUEST ), FilePart( archivecf->cf_FileName ), archivecf->cf_Size, cf->cf_FileName, cf->cf_Size, archivecf->cf_VirusName );
			
			if( !act )
				return;
			
			act--;
			}
		
		switch( act )
			{
			case VIRUSFOUND_DISABLE:
				xCHK_DisableInfectedFile( cf );
				break;
			
			case VIRUSFOUND_DELETE:
				xCHK_AddFileToDelete( cc, cf );
				break;
			
			case VIRUSFOUND_RENAME:
				xCHK_RenameInfectedFile( cf );
				break;
			
			case VIRUSFOUND_COMMENT:
				xCHK_CommentInfectedFile( cf );
				break;
			
			case VIRUSFOUND_COPY:
				xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_COPYINGFILE ), FilePart( cf->cf_FileName ), XSettings->xs_PAT_VirusPath );
				
				if( lock = Lock( cf->cf_PathName, ACCESS_READ ) )
					{
					oldcd = CurrentDir( lock );
					
					if( xCopyFile( cf->cf_FileName, XSettings->xs_PAT_VirusPath ) )
						{
						strcpy( copiedname, XSettings->xs_PAT_VirusPath );
						AddPart( copiedname, cf->cf_FileName, DEFAULTBUFFERLEN );
						
							/// Exchange names
						
						strcpy( cf->cf_FileName, copiedname );
						
						xCHK_HandleVirusCopied( cc, cf );
						}
					else
						xAddFileCheckHistoryNodes( 3, GCS( MSG_FC_COULDNOTCOPYFILETO ), FilePart( cf->cf_FileName ), XSettings->xs_PAT_VirusPath );
					
					CurrentDir( oldcd );
					
					UnLock( lock );
					}
				break;
			}
		}
}

/*
 * Perform action requested/specified upon the copied virus
 */
VOID xCHK_HandleVirusCopied( struct CurrentCheck *cc, struct CurrentFile *cf )
{
	UWORD	act = XSettings->xs_FC_MIS_ActionWhenVirusCopied;
	BPTR	lock,oldcd;
	
	
		/// Request the user to take action...
	
	if( act == VIRUSCOPIED_REQUEST )
		{
		act = xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_WHATTODOWITHCOPIEDFILE ), GCS( MSG_FC_GF_VIRUSCOPIEDREQUEST ), FilePart( cf->cf_FileName ), cf->cf_Size );
		
		if( act == 0 )
			return;
		
		act--;
		}
	
	if( lock = Lock( XSettings->xs_PAT_VirusPath, ACCESS_READ ) )
		{
		oldcd = CurrentDir( lock );
		
		switch( act )
			{
			case VIRUSCOPIED_DISABLE:
				xCHK_DisableInfectedFile( cf );
				break;
			
			case VIRUSCOPIED_RENAME:
				xCHK_RenameInfectedFile( cf );
				break;
			
			case VIRUSCOPIED_COMMENT:
				xCHK_CommentInfectedFile( cf );
				break;
			}
		
		CurrentDir( oldcd );
		
		UnLock( lock );
		}
}

VOID xCHK_DisableInfectedFile( struct CurrentFile *cf )
{
	if( SetProtection( cf->cf_FileName, FIBF_EXECUTE ) )
		{
		cf->cf_FileActedUpon = TRUE;
		
		xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_FILEDISABLED ), FilePart( cf->cf_FileName ) );
		}
	else
		xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_COULDNOTDISABLEFILE ), FilePart( cf->cf_FileName ) );
}

VOID xCHK_AddFileToDelete( struct CurrentCheck *cc, struct CurrentFile *cf )
{
	UBYTE	namebuf[ DEFAULTBUFFERLEN ];
	BPTR	lock;
	
	
		/// Since it's not a good idea to delete a file during a examine loop
		/// we'll have to add it to the list of the files to delete and
		/// delete them at the end of the check.
	
	if( lock = Lock( cf->cf_FileName, ACCESS_READ ) )
		{
		if( NameFromLock( lock, namebuf, DEFAULTBUFFERLEN ) )
			{
			if( xAddNameNode( &cc->cc_DeleteList, namebuf ) )
				{
				cf->cf_FileActedUpon = TRUE;
				
				xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_FILEWILLBEDELETEDLATER ), FilePart( cf->cf_FileName ) );
				}
			else
				xAddFileCheckHistoryNodes( 3, GCS( MSG_NOTENOUGHMEMORY ) );
			}
		UnLock( lock );
		}
}

VOID xCHK_DeleteInfectedFiles( struct List *deletelist )
{
	struct NameNode *	dnode;
	
	
	for( dnode = (struct NameNode *)deletelist->lh_Head;
		 dnode->nn_Node.ln_Succ;
		 dnode = (struct NameNode *)dnode->nn_Node.ln_Succ )
		{
		if( DeleteFile( dnode->nn_Name ) )
			xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_FILEDELETED ), FilePart( dnode->nn_Name ) );
		else
			xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_COULDNOTDELETEFILE ), FilePart( dnode->nn_Name ) );
		}
}

VOID xCHK_RenameInfectedFile( struct CurrentFile *cf )
{
	UBYTE	strbuf1[ DEFAULTBUFFERLEN ];
	UBYTE	strbuf2[ DEFAULTBUFFERLEN ];
	UBYTE	strbuf3[ DEFAULTBUFFERLEN ];
	
	
		/// If we have an extension we'll just add that to the filename
	
	if( XSettings->xs_FC_MIS_VirusRenameExtension[0] )
		{
		xSPrintF( strbuf2, "%s%s", cf->cf_FileName, XSettings->xs_FC_MIS_VirusRenameExtension );
		strcpy( strbuf1, cf->cf_PathName );
		AddPart( strbuf1, strbuf2, DEFAULTBUFFERLEN );
		}
	else
		{
		strcpy( strbuf2, FilePart( cf->cf_FileName ) );
		xSPrintF( strbuf3, GCS( MSG_FC_ENTERNEWNAME ), strbuf2 );
		
		if( rtGetString( strbuf2, 31, GCS( MSG_FC_TL_RENAMEFILE ), NULL,
			RT_Window,		Window,
			RT_Underscore,	'_',
			RT_TextAttr,	&RequesterFontAttr,
			RTGS_TextFmt,	strbuf3,
			TAG_END ) )
			{
			strcpy( strbuf1, cf->cf_PathName );
			AddPart( strbuf1, strbuf2, DEFAULTBUFFERLEN );
			}
		else
			return;
		}
	
	strcpy( strbuf3, cf->cf_PathName );
	AddPart( strbuf3, cf->cf_FileName, DEFAULTBUFFERLEN );
	
	if( Rename( strbuf3, strbuf1 ) )
		{
		cf->cf_FileActedUpon = TRUE;
		
		xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_FILERENAMED ), FilePart( cf->cf_FileName ), FilePart( strbuf1 ) );
		
		strcpy( cf->cf_FileName, FilePart( strbuf1 ) );
		}
	else
		xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_COULDNOTRENAMEFILE ), FilePart( cf->cf_FileName ), FilePart( strbuf1 ) );
}

VOID xCHK_CommentInfectedFile( struct CurrentFile *cf )
{
	UBYTE	strbuf1[ DEFAULTBUFFERLEN ];
	UBYTE	strbuf2[ DEFAULTBUFFERLEN ];
	
	
		/// If we have an extension we'll just add that to the filename
	
	if( XSettings->xs_FC_MIS_VirusComment[0] )
		strcpy( strbuf1, XSettings->xs_FC_MIS_VirusComment );
	else
		{
			/// else we'll request a new name...
		
		strcpy( strbuf1, cf->cf_VirusName );
		
		xSPrintF( strbuf2, GCS( MSG_FC_VIRUSNAMECOMMENT ), cf->cf_Comment );
		
		if( rtGetString( strbuf1, 80, GCS( MSG_FC_TL_COMMENTFILE ), NULL,
			RT_Window,		Window,
			RT_Underscore,	'_',
			RT_TextAttr,	&RequesterFontAttr,
			RTGS_TextFmt,	strbuf2,
			TAG_END ) )
			{
			strcpy( strbuf1, strbuf2 );
			}
		else
			return;
		}
	
	strcpy( strbuf2, cf->cf_PathName );
	AddPart( strbuf2, cf->cf_FileName, DEFAULTBUFFERLEN );
	
	if( SetComment( strbuf2, strbuf1 ) )
		{
		cf->cf_FileActedUpon = TRUE;
		
		xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_FILECOMMENTED ), FilePart( cf->cf_FileName ), strbuf1 );
		}
	else
		xAddFileCheckHistoryNodes( 1, GCS( MSG_FC_COULDNOTCOMMENTFILE ), FilePart( cf->cf_FileName ), strbuf1 );
}

UBYTE	TxtBuf0[ DEFAULTBUFFERLEN ];
UBYTE	TxtBuf1[ DEFAULTBUFFERLEN ];

/*
 * Write the information on a file which the user wants...
 */
VOID __stdargs xCHK_ATxt( BOOL history, BOOL archive, STRPTR filename, ... )
{
	UWORD	space = 50;
	STRPTR	name;
	UBYTE	i;
	
	
	va_list fileargs;
	
	va_start( fileargs, filename );
	
		/// Filename
	
	if( archive )
		{
		strcpy( TxtBuf0, INDENT );
		strcat( TxtBuf0, " " );
		
		if( name = (STRPTR)GetTagData( FI_ExtractedFullName, (ULONG)"", (struct TagItem *)fileargs ) )
			{
			for( i = 0; i < strlen( name ); i++ )
				if( name[i] == '/' )
					strcat( TxtBuf0, "  " );
			}
		
		strcat( TxtBuf0, ">> " );
		strcat( TxtBuf0, FilePart( filename ) );
		}
	else
		xSPrintF( TxtBuf0, "%s%s", INDENT, FilePart( filename ) );
	
		/// If the length of indention + filename is lower than the
		/// length of Size + Filetype then fill the space between with
		/// spaces...
	
	if( strlen( TxtBuf0 ) < space )
		{
		UWORD	a;
		
		
			/// Fill up with spaces...
		
		for( a = strlen( TxtBuf0 ); a < space; a++ )
			TxtBuf0[a] = ' ';
		
		TxtBuf0[a] = '\0';
		
			/// Size - 8 chars... (99100100)
		
		if( XSettings->xs_FC_INF_WrtInfo_Size )
			{
			UBYTE	buf[10];
			
			
			xSPrintF( buf, " %8ld", GetTagData( FI_Size, 0, (struct TagItem *)fileargs ) );
			
			strcat( TxtBuf0, buf );
			}
		
			/// Filetype - Max is 35 chars...
		
		if( XSettings->xs_FC_INF_WrtInfo_Filetype )
			{
			xSPrintF( TxtBuf1, " %-35s", GetTagData( FI_Filetype, (ULONG)"", (struct TagItem *)fileargs ) );
			strcat( TxtBuf0, TxtBuf1 );
			}
		
			/// Date - Max is variable
		
		if( XSettings->xs_FC_INF_WrtInfo_Date )
			{
			xSPrintF( TxtBuf1, " %-20s", GetTagData( FI_Date, (ULONG)"", (struct TagItem *)fileargs ) );
			strcat( TxtBuf0, TxtBuf1 );
			}
		
			/// Protection bits - Always 8 chars so no need for sprintf
		
		if( XSettings->xs_FC_INF_WrtInfo_ProtBits )
			{
			strcat( TxtBuf0, " " );
			xCHK_ConvertProtectionBitsToStr( GetTagData( FI_Protection, 0, (struct TagItem *)fileargs ), TxtBuf1 );
			
			strcat( TxtBuf0, TxtBuf1 );
			}
		
		xAddFileCheckHistoryNodes( 1, TxtBuf0 );
		}
	else
		{
		UWORD	endind = space - strlen( FilePart( filename ) ) - 3;
		UWORD	a;
		
		
		for( a = 0; a < endind; a++ )
			TxtBuf0[a] = ' ';
		
			/// Put in 3 dots to visualize truncation...
		
		for( a = endind; a < endind + 3; a++ )
			TxtBuf0[a] = '.';
		
		TxtBuf0[a] = '\0';
		
		strcat( TxtBuf0, FilePart( filename ) );
		
			/// Size - 8 chars... (99100100)
		
		if( XSettings->xs_FC_INF_WrtInfo_Size )
			{
			UBYTE	buf[10];
			
			
			xSPrintF( buf, " %8ld", GetTagData( FI_Size, 0, (struct TagItem *)fileargs ) );
			
			strcat( TxtBuf0, buf );
			}
		
			/// Filetype - Max is 35 chars...
		
		if( XSettings->xs_FC_INF_WrtInfo_Filetype )
			{
			xSPrintF( TxtBuf1, " %-35s", GetTagData( FI_Filetype, (ULONG)"", (struct TagItem *)fileargs ) );
			strcat( TxtBuf0, TxtBuf1 );
			}
		
			/// Date - Max is variable
		
		if( XSettings->xs_FC_INF_WrtInfo_Date )
			{
			xSPrintF( TxtBuf1, " %-20s", GetTagData( FI_Date, (ULONG)"", (struct TagItem *)fileargs ) );
			strcat( TxtBuf0, TxtBuf1 );
			}
		
			/// Protection bits - Always 8 chars so no need for sprintf
		
		if( XSettings->xs_FC_INF_WrtInfo_ProtBits )
			{
			strcat( TxtBuf0, " " );
			xCHK_ConvertProtectionBitsToStr( GetTagData( FI_Protection, 0, (struct TagItem *)fileargs ), TxtBuf1 );
			
			strcat( TxtBuf0, TxtBuf1 );
			}
		
		xAddFileCheckHistoryNodes( 1, TxtBuf0 );
		}
	
	va_end( filearg );
}

LONG xCHK_DeleteAll( STRPTR pathsted, BOOL ask )
{
	struct AnchorPath *	ap;
	struct NameNode *	node;
	struct List *		dellist;
	LONG				filesdeleted = 0;
	LONG				retval;
	
	
	if( ask )
		{
		if( !xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_DELETEALLIN ), GCS( MSG_GF_YESNO ), pathsted ) )
			return( 0 );
		}
	
	if( ap = AllocVec( sizeof( struct AnchorPath ) + 256, MEMF_PUBLIC | MEMF_CLEAR ) )
		{
		if( dellist = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
			{
				/// Prepare List structure
			
			NewList( dellist );
			
			ap->ap_Strlen = 256;
			
			for( retval = MatchFirst( pathsted, ap ); retval == 0; retval = MatchNext( ap ) )
				{
					/// Is it a file
				
				if( ap->ap_Info.fib_DirEntryType < 0 )
					{
					SetProtection( ap->ap_Buf, 0 );
					
					if( !xAddNameNode( dellist, ap->ap_Buf ) )
						break;
					}
				else
					{
					if( (ap->ap_Flags & APF_DIDDIR) == 0 )
						ap->ap_Flags |= APF_DODIR;
					else
						if( !xAddNameNode( dellist, ap->ap_Buf ) )
							break;
					
					ap->ap_Flags &= ~APF_DIDDIR;
					}
				}
			
			MatchEnd( ap );
			
				/// Delete all...
			
			for( node = (struct NameNode *)dellist->lh_Head;
				 node->nn_Node.ln_Succ;
				 node = (struct NameNode *)node->nn_Node.ln_Succ )
				{
				if( ((struct Node *)node->nn_Node.ln_Succ)->ln_Succ )
					{
					if( DeleteFile( node->nn_Name ) )
						filesdeleted++;
					}
				}
			
			xFreeNameNodes( dellist );
			
			FreeVec( dellist );
			}
		FreeVec( ap );
		}
	
	return( filesdeleted );
}

VOID xCHK_ConvertProtectionBitsToStr( ULONG pb, STRPTR buf )
{
	buf[0] = '\0';
	
	if( !(pb & (1<<7)) )
		strcat( buf, "-" );
	else
		strcat( buf, "H" );
	
	if( !(pb & FIBF_ARCHIVE) )
		strcat( buf, "-" );
	else
		strcat( buf, "A" );
	
	if( !(pb & FIBF_PURE) )
		strcat( buf, "-" );
	else
		strcat( buf, "P" );
	
	if( !(pb & FIBF_SCRIPT) )
		strcat( buf, "-" );
	else
		strcat( buf, "S" );
	
	if( !(pb & FIBF_READ) )
		strcat( buf, "R" );
	else
		strcat( buf, "-" );
	
	if( !(pb & FIBF_WRITE) )
		strcat( buf, "W" );
	else
		strcat( buf, "-" );
	
	if( !(pb & FIBF_EXECUTE) )
		strcat( buf, "E" );
	else
		strcat( buf, "-" );
	
	if( !(pb & FIBF_DELETE) )
		strcat( buf, "D" );
	else
		strcat( buf, "-" );
}

STRPTR xCHK_FormatFileDate( struct FileInfoBlock *fib, STRPTR retbuffer )
{
	struct DateTime	dt;
	UBYTE			datebuf[20];
	UBYTE			timebuf[20];
	
	
	dt.dat_Stamp = fib->fib_Date;
	dt.dat_Format = FORMAT_DOS;
	dt.dat_Flags = 0;
	dt.dat_StrDay = NULL;
	dt.dat_StrDate = datebuf;
	dt.dat_StrTime = timebuf;
	
	if( DateToStr( &dt ) )
		{
		xSPrintF( retbuffer, "%s %s", datebuf, timebuf );
		
		return( retbuffer );
		}
	
	return( NULL );
}

STRPTR xCHK_FileType( STRPTR filename, STRPTR retbuffer )
{
	UBYTE *					filebuffer;
	BPTR					file;
	
	
	retbuffer[0] = '\0';
	
	if( filebuffer = AllocVec( 1200, MEMF_CLEAR ) )
		{
		if( file = Open( filename, MODE_OLDFILE ) )
			{
			if( Read( file, filebuffer, 1200 ) != -1 )
				{
				FIIdentify( filebuffer, FInfo );
				
				strcpy( retbuffer, FInfo->FI_Description );
				}
			Close( file );
			}
		FreeVec( filebuffer );
		}
	
	return( retbuffer );
}
