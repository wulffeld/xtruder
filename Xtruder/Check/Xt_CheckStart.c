/*
**	$VER: XtruderCheckStart.c (2.8.95)
**
**	Xtruder check start module
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Master function for several types of checks.
 *
 * Returns how many vira found or a negative number for errors
 */
LONG xStartCheck( struct List *filelist, UBYTE check_type, BOOL disablewindow )
{
	struct rtFileList *			fl,*tl;
	struct CurrentCheck *		cc;
	struct NameNode *			nnode;
	struct List *				checklist;
	BOOL						guiclosed = (Screen ? FALSE : TRUE);
	LONG						virafound = 0;
	UBYTE						strbuf[ DEFAULTBUFFERLEN ];
	
	
		/// Pop up GUI if needed
	
	if( !Screen && !XSettings->xs_MIS_StayHidden )
		xPopUpGUI();
	else
	if( Screen )
		ScreenToFront( Screen );
	
		/// Disable main window
	
	if( disablewindow )
		xDisableWindow( Window );
	
		/// Check if all check paths are present
	
	if( xCheckPathsExistance() )
		{
			/// This list will be used to hold all the files we should check
		
		if( checklist = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
			{
			NewList( checklist );
			
				/// Allocate space for information on this check
			
			if( cc = AllocVec( sizeof( struct CurrentCheck ), MEMF_CLEAR ) )
				{
				NewList( &cc->cc_DeleteList );
			
					/// Initialize various things
				
				switch( check_type )
					{
					case CHECKTYPE_PRESPECIFIED:
							/// Is there any prespecified entries?
						
						if( !IsListEmpty( &PreList->pl_List ) )
							{
								/// Copy to checklist
							
							xCopyList( &PreList->pl_List, checklist );
							}
						else
							{
							xAddFileCheckHistoryNodes( 3, GCS( MSG_FC_NOPRESPECIFIED ) );
							
							virafound = -1;
							}
						break;
					
					case CHECKTYPE_SPECIFIED:
							/// Check if list is empty because of ARexx support or
							/// 'list prespecified'
						
						if( !filelist || IsListEmpty( filelist ) )
							{
								/// Request the user to select the drawers and files he wants checked...
							
							rtChangeReqAttr( FileReq, RTFI_Dir, RPATH, TAG_END );
							
							if( fl = rtFileRequest( FileReq, RFILE, GCS( MSG_TL_SELECTFILESDRAWERS ),
								RT_Window,		Window,
								RT_Underscore,	'_',
								RT_TextAttr,	&RequesterFontAttr,
								RTFI_Height,	Screen->Height,
								RTFI_Flags,		FREQF_MULTISELECT | FREQF_SELECTDIRS,
								RTFI_OkText,	GCS( MSG_GF_SELECT ),
								TAG_END ) )
								{
								strncpy( RPATH, FileReq->Dir, DEFAULTPATHLEN );
								
								tl = fl;
								
									/// Copy the list of selected files and dirs...
								
								while( tl )
									{
									strcpy( strbuf, FileReq->Dir );
									AddPart( strbuf, tl->Name, DEFAULTBUFFERLEN );
									
									xAddNameNode( checklist, strbuf );
									
									tl = tl->Next;
									}
								
								rtFreeFileList( fl );
								}
							else
								virafound = -1;
							}
						else
							xCopyList( filelist, checklist );
						break;
					
					case CHECKTYPE_CLIARGS:
					case CHECKTYPE_ICONS:
					case CHECKTYPE_FLOPPY:
							/// Copy nodes to checklist
						
						xCopyList( filelist, checklist );
						break;
					}
				
					/// Here we go...
				
				if( virafound == 0 )
					{
					struct Window *	oldwin = Process->pr_WindowPtr;
					BOOL			chkwin;
					
					
					if( XSettings->xs_FC_MIS_SuppressSystemMessages )
						Process->pr_WindowPtr = (APTR)-1;
					
						/// If both XSettings->xs_MIS_StayHidden and guiclosed are TRUE
						/// don't try to open the check window
					
					if( XSettings->xs_MIS_StayHidden && guiclosed )
						chkwin = TRUE;
					else
						chkwin = xOpenCheckWindow();
					
					if( chkwin )
						{
							/// Update VirusList
						
						xCHK_AddVirusToVirusList( NULL, NULL );
						
							/// Init info boxes
						
						CurrentTime( &TimeSeconds, &TimeMics );
						
						xCW_UpdateCheckInfo( cc );
						xCW_UpdateMemoryInfo();
						
							/// Warnings
						
						xAddWarnings();
						
							/// Preliminary information
						
						if( check_type == CHECKTYPE_SPECIFIED || check_type == CHECKTYPE_CLIARGS || check_type == CHECKTYPE_ICONS || check_type == CHECKTYPE_FLOPPY )
							xAddFileCheckHistoryNodes( 2, GCS( MSG_FC_CHECKINGSPECIFIED ) );
						
							/// CHECK!
						
						for( nnode = (struct NameNode *)checklist->lh_Head;
							 nnode->nn_Node.ln_Succ;
							 nnode = (struct NameNode *)nnode->nn_Node.ln_Succ )
							{
								/// Handle special things
							
							switch( check_type )
								{
								case CHECKTYPE_PRESPECIFIED:
								case CHECKTYPE_PRESPECIFIEDTIMECHECK:
									xAddFileCheckHistoryNodes( 2, GCS( MSG_FC_CHECKINGPRESPECIFIED ), nnode->nn_Node.ln_Name );
									break;
								}
							
							xDoObject( cc, nnode->nn_Node.ln_Name );
							
								/// User pressed Stop?
							
							if( cc->cc_Result == CCRES_USERBREAK )
								break;
							
							cc->cc_Result = 0;
							}
						
						xCW_UpdateCheckInfo( cc );
						xCW_UpdateMemoryInfo();
						
							/// Are there any files which we should delete ?
						
						if( !IsListEmpty( &cc->cc_DeleteList ) )
							{
							xCHK_DeleteInfectedFiles( &cc->cc_DeleteList );
							
							xFreeNameNodes( &cc->cc_DeleteList );
							}
						
							/// Add the check information to the global history
						
						xAddCheckInfoToHistory( cc );
						
							/// Handle History...
						
						if( XSettings->xs_HIS_SaveMethod != HISSM_DONTSAVE )
							{
								/// Did we record information for this path/file check?
							
							if( XSettings->xs_HIS_RecFileCheck )
								xPerformHistorySave();
							}
						
						virafound = cc->cc_ViraFound;
						
							/// Close check window
						
						if( XSettings->xs_FC_CHW_AutoClose == FALSE )
							{
							cc->cc_Result = CCRES_CHECKEND;
							
							xHandleCheckWindowIDCMP( cc );
							}
						
						xCloseCheckWindow();
						}
					else
						virafound = -3;
					
					Process->pr_WindowPtr = oldwin;
					}
				
				FreeVec( cc );
				}
			else
				virafound = -4;
			
				/// Free any left check nodes
			
			xFreeNameNodes( checklist );
			
			FreeVec( checklist );
			}
		}
	else
		{
		xAddFileCheckHistoryNodes( 3, GCS( MSG_FC_ONEORMOREPATHSNOTPRESENT ) );
		
		xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_FC_ONEORMOREPATHSNOTPRESENT ), GCS( MSG_GF_OKAY ) );
		
		virafound = -2;
		}
	
		/// Enable main window
	
	if( disablewindow )
		xEnableWindow( Window );
	
		/// If GUI was closed when check started and is now open we'll have to close it again (feature)
	
	if( guiclosed )
		if( XSettings->xs_MIS_StayHidden && Screen )
			xCloseInterface();
	
	return( virafound );
}

