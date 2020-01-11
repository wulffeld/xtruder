/*
**	$VER: XtruderSSMiscellaneous.c (26.7.96)
**
**	Xtruder miscellaneous settings
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"



#define		MIGT_GADS							6

VOID xOpenMiscellaneousSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	ULONG					txtlength;
	ULONG					t;
	UWORD					b1;
	UWORD					wh,ww;
	UWORD					stringgadlength = xProportionalLength( ScreenFont, 25 );
	
	
	t          = xMaxStringLength( ScreenFont, MSG_MI_POPKEY, MSG_MI_POPUPATSTART );
	txtlength  = xMaxStringLength( ScreenFont, MSG_MI_STAYHIDDEN, MSG_MI_STAYHIDDEN );
	
	if( t > txtlength )
		txtlength = t;
	
	txtlength += 15;
	
	if( gads = AllocVec( sizeof( struct xGadget ) * MIGT_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= stringgadlength;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_MI_POPKEY );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_MI_POPKEY;
		
		gads[0].xg_Gad		= CreateGadget( STRING_KIND, gads[0].xg_Gad, &ng,
			GTST_MaxChars,	39,
			GTST_String,	XSettings->xs_MIS_CxPopkey,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= stringgadlength - xProportionalLength( ScreenFont, 5 );
		ng.ng_Height		= ScrFontYSize + 4;
		ng.ng_GadgetText	= gads[1].xg_GadgetText = GCS( MSG_MI_PRIORITY );
		ng.ng_GadgetID		= gads[1].xg_GadgetID = GTID_MI_PRIORITY;
		
		gads[1].xg_Gad		= CreateGadget( SLIDER_KIND, gads[0].xg_Gad, &ng,
			GA_Immediate,		TRUE,
			GA_RelVerify,		TRUE,
			GTSL_Min,			-127,
			GTSL_Max,			127,
			GTSL_MaxLevelLen,	2,
			GTSL_Level,			XSettings->xs_MIS_CxPriority,
			GTSL_LevelFormat,	"%ld",
			GTSL_LevelPlace,	PLACETEXT_RIGHT,
			GT_Underscore,		'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_MI_POPUPATSTART );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_MI_POPUPATSTART;
		
		gads[2].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[1].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_MIS_CxPopup,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		b1 = ng.ng_TopEdge	+= ng.ng_Height + 2 + ScrFontYSize + 4;

		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[3].xg_GadgetText = GCS( MSG_MI_STAYHIDDEN );
		ng.ng_GadgetID		= gads[3].xg_GadgetID = GTID_MI_STAYHIDDEN;

		gads[3].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[2].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_MIS_StayHidden,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[4].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[4].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[3].xg_Gad,
			BUT_Text,		gads[4].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[5].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[5].xg_GadgetID = GTID_CANCEL,
			GA_Left,		txtlength + stringgadlength - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[4].xg_Gad,
			BUT_Text,		gads[5].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = txtlength + stringgadlength + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_MI_SETTINGS ), ww, wh, glist, gads, 5 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_MI_BAR_COMMODITY ) );
			xBarTxt( win, b1 - 4, GCS( MSG_MI_BAR_MISCELLANEOUS ) );
			xBarTxt( win, gads[5].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleMiscellaneousSettings( win, gads ) )
				{
				UWORD	pri = xSliderLevel( gads[1].xg_Gad, win );
				
				
				SettingsChanged = TRUE;
				
				if( stricmp( XSettings->xs_MIS_CxPopkey, GETSTRING( gads[0].xg_Gad ) ) || XSettings->xs_MIS_CxPriority != pri )
					{
					strcpy( XSettings->xs_MIS_CxPopkey, GETSTRING( gads[0].xg_Gad ) );
					
					XSettings->xs_MIS_CxPriority = pri;
					
					xRemoveCommodity();
					
					if( xCreateCommodity() )
						OnMenu( Window, FULLMENUNUM( 0, 0, NOSUB ) );
					else
						{
						xRemoveCommodity();
						
						OffMenu( Window, FULLMENUNUM( 0, 0, NOSUB ) );
						}
					}
				
				XSettings->xs_MIS_CxPopup = xIsGadSelected( gads[2].xg_Gad );
				XSettings->xs_MIS_StayHidden = xIsGadSelected( gads[3].xg_Gad );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[5].xg_Gad );
		DisposeObject( gads[4].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleMiscellaneousSettings( struct Window *win, struct xGadget *gads )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad = NULL;
	APTR					ia;
	ULONG					class;
	UWORD					altcode,code,qualifier;
	BOOL					fromraw;
	WORD					xp_cxpriority = XSettings->xs_MIS_CxPriority;
	
	
	while( 1 )
		{
		WaitPort( win->UserPort );
		
		while( msg = GT_GetIMsg( win->UserPort ) )
			{
			class = msg->Class;
			altcode = code = msg->Code;
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
							xPressButton( win, gads[5].xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_MISCELLANEOUS );
							break;
						
						default:
							if( tmpgad = xCheckUnderscore( gads, code, qualifier, ia ) )
								{
								switch( tmpgad->xg_GadgetID )
									{
									case GTID_OKAY:
									case GTID_CANCEL:
										xPressButton( win, tmpgad->xg_Gad );
										
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_MI_PRIORITY:
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_MI_POPKEY:
										ActivateGadget( tmpgad->xg_Gad, win, NULL );
										
										code = 0;
										break;
									
									case GTID_MI_STAYHIDDEN:
									case GTID_MI_POPUPATSTART:
										GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
											GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
											TAG_END );
									
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
						case GTID_MI_PRIORITY:
							if( fromraw )
								{
								qualifier &= IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT;
								
								switch( code )
									{
									case GTID_MI_PRIORITY:
										if( (qualifier && xp_cxpriority > -127) || (!qualifier && xp_cxpriority < 127) )
											{
											if( qualifier )
												xp_cxpriority--;
											else
												xp_cxpriority++;
											
											GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
												GTSL_Level, xp_cxpriority,
												TAG_END );
											}
										break;
									}
								}
							else
								{
								switch( code )
									{
									case GTID_MI_PRIORITY:
										xp_cxpriority = altcode;
										break;
									}
								}
							break;
						
						case GTID_OKAY:
							return( TRUE );
							break;
						
						case GTID_CANCEL:
							return( FALSE );
							break;
						}
					break;
				
				case IDCMP_CLOSEWINDOW:
					return( FALSE );
					break;
				}
			}
		}
}
