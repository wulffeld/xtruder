/*
**	$VER: XtruderLoadSettings.c (27.7.96)
**
**	Xtruder settings load functions
**
**	Copyright (C) 1994-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


VOID xLoadSettings( BOOL notstartup )
{
	UBYTE	strbuf1[ DEFAULTBUFFERLEN ] = "";
	UBYTE	strbuf2[ DEFAULTBUFFERLEN ] = "";
	UBYTE *	pbuffer;
	ULONG	psize;
	LONG	len = 1;
	UBYTE	a;
	STRPTR	prefsname;
	
	
	pbuffer = NULL;
	
	if( !notstartup )
		{
		if( (len = GetVar( "Xtruder/PREFSLOCATION", XStrings->xs_PrefsName, DEFAULTBUFFERLEN, GVF_GLOBAL_ONLY ) ) == -1 )
			{
				/// Try and see if it exists in the CD
			
			if( len = xFileSize( "PREFSLOCATION" ) )
				{
				if( len < 250 )
					{
					if( pbuffer = xStoreFile( "PREFSLOCATION", len ) )
						{
						for( a = 0; a < len; a++ )
							{
							if( pbuffer[a] == '\n' )
								break;
							
							XStrings->xs_PrefsName[a] = pbuffer[a];
							}
						
						XStrings->xs_PrefsName[a] = 0;
						
						FreeVec( pbuffer );
						pbuffer = NULL;
						}
					}
				else
					len = 0;
				}
			}
		}
	else
		{
		if( !xSelectFile( Window,
			SELFP_Dir,		RPATH,
			SELFP_File,		strbuf1,
			SELFP_DestFile,	strbuf2,
			TAG_END ) )
			{
			return;
			}
		}

	if( len <= 0 )
		strcpy( XStrings->xs_PrefsName, "ENV:Xtruder/Xtruder.prefs" );
	else
		AddPart( XStrings->xs_PrefsName, "Xtruder.prefs", 255 );
	
	if( notstartup )
		prefsname = strbuf2;
	else
		prefsname = XStrings->xs_PrefsName;
	
	if( psize = xFileSize( prefsname ) )
		pbuffer = xStoreFile( prefsname, psize );
	
		/// Load prefs
	
	if( notstartup )
		xCloseInterface();
	
	xFillSettingsStructure( pbuffer, psize );
	
		/// Reopen GUI
	
	if( notstartup )
		xOpenInterface();
	
	FreeVec( pbuffer );
}

/*
 * Initialize the prefs structure
 */
