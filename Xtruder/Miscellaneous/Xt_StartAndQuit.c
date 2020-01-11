/*
**	$VER: Xt_StartAndQuit.c (11.11.96)
**
**	Xtruder start and quit functions
**
**	Copyright (C) 1993-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


struct LibTab
{
	STRPTR				lt_Name;
	WORD				lt_Version;
	struct Library **	lt_Lib;
};

/*
 * Opens and allocates all resources possible. Needed for a full Xtruder.
 */
VOID xOpenFullSystem( VOID )
{
		/// Libraries

	if( !xOpenLibraries() )
		xCloseSystem( 0, RETURN_FAIL, 0 );

		/// Fonts

	xInitializeFonts();

		/// Allocate miscellaneous

	if( !xAllocMiscMemory() )
		xCloseSystem( MSG_NOTENOUGHMEMORY, RETURN_FAIL, 0 );

		/// Initialize miscellaneous

	if( !xInitializeMiscellaneous() )
		xCloseSystem( 0, RETURN_FAIL, 0 );

		/// Read keyfile

	xReadKeyfile();

		/// Load settings

	xLoadSettings( FALSE );

		/// Create AppIcon

	if( !XSettings->xs_GUI_HideAppIcon )
		xCreateAppIcon();

		/// Create unpack paths etc.

	xCreatePaths();

		/// Load Bootblock library's brainfile

	xReadBBBrainFile();

		/// Create a commodity for Xtruder

	if( !xCreateCommodity() )
		{
		xRemoveCommodity();

		XSettings->xs_MIS_CxPopup = TRUE;
		}

		/// Set up the ARexx interface

	RexxStuff = xInitARexx( "XTRUDER", "" );

	ARexxSignal = xARexxSignal( RexxStuff );
}

VOID xOpenSmallSystem( VOID )
{
		/// Libraries

	if( !xOpenLibraries() )
		xCloseSystem( 0, RETURN_FAIL, 0 );

		/// Fonts

	xInitializeFonts();

		/// Allocate miscellaneous

	if( !xAllocMiscMemory() )
		xCloseSystem( MSG_NOTENOUGHMEMORY, RETURN_FAIL, 0 );

		/// Initialize miscellaneous

	if( !xInitializeMiscellaneous() )
		xCloseSystem( 0, RETURN_FAIL, 0 );
}

VOID xCloseSystem( UWORD errmsgid, UWORD exitcode, UWORD quitmode )
{
	struct Message *	msg;


		/// Error?

	if( errmsgid && ReqToolsBase )
		xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( errmsgid ), GCS( MSG_GF_OKAY ) );

		/// Settings changed? Maybe save... maybe not..

	if( (quitmode & QUIT_ASK_SETTINGS) && SettingsChanged )
		{
		UBYTE *	str = GCS( MSG_GF_SETTINGSHAVECHANGED );
		UWORD	r,b;


			/// If we can't return we must remove the 'Cancel' string

		if( !(quitmode & QUIT_RETURN_POSSIBLE) )
			{
			for( r = 0, b = 0; r < strlen( str ); r++ )
				{
				if( str[r] == '|' )
					{
					b++;

					if( b == 2 )
						str[r] = '\0';
					}
				}
			}

		r = xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_SETTINGSHAVECHANGED ), str );

		if( r == 1 )
			xSaveSettings( FALSE, FALSE );
		else
		if( (quitmode & QUIT_RETURN_POSSIBLE) && (r == 0) )
			return;
		}

		/// Auto delete paths

	if( !SecondCopy && XSettings->xs_PAT_AutoDeleteDrawers )
		xAutoDeletePaths();

		/// Online help process

	xGuideCleanup();

		/// App icon

	xRemoveAppIcon();

		/// GUI

	xCloseInterface();

		/// Cx process

	xRemoveCommodity();

		/// Remove the ARexx interface

	xFreeARexx( RexxStuff );

		/// Free IO units

	xFreeTrackUnits();

		/// Misc memory

	xFreeMiscMemory();

		/// Libraries

	xCloseLibraries();

		/// Close port

	if( XtruderCPort )
		{
		RemPort( XtruderCPort );

		while( msg = GetMsg( XtruderCPort ) )
			ReplyMsg( msg );

		DeleteMsgPort( XtruderCPort );
		}

	exit( exitcode );
}

