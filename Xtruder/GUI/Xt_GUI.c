/*
**	$VER: XtruderGUI.c (24.7.96)
**
**	Xtruder graphical user interface functions
**
**	Copyright (C) 1993-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


UWORD xOpenInterface( VOID )
{
	UWORD	error = 0;
	
	
		/// Gain exclusive access to Screen and Window ptr
	
	ObtainSemaphore( &GUISemaphore );
	
	if( !Screen )
		{
		xCreateMenus();
		
			/// Open required fonts...
		
		xOpenXtruderFonts();
		
			/// Should we open Xtruder on a public screen?
		
		if( XSettings->xs_GUI_PublicScreenName[0] )
			{
			Screen = LockPubScreen( XSettings->xs_GUI_PublicScreenName );
			
			PublicScrLocked = OnPublic = Screen ? TRUE : FALSE;
			}
		
		if( !Screen )
			{
			BYTE	try = 3;
			
			
			if( ModeNotAvailable( XSettings->xs_GUI_ScreenMode ) )
				XSettings->xs_GUI_ScreenMode = 0;
			
				/// Open screen
			
			while( !Screen && try )
				{
				Screen = OpenScreenTags( NULL,
					XSettings->xs_GUI_ScreenMode ? SA_DisplayID : TAG_IGNORE, XSettings->xs_GUI_ScreenMode,
					XSettings->xs_GUI_ScreenMode ? SA_Overscan  : TAG_IGNORE, XSettings->xs_GUI_ScreenOverscan,
					SA_Width,		XSettings->xs_GUI_ScreenMode ? XSettings->xs_GUI_ScreenWidth : -1,
					SA_Height,		XSettings->xs_GUI_ScreenMode ? XSettings->xs_GUI_ScreenHeight : -1,
					SA_Type,		PUBLICSCREEN,
					SA_Depth,		XSettings->xs_GUI_ScreenDepth,
					SA_AutoScroll,	XSettings->xs_GUI_AutoScroll,
					SA_Pens,		(ULONG)Pens,
					SA_SharePens,	TRUE,
					SA_Font,		&ScreenFontAttr,
					SA_Colors,		ColorSpec,
					SA_Colors32,	ColorTable,
					SA_PubName,		"XTRUDER",
					SA_Title,		ScreenTitle,
					SA_Interleaved,	TRUE,
					TAG_END );
				
				if( !Screen )
					{
					DisplayBeep( NULL );
					
					XSettings->xs_GUI_ScreenWidth = XSettings->xs_GUI_ScreenHeight = -1;
					
					if( try == 2 )
						XSettings->xs_GUI_ScreenMode = HIRES_KEY;
					else
					if( try == 1 )
						XSettings->xs_GUI_ScreenMode = 0;
					}
				
				try--;
				}
			}
		
		if( Screen )
			{
			if( !OnPublic )
				PubScreenStatus( Screen, 0 );
			
			if( VisualInfo = GetVisualInfo( Screen, TAG_DONE ) )
				{
				if( DrawInfo = GetScreenDrawInfo( Screen ) )
					{
					if( ButtonClass = xInitButtonGadgetClass() )
						{
							/// Load background pattern
						
						LoadBackgroundImage( &BFInfo, XSettings->xs_GUI_BackgroundPattern, Screen, &Options );
						
							/// Open main window
						
						if( Window = OpenWindowTags( NULL,
							WA_Left,			OnPublic ? Coords->xc_MainWindow[0] : 0,
							WA_Top,				OnPublic ? Coords->xc_MainWindow[1] : Screen->BarHeight + 1,
							WA_Width,			OnPublic ? xTextLength( ScreenFont, "123456789012345678901234567890" ) : Screen->Width,
							WA_Height,			OnPublic ? Screen->Font->ta_YSize + 3 : Screen->Height - Screen->BarHeight - 1,
							WA_IDCMP,			IDCMP_REFRESHWINDOW |
												IDCMP_MOUSEMOVE |
												IDCMP_GADGETUP |
												IDCMP_MENUPICK |
												IDCMP_RAWKEY |
												IDCMP_DISKINSERTED |
												IDCMP_DISKREMOVED |
												IDCMP_MENUHELP |
												BUTTONIDCMP |
												(OnPublic ? IDCMP_CLOSEWINDOW : 0),
							WA_Activate,		TRUE,
							WA_SmartRefresh,	TRUE,
							WA_NoCareRefresh,	TRUE,
							WA_NewLookMenus,	TRUE,
							WA_MenuHelp,		TRUE,
							OnPublic ? WA_PubScreen		: WA_CustomScreen,	Screen,
							OnPublic ? TAG_IGNORE		: WA_Backdrop,		TRUE,
							OnPublic ? TAG_IGNORE		: WA_Borderless,	TRUE,
							OnPublic ? WA_Title			: TAG_IGNORE,		"ARexx port: XTRUDER",
							OnPublic ? WA_DragBar		: TAG_IGNORE,		TRUE,
							OnPublic ? WA_CloseGadget	: TAG_IGNORE,		TRUE,
							OnPublic ? WA_DepthGadget	: TAG_IGNORE,		TRUE,
							OnPublic || !BFInfo.PictureObject || !DataTypesBase ? TAG_IGNORE : WA_BackFill, &BFInfo,
							TAG_DONE ) )
							{
							SetFont( RastPort = Window->RPort, TextFont );
							
							xCalculateConstants();
							
							if( OnPublic )
								if( AppWinPort = CreateMsgPort() )
									AppWin = AddAppWindowA( 1, 0, Window, AppWinPort, NULL );
							
							if( xSetMenus() )
								{
								OldWin = Process->pr_WindowPtr;
								Process->pr_WindowPtr = (APTR)Window;
								
								xFindPresentDisks();
								
								if( !CommodityProcess )
									OffMenu( Window, FULLMENUNUM( 0, 0, NOSUB ) );
								
								if( OnPublic )
									WindowToFront( Window );
								
								WindowSignal = 1 << Window->UserPort->mp_SigBit;
								
								if( AppWin )
									AppWinSignal = 1 << AppWinPort->mp_SigBit;
								
									/// Get available trackdisk units
								
								xGetTrackUnits();
								}
							else
								error = MSG_COULDNOTCREATEGUI;
							}
						else
							error = MSG_COULDNOTOPENWINDOW;
						}
					}
				}
			else
				error = MSG_COULDNOTCREATEGUI;
			}
		else
			error = MSG_COULDNOTOPENSCREEN;
		}
	
	ReleaseSemaphore( &GUISemaphore );
	
	return( error );
}

VOID xCloseInterface( VOID )
{
	struct Message *	msg;
	BOOL				morewins = TRUE;
	
	
		/// Gain exclusive access to Screen and Window ptr
	
	ObtainSemaphore( &GUISemaphore );
	
	if( Window )
		{
		if( !OnPublic )
			{
				/// Check for visitor windows...
			
			if( Screen->FirstWindow->NextWindow )
				{
				while( morewins )
					{
					xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_CLOSEVISITORWINDOWS ), GCS( MSG_GF_OKAY ) );
					
					if( Screen->FirstWindow->NextWindow == NULL )
						morewins = FALSE;
					}
				}
			}
		
		xRemoveMenus();
		
		Process->pr_WindowPtr = OldWin;
		
			/// Remove App window
		
		if( AppWin )
			{
			RemoveAppWindow( AppWin );
			AppWin = NULL;
			AppWinSignal = 0;
			}
		
        	/// Make sure there are no more outstanding messages on the app msgport
        
        if( AppWinPort )
        	{
			while( msg = GetMsg( AppWinPort ) )
				ReplyMsg( msg );
	        
			DeleteMsgPort( AppWinPort );
	       	AppWinPort = NULL;
	       	}
       	
       		/// Store size and position of window on public screen
       	
       	if( OnPublic )
       		{
			Coords->xc_MainWindow[0] = Window->LeftEdge;
			Coords->xc_MainWindow[1] = Window->TopEdge;
			}
		
		CloseWindow( Window );
		Window = NULL;
		RastPort = NULL;
		
		WindowSignal = 0;
		}
	
	UnloadBackgroundImage( &BFInfo );
	
	if( ButtonClass )
		{
		FreeClass( ButtonClass );
		
		ButtonClass = NULL;
		}
	
	if( Screen )
		{
		FreeScreenDrawInfo( Screen, DrawInfo );
		DrawInfo = NULL;
		
		FreeVisualInfo( VisualInfo );
		VisualInfo = NULL;
		
		if( OnPublic )
			UnlockPubScreen( NULL, Screen );
		else
			CloseScreen( Screen );
		
		Screen = NULL;
		
		OnPublic = FALSE;
		}
	
		/// Close fonts...
	
	if( ScreenFont )
		{
		CloseFont( ScreenFont );
		
		ScreenFont = NULL;
		}
	
	if( TextFont )
		{
		CloseFont( TextFont );
		
		TextFont = NULL;
		}
	
	if( RequesterFont )
		{
		CloseFont( RequesterFont );
		
		RequesterFont = NULL;
		}
	
	ReleaseSemaphore( &GUISemaphore );
}

/*
 * Calculate some constants for faster program execution...
 */
