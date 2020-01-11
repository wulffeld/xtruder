/*
**	$VER: XtruderSSMiscellaneous.c (2.8.96)
**
**	Xtruder file check miscellaneous settings
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/**
 ---> +---------------------------------------------------------+
      | Check                                                   |
      +---------------------------------------------------------+
      | WX_Miscellaneous                                        |
      |                                                         |
      |                File filter  GT_Str....................  |
      |             Archive filter  GT_Str....................  |
      |        Do with virus found  GT_Cycle..................  |
      |       Do with virus copied  GT_Cycle..................  |
      |          Comment for virus  GT_Str....................  |
      | Rename extension for virus  GT_Str....................  |
      |             Auto deprotect  GT_Chk                      |
      |          Enter directories  GT_Chk                      |
      |            Auto file check  GT_Chk                      |
      |             Path indention  GT_Chk                      |
      |   Suppress system messages  GT_Chk                      |
      +---------------------------------------------------------+
**/

#define		CH2GT_GADS							13

VOID xOpenFCMiscellaneousSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	UBYTE *					found_labels[] = { "", "", "", "", "", "", "", NULL };
	UBYTE *					copied_labels[] = { "", "", "", "", "", NULL };
	ULONG					txtlength;
	ULONG					cyc,d;
	UWORD					wh,ww;
	UWORD					stringgadlength = xProportionalLength( ScreenFont, 25 );
	
	
	txtlength = 15 + xMaxStringLength( ScreenFont, MSG_FCMI_FILEFILTER, MSG_FCMI_SUPPRESSSYSTEMMESSAGES );
	cyc  = 35 + xMaxStringLength( ScreenFont, MSG_FCMI_DISABLE, MSG_FCMI_NOTHING );
	
	if( cyc < stringgadlength )
		cyc = stringgadlength;
	
	for( d = MSG_FCMI_DISABLE; d <= MSG_FCMI_NOTHING; d++ )
		found_labels[d - MSG_FCMI_DISABLE] = GCS( d );
	
	copied_labels[0] = GCS( MSG_FCMI_DISABLE );
	copied_labels[1] = GCS( MSG_FCMI_RENAME );
	copied_labels[2] = GCS( MSG_FCMI_COMMENT );
	copied_labels[3] = GCS( MSG_FCMI_REQUEST );
	copied_labels[4] = GCS( MSG_FCMI_NOTHING );
	
	if( gads = AllocVec( sizeof( struct xGadget ) * CH2GT_GADS, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
		ng.ng_Width			= cyc;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= gads[0].xg_GadgetText = GCS( MSG_FCMI_FILEFILTER );
		ng.ng_GadgetID		= gads[0].xg_GadgetID = GTID_FC_MI_FILEFILTER;
		
		gads[0].xg_Gad = CreateGadget( STRING_KIND, gads[0].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTBUFFERLEN,
			GTST_String,	XSettings->xs_FC_MIS_MatchFilter,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ScrFontYSize + 8;
		ng.ng_GadgetText	= gads[1].xg_GadgetText = GCS( MSG_FCMI_ARCHIVEFILTER );
		ng.ng_GadgetID		= gads[1].xg_GadgetID = GTID_FC_MI_ARCHIVEFILTER;
		
		gads[1].xg_Gad = CreateGadget( STRING_KIND, gads[0].xg_Gad, &ng,
			GTST_MaxChars,	DEFAULTBUFFERLEN,
			GTST_String,	XSettings->xs_FC_MIS_ArchiveFilter,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ScrFontYSize + 8;
		ng.ng_GadgetText	= gads[2].xg_GadgetText = GCS( MSG_FCMI_DOWITHVIRUSFOUND );
		ng.ng_GadgetID		= gads[2].xg_GadgetID = GTID_FC_MI_DOWITHVIRUSFOUND;
		
		gads[2].xg_Gad = CreateGadget( CYCLE_KIND, gads[1].xg_Gad, &ng,
			GTCY_Active,	XSettings->xs_FC_MIS_ActionWhenVirusFound,
			GTCY_Labels,	found_labels,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ScrFontYSize + 8;
		ng.ng_GadgetText	= gads[3].xg_GadgetText = GCS( MSG_FCMI_DOWITHVIRUSCOPIED );
		ng.ng_GadgetID		= gads[3].xg_GadgetID = GTID_FC_MI_DOWITHVIRUSCOPIED;
		
		gads[3].xg_Gad = CreateGadget( CYCLE_KIND, gads[2].xg_Gad, &ng,
			GTCY_Active,	XSettings->xs_FC_MIS_ActionWhenVirusCopied,
			GTCY_Labels,	copied_labels,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ScrFontYSize + 8;
		ng.ng_GadgetText	= gads[4].xg_GadgetText = GCS( MSG_FCMI_COMMENTFORVIRUS );
		ng.ng_GadgetID		= gads[4].xg_GadgetID = GTID_FC_MI_COMMENTFORVIRUS;
		
		gads[4].xg_Gad = CreateGadget( STRING_KIND, gads[3].xg_Gad, &ng,
			GTST_MaxChars,	80,
			GTST_String,	XSettings->xs_FC_MIS_VirusComment,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ScrFontYSize + 8;
		ng.ng_GadgetText	= gads[5].xg_GadgetText = GCS( MSG_FCMI_RENAMEEXTENSION );
		ng.ng_GadgetID		= gads[5].xg_GadgetID = GTID_FC_MI_RENAMEEXTENSION;
		
		gads[5].xg_Gad = CreateGadget( STRING_KIND, gads[4].xg_Gad, &ng,
			GTST_MaxChars,	32,
			GTST_String,	XSettings->xs_FC_MIS_VirusRenameExtension,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ScrFontYSize + 8;
		ng.ng_Width			= xProportionalLength( ScreenFont, 5 );
		ng.ng_GadgetText	= gads[6].xg_GadgetText = GCS( MSG_FCMI_PATHINDENTION );
		ng.ng_GadgetID		= gads[6].xg_GadgetID = GTID_FC_MI_PATHINDENTION;
		
		gads[6].xg_Gad = CreateGadget( INTEGER_KIND, gads[5].xg_Gad, &ng,
			GTIN_MaxChars,	2,
			GTIN_Number,	XSettings->xs_FC_MIS_PathIndention,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_LeftEdge		= txtlength;
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= 26;
		ng.ng_Height		= ScrFontYSize + 3;
		ng.ng_GadgetText	= gads[7].xg_GadgetText = GCS( MSG_FCMI_AUTODEPROTECT );
		ng.ng_GadgetID		= gads[7].xg_GadgetID = GTID_FC_MI_AUTODEPROTECT;
		
		gads[7].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[6].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_MIS_AutoDeprotect,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[8].xg_GadgetText = GCS( MSG_FCMI_ENTERDIRECTORIES );
		ng.ng_GadgetID		= gads[8].xg_GadgetID = GTID_FC_MI_ENTERDIRECTORIES;
		
		gads[8].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[7].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_MIS_EnterRecursively,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Auto path check
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[9].xg_GadgetText = GCS( MSG_FCMI_AUTOPATHCHECK );
		ng.ng_GadgetID		= gads[9].xg_GadgetID = GTID_FC_MI_AUTOPATHCHECK;
		
		gads[9].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[8].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_MIS_AutoPathCheck,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Suppress system messages
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= gads[10].xg_GadgetText = GCS( MSG_FCMI_SUPPRESSSYSTEMMESSAGES );
		ng.ng_GadgetID		= gads[10].xg_GadgetID = GTID_FC_MI_SUPPRESSSYSMSGS;
		
		gads[10].xg_Gad = CreateGadget( CHECKBOX_KIND, gads[9].xg_Gad, &ng,
			GTCB_Checked,	XSettings->xs_FC_MIS_SuppressSystemMessages,
			GTCB_Scaled,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Okay and Cancel
		
 		wh = ng.ng_TopEdge + ng.ng_Height + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[11].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[11].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[9].xg_Gad,
			BUT_Text,		gads[11].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[12].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			gads[12].xg_GadgetID = GTID_CANCEL,
			GA_Left,		txtlength + cyc - ConfirmGadWidth,
			GA_Top,			wh,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[11].xg_Gad,
			BUT_Text,		gads[12].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh += GadgetHeight + CONFIRMGAP + 3;
		ww = txtlength + cyc + 10;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_FCMI_SETTINGS ), ww, wh, glist, gads, 12 ) )
			{
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_FCMI_BAR_MISCELLANEOUS ) );
			xBarTxt( win, gads[12].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
			if( xHandleFCMiscellaneousSettings( win, gads ) )
				{
				SettingsChanged = TRUE;
				
				strcpy( XSettings->xs_FC_MIS_MatchFilter, GETSTRING( gads[0].xg_Gad ) );
				strcpy( XSettings->xs_FC_MIS_ArchiveFilter, GETSTRING( gads[1].xg_Gad ) );
				XSettings->xs_FC_MIS_ActionWhenVirusFound = xCycleActive( gads[2].xg_Gad, win );
				XSettings->xs_FC_MIS_ActionWhenVirusCopied = xCycleActive( gads[3].xg_Gad, win );
				strcpy( XSettings->xs_FC_MIS_VirusComment, GETSTRING( gads[4].xg_Gad ) );
				strcpy( XSettings->xs_FC_MIS_VirusRenameExtension, GETSTRING( gads[5].xg_Gad ) );
				XSettings->xs_FC_MIS_PathIndention = GETNUMBER( gads[6].xg_Gad );
				XSettings->xs_FC_MIS_AutoDeprotect = xIsGadSelected( gads[7].xg_Gad );
				XSettings->xs_FC_MIS_EnterRecursively = xIsGadSelected( gads[8].xg_Gad );
				XSettings->xs_FC_MIS_AutoPathCheck = xIsGadSelected( gads[9].xg_Gad );
				XSettings->xs_FC_MIS_SuppressSystemMessages = xIsGadSelected( gads[10].xg_Gad );
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[12].xg_Gad );
		DisposeObject( gads[11].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleFCMiscellaneousSettings( struct Window *win, struct xGadget *gads )
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
							xGuideDisplay( AGH_STGS_FCHK_MISCELLANEOUS );
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
									
									case GTID_FC_MI_DOWITHVIRUSFOUND:
										xRefreshCycleGadget( win, tmpgad->xg_Gad, qualifier, 6 );
										break;
									
									case GTID_FC_MI_DOWITHVIRUSCOPIED:
										xRefreshCycleGadget( win, tmpgad->xg_Gad, qualifier, 4 );
										break;
									
									case GTID_FC_MI_FILEFILTER:
									case GTID_FC_MI_ARCHIVEFILTER:
									case GTID_FC_MI_COMMENTFORVIRUS:
									case GTID_FC_MI_RENAMEEXTENSION:
									case GTID_FC_MI_PATHINDENTION:
										ActivateGadget( tmpgad->xg_Gad, win, NULL );
										break;
									
									case GTID_FC_MI_AUTODEPROTECT:
									case GTID_FC_MI_ENTERDIRECTORIES:
									case GTID_FC_MI_AUTOPATHCHECK:
									case GTID_FC_MI_SUPPRESSSYSMSGS:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											{
											GT_SetGadgetAttrs( tmpgad->xg_Gad, win, NULL,
												GTCB_Checked, xIsGadSelected( tmpgad->xg_Gad ) ? FALSE : TRUE,
												TAG_END );
											
											code = tmpgad->xg_GadgetID;
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