VOID xAddWarnings( VOID )
{
	UBYTE	strbuf[ DEFAULTBUFFERLEN ];
	
	
		/// List is not attached to listview yet so don't bother removing it first

	if( XSettings->xs_FC_CHW_Warnings && CheckWindow )
		{
		if( xParsePatternNoCase( XSettings->xs_FC_MIS_MatchFilter, strbuf ) == 1 )
			xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_NOTALLFILESWILLBECHECKED ), XSettings->xs_FC_MIS_MatchFilter );
		
		if( XSettings->xs_FC_PBC_CheckPBCFiles == FALSE )
			xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_FILESWITHPBCNOTCHECKED ) );
		
		if( XSettings->xs_FC_MIS_AutoDeprotect == FALSE )
			xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_READPROTECTEDNOTCHECKED ) );
		
		if( XSettings->xs_FC_MIS_SuppressSystemMessages  )
			xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_SYSTEMMESSAGESSUPPRESSED ) );
		
		if( XSettings->xs_FC_UNP_DecrunchExecutables == FALSE )
			xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_CRUNCHEDEXENOTCHECKED ) );
		
		if( XSettings->xs_FC_UNP_DecrunchDataFiles == FALSE )
			xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_CRUNCHEDDATANOTCHECKED ) );
		
		if( XSettings->xs_FC_UNP_DecrunchEncryptedFiles == FALSE )
			xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_ENCRYPTEDNOTCHECKED ) );
		
		if( XSettings->xs_FC_UNP_ExtractArchives == FALSE )
			xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_ARCHIVESNOTEXTRACTED ) );
		else
			{
			if( xParsePatternNoCase( XSettings->xs_FC_MIS_ArchiveFilter, strbuf ) == 1 )
				xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_NOTALLARCHIVESCHECKED ), XSettings->xs_FC_MIS_ArchiveFilter );
			
			if( XSettings->xs_FC_ARC_ExtractLhA == FALSE )
				xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_LHAARCHIVESNOTCHECKED ) );
			
			if( XSettings->xs_FC_ARC_LhAMaxSize )
				xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_NOTALLLHAARCHIVESEXTRACTED ) );
			
			if( XSettings->xs_FC_ARC_ExtractLZX == FALSE )
				xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_LZXARCHIVESNOTCHECKED ) );
			
			if( XSettings->xs_FC_ARC_LZXMaxSize )
				xAddNameNode( &CheckWindowWarningsList, GCS( MSG_WA_NOTALLLZXARCHIVESEXTRACTED ) );
			}
		
			/// Attach list to ListView
		
		GT_SetGadgetAttrs( CheckWindowGadgets[ GAD_CW_WARNINGS ].xg_Gad, CheckWindow, NULL,
			GTLV_Labels, &CheckWindowWarningsList,
			TAG_END );
		}
}