VOID xCalculateConstants( VOID )
{
	ScrFontYSize = ScreenFont->tf_YSize;
	
	GadgetHeight = ScrFontYSize + 6;
	
	TxtFontYSize = TextFont->tf_YSize;
	TxtFontXSize = TextFont->tf_XSize;
	
	ConfirmGadWidth = 10 + xMaxStringLength( ScreenFont, MSG_GAD_OKAY, MSG_GAD_CANCEL );
	
	if( OnPublic )
		{
		ScreenBarHeight = Screen->Font->ta_YSize + 3;
		}
	else
		{
		ScreenBarHeight = ScrFontYSize + 3;
		
			/// Box dimensions (left,top,width,height)
		
		ActionBoxL		= 2;
		ActionBoxT		= 1;
		ActionBoxW		= Window->Width - 5;
		ActionBoxH		= Window->Height - ActionBoxT - 2;
		
			/// RectFill coords
		
		ActionRecX0		= 4;
		ActionRecY0		= ActionBoxT + 1;
		ActionRecX1		= Window->Width - 6;
		
			/// Misc
		
		WriteX			= 6;
		
		MaxYChars		= (ActionBoxH - 2) / TxtFontYSize;
		MaxXChars		= (ActionBoxW - 4) / TxtFontXSize;
		//ActionBoxH		= MaxYChars * TxtFontYSize + 2 + 2;
		ActionRecY1		= ActionRecY0 + ActionBoxH - 3;
		
		ActionWidth		= ActionRecX1 - ActionRecX0;
		ActionHeight	= ActionRecY1 - ActionRecY0;
		}
	
	CurrentWriteY = InitialWriteY = ActionRecY0 + 1 + TextFont->tf_Baseline;
	MaxWriteY = CurrentWriteY + MaxYChars * TextFont->tf_YSize;
}

