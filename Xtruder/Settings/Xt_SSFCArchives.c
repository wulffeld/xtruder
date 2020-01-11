/*
**	$VER: XtruderSettingsArchives.c (2.3.96)
**
**	Xtruder settings
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/**
 ---> +----------------------------------------------+
      | Archives settings                            |
      +----------------------------------------------+
      | WX_LhA                                       |
      |                                              |
      |     Extract LhA GTCHK                        |
      | Extract routine GTCYC_unpack lib|internal    |
      |        Max size GTNUM                        |
      |                                              |
      | WX_LZX                                       |
      |     Extract LZX GTCHK                        |
      | Extract routine GTCYC_unpack lib|internal    |
      |        Max size GTNUM                        |
      |                                              |
      | WX_Output                                    |
      |          Output STRING                       |
      +----------------------------------------------+
**/

#define		ARGT_GADS							9

VOID xOpenFCArchivesSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	UBYTE *					extract_labels[] = { "","",NULL };
	ULONG					txtlength;
	ULONG					t;
	UWORD					ww,wh,gad4top;
	UWORD					stringgadlength = xProportionalLength( ScreenFont, 25 );
	
	
	extract_labels[0] = GCS( MSG_FCAR_INTERNAL1 );
	extract_labels[1] = GCS( MSG_FCAR_INTERNAL2 );
	
	t         = 30 + xMaxStringLength( ScreenFont, MSG_FCAR_INTERNAL1, MSG_FCAR_INTERNAL2 );
	txtlength = 15 + xMaxStringLength( ScreenFont, MSG_FCAR_EXTRACTROUTINELHA, MSG_FCAR_OUTPUT );
	
	if( t > stringgadlength )
		stringgadlength = t;
	
	if( gads = AllocVec( sizeof( struct xGadget ) * ARGT_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_FCAR_EXTRACTLHA );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_FC_AR_EXTRACTLHA;
		
		gads[0].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[0].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_ARC_ExtractLhA,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= stringgadlength;
		ng.ng_Height		= GadgetHeight;
		ng.ng_GadgetText	= gads[1].xg_GadgetText = GCS( MSG_FCAR_EXTRACTROUTINELHA );
		ng.ng_GadgetID		= gads[1].xg_GadgetID = GTID_FC_AR_LHAEXTRACTROUTINE;
		
		gads[1].xg_Gad = CreateGadget( CYCLE_KIND, gads[0].xg_Gad, &ng,
			GTCY_Active,	XSettings->xs_FC_ARC_LhAExtractRoutine,
			GTCY_Labels,	extract_labels,
			GT_Underscore,	'_',
			GA_Disabled,	!XSettings->xs_FC_ARC_ExtractLhA,
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_FCAR_LHAMAXSIZE );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_FC_AR_LHAMAXSIZE;
		
		gads[2].xg_Gad = CreateGadget( INTEGER_KIND, gads[1].xg_Gad, &ng,
			GTIN_Number,		XSettings->xs_FC_ARC_LhAMaxSize,
			STRINGA_ExitHelp,	TRUE,
			GT_Underscore,		'_',
			GA_Disabled,		!XSettings->xs_FC_ARC_ExtractLhA,
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2 + ScrFontYSize + 4;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[3].xg_GadgetText = GCS( MSG_FCAR_EXTRACTLZX );
		ng.ng_GadgetID		= gads[3].xg_GadgetID = GTID_FC_AR_EXTRACTLZX;
		
		gads[3].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[2].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_ARC_ExtractLZX,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= stringgadlength;
		ng.ng_Height		= GadgetHeight;
		ng.ng_GadgetText	= gads[4].xg_GadgetText = GCS( MSG_FCAR_EXTRACTROUTINELZX );
		ng.ng_GadgetID		= gads[4].xg_GadgetID = GTID_FC_AR_LZXEXTRACTROUTINE;
		
		gads[4].xg_Gad = CreateGadget( CYCLE_KIND, gads[3].xg_Gad, &ng,
			GTCY_Active,	XSettings->xs_FC_ARC_LZXExtractRoutine,
			GTCY_Labels,	extract_labels,
			GT_Underscore,	'_',
			GA_Disabled,	!XSettings->xs_FC_ARC_ExtractLZX,
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[5].xg_GadgetText = GCS( MSG_FCAR_LZXMAXSIZE );
		ng.ng_GadgetID		= gads[5].xg_GadgetID = GTID_FC_AR_LZXMAXSIZE;
		
		gads[5].xg_Gad = CreateGadget( INTEGER_KIND, gads[4].xg_Gad, &ng,
			GTIN_Number,		XSettings->xs_FC_ARC_LZXMaxSize,
			STRINGA_ExitHelp,	TRUE,
			GT_Underscore,		'_',
			GA_Disabled,		!XSettings->xs_FC_ARC_ExtractLZX,
			TAG_END );
		
		gad4top = ng.ng_TopEdge += ng.ng_Height + 2 + ScrFontYSize + 4;
		
		ng.ng_Width			= stringgadlength;
		ng.ng_Height		= GadgetHeight;
		ng.ng_GadgetText	= gads[6].xg_GadgetText = GCS( MSG_FCAR_OUTPUT );
		ng.ng_GadgetID		= gads[6].xg_GadgetID = GTID_FC_AR_OUTPUT;
		
		gads[6].xg_Gad = CreateGadget( STRING_KIND, gads[5].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTBUFFERLEN,
			GTST_String,	XSettings->xs_FC_ARC_Output,
			GT_Underscore,	'_',
			TAG_END );
		
		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[7].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[7].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[6].xg_Gad,
			BUT_Text,		gads[7].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[8].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[8].xg_GadgetID = GTID_CANCEL,
			GA_Left,		txtlength + stringgadlength - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[7].xg_Gad,
			BUT_Text,		gads[8].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		ww = txtlength + stringgadlength + 10;
		wh += GadgetHeight + CONFIRMGAP + 3;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_FCAR_SETTINGS ), ww, wh, glist, gads, 8 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_FCAR_BAR_LHA ) );
			xBarTxt( win, gads[3].xg_Gad->TopEdge - 4, GCS( MSG_FCAR_BAR_LZX ) );
			xBarTxt( win, gad4top - 4, GCS( MSG_FCAR_BAR_OUTPUT ) );
			xBarTxt( win, gads[8].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleFCArchivesSettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				XSettings->xs_FC_ARC_ExtractLhA = xIsGadSelected( gads[0].xg_Gad );
				XSettings->xs_FC_ARC_LhAExtractRoutine = xCycleActive( gads[1].xg_Gad, win );
				XSettings->xs_FC_ARC_LhAMaxSize = GETNUMBER( gads[2].xg_Gad );
				XSettings->xs_FC_ARC_ExtractLZX = xIsGadSelected( gads[3].xg_Gad );
				XSettings->xs_FC_ARC_LZXExtractRoutine = xCycleActive( gads[4].xg_Gad, win );
				XSettings->xs_FC_ARC_LZXMaxSize = GETNUMBER( gads[5].xg_Gad );
				strcpy( XSettings->xs_FC_ARC_Output, GETSTRING( gads[6].xg_Gad ) );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[8].xg_Gad );
		DisposeObject( gads[7].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleFCArchivesSettings( struct Window *win, struct xGadget *gads )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	APTR					ia;
	ULONG					class;
	UWORD					code,qualifier;
	
	
	while( 1 )
		{
		WaitPort( win->UserPort );
		
		while( msg = GT_GetIMsg( win->UserPort ) )
			{
			class = msg->Class;
			code = msg->Code;
			qualifier = msg->Qualifier;
			ia = msg->IAddress;
			
			if( (class == IDCMP_GADGETUP) || (class == IDCMP_GADGETDOWN) )
				code = ((struct Gadget *)msg->IAddress)->GadgetID;
			
			GT_ReplyIMsg( msg );
			
			switch( class )
				{
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( win );
					GT_EndRefresh( win, TRUE );
					break;
				
				case IDCMP_RAWKEY:
					switch( code )
						{
						case KEY_ESCAPE:
							xPressButton( win, gads[8].xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_FCHK_ARCHIVES );
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
									
									case GTID_FC_AR_LHAEXTRACTROUTINE:
									case GTID_FC_AR_LZXEXTRACTROUTINE:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											xRefreshCycleGadget( win, tmpgad->xg_Gad, qualifier, 1 );
										break;
									
									case GTID_FC_AR_EXTRACTLHA:
									case GTID_FC_AR_EXTRACTLZX:
										GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
											GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
											TAG_END );
										
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_FC_AR_LHAMAXSIZE:
									case GTID_FC_AR_LZXMAXSIZE:
									case GTID_FC_AR_OUTPUT:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											ActivateGadget( tmpgad->xg_Gad, win, NULL );
										break;
									}
								}
							else
								break;
						}
				
				case IDCMP_GADGETUP:
					switch( code )
						{
						case GTID_FC_AR_EXTRACTLHA:
							if( xIsGadSelected( gads[0].xg_Gad ) )
								{
								GT_SetGadgetAttrsA( gads[1].xg_Gad, win, NULL, DisabledFalseTags );
								GT_SetGadgetAttrsA( gads[2].xg_Gad, win, NULL, DisabledFalseTags );
								}
							else
								{
								GT_SetGadgetAttrsA( gads[1].xg_Gad, win, NULL, DisabledTrueTags );
								GT_SetGadgetAttrsA( gads[2].xg_Gad, win, NULL, DisabledTrueTags );
								}
							break;
						
						case GTID_FC_AR_EXTRACTLZX:
							if( xIsGadSelected( gads[3].xg_Gad ) )
								{
								GT_SetGadgetAttrsA( gads[4].xg_Gad, win, NULL, DisabledFalseTags );
								GT_SetGadgetAttrsA( gads[5].xg_Gad, win, NULL, DisabledFalseTags );
								}
							else
								{
								GT_SetGadgetAttrsA( gads[4].xg_Gad, win, NULL, DisabledTrueTags );
								GT_SetGadgetAttrsA( gads[5].xg_Gad, win, NULL, DisabledTrueTags );
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