VOID xAddCheckInfoToHistory( struct CurrentCheck *cc )
{
	ULONG	secs2,mics;
	ULONG	hours,mins;
	
	
		/// Store end time of check
	
	CurrentTime( &secs2, &mics );
	
		/// Calculate how long the check took
	
	secs2 -= TimeSeconds;
	
	hours = secs2 / 3600;
	secs2 -= hours * 3600;
	mins = secs2 / 60;
	secs2 -= mins * 60;
		
	xAddFileCheckHistoryNodes( 2, GCS( MSG_CW_CHECKINFO1 ), cc->cc_DirsEncountered, cc->cc_FilesEncountered, cc->cc_FilesChecked );
	xAddFileCheckHistoryNodes( 2, GCS( MSG_CW_CHECKINFO2 ), cc->cc_FilesCrunched, cc->cc_FilesDecrunched );
	xAddFileCheckHistoryNodes( 2, GCS( MSG_CW_CHECKINFO3 ), cc->cc_ArchivedFilesEncountered, cc->cc_ArchivedFilesExtracted );
	xAddFileCheckHistoryNodes( 2, GCS( MSG_CW_CHECKINFO4 ), cc->cc_ViraFound, hours, mins, secs2 );
}

VOID xCopyList( struct List *srclist, struct List *destlist )
{
	struct Node *	nnode;
	
	
	for( nnode = srclist->lh_Head; nnode->ln_Succ; nnode = nnode->ln_Succ )
		xAddNameNode( destlist, nnode->ln_Name  );
}

BOOL xCheckPathsExistance( VOID )
{
	UBYTE *	paths[3];
	UBYTE	p;
	BPTR	lock;
	BOOL	allpresent = TRUE;
	
	
	paths[0] = XSettings->xs_PAT_ArchiveExtractPath;
	paths[1] = XSettings->xs_PAT_VirusPath;
	paths[2] = XSettings->xs_PAT_RecheckPath;
	
	for( p = 0; p < 3; p++ )
		{
		if( paths[ p ][ 0 ] )
			{
			if( lock = Lock( paths[ p ], ACCESS_READ ) )
				UnLock( lock );
			else
				allpresent = FALSE;
			}
		else
			allpresent = FALSE;
		}
	
	return( allpresent );
}

VOID xListPrespecified( VOID )
{
	struct List		checklist;
	struct Node *	node;
	
	
	NewList( &checklist );
	
	if( node = xOpenSelectNodeWindow( &PreList->pl_List, GCS( MSG_TL_PRESPECIFIEDENTRIES ), GCS( MSG_TL_PRESPECIFIEDENTRIES ), 0, Coords->xc_ListPrespecifiedWindow ) )
		{
		xAddNameNode( &checklist, node->ln_Name );
		
		if( !IsListEmpty( &checklist ) )
			{
			xStartCheck( &checklist, CHECKTYPE_SPECIFIED, TRUE );
			
			xFreeNameNodes( &checklist );
			}
		}
}
