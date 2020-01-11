/*
**	$VER: Xt_SaveSettings.c (21.4.97)
**
**	Xtruder settings save functions
**
**	Copyright (C) 1994-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


VOID xSaveSettings( BOOL askfile, BOOL ask )
{
	struct PrespecifiedNode *	pnode;
	struct ARexxNode *			anode;
	struct FileLock *			fl;
	UBYTE *						buffer;
	BPTR						pfile,efile,old_output;
	UWORD						a,e;
	UBYTE						settingsname[ DEFAULTBUFFERLEN ] = "";
	UBYTE						envarcxtrudername[ DEFAULTBUFFERLEN ] = "";
	UBYTE						envxtrudername[ DEFAULTBUFFERLEN ] = "";
	UBYTE						xtruderprefsname[ DEFAULTBUFFERLEN ] = "";
	UWORD						envprefs = FALSE;
	
	
	xDisableWindow( Window );
	
	if( !askfile && ask )
		{
		if( !xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_SAVESETTINGS ), GCS( MSG_GF_OKAYCANCEL ) ) )
			{
			xEnableWindow( Window );
			
			return;
			}
		}
	
	strcpy( xtruderprefsname, "Xtruder.prefs" );
	strcpy( envarcxtrudername, "ENVARC:Xtruder" );
	
	if( askfile )
		{
		if( !xSelectFile( Window,
			SELFP_Dir,		envarcxtrudername,
			SELFP_File,		xtruderprefsname,
			SELFP_DestFile,	settingsname,
			TAG_END ) )
			{
			xEnableWindow( Window );
			
			return;
			}
		}
	else
		{
			/// If settings is to be saved in ENV:Xtruder/ then first create the sub directory
			/// in ENV: and ENVARC:
		
		envprefs = strnicmp( XStrings->xs_PrefsName, "ENV:Xtruder/Xtruder.prefs", 21 );
		
		if( !envprefs )
			{
			strcpy( settingsname, "ENVARC:Xtruder" );
			strcpy( envxtrudername, "ENV:Xtruder" );
			
			if( fl = (struct FileLock *)CreateDir( settingsname ) )
				UnLock( (BPTR)fl );
			
			if( fl = (struct FileLock *)CreateDir( envxtrudername ) )
				UnLock( (BPTR)fl );
			
			AddPart( settingsname, xtruderprefsname, DEFAULTBUFFERLEN );
			AddPart( envxtrudername, xtruderprefsname, DEFAULTBUFFERLEN );
			}
		else
			strcpy( settingsname, XStrings->xs_PrefsName );
		}
	
		/// Write the preferences
	
	if( pfile = Open( settingsname, MODE_NEWFILE ) )
		{
		old_output = SelectOutput( pfile );
		
		SettingsChanged = FALSE;
		
		Printf( "\n $VER: Xtruder_Settings 3.0 (25.8.96)\n\n ** Don't delete the above **\n\n" );
		
		Printf( "PATH=%s\n",						RPATH );
		Printf( "FILE=%s\n",						RFILE );
		Printf( "REXXPATH=%s\n",					REXXPATH );
		Printf( "REXXFILE=%s\n",					REXXFILE );
		
		Printf( "CUSTOMBBFILE=%s\n",				XSettings->xs_BOO_CustomBBFile );
		Printf( "CHECKINSERTEDDISKS=%ld\n",			XSettings->xs_BOO_CheckInsertedDisks );
		Printf( "NOINTERFACE=%ld\n",				XSettings->xs_BOO_NoInterface );
		
		Printf( "LHAEXTRACTROUTINE=%ld\n",			XSettings->xs_FC_ARC_LhAExtractRoutine );
		Printf( "EXTRACTLHA=%ld\n",					XSettings->xs_FC_ARC_ExtractLhA );
		Printf( "LHAMAXSIZE=%ld\n",					XSettings->xs_FC_ARC_LhAMaxSize );
		Printf( "LZXEXTRACTROUTINE=%ld\n",			XSettings->xs_FC_ARC_LZXExtractRoutine );
		Printf( "EXTRACTLZX=%ld\n",					XSettings->xs_FC_ARC_ExtractLZX );
		Printf( "LZXMAXSIZE=%ld\n",					XSettings->xs_FC_ARC_LZXMaxSize );
		Printf( "EXTRACTOUTPUT=%s\n",				XSettings->xs_FC_ARC_Output );
		
		Printf( "CWAUTOCLOSE=%ld\n",				XSettings->xs_FC_CHW_AutoClose );
		Printf( "CWMEMINFO=%ld\n",					XSettings->xs_FC_CHW_MemoryInformation );
		Printf( "CWCHECKINFO=%ld\n",				XSettings->xs_FC_CHW_CheckInformation );
		Printf( "CWHISTORY=%ld\n",					XSettings->xs_FC_CHW_History );
		Printf( "CWWARNINGS=%ld\n",					XSettings->xs_FC_CHW_Warnings );
		Printf( "CWAUTOSCROLL=%ld\n",				XSettings->xs_FC_CHW_AutoScroll );
		
		Printf( "WRITEINFOSIZE=%ld\n",				XSettings->xs_FC_INF_WrtInfo_Size );
		Printf( "WRITEINFOPROTBITS=%ld\n",			XSettings->xs_FC_INF_WrtInfo_ProtBits );
		Printf( "WRITEINFODATE=%ld\n",				XSettings->xs_FC_INF_WrtInfo_Date );
		Printf( "WRITEINFOFILETYPE=%ld\n",			XSettings->xs_FC_INF_WrtInfo_Filetype );
		
		Printf( "MATCHFILTER=%s\n",					XSettings->xs_FC_MIS_MatchFilter );
		Printf( "ARCHIVEFILTER=%s\n",				XSettings->xs_FC_MIS_ArchiveFilter );
		Printf( "ACTIONWHENVIRUSFOUND=%ld\n",		XSettings->xs_FC_MIS_ActionWhenVirusFound );
		Printf( "ACTIONWHENVIRUSCOPIED=%ld\n",		XSettings->xs_FC_MIS_ActionWhenVirusCopied );
		Printf( "VIRUSCOMMENT=%s\n",				XSettings->xs_FC_MIS_VirusComment );
		Printf( "VIRUSRENAMEEXTENSION=%s\n",		XSettings->xs_FC_MIS_VirusRenameExtension );
		Printf( "PATHINDENTION=%ld\n",				XSettings->xs_FC_MIS_PathIndention );
		Printf( "AUTODEPROTECT=%ld\n",				XSettings->xs_FC_MIS_AutoDeprotect );
		Printf( "ENTERRECURSIVELY=%ld\n",			XSettings->xs_FC_MIS_EnterRecursively );
		Printf( "AUTOPATHCHECK=%ld\n",				XSettings->xs_FC_MIS_AutoPathCheck );
		Printf( "SUPPRESSSYSTEMMESSAGES=%ld\n",		XSettings->xs_FC_MIS_SuppressSystemMessages );
		
		Printf( "PBCBITS=%ld\n",					XSettings->xs_FC_PBC_PBCBits );
		Printf( "CHECKPBCFILES=%ld\n",				XSettings->xs_FC_PBC_CheckPBCFiles );
		Printf( "SETPBCONEXE=%ld\n",				XSettings->xs_FC_PBC_SetPBCOnExe );
		Printf( "SETPBCONARCS=%ld\n",				XSettings->xs_FC_PBC_SetPBCOnArchives );
		
		Printf( "WHENDECRUNCHINGUSE=%ld\n",			XSettings->xs_FC_UNP_WhenDecrunchingUse);
		Printf( "EXTRACTARCHIVES=%ld\n",			XSettings->xs_FC_UNP_ExtractArchives );
		Printf( "ASKBEFOREEXRACTING=%ld\n",			XSettings->xs_FC_UNP_AskBeforeExtracting );
		Printf( "ASKBEFOREDELETEINGEF=%ld\n",		XSettings->xs_FC_UNP_AskBeforeDeletingExtFiles );
		Printf( "DECRUNCHEXECUTABLES=%ld\n",		XSettings->xs_FC_UNP_DecrunchExecutables );
		Printf( "DECRUNCHDATAFILES=%ld\n",			XSettings->xs_FC_UNP_DecrunchDataFiles );
		Printf( "ASKBEFOREDECRUNCHING=%ld\n",		XSettings->xs_FC_UNP_AskBeforeDecrunching );
		Printf( "ASKBEFOREDELETINGDECFILES=%ld\n",	XSettings->xs_FC_UNP_AskBeforeDeletingDecFiles );
		Printf( "DECRUNCHENCRYPTED=%ld\n",			XSettings->xs_FC_UNP_DecrunchEncryptedFiles );
		Printf( "CHECKDMSBOOTBLOCK=%ld\n",			XSettings->xs_FC_UNP_CheckDMSBootblock );
		
		Printf( "SCREENMODE=%ld\n",					XSettings->xs_GUI_ScreenMode );
		Printf( "PUBLICSCREENNAME=%s\n",			XSettings->xs_GUI_PublicScreenName );
		Printf( "SCREENOVERSCAN=%ld\n",				XSettings->xs_GUI_ScreenOverscan );
		Printf( "SCREENWIDTH=%ld\n",				XSettings->xs_GUI_ScreenWidth );
		Printf( "SCREENHEIGHT=%ld\n",				XSettings->xs_GUI_ScreenHeight );
		Printf( "SCREENDEPTH=%ld\n",				XSettings->xs_GUI_ScreenDepth );
		Printf( "AUTOSCROLL=%ld\n",					XSettings->xs_GUI_AutoScroll );
		Printf( "SCREENFONTNAME=%s\n",				XSettings->xs_GUI_ScreenFontName );
		Printf( "SCREENFONTSIZE=%ld\n",				XSettings->xs_GUI_ScreenFontSize );
		Printf( "SCREENFONTSTYLE=%ld\n",			XSettings->xs_GUI_ScreenFontStyle );
		Printf( "TEXTFONTNAME=%s\n",				XSettings->xs_GUI_TextFontName );
		Printf( "TEXTFONTSIZE=%ld\n",				XSettings->xs_GUI_TextFontSize );
		Printf( "REQUESTERFONTNAME=%s\n",			XSettings->xs_GUI_RequesterFontName );
		Printf( "REQUESTERFONTSIZE=%ld\n",			XSettings->xs_GUI_RequesterFontSize );
		Printf( "REQUESTERFONTSTYLE=%ld\n",			XSettings->xs_GUI_RequesterFontStyle );
		Printf( "BACKGROUNDPATTERN=%s\n",			XSettings->xs_GUI_BackgroundPattern );
		Printf( "SCROLLERWIDTH=%ld\n",				XSettings->xs_GUI_ScrollerWidth );
		Printf( "HIDEAPPICON=%ld\n",				XSettings->xs_GUI_HideAppIcon );
		
		Printf( "HISTORYPATH=%s\n",					XSettings->xs_HIS_HistoryPath );
		Printf( "HISTORYFILE=%s\n",					XSettings->xs_HIS_HistoryFile );
		Printf( "USEDATEANDTIME=%ld\n",				XSettings->xs_HIS_UseDateAndTime );
		Printf( "APPEND=%ld\n",						XSettings->xs_HIS_Append );
		Printf( "REQUESTFILENAME=%ld\n",			XSettings->xs_HIS_RequestFilename );
		Printf( "MAXENTRIES=%ld\n",					XSettings->xs_HIS_MaxEntries );
		Printf( "SAVEMETHOD=%ld\n",					XSettings->xs_HIS_SaveMethod );
		Printf( "RECFILECHECK=%ld\n",				XSettings->xs_HIS_RecFileCheck );
		
		Printf( "CXPOPKEY=%s\n",					XSettings->xs_MIS_CxPopkey );
		Printf( "CXPRIORITY=%ld\n",					XSettings->xs_MIS_CxPriority );
		Printf( "CXPOPUP=%ld\n",					XSettings->xs_MIS_CxPopup );
		Printf( "STAYHIDDEN=%ld\n",					XSettings->xs_MIS_StayHidden );
		
		Printf( "ARCHIVEEXTRACTPATH=%s\n",			XSettings->xs_PAT_ArchiveExtractPath );
		Printf( "EXEDECRUNCHPATH=%s\n",				XSettings->xs_PAT_FileDecrunchPath );
		Printf( "VIRUSPATH=%s\n",					XSettings->xs_PAT_VirusPath );
		Printf( "RECHECKPATH=%s\n",					XSettings->xs_PAT_RecheckPath );
		Printf( "AUTOCREATEDRAWERS=%ld\n",			XSettings->xs_PAT_AutoCreateDrawers );
		Printf( "AUTODELETEDRAWERS=%ld\n",			XSettings->xs_PAT_AutoDeleteDrawers );
		Printf( "GUIDEPATH=%s\n",					XSettings->xs_PAT_GuidePath );
		
		Printf( "STARTUPSCRIPT=%s\n",				XSettings->xs_STA_StartupScript );
		Printf( "EXECUTESCRIPT=%ld\n",				XSettings->xs_STA_ExecuteScript );
		Printf( "CHECKPRESPECIFIED=%ld\n",			XSettings->xs_STA_CheckPrespecified );
		Printf( "CHECKVECTORS=%ld\n",				XSettings->xs_STA_CheckVectors );
		Printf( "SHOWSYSTEMINFORMATION=%ld\n",		XSettings->xs_STA_ShowSystemInformation );
		
		Printf( "REPORTSPATH=%s\n",					XSettings->xs_REP_ReportsPath );
		
			/// Virus information
		
		Printf( "DESCRIPTIONSPATH=%s\n",			XSettings->xs_VIN_DescriptionsPath );
		Printf( "EXTERNALVIEWER=%s\n",				XSettings->xs_VIN_ExternalViewer );
		
		for( a = 0; a < 3; a++ )
			Printf( "VIBSEARCHSTRING%ld=%s\n", a + 1, XStrings->xs_SearchStrings[ a ] );
		
			/// Window coordinates...
		
       	if( OnPublic && Window )
       		{
			Coords->xc_MainWindow[0] = Window->LeftEdge;
			Coords->xc_MainWindow[1] = Window->TopEdge;
			}
		
		Printf( "MAINWIN=%ld %ld\n", Coords->xc_MainWindow[0], Coords->xc_MainWindow[1] );
		
		Printf( "BOOTBLOCKWIN=%ld %ld %ld %ld\n", Coords->xc_BootblockWindow[0], Coords->xc_BootblockWindow[1], Coords->xc_BootblockWindow[2], Coords->xc_BootblockWindow[3] );
		Printf( "BOOTBLOCKSELECTWIN=%ld %ld\n", Coords->xc_BootblockSelectWindow[0], Coords->xc_BootblockSelectWindow[1] );
		Printf( "CHECKWIN=%ld %ld %ld %ld\n", Coords->xc_CheckWindow[0], Coords->xc_CheckWindow[1], Coords->xc_CheckWindow[2], Coords->xc_CheckWindow[3] );
		Printf( "HISTORYWIN=%ld %ld %ld %ld\n", Coords->xc_HistoryWindow[0], Coords->xc_HistoryWindow[1], Coords->xc_HistoryWindow[2], Coords->xc_HistoryWindow[3] );
		Printf( "BOOTBLOCKVIRAWIN=%ld %ld %ld %ld\n", Coords->xc_ListKnownBootblockViraWindow[0], Coords->xc_ListKnownBootblockViraWindow[1], Coords->xc_ListKnownBootblockViraWindow[2], Coords->xc_ListKnownBootblockViraWindow[3] );
		Printf( "FILEBASEDVIRAWIN=%ld %ld %ld %ld\n", Coords->xc_ListKnownFileBasedViraWindow[0], Coords->xc_ListKnownFileBasedViraWindow[1], Coords->xc_ListKnownFileBasedViraWindow[2], Coords->xc_ListKnownFileBasedViraWindow[3] );
		Printf( "CRUNCHERSWIN=%ld %ld %ld %ld\n", Coords->xc_ListKnownCrunchersWindow[0], Coords->xc_ListKnownCrunchersWindow[1], Coords->xc_ListKnownCrunchersWindow[2], Coords->xc_ListKnownCrunchersWindow[3] );
		Printf( "PATCHESWIN=%ld %ld %ld %ld\n", Coords->xc_ListKnownPatchesWindow[0], Coords->xc_ListKnownPatchesWindow[1], Coords->xc_ListKnownPatchesWindow[2], Coords->xc_ListKnownPatchesWindow[3] );
		Printf( "FILETYPESWIN=%ld %ld %ld %ld\n", Coords->xc_ListKnownFileTypesWindow[0], Coords->xc_ListKnownFileTypesWindow[1], Coords->xc_ListKnownFileTypesWindow[2], Coords->xc_ListKnownFileTypesWindow[3] );
		Printf( "LISTPRESPECIFIEDWIN=%ld %ld %ld %ld\n", Coords->xc_ListPrespecifiedWindow[0], Coords->xc_ListPrespecifiedWindow[1], Coords->xc_ListPrespecifiedWindow[2], Coords->xc_ListPrespecifiedWindow[3] );
		Printf( "LIBRARIESWIN=%ld %ld %ld %ld\n", Coords->xc_MemoryLibrariesWindow[0], Coords->xc_MemoryLibrariesWindow[1], Coords->xc_MemoryLibrariesWindow[2], Coords->xc_MemoryLibrariesWindow[3] );
		Printf( "DEVICESWIN=%ld %ld %ld %ld\n", Coords->xc_MemoryDevicesWindow[0], Coords->xc_MemoryDevicesWindow[1], Coords->xc_MemoryDevicesWindow[2], Coords->xc_MemoryDevicesWindow[3]  );
		Printf( "RESOURCESWIN=%ld %ld %ld %ld\n", Coords->xc_MemoryResourcesWindow[0], Coords->xc_MemoryResourcesWindow[1], Coords->xc_MemoryResourcesWindow[2], Coords->xc_MemoryResourcesWindow[3] );
		Printf( "PORTSWIN=%ld %ld %ld %ld\n", Coords->xc_MemoryPortsWindow[0], Coords->xc_MemoryPortsWindow[1], Coords->xc_MemoryPortsWindow[2], Coords->xc_MemoryPortsWindow[3] );
		Printf( "SEMAPHORESWIN=%ld %ld %ld %ld\n", Coords->xc_MemorySemaphoresWindow[0], Coords->xc_MemorySemaphoresWindow[1], Coords->xc_MemorySemaphoresWindow[2], Coords->xc_MemorySemaphoresWindow[3] );
		Printf( "TASKSWIN=%ld %ld %ld %ld\n", Coords->xc_MemoryTasksWindow[0], Coords->xc_MemoryTasksWindow[1], Coords->xc_MemoryTasksWindow[2], Coords->xc_MemoryTasksWindow[3] );
		Printf( "MEMORYVECTORSWIN=%ld %ld %ld %ld\n", Coords->xc_MemoryVectorsWindow[0], Coords->xc_MemoryVectorsWindow[1], Coords->xc_MemoryVectorsWindow[2], Coords->xc_MemoryVectorsWindow[3] );
		Printf( "MEMORYLVOWIN=%ld %ld %ld %ld\n", Coords->xc_MemoryLVOWindow[0], Coords->xc_MemoryLVOWindow[1], Coords->xc_MemoryLVOWindow[2], Coords->xc_MemoryLVOWindow[3] );
		Printf( "REPORTSMAINWIN=%ld %ld %ld %ld\n", Coords->xc_ReportsMainWindow[0], Coords->xc_ReportsMainWindow[1], Coords->xc_ReportsMainWindow[2], Coords->xc_ReportsMainWindow[3] );
		Printf( "REPORTSCREATEWIN=%ld %ld %ld %ld\n", Coords->xc_ReportsCreateWindow[0], Coords->xc_ReportsCreateWindow[1], Coords->xc_ReportsCreateWindow[2], Coords->xc_ReportsCreateWindow[3] );
		Printf( "REPORTSCHECKWIN=%ld %ld %ld %ld\n", Coords->xc_ReportsCheckWindow[0], Coords->xc_ReportsCheckWindow[1], Coords->xc_ReportsCheckWindow[2], Coords->xc_ReportsCheckWindow[3] );
		Printf( "SYSTEMINFORMATIONWIN=%ld %ld %ld %ld\n", Coords->xc_SystemInformationWindow[0], Coords->xc_SystemInformationWindow[1], Coords->xc_SystemInformationWindow[2], Coords->xc_SystemInformationWindow[3] );
		Printf( "VIRUSINFORMATIONWIN=%ld %ld %ld %ld\n", Coords->xc_VirusInformationWindow[0], Coords->xc_VirusInformationWindow[1], Coords->xc_VirusInformationWindow[2], Coords->xc_VirusInformationWindow[3] );
		
			/// Palette
		
		Printf( "PALETTE=" );
		
			/// If AGA
		
		if( GfxBase->lib_Version >= 39 )
			{
			ULONG	coltable[12];
			
			
			GetRGB32( ((struct ViewPort *)&Window->WScreen->ViewPort)->ColorMap, 0, 4, coltable );
			
				/// Mask out real values
			
			for( a = 0; a < 12; a++ )
				{
				coltable[a] &= 0x000000FF;
				
				Printf( "%ld%s", coltable[a], a == 11 ? "\n" : " " );
				}
			}
		else
			{
				/// Save pre-AGA values
			
			for( a = 0; a < 4; a++ )
				Printf( "%ld%s", GetRGB4( ((struct ViewPort *)&Window->WScreen->ViewPort)->ColorMap, a ), a == 3 ? "\n" : " " );
			}
		
			/// Save preloaded dir/file list
		
		if( !IsListEmpty( &PreList->pl_List ) )
			{
			for( pnode = (struct PrespecifiedNode *)PreList->pl_List.lh_Head, e = 1;
				 pnode->pn_Node.ln_Succ;
				 pnode = (struct PrespecifiedNode *)pnode->pn_Node.ln_Succ, e++ )
				{
				Printf( "P%ld=%s\n", e, pnode->pn_Name );
				}
			}
		
			/// Save ARexx scripts
		
		if( !IsListEmpty( ARexxScriptsList ) )
			{
			for( anode = (struct ARexxNode *)ARexxScriptsList->lh_Head, e = 1;
				 anode->an_Node.ln_Succ;
				 anode = (struct ARexxNode *)anode->an_Node.ln_Succ, e++ )
				{
				Printf( "AM%ld=%s\n", e, anode->an_MenuName );
				Printf( "AF%ld=%s\n", e, anode->an_FileName );
				//Printf( "AO%ld=%s\n", e, anode->an_Output );
				Printf( "AB%ld=%ld\n", e, anode->an_Separator );
				}
			}
		
			/// Restore original output handle
		
		SelectOutput( old_output );
		
		Close( pfile );
		
			/// If this isn't the 'Save as' command copy the file to 'ENV:Xtruder/'
		
		if( !askfile && !envprefs )
			{
			if( e = xFileSize( settingsname ) )
				{
				if( buffer = xStoreFile( settingsname, e ) )
					{
						/// Write ENV: file
					
					if( efile = Open( envxtrudername, MODE_NEWFILE ) )
						{
						Write( efile, buffer, e );
						
						Close( efile );
						}
					
					FreeVec( buffer );
					}
				}
			}
		}
	
	xEnableWindow( Window );
}
