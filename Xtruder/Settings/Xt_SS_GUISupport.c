/*
**	$VER: XtruderPrefs_GUISupport.c (14.4.95)
**
**	Xtruder GUI support functions for preferences windows
**
**	Copyright (C) 1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


struct SettingsGUI *xOpenSettingsGUI( STRPTR title, UWORD width, UWORD height, struct Gadget *glist, struct xGadget *gads, UWORD numgads )
{
	static struct TextAttr	ta;
	struct SettingsGUI *	sg;
	UWORD					g;
	BOOL					scrok = TRUE;
	
	
		/// First link gadgets and thereby check if all were created
	
	for( g = 0; g < numgads; g++ )
		{
		if( !gads[g].xg_Gad || !gads[g+1].xg_Gad )
			return( NULL );
		
		gads[g].xg_NextGad = &gads[g+1];
		}
	
	gads[g].xg_NextGad = NULL;
	
	xDisableWindow( Window );
	
	if( sg = AllocVec( sizeof( struct SettingsGUI ), MEMF_CLEAR ) )
		{
		if( height > Screen->Height || width > Screen->Width )
			{
			if( OnPublic )
				{
					/// Clone system font
				
                ta.ta_Name  = DrawInfo->dri_Font->tf_Message.mn_Node.ln_Name;
                ta.ta_YSize = DrawInfo->dri_Font->tf_YSize;
                ta.ta_Style = DrawInfo->dri_Font->tf_Style;
                ta.ta_Flags = DrawInfo->dri_Font->tf_Flags;
				}
			else
				{
                ta.ta_Name  = ScreenFont->tf_Message.mn_Node.ln_Name;
                ta.ta_YSize = ScreenFont->tf_YSize;
                ta.ta_Style = ScreenFont->tf_Style;
                ta.ta_Flags = ScreenFont->tf_Flags;
				}
			
			if( !(sg->sg_Screen = OpenScreenTags( NULL,
				SA_DisplayID,	GetVPModeID( &(Screen->ViewPort) ),
				SA_Overscan,	OSCAN_TEXT,
				SA_Width,		width > Screen->Width ? width : Screen->Width,
				SA_Height,		height + 11,
				SA_Type,		CUSTOMSCREEN,
				SA_Depth,		2,
				SA_AutoScroll,	TRUE,
				SA_Pens,		(ULONG)Pens,
				SA_SharePens,	TRUE,
				SA_Font,		&ta,
				SA_Colors,		ColorSpec,
				SA_Colors32,	ColorTable,
				SA_Title,		title,
				TAG_END ) ) )
				{
				scrok = FALSE;
				
				xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_COULDNOTOPENSCREEN ), GCS( MSG_GF_OKAY ) );
				}
			}
		
		if( scrok )
			{
			if( sg->sg_Window = OpenWindowTags( NULL,
				WA_CustomScreen,	sg->sg_Screen ? sg->sg_Screen : Screen,
				WA_Title,			title,
				WA_IDCMP,			IDCMP_REFRESHWINDOW | IDCMP_GADGETUP |
									IDCMP_GADGETDOWN | IDCMP_CLOSEWINDOW |
									IDCMP_MOUSEMOVE | IDCMP_RAWKEY |
									IDCMP_INTUITICKS,
				WA_Gadgets,			glist,
				WA_Left,			((sg->sg_Screen ? sg->sg_Screen->Width : Screen->Width) - width) / 2,
				WA_Top,				sg->sg_Screen ? ScreenBarHeight : (Screen->Height - height) / 2,
				WA_Width,			width,
				WA_Height,			height,
				WA_DragBar,			TRUE,
				WA_CloseGadget,		TRUE,
				WA_DepthGadget,		TRUE,
				WA_Activate,		TRUE,
				WA_SmartRefresh,	TRUE,
				WA_RMBTrap,			TRUE,
				TAG_END ) )
				{
				sg->sg_OldProcessWin = Process->pr_WindowPtr;
				Process->pr_WindowPtr = sg->sg_Window;
				
				return( sg );
				}
			else
				xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_COULDNOTOPENWINDOW ), GCS( MSG_GF_OKAY ) );
			
			if( sg->sg_Screen )
				CloseScreen( sg->sg_Screen );
			}
		FreeVec( sg );
		}
	
	xEnableWindow( Window );
	
	return( NULL );
}

VOID xCloseSettingsGUI( struct SettingsGUI *sg )
{
	Process->pr_WindowPtr = sg->sg_OldProcessWin;
	
	if( sg->sg_Window )
		CloseWindow( sg->sg_Window );
	
	if( sg->sg_Screen )
		CloseScreen( sg->sg_Screen );
	
	FreeVec( sg );
	
	xEnableWindow( Window );
}