/*
 * Open the fonts which Xtruder use
 */
VOID xOpenXtruderFonts( VOID )
{
	BOOL	error = FALSE;
	
	
	if( ScreenFontAttr.ta_YSize = XSettings->xs_GUI_ScreenFontSize )
		{
		ScreenFontAttr.ta_Style = XSettings->xs_GUI_ScreenFontStyle;
		ScreenFontAttr.ta_Name = XSettings->xs_GUI_ScreenFontName;
		}
	else
		{
		ScreenFontAttr.ta_YSize = 8;
		ScreenFontAttr.ta_Style = 0;
		ScreenFontAttr.ta_Name = "topaz.font";
		}
	
		/// We couldn't open screen font...
	
	if( !(ScreenFont = OpenDiskFont( &ScreenFontAttr ) ) )
		{
		xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_COULDNOTOPENFONT ), GCS( MSG_GF_OKAY ), XSettings->xs_GUI_ScreenFontName, XSettings->xs_GUI_ScreenFontSize );
		
		ScreenFontAttr.ta_YSize = 8;
		ScreenFontAttr.ta_Style = 0;
		ScreenFontAttr.ta_Name = "topaz.font";
		
		if( !(ScreenFont = OpenDiskFont( &ScreenFontAttr ) ) )
			xCloseSystem( 0, RETURN_FAIL, QUIT_ASK_SETTINGS );
		}
	
	if( TextFontAttr.ta_YSize = XSettings->xs_GUI_TextFontSize )
		TextFontAttr.ta_Name = XSettings->xs_GUI_TextFontName;
	else
		{
		TextFontAttr.ta_YSize = 8;
		TextFontAttr.ta_Name = "topaz.font";
		}
	
		/// We couldn't open text font...
	
	if( TextFont = OpenDiskFont( &TextFontAttr ) )
		{
		if( TextFont->tf_Flags & FPF_PROPORTIONAL )
			{
			error = TRUE;
			
			CloseFont( TextFont );
			TextFont = NULL;
			}
		}
	else
		error = TRUE;
	
	if( error )
		{
		xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_COULDNOTOPENFONT ), GCS( MSG_GF_OKAY ), XSettings->xs_GUI_TextFontName, XSettings->xs_GUI_TextFontSize );
		
		TextFontAttr.ta_YSize = 8;
		TextFontAttr.ta_Style = 0;
		TextFontAttr.ta_Name = "topaz.font";
		
		if( !(TextFont = OpenDiskFont( &TextFontAttr )) )
			xCloseSystem( 0, RETURN_FAIL, QUIT_ASK_SETTINGS );
		}
	
	if( RequesterFontAttr.ta_YSize = XSettings->xs_GUI_RequesterFontSize )
		{
		RequesterFontAttr.ta_Style = XSettings->xs_GUI_RequesterFontStyle;
		RequesterFontAttr.ta_Name = XSettings->xs_GUI_RequesterFontName;
		}
	else
		{
		RequesterFontAttr.ta_YSize = 8;
		RequesterFontAttr.ta_Style = 0;
		RequesterFontAttr.ta_Name = "topaz.font";
		}
	
		/// We couldn't open requester font...
	
	if( !(RequesterFont = OpenDiskFont( &RequesterFontAttr ) ) )
		{
		xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_COULDNOTOPENFONT ), GCS( MSG_GF_OKAY ), XSettings->xs_GUI_RequesterFontName, XSettings->xs_GUI_RequesterFontSize );
		
		RequesterFontAttr.ta_YSize = 8;
		RequesterFontAttr.ta_Style = 0;
		RequesterFontAttr.ta_Name = "topaz.font";
		
		if( !(RequesterFont = OpenDiskFont( &RequesterFontAttr ) ) )
			xCloseSystem( 0, RETURN_FAIL, QUIT_ASK_SETTINGS );
		}
}

BOOL xPopUpGUI( VOID )
{
	if( Screen )
		{
		ScreenToFront( Screen );
		
		if( OnPublic )
			WindowToFront( Window );
		
		ActivateWindow( Window );
		}
	else
		{
		UWORD	msgid;
		
		
		if( msgid = xOpenInterface() )
			xCloseSystem( msgid, RETURN_ERROR, QUIT_ASK_SETTINGS );
		
		ScreenToFront( Screen );
		}
	
	return( TRUE );
}
