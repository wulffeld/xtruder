/*
**	$VER: Xt_SSGUI.c (21.4.97)
**
**	Xtruder settings
**
**	Copyright (C) 1995-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		GUGT_GADS							17

BOOL xOpenGUISettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Screen *			scr;
	struct NewGadget		ng;
	struct NameInfo			ni;
	struct Window *			win;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	DisplayInfoHandle		handle;
	UBYTE					strbuf1[ DEFAULTBUFFERLEN ];
	UBYTE					strbuf2[ DEFAULTBUFFERLEN ];
	UBYTE					strbuf3[ DEFAULTBUFFERLEN ];
	UBYTE					strbuf4[ DEFAULTBUFFERLEN ];
	ULONG					txtlength;
	ULONG					pallength;
	ULONG					conlength;
	ULONG					boxwidth;
	UWORD					ww,wh;
	BOOL					changed = FALSE;
	
	
	boxwidth  = xProportionalLength( ScreenFont, 35 );
	pallength = xTextLength( ScreenFont, GCS( MSG_GU_PALETTE ) );
	conlength = xMaxStringLength( ScreenFont, MSG_GU_SCROLLERWIDTH, MSG_GU_HIDEAPPICON );
	txtlength = xMaxStringLength( ScreenFont, MSG_GU_SCREENMODE, MSG_GU_REQUESTERFONT );
	
	if( pallength > boxwidth )
		boxwidth = pallength;
	
	if( txtlength < conlength )
		txtlength = conlength;
	
	txtlength += 15;
	
	if( gads = AllocVec( sizeof( struct xGadget ) * GUGT_GADS, MEMF_CLEAR ) )
		{
		if( handle = FindDisplayInfo( XSettings->xs_GUI_ScreenMode ) )
			{
			if( GetDisplayInfoData( handle, (UBYTE *)&ni, sizeof( struct NameInfo ), DTAG_NAME, XSettings->xs_GUI_ScreenMode ) )
				strcpy( strbuf1, ni.Name );
			else
				xSPrintF( strbuf1, "%ld x %ld", Screen->Width, Screen->Height );
			}
		
		gads[0].xg_Gad = CreateContext( &glist );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= boxwidth - GETGADWIDTH;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_GU_SCREENMODE );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_GU_SCREENMODE;
		
		gads[0].xg_Gad = CreateGadget( TEXT_KIND, gads[0].xg_Gad, &ng,
			GTTX_Text,		strbuf1,
			GTTX_Border,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[1].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[1].xg_GadgetID = GTID_GU_GETSCREENMODE,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[0].xg_Gad,
			BUT_Type,		BUTTYPE_ARROW,
			TAG_DONE );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_GU_PUBLICSCREENNAME );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_GU_PUBLICSCREENNAME;
		
		gads[2].xg_Gad = CreateGadget( STRING_KIND, gads[1].xg_Gad, &ng,
			GTST_MaxChars,	140,
			GTST_String,	XSettings->xs_GUI_PublicScreenName,
			GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[3].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[3].xg_GadgetID = GTID_GU_GETPUBLICSCREEN,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[2].xg_Gad,
			BUT_Type,		BUTTYPE_ARROW,
			TAG_DONE );
		
			/// Screen font gadgets
		
		xSPrintF( strbuf2, "%s %ld", XSettings->xs_GUI_ScreenFontName, XSettings->xs_GUI_ScreenFontSize );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[4].xg_GadgetText = GCS( MSG_GU_SCREENFONT );
		ng.ng_GadgetID		= gads[4].xg_GadgetID = GTID_GU_SCREENFONT;
		
		gads[4].xg_Gad = CreateGadget( TEXT_KIND, gads[3].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			GTTX_Text,		strbuf2,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[5].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[5].xg_GadgetID = GTID_GU_GETSCREENFONT,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[4].xg_Gad,
			BUT_Type,		BUTTYPE_ARROW,
			TAG_DONE );
		
			/// Text font gadgets
		
		xSPrintF( strbuf3, "%s %ld", XSettings->xs_GUI_TextFontName, XSettings->xs_GUI_TextFontSize );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[6].xg_GadgetText = GCS( MSG_GU_TEXTFONT );
		ng.ng_GadgetID		= gads[6].xg_GadgetID = GTID_GU_TEXTFONT;
		
		gads[6].xg_Gad = CreateGadget( TEXT_KIND, gads[5].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			GTTX_Text,		strbuf3,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[7].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[7].xg_GadgetID = GTID_GU_GETTEXTFONT,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[6].xg_Gad,
			BUT_Type,		BUTTYPE_ARROW,
			TAG_DONE );
		
			/// Requester font gadgets
		
		xSPrintF( strbuf4, "%s %ld", XSettings->xs_GUI_RequesterFontName, XSettings->xs_GUI_RequesterFontSize );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[8].xg_GadgetText = GCS( MSG_GU_REQUESTERFONT );
		ng.ng_GadgetID		= gads[8].xg_GadgetID = GTID_GU_REQUESTERFONT;
		
		gads[8].xg_Gad = CreateGadget( TEXT_KIND, gads[7].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			GTTX_Text,		strbuf4,
			GT_Underscore,	'_',
			TAG_END );
		
		gads[9].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[9].xg_GadgetID = GTID_GU_GETREQUESTERFONT,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[8].xg_Gad,
			BUT_Type,		BUTTYPE_ARROW,
			TAG_DONE );
		
			/// Background pattern
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[10].xg_GadgetText = GCS( MSG_GU_BACKGROUNDPATTERN );
		ng.ng_GadgetID		= gads[10].xg_GadgetID = GTID_GU_BACKGROUNDPATTERN;
		
		gads[10].xg_Gad = CreateGadget( STRING_KIND, gads[9].xg_Gad, &ng,
			GTST_MaxChars,	256,
			GTST_String,	XSettings->xs_GUI_BackgroundPattern,
        	GTIN_EditHook,	&CursorDownHook,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Get background pattern
		
		gads[11].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[11].xg_GadgetID = GTID_GU_GETBACKGROUNDPATTERN,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		20,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[10].xg_Gad,
			BUT_Type,		BUTTYPE_FILE,
			TAG_DONE );
		
			/// Palette
		
		gads[12].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[12].xg_GadgetID = GTID_GU_PALETTE,
			GA_Left,		txtlength,
			GA_Top,			ng.ng_TopEdge + ng.ng_Height + 2, 
			GA_Width,		boxwidth,
			GA_Height,		ng.ng_Height,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[11].xg_Gad,
			BUT_Text,		gads[12].xg_GadgetText = GCS( MSG_GU_PALETTE ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		ng.ng_TopEdge		+= ng.ng_Height * 2 + 4;
		ng.ng_Width			= boxwidth - xProportionalLength( ScreenFont, 4 );
		ng.ng_Height		= ScrFontYSize + 4;
		ng.ng_GadgetText	= gads[13].xg_GadgetText = GCS( MSG_GU_SCROLLERWIDTH );
		ng.ng_GadgetID		= gads[13].xg_GadgetID = GTID_GU_SCROLLERWIDTH;
		
		gads[13].xg_Gad = CreateGadget( SLIDER_KIND, gads[12].xg_Gad, &ng,
			GA_Immediate,		TRUE,
			GA_RelVerify,		TRUE,
			GTSL_Min,			14,
			GTSL_Max,			40,
			GTSL_MaxLevelLen,	2,
			GTSL_Level,			XSettings->xs_GUI_ScrollerWidth,
			GTSL_LevelFormat,	"%ld",
			GTSL_LevelPlace,	PLACETEXT_RIGHT,
			GT_Underscore,		'_',
			TAG_END );
		
			/// Hide AppIcon
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[14].xg_GadgetText = GCS( MSG_GU_HIDEAPPICON );
		ng.ng_GadgetID		= gads[14].xg_GadgetID = GTID_GU_HIDEAPPICON;
		
		gads[14].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[13].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_GUI_HideAppIcon,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[15].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[15].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[14].xg_Gad,
			BUT_Text,		gads[15].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[16].xg_Gad	= NewObject( ButtonClass, NULL,
			GA_ID,			gads[16].xg_GadgetID = GTID_CANCEL,
			GA_Left,		txtlength + boxwidth - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[15].xg_Gad,
			BUT_Text,		gads[16].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = txtlength + boxwidth + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_GU_SETTINGS ), ww, wh, glist, gads, GUGT_GADS - 1 ) )
			{
			scr = settingsgui->sg_Screen;
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_GU_BAR_SCREEN ) );
			xBarTxt( win, gads[ GUGT_GADS - 1 ].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleGUISettings( scr, win, gads, &changed ) )
				{
				SettingsChanged = TRUE;
				
				XSettings->xs_GUI_ScrollerWidth = xSliderLevel( gads[13].xg_Gad, win );
				XSettings->xs_GUI_HideAppIcon = xIsGadSelected( gads[14].xg_Gad );
				
				strcpy( XSettings->xs_GUI_PublicScreenName, GETSTRING( gads[2].xg_Gad ) );
				strcpy( XSettings->xs_GUI_BackgroundPattern, GETSTRING( gads[10].xg_Gad ) );
				
					/// Check if we should initialize the AppIcon or
					/// perhaps even remove it
				
				if( !XSettings->xs_GUI_HideAppIcon && !AppIcon )
					xCreateAppIcon();
				else
				if( XSettings->xs_GUI_HideAppIcon && AppIcon )
					xRemoveAppIcon();
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[16].xg_Gad );
		DisposeObject( gads[15].xg_Gad );
		DisposeObject( gads[12].xg_Gad );
		DisposeObject( gads[11].xg_Gad );
		DisposeObject( gads[9].xg_Gad );
		DisposeObject( gads[7].xg_Gad );
		DisposeObject( gads[5].xg_Gad );
		DisposeObject( gads[3].xg_Gad );
		DisposeObject( gads[1].xg_Gad );
		
		FreeVec( gads );
		}
	
	return( changed );
}

BOOL xHandleGUISettings( struct Screen *scr, struct Window *win, struct xGadget *gads, BOOL *changed )
{
	struct rtScreenModeRequester *	scrmodereq;
	struct rtFontRequester *		fontreq;
	struct NameInfo					ni;
	struct IntuiMessage *			msg;
	struct xGadget *				tmpgad = NULL;
	struct List *					list;
	struct Node *					node;
	APTR							ia;
	ULONG							class;
	UWORD							code,altcode,qualifier;
	ULONG							screenmode = XSettings->xs_GUI_ScreenMode;
	ULONG							screenwidth = XSettings->xs_GUI_ScreenWidth;
	ULONG							screenheight = XSettings->xs_GUI_ScreenHeight;
	ULONG							screendepth = XSettings->xs_GUI_ScreenDepth;
	ULONG							screenoverscan = XSettings->xs_GUI_ScreenOverscan; 
	ULONG							autoscroll = XSettings->xs_GUI_AutoScroll;
	BOOL							done = FALSE;
	BOOL							okay = FALSE;
	BOOL							fromraw;
	UBYTE							screenname[140];
	UBYTE							screenfontname[33];
	UBYTE							textfontname[33];
	UBYTE							requesterfontname[33];
	UBYTE							screenfontsize = XSettings->xs_GUI_ScreenFontSize;
	UBYTE							textfontsize = XSettings->xs_GUI_TextFontSize;
	UBYTE							requesterfontsize = XSettings->xs_GUI_RequesterFontSize;
	UBYTE							requesterfontstyle = XSettings->xs_GUI_RequesterFontStyle;
	UBYTE							xp_scrollerwidth = XSettings->xs_GUI_ScrollerWidth;
	UBYTE							strbuf[ DEFAULTBUFFERLEN ] = "";
			
			
	strcpy( screenname, XSettings->xs_GUI_PublicScreenName );
	strcpy( screenfontname, XSettings->xs_GUI_ScreenFontName );
	strcpy( textfontname, XSettings->xs_GUI_TextFontName );
	strcpy( requesterfontname, XSettings->xs_GUI_RequesterFontName );
	
	while( !done )
		{
		WaitPort( win->UserPort );
		
		while( msg = GT_GetIMsg( win->UserPort ) )
			{
			class = msg->Class;
			code = altcode = msg->Code;
			qualifier = msg->Qualifier;
			ia = msg->IAddress;
			
			if( (class == IDCMP_GADGETUP) || (class == IDCMP_GADGETDOWN) )
				code = ((struct Gadget *)msg->IAddress)->GadgetID;
			
			GT_ReplyIMsg( msg );
			
			fromraw = FALSE;
			
			switch( class )
				{
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( win );
					GT_EndRefresh( win, TRUE );
					break;
				
				case IDCMP_RAWKEY:
					fromraw = TRUE;
					
					switch( code )
						{
						case KEY_ESCAPE:
							xPressButton( win, gads[ GUGT_GADS - 1 ].xg_Gad );
							
							done = TRUE;
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_GUI );
							break;
						
						default:
							if( tmpgad = xCheckUnderscore( gads, code, qualifier, ia ) )
								{
								switch( tmpgad->xg_GadgetID )
									{
									case GTID_OKAY:
									case GTID_CANCEL:
									case GTID_GU_PALETTE:
										xPressButton( win, tmpgad->xg_Gad );
										
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_GU_HIDEAPPICON:
										GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
											GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
											TAG_END );
									
									case GTID_GU_PUBLICSCREENNAME:
									case GTID_GU_BACKGROUNDPATTERN:
										ActivateGadget( tmpgad->xg_Gad, win, NULL );
										break;
									
									case GTID_GU_SCREENMODE:
									case GTID_GU_SCREENFONT:
									case GTID_GU_TEXTFONT:
									case GTID_GU_REQUESTERFONT:
									case GTID_GU_SCROLLERWIDTH:
										code = tmpgad->xg_GadgetID;
										break;
									
									default:
										code = 0;
										break;
									}
								}
							else
								break;
						}
				
				case IDCMP_GADGETUP:
					switch( code )
						{
						case GTID_GU_SCREENMODE:
							if( fromraw )
								xPressButton( win, gads[1].xg_Gad );
							else
								break;
						
						case GTID_GU_GETSCREENMODE:
							xDisableWindow( win );
							
							if( scrmodereq = rtAllocRequestA( RT_SCREENMODEREQ, NULL ) )
								{
								rtChangeReqAttr( scrmodereq,
									RTSC_DisplayID,		screenmode,
									RTSC_DisplayWidth,	screenwidth,
									RTSC_DisplayHeight,	screenheight,
									RTSC_DisplayDepth,	screendepth,
									RTSC_OverscanType,	screenoverscan,
									RTSC_AutoScroll,	autoscroll,
									TAG_END );
								
								if( rtScreenModeRequest( scrmodereq, GCS( MSG_TL_SELECTSCREENMODE ),
									RT_Window,			win,
									RT_TextAttr,		&ScreenFontAttr,
									RTSC_MinHeight,		100,
									RTSC_MinWidth,		640,
									RTSC_MinDepth,		2,
									RTSC_Flags,			SCREQF_SIZEGADS | SCREQF_OVERSCANGAD | SCREQF_GUIMODES | SCREQF_AUTOSCROLLGAD | SCREQF_DEPTHGAD,
									RTSC_AutoScroll,	XSettings->xs_GUI_AutoScroll,
									TAG_END ) )
									{
									DisplayInfoHandle	handle;
									
									
									screenmode = scrmodereq->DisplayID;
									screenwidth = scrmodereq->DisplayWidth;
									screenheight = scrmodereq->DisplayHeight;
									screendepth = scrmodereq->DisplayDepth;
									screenoverscan = scrmodereq->OverscanType;
									autoscroll = scrmodereq->AutoScroll; 
									
									if( handle = FindDisplayInfo( screenmode ) )
										{
										if( GetDisplayInfoData( handle, (UBYTE *)&ni, sizeof( struct NameInfo ), DTAG_NAME, screenmode ) )
											{
											GT_SetGadgetAttrs( gads[0].xg_Gad, win, NULL,
												GTTX_Text, ni.Name,
												TAG_END );
											}
										}
									}
								
								rtFreeRequest( scrmodereq );
								}
							
							xEnableWindow( win );
							break;
						
						case GTID_GU_PUBLICSCREENNAME:
							if( altcode != HOOK_CURSORDOWN )
								break;
						
						case GTID_GU_GETPUBLICSCREEN:
							xDisableWindow( win );
							
							if( list = xBuildScreenList() )
								{
								if( node = xSelectListEntry( list, xCountListEntries( list ), GCS( MSG_TL_SELECTPUBLICSCREEN ), GCS( MSG_PUBLICSCREENS ), scr ) )
									{
									GT_SetGadgetAttrs( gads[2].xg_Gad, win, NULL,
										GTST_String, node->ln_Name,
										TAG_END );
									}
								xFreeNameNodes( list );
								}
							
							xEnableWindow( win );
							break;
						
						case GTID_GU_SCREENFONT:
							xPressButton( win, gads[5].xg_Gad );
						case GTID_GU_GETSCREENFONT:
							xDisableWindow( win );
							
							if( fontreq = rtAllocRequestA( RT_FONTREQ, NULL ) )
								{
								rtChangeReqAttr( fontreq,
									RTFO_FontName, screenfontname,
									RTFO_FontHeight, screenfontsize,
									TAG_END );
								
								if( rtFontRequest( fontreq, GCS( MSG_TL_SELECTFONT ),
									RT_Window,			win,
									RT_TextAttr,		&ScreenFontAttr,
									RTFO_Flags,			FREQF_SCALE,
									RTFO_SampleHeight,	40,
									RTFO_MinHeight,		7,
									TAG_END ) )
									{
									strncpy( screenfontname, fontreq->Attr.ta_Name, 31 );
									
									screenfontsize = fontreq->Attr.ta_YSize;
									
									xSPrintF( strbuf, "%s %ld", screenfontname, screenfontsize );
									
									GT_SetGadgetAttrs( gads[4].xg_Gad, win, NULL,
										GTTX_Text, strbuf,
										TAG_END );
									}
								
								rtFreeRequest( fontreq );
								}
							
							xEnableWindow( win );
							break;
						
						case GTID_GU_TEXTFONT:
							xPressButton( win, gads[7].xg_Gad );
						case GTID_GU_GETTEXTFONT:
							xDisableWindow( win );
							
							if( fontreq = rtAllocRequestA( RT_FONTREQ, NULL ) )
								{
								rtChangeReqAttr( fontreq,
									RTFO_FontStyle, 0,
									RTFO_FontName, textfontname,
									RTFO_FontHeight, textfontsize,
									TAG_END );
								
								if( rtFontRequest( fontreq, GCS( MSG_TL_SELECTFONT ),
									RT_Window,			win,
									RT_TextAttr,		&ScreenFontAttr,
									RTFO_Flags,			FREQF_FIXEDWIDTH,
									RTFO_SampleHeight,	40,
									RTFO_MinHeight,		7,
									TAG_END ) )
									{
									strncpy( textfontname, fontreq->Attr.ta_Name, 31 );
									
									textfontsize = fontreq->Attr.ta_YSize;
									
									xSPrintF( strbuf, "%s %ld", textfontname, textfontsize );
									
									GT_SetGadgetAttrs( gads[6].xg_Gad, win, NULL,
										GTTX_Text, strbuf,
										TAG_END );
									}
								
								rtFreeRequest( fontreq );
								}
							
							xEnableWindow( win );
							break;
						
						case GTID_GU_REQUESTERFONT:
							xPressButton( win, gads[9].xg_Gad );
						case GTID_GU_GETREQUESTERFONT:
							xDisableWindow( win );
							
							if( fontreq = rtAllocRequestA( RT_FONTREQ, NULL ) )
								{
								rtChangeReqAttr( fontreq,
									RTFO_FontName, requesterfontname,
									RTFO_FontStyle, requesterfontstyle,
									RTFO_FontHeight, requesterfontsize,
									TAG_END );
								
								if( rtFontRequest( fontreq, GCS( MSG_TL_SELECTFONT ),
									RT_Window,			win,
									RT_TextAttr,		&ScreenFontAttr,
									RTFO_Flags,			FREQF_STYLE,
									RTFO_SampleHeight,	40,
									RTFO_MinHeight,		7,
									TAG_END ) )
									{
									strncpy( requesterfontname, fontreq->Attr.ta_Name, 31 );
									
									requesterfontsize = fontreq->Attr.ta_YSize;
									requesterfontstyle = fontreq->Attr.ta_Style;
									
									xSPrintF( strbuf, "%s %ld", requesterfontname, requesterfontsize );
									
									GT_SetGadgetAttrs( gads[8].xg_Gad, win, NULL,
										GTTX_Text, strbuf,
										TAG_END );
									}
								
								rtFreeRequest( fontreq );
								}
							
							xEnableWindow( win );
							break;
						
						case GTID_GU_BACKGROUNDPATTERN:
							if( altcode != HOOK_CURSORDOWN )
								break;
						
						case GTID_GU_GETBACKGROUNDPATTERN:
							xDisableWindow( win );
							
							if( xSelectFile( win,
								SELFP_ReqTitle,	GCS( MSG_TL_SELECTPATTERN ),
								SELFP_DestFile,	strbuf,
								SELFP_Gadget,	gads[10].xg_Gad,
								TAG_END ) )
								{
								xSetGadgetString( win, gads[10].xg_Gad, strbuf );
								}
							
							xEnableWindow( win );
							break;
						
						case GTID_GU_PALETTE:
							xDisableWindow( win );
							
							rtPaletteRequest( GCS( MSG_TL_SETCOLORS ), NULL,
								RT_TextAttr,	&ScreenFontAttr,
								RT_Window,		win,
								TAG_END );
							
							xEnableWindow( win );
							break;
						
						case GTID_GU_SCROLLERWIDTH:
							if( fromraw )
								{
								qualifier &= IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT;
								
								if( (qualifier && xp_scrollerwidth > 14) || (!qualifier && xp_scrollerwidth < 40) )
									{
									if( qualifier )
										xp_scrollerwidth--;
									else
										xp_scrollerwidth++;
									
									GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
										GTSL_Level, xp_scrollerwidth,
										TAG_END );
									}
								}
							break;
						
						case GTID_OKAY:
							okay = TRUE;
						case GTID_CANCEL:
							done = TRUE;
							break;
						}
					break;
				
				case IDCMP_CLOSEWINDOW:
					done = TRUE;
					break;
				}
			}
		}
	
	if( okay )
		{
		if( XSettings->xs_GUI_ScreenMode != screenmode ||
			XSettings->xs_GUI_ScreenWidth != screenwidth ||
			XSettings->xs_GUI_ScreenHeight != screenheight ||
			XSettings->xs_GUI_ScreenDepth != screendepth ||
			XSettings->xs_GUI_ScreenOverscan != screenoverscan ||
			XSettings->xs_GUI_AutoScroll != autoscroll ||
			XSettings->xs_GUI_ScreenFontSize != screenfontsize ||
			XSettings->xs_GUI_TextFontSize != textfontsize ||
			XSettings->xs_GUI_RequesterFontSize != requesterfontsize ||
			XSettings->xs_GUI_RequesterFontStyle != requesterfontstyle ||
			stricmp( XSettings->xs_GUI_PublicScreenName, GETSTRING( gads[2].xg_Gad ) ) ||
			stricmp( XSettings->xs_GUI_ScreenFontName, screenfontname ) ||
			stricmp( XSettings->xs_GUI_TextFontName, textfontname ) ||
			stricmp( XSettings->xs_GUI_RequesterFontName, requesterfontname ) ||
			stricmp( XSettings->xs_GUI_BackgroundPattern, GETSTRING( gads[10].xg_Gad ) ) )
			{
			XSettings->xs_GUI_ScreenMode = screenmode;
			XSettings->xs_GUI_ScreenWidth = screenwidth;
			XSettings->xs_GUI_ScreenHeight = screenheight;
			XSettings->xs_GUI_ScreenDepth = screendepth;
			XSettings->xs_GUI_ScreenOverscan = screenoverscan;
			XSettings->xs_GUI_AutoScroll = autoscroll;
			XSettings->xs_GUI_ScreenFontSize = screenfontsize;
			XSettings->xs_GUI_TextFontSize = textfontsize;
			XSettings->xs_GUI_RequesterFontSize = requesterfontsize;
			XSettings->xs_GUI_RequesterFontStyle = requesterfontstyle;
			
			strcpy( XSettings->xs_GUI_ScreenFontName, screenfontname );
			strcpy( XSettings->xs_GUI_TextFontName, textfontname );
			strcpy( XSettings->xs_GUI_RequesterFontName, requesterfontname );
			
			*changed = TRUE;
			}
		}
	
	return( okay );
}

struct List *xBuildScreenList( VOID )
{
	struct List *			pubscreenlist;
	struct List *			list = NULL;
	
	
	
	if( pubscreenlist = LockPubScreenList() )
		{
		if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
			{
			struct Node *next,*node;
			
			
			NewList( list );
			
			node = pubscreenlist->lh_Head;
			
				// Scan the list
			
			while( next = node->ln_Succ )
				{
					/// Don't include the current `XTRUDER' public screen name in it.
				
				if( strcmp( node->ln_Name, "XTRUDER" ) )
					xAddNameNode( list, node->ln_Name );
				
				node = next;
				}
			}
		UnlockPubScreenList();
		}
	
	return( list );
}
