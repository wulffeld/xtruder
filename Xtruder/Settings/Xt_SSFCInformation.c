/*
**	$VER: XtruderSSFCInformation.c (2.8.96)
**
**	Xtruder file check information settings
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/**
 ---> +---------------------------------------------------------+
      | Check                                                   |
      +---------------------------------------------------------+
      | WX_Information                                          |
      |                                                         |
      |                      Size  GT_Chk                       |
      |                 File type  GT_Chk                       |
      |                      Date  GT_Chk                       |
      |           Protection bits  GT_Chk                       |
      +---------------------------------------------------------+
**/

#define		INFO_GADS							6

VOID xOpenFCInformationSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	ULONG					txtlength;
	UWORD					wh,ww;
	
	
	txtlength = 15 + xMaxStringLength( ScreenFont, MSG_FCIN_SIZE, MSG_FCIN_DATE );
	
	if( gads = AllocVec( sizeof( struct xGadget ) * INFO_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_FCIN_SIZE );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_FC_IN_SIZE;
		
		gads[0].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[0].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_INF_WrtInfo_Size,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[1].xg_GadgetText = GCS( MSG_FCIN_FILETYPE );
		ng.ng_GadgetID		= gads[1].xg_GadgetID = GTID_FC_IN_FILETYPE;
		
		gads[1].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[0].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_INF_WrtInfo_Filetype,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_FCIN_PROTECTIONBITS );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_FC_IN_PROTECTIONBITS;
		
		gads[2].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[1].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_INF_WrtInfo_ProtBits,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[3].xg_GadgetText = GCS( MSG_FCIN_DATE );
		ng.ng_GadgetID		= gads[3].xg_GadgetID = GTID_FC_IN_DATE;
		
		gads[3].xg_Gad		= CreateGadget( CHECKBOX_KIND, gads[2].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_INF_WrtInfo_Date,
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
			GA_Left,		txtlength + 26 - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[4].xg_Gad,
			BUT_Text,		gads[5].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = txtlength + 26 + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_FCIN_SETTINGS ), ww, wh, glist, gads, 5 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_FCIN_BAR_INFORMATION ) );
			xBarTxt( win, gads[5].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleFCInformationSettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				XSettings->xs_FC_INF_WrtInfo_Size = xIsGadSelected( gads[0].xg_Gad );
				XSettings->xs_FC_INF_WrtInfo_Filetype = xIsGadSelected( gads[1].xg_Gad );
				XSettings->xs_FC_INF_WrtInfo_ProtBits = xIsGadSelected( gads[2].xg_Gad );
				XSettings->xs_FC_INF_WrtInfo_Date = xIsGadSelected( gads[3].xg_Gad );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[5].xg_Gad );
		DisposeObject( gads[4].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleFCInformationSettings( struct Window *win, struct xGadget *gads )
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
							if( tmpgad = xFindGadget( gads, GTID_CANCEL ) )
								xPressButton( win, tmpgad->xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_FCHK_INFORMATION );
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
									
									case GTID_FC_IN_SIZE:
									case GTID_FC_IN_FILETYPE:
									case GTID_FC_IN_PROTECTIONBITS:
									case GTID_FC_IN_DATE:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											{
											GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
												GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
												TAG_END );
											}
										else
											code = 0;
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