BOOL xOpenLibraries( VOID )
{
	struct LibTab lt[16] =
	{
		{"intuition.library",	37,(struct Library **)&IntuitionBase},
		{"graphics.library",	37,(struct Library **)&GfxBase},
		{"gadtools.library",	37,(struct Library **)&GadToolsBase},
		{"utility.library",		37,(struct Library **)&UtilityBase},
		{"commodities.library",	37,(struct Library **)&CxBase},
		{"icon.library",		37,(struct Library **)&IconBase},
		{"workbench.library",	37,(struct Library **)&WorkbenchBase},
		{"keymap.library",		37,(struct Library **)&KeymapBase},
		{"diskfont.library",	36,(struct Library **)&DiskfontBase},
		{"filevirus.library",	 2,(struct Library **)&FilevirusBase},
		{"Bootblock.library",	 1,(struct Library **)&BootblockBase},
		{"unpack.library",		41,(struct Library **)&UnpackBase},
		{"xfdmaster.library",	36,(struct Library **)&xfdMasterBase},
		{"FileID.library",		 7,(struct Library **)&FileIDBase},
		{"xtruder.library",		37,(struct Library **)&XtruderBase},
		{"reqtools.library",	38,(struct Library **)&ReqToolsBase},
	};
	UBYTE	strbuf1[ 512 ] = "";
	UBYTE	strbuf2[ 128 ] = "";
	UBYTE	a;
	UBYTE	length = strlen( GCS( MSG_YOUNEEDTHESELIBS ) );


		/// Open libraries...

	strncpy( strbuf1, GCS( MSG_YOUNEEDTHESELIBS ), DEFAULTBUFFERLEN );

		/// AmigaGuide library forgets to release something of size 96 bytes!!!

	AmigaGuideBase = OpenLibrary( "amigaguide.library", 34 );
	DataTypesBase = OpenLibrary( "datatypes.library", 39 );

		/// Go through the library table and open required libraries or complain about missing ones.

	for( a = 0; a < 16; a++ )
		{
		if( !(*(lt[a].lt_Lib) = OpenLibrary( lt[a].lt_Name, lt[a].lt_Version ) ) )
			{
			xSPrintF( strbuf2, GCS( MSG_INFOLIBVERSION ), lt[a].lt_Name, lt[a].lt_Version );
			strncat( strbuf1, strbuf2, DEFAULTBUFFERLEN );
			}
		}

		/// Check if an error was added to the buffer

	if( strlen( strbuf1 ) > length )
		{
		xReqMsg( Window, GCS( MSG_REQ_ERROR ), strbuf1, GCS( MSG_GF_OKAY ) );

		return( FALSE );
		}

	return( TRUE );
}

VOID xCloseLibraries( VOID )
{
	if( ReqToolsBase )			CloseLibrary( (struct Library *)ReqToolsBase );
	if( XtruderBase )			CloseLibrary( XtruderBase );
	if( FileIDBase )			CloseLibrary( FileIDBase );
	if( xfdMasterBase )			CloseLibrary( xfdMasterBase );
	if( UnpackBase )			CloseLibrary( UnpackBase );
	if( BootblockBase )			CloseLibrary( BootblockBase );
	if( FilevirusBase )			CloseLibrary( FilevirusBase );
	if( DiskfontBase )			CloseLibrary( DiskfontBase );
	if( KeymapBase )			CloseLibrary( KeymapBase );
	if( WorkbenchBase )			CloseLibrary( WorkbenchBase );
	if( IconBase )				CloseLibrary( IconBase );
	if( CxBase )				CloseLibrary( CxBase );
	if( UtilityBase )			CloseLibrary( UtilityBase );
	if( GadToolsBase )			CloseLibrary( GadToolsBase );
	if( GfxBase )				CloseLibrary( GfxBase );
	if( IntuitionBase )			CloseLibrary( IntuitionBase );

	if( DataTypesBase )			CloseLibrary( DataTypesBase );
	if( AmigaGuideBase )		CloseLibrary( AmigaGuideBase );
}

/*
 * Initializes the fonts to use the system font instead of the topaz 8
 */
VOID xInitializeFonts( VOID )
{
	ScreenFontAttr.ta_Name = ((struct GfxBase *)GfxBase)->DefaultFont->tf_Message.mn_Node.ln_Name;
	ScreenFontAttr.ta_YSize = ((struct GfxBase *)GfxBase)->DefaultFont->tf_YSize;

	TextFontAttr.ta_Name = ((struct GfxBase *)GfxBase)->DefaultFont->tf_Message.mn_Node.ln_Name;
	TextFontAttr.ta_YSize = ((struct GfxBase *)GfxBase)->DefaultFont->tf_YSize;

	RequesterFontAttr.ta_Name = ((struct GfxBase *)GfxBase)->DefaultFont->tf_Message.mn_Node.ln_Name;
	RequesterFontAttr.ta_YSize = ((struct GfxBase *)GfxBase)->DefaultFont->tf_YSize;
}

/*
 * All libraries have opened successfully if this function is called!
 */