VOID xFillSettingsStructure( UBYTE *pbuffer, ULONG psize )
{
	struct Screen *	scr;
	UBYTE			cols[50] = "";
	UBYTE			color[6];
	UBYTE			a;
	LONG			sep;
	UWORD			wi = 640,he = 256;
	ULONG			e,modeid = 0;
	STRPTR			str;
	UBYTE			strbuf1[ DEFAULTBUFFERLEN ] = "";
	UBYTE			strbuf2[ DEFAULTBUFFERLEN ] = "";
	
	
	if( scr = LockPubScreen( NULL ) )
		{
		wi = scr->Width;
		he = scr->Height;
		
		modeid = GetVPModeID( &(scr->ViewPort) );
		
		UnlockPubScreen( NULL, scr );
		}
	
	strncpy( RPATH, xFindString( pbuffer, psize, "PATH", "" ), DEFAULTPATHLEN );
	strncpy( RFILE, xFindString( pbuffer, psize, "FILE", "" ), DEFAULTBUFFERLEN );
	strncpy( REXXPATH, xFindString( pbuffer, psize, "REXXPATH", "" ), DEFAULTPATHLEN );
	strncpy( REXXFILE, xFindString( pbuffer, psize, "REXXFILE", "" ), DEFAULTBUFFERLEN );
	
	strncpy( XSettings->xs_BOO_CustomBBFile, xFindString( pbuffer, psize, "CUSTOMBBFILE", "" ), DEFAULTBUFFERLEN );
	XSettings->xs_BOO_CheckInsertedDisks			= xFindNumber( pbuffer, psize, "CHECKINSERTEDDISKS", FALSE );
	XSettings->xs_BOO_NoInterface					= xFindNumber( pbuffer, psize, "NOINTERFACE", FALSE );
	
	XSettings->xs_FC_ARC_LhAExtractRoutine			= xFindNumber( pbuffer, psize, "LHAEXTRACTROUTINE", XP_INTERNAL2EXTRACT );
	XSettings->xs_FC_ARC_ExtractLhA					= xFindNumber( pbuffer, psize, "EXTRACTLHA", TRUE );
	XSettings->xs_FC_ARC_LhAMaxSize					= xFindNumber( pbuffer, psize, "LHAMAXSIZE", 0 );
	XSettings->xs_FC_ARC_LZXExtractRoutine			= xFindNumber( pbuffer, psize, "LZXEXTRACTROUTINE", XP_INTERNAL2EXTRACT );
	XSettings->xs_FC_ARC_ExtractLZX					= xFindNumber( pbuffer, psize, "EXTRACTLZX", TRUE );
	XSettings->xs_FC_ARC_LZXMaxSize					= xFindNumber( pbuffer, psize, "LZXMAXSIZE", 0 );
	strncpy( XSettings->xs_FC_ARC_Output, xFindString( pbuffer, psize, "EXTRACTOUTPUT", "CON:0/30//150/%s/SCREENXTRUDER" ), DEFAULTPATHLEN );
	
	strncpy( XSettings->xs_FC_MIS_MatchFilter, xFindString( pbuffer, psize, "MATCHFILTER", "#?" ), DEFAULTBUFFERLEN );
	strncpy( XSettings->xs_FC_MIS_ArchiveFilter, xFindString( pbuffer, psize, "ARCHIVEFILTER", "~(#?.guide|#?.doc)" ), DEFAULTBUFFERLEN );
	XSettings->xs_FC_MIS_ActionWhenVirusFound		= xFindNumber( pbuffer, psize, "ACTIONWHENVIRUSFOUND", VIRUSFOUND_REQUEST );
	XSettings->xs_FC_MIS_ActionWhenVirusCopied		= xFindNumber( pbuffer, psize, "ACTIONWHENVIRUSCOPIED", VIRUSCOPIED_REQUEST );
	strncpy( XSettings->xs_FC_MIS_VirusComment, xFindString( pbuffer, psize, "VIRUSCOMMENT", "This is a virus infected file!" ), 80 );
	strncpy( XSettings->xs_FC_MIS_VirusRenameExtension, xFindString( pbuffer, psize, "VIRUSRENAMEEXTENSION", "_VIRUS" ), DEFAULTFILELEN );
	XSettings->xs_FC_MIS_PathIndention				= xFindNumberLimits( pbuffer, psize, "PATHINDENTION", 2, 1, 8 );
	XSettings->xs_FC_MIS_AutoDeprotect				= xFindNumber( pbuffer, psize, "AUTODEPROTECT", FALSE );
	XSettings->xs_FC_MIS_EnterRecursively			= xFindNumber( pbuffer, psize, "ENTERRECURSIVELY", TRUE );
	XSettings->xs_FC_MIS_AutoPathCheck				= xFindNumber( pbuffer, psize, "AUTOPATHCHECK", FALSE );
	XSettings->xs_FC_MIS_SuppressSystemMessages		= xFindNumber( pbuffer, psize, "SUPPRESSSYSTEMMESSAGES", FALSE );
	
	XSettings->xs_FC_INF_WrtInfo_Size				= xFindNumber( pbuffer, psize, "WRITEINFOSIZE", TRUE );
	XSettings->xs_FC_INF_WrtInfo_ProtBits			= xFindNumber( pbuffer, psize, "WRITEINFOPROTBITS", FALSE );
	XSettings->xs_FC_INF_WrtInfo_Date				= xFindNumber( pbuffer, psize, "WRITEINFODATE", FALSE );
	XSettings->xs_FC_INF_WrtInfo_Filetype			= xFindNumber( pbuffer, psize, "WRITEINFOFILETYPE", TRUE );
	
	XSettings->xs_FC_CHW_AutoClose					= xFindNumber( pbuffer, psize, "CWAUTOCLOSE", FALSE );
	XSettings->xs_FC_CHW_MemoryInformation			= xFindNumber( pbuffer, psize, "CWMEMINFO", TRUE );
	XSettings->xs_FC_CHW_CheckInformation			= xFindNumber( pbuffer, psize, "CWCHECKINFO", TRUE );
	XSettings->xs_FC_CHW_History					= xFindNumber( pbuffer, psize, "CWHISTORY", TRUE );
	XSettings->xs_FC_CHW_Warnings					= xFindNumber( pbuffer, psize, "CWWARNINGS", TRUE );
	XSettings->xs_FC_CHW_AutoScroll					= xFindNumber( pbuffer, psize, "CWAUTOSCROLL", FALSE );
	
	XSettings->xs_FC_PBC_PBCBits					= xFindNumber( pbuffer, psize, "PBCBITS", 0 );
	XSettings->xs_FC_PBC_CheckPBCFiles				= xFindNumber( pbuffer, psize, "CHECKPBCFILES", TRUE );
	XSettings->xs_FC_PBC_SetPBCOnExe				= xFindNumber( pbuffer, psize, "SETPBCONEXE", FALSE );
	XSettings->xs_FC_PBC_SetPBCOnArchives			= xFindNumber( pbuffer, psize, "SETPBCONARCS", FALSE );
	
	XSettings->xs_FC_UNP_WhenDecrunchingUse			= xFindNumber( pbuffer, psize, "WHENDECRUNCHINGUSE", FALSE );
	XSettings->xs_FC_UNP_ExtractArchives			= xFindNumber( pbuffer, psize, "EXTRACTARCHIVES", FALSE );
	XSettings->xs_FC_UNP_AskBeforeExtracting		= xFindNumber( pbuffer, psize, "ASKBEFOREEXRACTING", TRUE );
	XSettings->xs_FC_UNP_AskBeforeDeletingExtFiles	= xFindNumber( pbuffer, psize, "ASKBEFOREDELETEINGEF", TRUE );
	XSettings->xs_FC_UNP_DecrunchExecutables		= xFindNumber( pbuffer, psize, "DECRUNCHEXECUTABLES", TRUE );
	XSettings->xs_FC_UNP_DecrunchDataFiles			= xFindNumber( pbuffer, psize, "DECRUNCHDATAFILES", TRUE );
	XSettings->xs_FC_UNP_AskBeforeDecrunching		= xFindNumber( pbuffer, psize, "ASKBEFOREDECRUNCHING", TRUE );
	XSettings->xs_FC_UNP_AskBeforeDeletingDecFiles	= xFindNumber( pbuffer, psize, "ASKBEFOREDELETINGDECFILES", TRUE );
	XSettings->xs_FC_UNP_DecrunchEncryptedFiles		= xFindNumber( pbuffer, psize, "DECRUNCHENCRYPTED", FALSE );
	XSettings->xs_FC_UNP_CheckDMSBootblock			= xFindNumber( pbuffer, psize, "CHECKDMSBOOTBLOCK", FALSE );
	
	XSettings->xs_GUI_ScreenMode					= xFindNumber( pbuffer, psize, "SCREENMODE", modeid );
	strncpy( XSettings->xs_GUI_PublicScreenName, xFindString( pbuffer, psize, "PUBLICSCREENNAME", "" ), 140 );
	XSettings->xs_GUI_ScreenOverscan				= xFindNumber( pbuffer, psize, "SCREENOVERSCAN", OSCAN_TEXT );
	XSettings->xs_GUI_ScreenWidth					= xFindNumber( pbuffer, psize, "SCREENWIDTH", 0 );
	XSettings->xs_GUI_ScreenHeight					= xFindNumber( pbuffer, psize, "SCREENHEIGHT", 0 );
	XSettings->xs_GUI_ScreenDepth					= xFindNumberLimits( pbuffer, psize, "SCREENDEPTH", 2, 2, 8 );
	
	if( XSettings->xs_GUI_ScreenWidth < 15 )
		XSettings->xs_GUI_ScreenWidth = wi;
	
	if( XSettings->xs_GUI_ScreenHeight == 0 )
		XSettings->xs_GUI_ScreenHeight = he;
	
	XSettings->xs_GUI_AutoScroll					= xFindNumber( pbuffer, psize, "AUTOSCROLL", TRUE );
	strncpy( XSettings->xs_GUI_ScreenFontName, xFindString( pbuffer, psize, "SCREENFONTNAME", ScreenFontAttr.ta_Name ), DEFAULTBUFFERLEN );
	XSettings->xs_GUI_ScreenFontSize				= xFindNumberLimits( pbuffer, psize, "SCREENFONTSIZE", ScreenFontAttr.ta_YSize, 7, 40 );
	strncpy( XSettings->xs_GUI_TextFontName, xFindString( pbuffer, psize, "TEXTFONTNAME", "topaz.font" ), DEFAULTBUFFERLEN );
	XSettings->xs_GUI_TextFontSize					= xFindNumberLimits( pbuffer, psize, "TEXTFONTSIZE", 8, 7, 40 );
	strncpy( XSettings->xs_GUI_RequesterFontName, xFindString( pbuffer, psize, "REQUESTERFONTNAME", "topaz.font" ), DEFAULTBUFFERLEN );
	XSettings->xs_GUI_RequesterFontSize				= xFindNumberLimits( pbuffer, psize, "REQUESTERFONTSIZE", 8, 7, 40 );
	strncpy( XSettings->xs_GUI_BackgroundPattern, xFindString( pbuffer, psize, "BACKGROUNDPATTERN", "" ), DEFAULTBUFFERLEN );
	XSettings->xs_GUI_ScrollerWidth					= xFindNumberLimits( pbuffer, psize, "SCROLLERWIDTH", 16, 14, 40 );
	XSettings->xs_GUI_HideAppIcon					= xFindNumber( pbuffer, psize, "HIDEAPPICON", FALSE );
	
	strncpy( XSettings->xs_HIS_HistoryPath, xFindString( pbuffer, psize, "HISTORYPATH", "" ), DEFAULTPATHLEN );
	strncpy( XSettings->xs_HIS_HistoryFile, xFindString( pbuffer, psize, "HISTORYFILE", "" ), DEFAULTFILELEN );
	XSettings->xs_HIS_UseDateAndTime				= xFindNumber( pbuffer, psize, "USEDATEANDTIME", TRUE );
	XSettings->xs_HIS_Append						= xFindNumber( pbuffer, psize, "APPEND", TRUE );
	XSettings->xs_HIS_RequestFilename				= xFindNumber( pbuffer, psize, "REQUESTFILENAME", TRUE );
	XSettings->xs_HIS_MaxEntries					= xFindNumber( pbuffer, psize, "MAXENTRIES", 0 );
	XSettings->xs_HIS_SaveMethod					= xFindNumber( pbuffer, psize, "SAVEMETHOD", HISSM_DONTSAVE );
	XSettings->xs_HIS_RecFileCheck					= xFindNumber( pbuffer, psize, "RECFILECHECK", TRUE );
	
	strncpy( XSettings->xs_MIS_CxPopkey, xFindString( pbuffer, psize, "CXPOPKEY", "control lamiga x" ), 39 );
	XSettings->xs_MIS_CxPriority					= xFindNumber( pbuffer, psize, "CXPRIORITY", 1 );
	XSettings->xs_MIS_CxPopup						= xFindNumber( pbuffer, psize, "CXPOPUP", 1 );
	XSettings->xs_MIS_StayHidden					= xFindNumber( pbuffer, psize, "STAYHIDDEN", FALSE );
	
	strncpy( XSettings->xs_PAT_ArchiveExtractPath, xFindString( pbuffer, psize, "ARCHIVEEXTRACTPATH", "T:" ), DEFAULTPATHLEN );
	strncpy( XSettings->xs_PAT_FileDecrunchPath, xFindString( pbuffer, psize, "EXEDECRUNCHPATH", "T:" ), DEFAULTPATHLEN );
	strncpy( XSettings->xs_PAT_VirusPath, xFindString( pbuffer, psize, "VIRUSPATH", "RAM:Virus" ), DEFAULTPATHLEN );
	strncpy( XSettings->xs_PAT_RecheckPath, xFindString( pbuffer, psize, "RECHECKPATH", "RAM:RecheckFiles" ), DEFAULTPATHLEN );
	XSettings->xs_PAT_AutoCreateDrawers			= xFindNumber( pbuffer, psize, "AUTOCREATEDRAWERS", FALSE );
	XSettings->xs_PAT_AutoDeleteDrawers			= xFindNumber( pbuffer, psize, "AUTODELETEDRAWERS", FALSE );
	strncpy( XSettings->xs_PAT_GuidePath, xFindString( pbuffer, psize, "GUIDEPATH", "PROGDIR:" ), DEFAULTPATHLEN );
	
	strncpy( XSettings->xs_STA_StartupScript, xFindString( pbuffer, psize, "STARTUPSCRIPT", "Rexx/Xtruder/Startup.rexx" ), DEFAULTBUFFERLEN );
	XSettings->xs_STA_ExecuteScript					= xFindNumber( pbuffer, psize, "EXECUTESCRIPT", FALSE );
	XSettings->xs_STA_CheckPrespecified				= xFindNumber( pbuffer, psize, "CHECKPRESPECIFIED", FALSE );
	XSettings->xs_STA_CheckVectors					= xFindNumber( pbuffer, psize, "CHECKVECTORS", FALSE );
	XSettings->xs_STA_ShowSystemInformation			= xFindNumber( pbuffer, psize, "SHOWSYSTEMINFORMATION", FALSE );
	
	strncpy( XSettings->xs_REP_ReportsPath, xFindString( pbuffer, psize, "REPORTSPATH", "" ), DEFAULTBUFFERLEN );
	
		/// Virus information
	
	strncpy( XSettings->xs_VIN_DescriptionsPath, xFindString( pbuffer, psize, "DESCRIPTIONSPATH", "" ), DEFAULTPATHLEN );
	strncpy( XSettings->xs_VIN_ExternalViewer, xFindString( pbuffer, psize, "EXTERNALVIEWER", "" ), DEFAULTBUFFERLEN );
	
	for( a = 0; a < 3; a++ )
		{
		xSPrintF( strbuf1, "VIBSEARCHSTRING%ld", a + 1 );
		
		strncpy( XStrings->xs_SearchStrings[ a ], xFindString( pbuffer, psize, strbuf1, "" ), 100 );
		}
	
		/// Get coordinates for windows
	
	xFindCoords( pbuffer, psize, Coords->xc_MainWindow, 2, "MAINWIN" );
	
	xFindCoords( pbuffer, psize, Coords->xc_BootblockWindow, 4, "BOOTBLOCKWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_BootblockSelectWindow, 2, "BOOTBLOCKSELECTWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_CheckWindow, 4, "CHECKWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_HistoryWindow, 4, "HISTORYWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_ListKnownBootblockViraWindow, 4, "BOOTBLOCKVIRAWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_ListKnownFileBasedViraWindow, 4, "FILEBASEDVIRAWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_ListKnownCrunchersWindow, 4, "CRUNCHERSWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_ListKnownPatchesWindow, 4, "PATCHESWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_ListKnownFileTypesWindow, 4, "FILETYPESWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_ListPrespecifiedWindow, 4, "LISTPRESPECIFIEDWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_MemoryLibrariesWindow, 4, "LIBRARIESWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_MemoryDevicesWindow, 4, "DEVICESWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_MemoryResourcesWindow, 4, "RESOURCESWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_MemoryPortsWindow, 4, "PORTSWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_MemorySemaphoresWindow, 4, "SEMAPHORESWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_MemoryTasksWindow, 4, "TASKSWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_MemoryLVOWindow, 4, "MEMORYLVOWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_MemoryVectorsWindow, 4, "MEMORYVECTORSWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_ReportsMainWindow, 4, "REPORTSMAINWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_ReportsCreateWindow, 4, "REPORTSCREATEWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_ReportsCheckWindow, 4, "REPORTSCHECKWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_SystemInformationWindow, 4, "SYSTEMINFORMATIONWIN" );
	xFindCoords( pbuffer, psize, Coords->xc_VirusInformationWindow, 4, "VIRUSINFORMATIONWIN" );
	
		/// Get palette
	
	strncpy( cols, xFindString( pbuffer, psize, "PALETTE", "" ), 50 );
	
		/// If AGA
	
	if( GfxBase->lib_Version >= 39 )
		{
		if( *cols )
			{
			ULONG	t;
			
			
			str = stptok( cols, color, 5, " " );
			
			for( a = 1; a < 13; a++ )
				{
				t = atol( color );
				
					/// Skip spaces
				
				*str++;
				
				str = stptok( str, color, 5, " " );
				
					/// Shift so we get the real AGA value
				
				t |= t << 8;
				t |= t << 16;
				
				ColorTable[a] = t;
				}
			}
		}
	else
		{
		if( *cols )
			{
			UWORD	col,col2;
			
			
				/// 4 color entries
			
			str = stptok( cols, color, 5, " " );
			
			for( a = 0; a < 4; a++ )
				{
				col = atol( color );
				
				*str++;
				str = stptok( str, color, 5, " " );
				
				col2 = col & 0x0F00;
				ColorSpec[a].Red   = col2 >> 8;
				col2 = col & 0x00F0;
				ColorSpec[a].Green = col2 >> 4;
				col2 = col & 0x000F;
				ColorSpec[a].Blue  = col2;
				}
			}
		}
	
		/// Free current nodes
	
	xFreePrespecifiedNodes( &PreList->pl_List );
	
	NewList( &PreList->pl_List );
	
		/// Get the prespecified dirs/files
	
	e = 1;
	
	while( e )
		{
		xSPrintF( color, "P%ld", e );
		
		str = xFindString( pbuffer, psize, color, "*" );
		
		if( (*str == '*') && (strlen( str ) == 1) )
			break;
		else
			{
			if( xAddPrespecifiedNode( &PreList->pl_List, str ) )
				PreList->pl_Entries++;
			
			e++;
			}
		}
	
	e = 1;
	
		/// Free current nodes
	
	xFreeARexxScriptNodes( ARexxScriptsList );
	
	NewList( ARexxScriptsList );
	
	while( e )
		{
		xSPrintF( color, "AM%ld", e );
		
		if( str = xFindString( pbuffer, psize, color, NULL ) )
			{
			strcpy( strbuf1, str );
			
			xSPrintF( color, "AF%ld", e );
			
			if( str = xFindString( pbuffer, psize, color, NULL ) )
				{
				strcpy( strbuf2, str );
				
				xSPrintF( color, "AB%ld", e );
				
				if( (sep = xFindNumber( pbuffer, psize, color, -1 )) != -1 )
					{
					xAddARexxScriptNode( ARexxScriptsList, strbuf1, strbuf2, sep );
					
					e++;
					}
				else
					break;
				}
			else
				break;
			}
		else
			break;
		}
}

/*
 * Find an entry with a string
 */
STRPTR xFindString( UBYTE *pbuffer, ULONG psize, STRPTR entry, STRPTR defaultstring )
{
	UBYTE *	p = pbuffer;
	UBYTE *	pend = pbuffer + psize;
	BOOL	eq = FALSE;
	BOOL	ten = FALSE;
	LONG	c;
	
	
	if( pbuffer )
		{
		for( p; p < pend; p++ )
			{
			if( !xCmpEntry( p, entry, strlen( entry ) ) )
				{
					/// Find the string
				
				for( p; p < pend; p++ )
					{
					if( *p == '=' )
						{
						eq = TRUE;
						
						break;
						}
					
					if( *p == 10 )
						{
						ten = TRUE;
						
						break;
						}
					}
				
				if( !eq && !ten )
					return( defaultstring );
				
				if( eq )
					{
					*p++;
					
					c = -1;
					
					while( (*p != 10) && (p < pend) && (c++ < DEFAULTBUFFERLEN) )
						STRBUF2[c] = *p++;
					
					STRBUF2[c+1] = 0;
					
					return( STRBUF2 );
					}
				}
			else
				{
				for( p; p < pend; p++ )
					{
					if( *p == 10 )
						break;
					}
				
				if( p == pend )
					return( defaultstring );
				}
			}
		}
	
	return( defaultstring );
}

/*
 * Find an entry with a number
 */
LONG xFindNumber( UBYTE *pbuffer, ULONG psize, STRPTR entry, LONG defaultval )
{
	UBYTE *	p = pbuffer;
	UBYTE *	pend = pbuffer + psize;
	LONG	c;
	BOOL	eq = FALSE;
	BOOL	ten = FALSE;
	
	
	if( pbuffer )
		{
		for( p; p < pend; p++ )
			{
			if( !xCmpEntry( p, entry, strlen( entry ) ) )
				{
					/// Find the string
				
				for( p; p < pend; p++ )
					{
					if( *p == '=' )
						{
						eq = TRUE;
						
						break;
						}
					
					if( *p == 10 )
						{
						ten = TRUE;
						
						break;
						}
					}
				
				if( !eq && !ten )
					return( defaultval );
				
				if( eq )
					{
					*p++;
					
					c = -1;
					
					while( (*p != 10) && (c++ < DEFAULTBUFFERLEN) )
						STRBUF2[c] = *p++;
					
					STRBUF2[c+1] = 0;
					
					return( atol( STRBUF2 ) );
					}
				}
			else
				{
				for( p; p < pend; p++ )
					{
					if( *p == 10 )
						break;
					}
				
				if( p == pend )
					return( defaultval );
				}
			}
		}
	
	return( defaultval );
}

LONG xFindNumberLimits( UBYTE *pbuffer, ULONG psize, STRPTR entry, LONG defaultval, LONG min, LONG max )
{
	LONG	p = xFindNumber( pbuffer, psize, entry, defaultval );
	
	
	p = p < min ? min : p > max ? max : p;
	
	return( p );
}

/*
 * Compare entry with a line
 */
LONG xCmpEntry( STRPTR str1, STRPTR str2, LONG length )
{
	while( (*str1++ == *str2++) && (length--) )
		;
	
	return( length || !(*(str1-1) == '=') );
}

VOID xFindCoords( UBYTE *pbuffer, ULONG psize, UWORD *coords, UBYTE numcoords, STRPTR strid )
{
	UBYTE	c;
	UBYTE *	tok;
	UBYTE	strbuf[ DEFAULTBUFFERLEN ] = "";
	
	
	coords[0] = 50;
	coords[1] = 30;
	
	if( numcoords > 2 )
		{
		coords[2] = 520;
		coords[3] = 130;
		}
	
	if( tok = xFindString( pbuffer, psize, strid, NULL ) )
		{
		for( c = 0; c < numcoords; c++ )
			{
			UWORD	number;
			
			
			tok = stptok( tok, strbuf, 40, " " );
			
			number = atol( strbuf ) < 0 ? coords[c] : atol( strbuf );
			
			coords[c] = __builtin_min( number, 32760 );
			
			if( !*tok )
				break;
			
			tok = stpblk( ++tok );
			}
		}
}