BOOL xAllocMiscMemory( VOID )
{
		/// Xtruder settings

	if( !(XSettings = AllocVec( sizeof( struct XtruderSettings ), MEMF_CLEAR )) )
		return( FALSE );

		/// Misc string buffers

	if( !(XStrings = AllocVec( sizeof( struct XtruderStrings ), MEMF_CLEAR )) )
		return( FALSE );

		/// Window coordinates

	if( !(Coords = AllocVec( sizeof( struct XtruderWinCoords ), MEMF_CLEAR )) )
		return( FALSE );

		/// Global Xtruder history list

	if( !(XtruderHistory = AllocVec( sizeof( struct XtruderHistory ), MEMF_CLEAR )) )
		return( FALSE );

	NewList( &XtruderHistory->his_List );

		/// Prespecified entries list

	if( !(PreList = AllocVec( sizeof( struct PrespecifiedList ), MEMF_CLEAR )) )
		return( FALSE );

	NewList( &PreList->pl_List );

		/// ARexx scripts list

	if( !(ARexxScriptsList = AllocVec( sizeof( struct List ), MEMF_CLEAR )) )
		return( FALSE );

	NewList( ARexxScriptsList );

		/// List of vira

	if( !(VirusList = AllocVec( sizeof( struct List ), MEMF_CLEAR )) )
		return( FALSE );

	NewList( VirusList );

		/// filevirus.library

	if( !(FilevirusNode = fvAllocNode()) )
		return( FALSE );

		/// unpack.library

	if( !(UInfo = upAllocCInfo()) )
		return( FALSE );

		/// FileID.library

	if( !(FInfo = FIAllocFileInfo()) )
		return( FALSE );

		/// xtruder.library

	if( !(XInfo = xlAllocXtruderInfo()) )
		return( FALSE );

		/// xfdmaster.library

	if( !(XFDInfo = xfdAllocObject( XFDOBJ_BUFFERINFO )) )
		return( FALSE );

		/// reqtools.library

	if( !(FileReq = rtAllocRequestA( RT_FILEREQ, NULL )) )
		return( FALSE );

	return( TRUE );
}

VOID xFreeMiscMemory( VOID )
{
	if( FileReq )
		rtFreeRequest( FileReq );

	if( XFDInfo )
		xfdFreeObject( XFDInfo );

	if( XInfo )
		xlFreeXtruderInfo( XInfo );

	if( FInfo )
		FIFreeFileInfo( FInfo );

	if( UInfo )
		upFreeCInfo( UInfo );

	if( FilevirusNode )
		fvFreeNode( FilevirusNode );

		/// Vira list

	xCHK_FlushVirusList();
	FreeVec( VirusList );

		/// ARexx scripts for the menu

	xFreeARexxScriptNodes( ARexxScriptsList );
	FreeVec( ARexxScriptsList );

		/// Prespecified entries

	xFreePrespecifiedNodes( &PreList->pl_List );
	FreeVec( PreList );

		/// Global history

	if( XtruderHistory )
		{
		xFreeNameNodes( &XtruderHistory->his_List );

		FreeVec( XtruderHistory );
		}

	FreeVec( Coords );
	FreeVec( XStrings );
	FreeVec( XSettings );
}

BOOL xInitializeMiscellaneous( VOID )
{
	InitSemaphore( &GUISemaphore );

	return( TRUE );
}

VOID xFreeMiscellaneous( VOID )
{
}

VOID xReadKeyfile( VOID )
{
	UBYTE	strbuf1[ 512 ] = "";
	LONG	len;
	ULONG	k = 0;
	UWORD	a;
	UBYTE	buffer[ 10 ];


		/// Load keyfile

	if( (len = GetVar( "Xtruder/KEYLOCATION", strbuf1, 250, GVF_GLOBAL_ONLY ) ) == -1 )
		{
			/// Try and see if it
			/// exists in the CD

		if( len = xFileSize( "KEYLOCATION" ) )
			{
			if( len < 250 )
				{
				UBYTE *	buf;


				if( buf = xStoreFile( "KEYLOCATION", len ) )
					{
					for( a = 0; a < len; a++ )
						{
						if( buf[a] == '\n' )
							break;

						strbuf1[a] = buf[a];
						}

					strbuf1[a] = 0;

					FreeVec( buf );
					}
				}
			else
				len = 0;
			}
		}

	if( len <= 0 )
		strcpy( strbuf1, "L:Xtruder.key" );
	else
		AddPart( strbuf1, "Xtruder.key", 250 );

	if( RegFile = Open( strbuf1, MODE_OLDFILE ) )
		{
		if( Read( RegFile, XStrings->xs_RegName, 500 ) == 500 )
			k = 1;
		}

	strncpy( buffer, XStrings->xs_RegName, 8 );

	if( RegFile && k )
		{
		UWORD	a;
		ULONG	cs;


		cs = ((ULONG *)XStrings->xs_RegName)[496/4];

		for( a = 0, k = 0; a < 490; a++ )
			k += XStrings->xs_RegName[a] * XStrings->xs_RegName[a] + 5;

		if( k == cs )
			{
			strcpy( RegName, &XStrings->xs_RegName[256] );

			KeyWord = 1;

			Close( RegFile );
			}
		else
			exit( 0 );
		}
}

/*
 * This function deletes the 4 check paths if they are empty
 */
VOID xAutoDeletePaths( VOID )
{
	BPTR				lock;
	UBYTE *				paths[4];
	UBYTE				p;


	paths[0] = XSettings->xs_PAT_ArchiveExtractPath;
	paths[1] = XSettings->xs_PAT_FileDecrunchPath;
	paths[2] = XSettings->xs_PAT_VirusPath;
	paths[3] = XSettings->xs_PAT_RecheckPath;

	for( p = 0; p < 4; p++ )
		{
		if( paths[ p ][ 0 ] )
			{
			if( lock = Lock( paths[ p ], ACCESS_READ ) )
				{
				UnLock( lock );

				DeleteFile( paths[ p ] );
				}
			}
		}
}
