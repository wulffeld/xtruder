/*
**	$VER: XtruderBBWindow.c (16.8.96)
**
**	Xtruder bootblock window interface
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		SPACE_BETWEEN_GADS				4


/*
 * Calculates the minimum width of button gadgets
 */
UWORD xCalc_BBTopGadgetsWidth( VOID )
{
	UWORD	max = 15 + xMaxStringLength( TextFont, MSG_BB_GAD_SHOWDF0, MSG_BB_GAD_SAVE );
	
	
	return( (UWORD)(max * 3 + 2 * SPACE_BETWEEN_GADS) );
}

/*
 * Calculates the minimum height of the main report listview
 */
UWORD xCalc_BBListViewHeight( VOID )
{
	return( (UWORD)((4 * TxtFontYSize) + 4) );
}

/*
 * Calculates the minimum width of the main report window
 */
UWORD xCalc_BBMinWindowWidth( VOID )
{
	return( (UWORD)(20 + xCalc_BBTopGadgetsWidth()) );
}

/*
 * Calculates the minimum height of the main report window
 */
UWORD xCalc_BBMinWindowHeight( VOID )
{
	UWORD	minheight;
	
	
	minheight = ScreenBarHeight + 2 + ((TxtFontYSize + 6) + 2) * 8 + 8;
	minheight += xCalc_BBListViewHeight() + 2;
	
	return( minheight );
}

VOID xRemoveBBWindowGadgets( VOID )
{
	UWORD	n;
	
	
		/// Remove gadgets from window
	
	BBBoxWidth = 0;
	
	if( BBWindowGadgets )
		{
		if( BBWindowGadList )
			{
			RemoveGList( BBWindow, BBWindowGadList, -1 );
			
			FreeGadgets( BBWindowGadList );
			BBWindowGadList = NULL;
			}
		
		DisposeObject( BBWindowGadgets[ 18 ].xg_Gad );
		DisposeObject( BBWindowGadgets[ 17 ].xg_Gad );
		
		for( n = 13; n > 0; n-- )
			DisposeObject( BBWindowGadgets[ n ].xg_Gad );
		
		FreeVec( BBWindowGadgets );
		BBWindowGadgets = NULL;
		}
}

BOOL xCreateBBWindowGadgets( VOID )
{
	struct NewGadget	ng;
	UWORD				n,g,top;
	UWORD				txtgads_left = 10 + xMaxStringLength( TextFont, MSG_BB_GAD_STATUS, MSG_BB_GAD_BUFFER );
	BOOL				success = TRUE;
	
	
		/// Remove previous gadgets
	
	xRemoveBBWindowGadgets();
	
		/// Refresh window so it's as neat as a baby's bottom
	
	SetAPen( BBWindow->RPort, 0 );
	RectFill( BBWindow->RPort, BBWindow->BorderLeft, BBWindow->BorderTop, BBWindow->Width - BBWindow->BorderRight - 1,  BBWindow->Height - BBWindow->BorderBottom - 1 );
	SetAPen( BBWindow->RPort, 1 );
	
	RefreshWindowFrame( BBWindow );
	
		/// Calculate something
	
	BBBoxWidth = __builtin_max( BBWindow->Width - 20, xCalc_BBMinWindowWidth() );
	
		/// Start creating those gadgets
	
	if( BBWindowGadgets = AllocVec( sizeof( struct xGadget ) * 19, MEMF_CLEAR ) )
		{
		BBWindowGadgets[0].xg_Gad = CreateContext( &BBWindowGadList );
		
			/// Show DF0
		
		BBWindowGadgets[0].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[0].xg_GadgetID = GTID_BB_SHOWDF0,
			GA_Left,		10,
			GA_Top,			top = ScreenBarHeight + 2,
			GA_Width,		n = ((BBWindow->Width - 20) - 2 * SPACE_BETWEEN_GADS) / 3,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[0].xg_Gad,
			GA_Disabled,	!DrvUnit[0],
			BUT_Text,		BBWindowGadgets[0].xg_GadgetText = GCS( MSG_BB_GAD_SHOWDF0 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Check DF0
		
		BBWindowGadgets[1].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[1].xg_GadgetID = GTID_BB_CHECKDF0,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS,
			GA_Top,			top,
			GA_Width,		n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[0].xg_Gad,
			GA_Disabled,	!DrvUnit[0],
			BUT_Text,		BBWindowGadgets[1].xg_GadgetText = GCS( MSG_BB_GAD_CHECKDF0 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Install DF0
		
		BBWindowGadgets[2].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[2].xg_GadgetID = GTID_BB_INSTALLDF0,
			GA_Left,		10 + (n + SPACE_BETWEEN_GADS) * 2,
			GA_Top,			top,
			GA_Width,		BBBoxWidth - (n + SPACE_BETWEEN_GADS) * 2,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[1].xg_Gad,
			GA_Disabled,	!DrvUnit[0],
			BUT_Text,		BBWindowGadgets[2].xg_GadgetText = GCS( MSG_BB_GAD_INSTALLDF0 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Show DF1
		
		BBWindowGadgets[3].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[3].xg_GadgetID = GTID_BB_SHOWDF1,
			GA_Left,		10,
			GA_Top,			top += TxtFontYSize + 6 + 2,
			GA_Width,		n = ((BBWindow->Width - 20) - 2 * SPACE_BETWEEN_GADS) / 3,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[2].xg_Gad,
			GA_Disabled,	!DrvUnit[1],
			BUT_Text,		BBWindowGadgets[3].xg_GadgetText = GCS( MSG_BB_GAD_SHOWDF1 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Check DF1
		
		BBWindowGadgets[4].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[4].xg_GadgetID = GTID_BB_CHECKDF1,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS,
			GA_Top,			top,
			GA_Width,		n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[3].xg_Gad,
			GA_Disabled,	!DrvUnit[1],
			BUT_Text,		BBWindowGadgets[4].xg_GadgetText = GCS( MSG_BB_GAD_CHECKDF1 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Install DF1
		
		BBWindowGadgets[5].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[5].xg_GadgetID = GTID_BB_INSTALLDF1,
			GA_Left,		10 + (n + SPACE_BETWEEN_GADS) * 2,
			GA_Top,			top,
			GA_Width,		BBBoxWidth - (n + SPACE_BETWEEN_GADS) * 2,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[4].xg_Gad,
			GA_Disabled,	!DrvUnit[1],
			BUT_Text,		BBWindowGadgets[5].xg_GadgetText = GCS( MSG_BB_GAD_INSTALLDF1 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Show DF2
		
		BBWindowGadgets[6].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[6].xg_GadgetID = GTID_BB_SHOWDF2,
			GA_Left,		10,
			GA_Top,			top += TxtFontYSize + 6 + 2,
			GA_Width,		n = ((BBWindow->Width - 20) - 2 * SPACE_BETWEEN_GADS) / 3,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[5].xg_Gad,
			GA_Disabled,	!DrvUnit[2],
			BUT_Text,		BBWindowGadgets[6].xg_GadgetText = GCS( MSG_BB_GAD_SHOWDF2 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Check DF2
		
		BBWindowGadgets[7].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[7].xg_GadgetID = GTID_BB_CHECKDF2,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS,
			GA_Top,			top,
			GA_Width,		n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[6].xg_Gad,
			GA_Disabled,	!DrvUnit[2],
			BUT_Text,		BBWindowGadgets[7].xg_GadgetText = GCS( MSG_BB_GAD_CHECKDF2 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Install DF2
		
		BBWindowGadgets[8].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[8].xg_GadgetID = GTID_BB_INSTALLDF2,
			GA_Left,		10 + (n + SPACE_BETWEEN_GADS) * 2,
			GA_Top,			top,
			GA_Width,		BBBoxWidth - (n + SPACE_BETWEEN_GADS) * 2,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[7].xg_Gad,
			GA_Disabled,	!DrvUnit[2],
			BUT_Text,		BBWindowGadgets[8].xg_GadgetText = GCS( MSG_BB_GAD_INSTALLDF2 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Show DF3
		
		BBWindowGadgets[9].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[9].xg_GadgetID = GTID_BB_SHOWDF3,
			GA_Left,		10,
			GA_Top,			top += TxtFontYSize + 6 + 2,
			GA_Width,		n = ((BBWindow->Width - 20) - 2 * SPACE_BETWEEN_GADS) / 3,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[8].xg_Gad,
			GA_Disabled,	!DrvUnit[3],
			BUT_Text,		BBWindowGadgets[9].xg_GadgetText = GCS( MSG_BB_GAD_SHOWDF3 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Check DF3
		
		BBWindowGadgets[10].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[10].xg_GadgetID = GTID_BB_CHECKDF3,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS,
			GA_Top,			top,
			GA_Width,		n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[9].xg_Gad,
			GA_Disabled,	!DrvUnit[3],
			BUT_Text,		BBWindowGadgets[10].xg_GadgetText = GCS( MSG_BB_GAD_CHECKDF3 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Install DF3
		
		BBWindowGadgets[11].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[11].xg_GadgetID = GTID_BB_INSTALLDF3,
			GA_Left,		10 + (n + SPACE_BETWEEN_GADS) * 2,
			GA_Top,			top,
			GA_Width,		BBBoxWidth - (n + SPACE_BETWEEN_GADS) * 2,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[10].xg_Gad,
			GA_Disabled,	!DrvUnit[3],
			BUT_Text,		BBWindowGadgets[11].xg_GadgetText = GCS( MSG_BB_GAD_INSTALLDF3 ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Show DMS
		
		BBWindowGadgets[12].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[12].xg_GadgetID = GTID_BB_SHOWDMS,
			GA_Left,		10,
			GA_Top,			top += TxtFontYSize + 6 + 2,
			GA_Width,		n = ((BBWindow->Width - 20) - 2 * SPACE_BETWEEN_GADS) / 3,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[11].xg_Gad,
			BUT_Text,		BBWindowGadgets[12].xg_GadgetText = GCS( MSG_BB_GAD_SHOWDMS ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Check DMS
		
		BBWindowGadgets[13].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[13].xg_GadgetID = GTID_BB_CHECKDMS,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS,
			GA_Top,			top,
			GA_Width,		n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[12].xg_Gad,
			BUT_Text,		BBWindowGadgets[13].xg_GadgetText = GCS( MSG_BB_GAD_CHECKDMS ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Status
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_TextAttr		= &TextFontAttr;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_LeftEdge		= 10 + txtgads_left;
		ng.ng_TopEdge		= top + TxtFontYSize + 6 + 2;
		ng.ng_Width			= BBBoxWidth - txtgads_left;
		ng.ng_Height		= TxtFontYSize + 6;
		ng.ng_GadgetText	= BBWindowGadgets[14].xg_GadgetText = GCS( MSG_BB_GAD_STATUS );
		ng.ng_GadgetID		= BBWindowGadgets[14].xg_GadgetID = GTID_BB_STATUS;
		
		BBWindowGadgets[14].xg_Gad = CreateGadget( TEXT_KIND, BBWindowGadgets[13].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			TAG_END );
		
			/// Buffer
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Height		= TxtFontYSize + 6;
		ng.ng_GadgetText	= BBWindowGadgets[15].xg_GadgetText = GCS( MSG_BB_GAD_BUFFER );
		ng.ng_GadgetID		= BBWindowGadgets[15].xg_GadgetID = GTID_BB_BUFFER;
		
		BBWindowGadgets[15].xg_Gad = CreateGadget( TEXT_KIND, BBWindowGadgets[14].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			TAG_END );
		
			/// Buffer listview
		
		ng.ng_LeftEdge		= 10;
		ng.ng_TopEdge		+= TxtFontYSize + 6 + 2;
		ng.ng_Width			= BBBoxWidth;
		ng.ng_Height		= BBWindow->Height - BBWindow->BorderBottom - 2 - ng.ng_TopEdge - TxtFontYSize - 6;
		ng.ng_Height		= (((ng.ng_Height - 4) / TxtFontYSize) * TxtFontYSize) + 4;
		ng.ng_GadgetText	= BBWindowGadgets[16].xg_GadgetText = "";
		ng.ng_GadgetID		= BBWindowGadgets[16].xg_GadgetID = GTID_BB_BUFFERLISTVIEW;
		
		BBWindowGadgets[16].xg_Gad = CreateGadget( LISTVIEW_KIND, BBWindowGadgets[15].xg_Gad, &ng,
			GTLV_Labels,		&BBBufferList,
			GTLV_ReadOnly,		TRUE,
			GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
			TAG_END );
		
		BBBufferListViewVisible = (ng.ng_Height - 4) / TxtFontYSize;
		
			/// Load
		
		BBWindowGadgets[17].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[17].xg_GadgetID = GTID_BB_LOAD,
			GA_Left,		10,
			GA_Top,			top = ng.ng_TopEdge + ng.ng_Height,
			GA_Width,		n = (BBWindow->Width - 20) / 2,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[16].xg_Gad,
			BUT_Text,		BBWindowGadgets[17].xg_GadgetText = GCS( MSG_BB_GAD_LOAD ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Save
		
		BBWindowGadgets[18].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			BBWindowGadgets[18].xg_GadgetID = GTID_BB_SAVE,
			GA_Left,		10 + n,
			GA_Top,			top,
			GA_Width,		(BBWindow->Width - 20) - n,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	BBWindowGadgets[17].xg_Gad,
			GA_Disabled,	TRUE,
			BUT_Text,		BBWindowGadgets[18].xg_GadgetText = GCS( MSG_BB_GAD_SAVE ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Check if all gadgets were created
		
		for( g = 0; g < 18; g++ )
			{
			if( !BBWindowGadgets[ g ].xg_Gad || !BBWindowGadgets[ g + 1 ].xg_Gad )
				success = FALSE;
			
			BBWindowGadgets[ g ].xg_NextGad = &BBWindowGadgets[ g + 1 ];
			}
		
		BBWindowGadgets[ g ].xg_NextGad = NULL;
		
		if( success )
			{
			AddGList( BBWindow, BBWindowGadList, -1, -1, NULL );
			RefreshGList( BBWindowGadList, BBWindow, NULL, -1 );
			
			GT_RefreshWindow( BBWindow, NULL );
			
			return( TRUE );
			}
		}
	
		/// Free what we DID create...
	
	xRemoveBBWindowGadgets();
	
	return( FALSE );
}

/*
 * Report window uses TextFont!
 */
VOID xOpenBBWindow( UWORD bypass_code, WORD pathcheck )
{
	UWORD	minheight = xCalc_BBMinWindowHeight();
	
	
	if( Screen )
		{
		NewList( &BBBufferList );
		
		if( BBWindow = OpenWindowTags( NULL,
			WA_Left,			Coords->xc_BootblockWindow[0],
			WA_Top,				Coords->xc_BootblockWindow[1],
			WA_Width,			__builtin_max( xCalc_BBMinWindowWidth() + 20, Coords->xc_BootblockWindow[2] ),
			WA_Height,			__builtin_max( minheight, Coords->xc_BootblockWindow[3] ),
			WA_IDCMP,			IDCMP_REFRESHWINDOW |
								IDCMP_RAWKEY |
								IDCMP_GADGETUP |
								IDCMP_GADGETDOWN |
								IDCMP_INTUITICKS |
								IDCMP_MOUSEBUTTONS |
								IDCMP_MOUSEMOVE |
								IDCMP_CLOSEWINDOW |
								IDCMP_DISKINSERTED |
								IDCMP_DISKREMOVED |
								IDCMP_NEWSIZE,
			WA_PubScreen,		Screen,
			WA_Title,			GCS( MSG_BB_TL_BOOTBLOCK ),
			WA_Activate,		TRUE,
			WA_CloseGadget,		TRUE,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_SizeGadget,		TRUE,
			WA_SizeBBottom,		TRUE,
			WA_SmartRefresh,	TRUE,
			WA_MinWidth,		xCalc_BBMinWindowWidth() + 20,
			WA_MinHeight,		minheight,
			WA_MaxWidth,		-1,
			WA_MaxHeight,		-1,
			TAG_END ) )
			{
			if( xCreateBBWindowGadgets() )
				{
				SetFont( BBWindow->RPort, TextFont );
				
				xHandleBBWindowIDCMP( bypass_code, FALSE, pathcheck );
				}
			
			xCloseBBWindow();
			}
		}
}

VOID xCloseBBWindow( VOID )
{
	xRemoveBBWindowGadgets();
	
		/// Store position of window
	
	Coords->xc_BootblockWindow[0] = BBWindow->LeftEdge;
	Coords->xc_BootblockWindow[1] = BBWindow->TopEdge;
	Coords->xc_BootblockWindow[2] = BBWindow->Width;
	Coords->xc_BootblockWindow[3] = BBWindow->Height;
	
		/// Close check window
	
	CloseWindow( BBWindow );
	BBWindow = NULL;
}

VOID xHandleBBWindowIDCMP( UWORD bypass_code, BOOL visiting, WORD pathcheck )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	struct NewLVPos			nlv = {0,0};
	APTR					ia;
	ULONG					class;
	UWORD					code,altcode,qualifier;
	LONG					curvistop,t;
	WORD					unit;
	
	
	while( 1 )
		{
		if( !bypass_code )
			WaitPort( BBWindow->UserPort );
		
		while( (msg = GT_GetIMsg( BBWindow->UserPort )) || (bypass_code) )
			{
			if( msg )
				{
				class = msg->Class;
				code = altcode = msg->Code;
				qualifier = msg->Qualifier;
				ia = msg->IAddress;
				
				if( class == IDCMP_GADGETUP )
					code = ((struct Gadget *)msg->IAddress)->GadgetID;
				
				GT_ReplyIMsg( msg );
				}
			else
				{
				class = IDCMP_GADGETUP;
				code = bypass_code;
				qualifier = 0;
				ia = NULL;
				
					/// Press gadget
				
				if( tmpgad = xFindGadget( BBWindowGadgets, bypass_code ) )
					{
					if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
						{
						Delay( 10 );
						
						xPressButton( BBWindow, tmpgad->xg_Gad );
						}
					}
				}
			
			switch( class )
				{
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( BBWindow );
					GT_EndRefresh( BBWindow, TRUE );
					break;
				
				case IDCMP_NEWSIZE:
					if( !xCreateBBWindowGadgets() )
						return;
					
					xBB_UpdateStatusGadget( TRUE, "" );
					xBB_UpdateBufferGadget( TRUE, "" );
					break;
				
				case IDCMP_DISKINSERTED:
					if( !visiting )
						xLocalHandleInsertedDisk();
					
					xFindPresentDisks();
					break;
				
				case IDCMP_DISKREMOVED:
					xFindPresentDisks();
					break;
				
				case IDCMP_RAWKEY:
					switch( code )
						{
						case CURSORUP:
						case CURSORDOWN:
							curvistop = xListViewTop( BBWindowGadgets[ 16 ].xg_Gad, BBWindow );
							
							xNewListPos( &nlv, qualifier, code, code == CURSORUP ? curvistop : curvistop + BBBufferListViewVisible - 1, curvistop, BBBufferListViewVisible, xCountListEntries( &BBBufferList ) );
							
							GT_SetGadgetAttrs( BBWindowGadgets[ 16 ].xg_Gad, BBWindow, NULL,
								GTLV_Top, nlv.nl_NewTop,
								TAG_END );
							break;
						
						case KEY_ESCAPE:
							return;
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_BOOTBLOCKINTERFACE );
							break;
						
						default:
							if( tmpgad = xCheckUnderscore( BBWindowGadgets, code, qualifier, ia ) )
								{
								switch( tmpgad->xg_GadgetID )
									{
									case GTID_BB_SHOWDF0:
									case GTID_BB_SHOWDF1:
									case GTID_BB_SHOWDF2:
									case GTID_BB_SHOWDF3:
									case GTID_BB_SHOWDMS:
									case GTID_BB_CHECKDF0:
									case GTID_BB_CHECKDF1:
									case GTID_BB_CHECKDF2:
									case GTID_BB_CHECKDF3:
									case GTID_BB_CHECKDMS:
									case GTID_BB_INSTALLDF0:
									case GTID_BB_INSTALLDF1:
									case GTID_BB_INSTALLDF2:
									case GTID_BB_INSTALLDF3:
									case GTID_BB_LOAD:
									case GTID_BB_SAVE:
										if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
											{
											xPressButton( BBWindow, tmpgad->xg_Gad );
											
											code = tmpgad->xg_GadgetID;
											}
										break;
									}
								}
							else
								break;
						}
				
				case IDCMP_GADGETUP:
					switch( code )
						{
						case GTID_BB_SHOWDF0:
						case GTID_BB_SHOWDF1:
						case GTID_BB_SHOWDF2:
						case GTID_BB_SHOWDF3:
						case GTID_BB_SHOWDMS:
							xShowBootblock( code - GTID_BB_SHOWDF0 + DRV_DF0, NULL, TRUE );
							break;
						
						case GTID_BB_CHECKDF0:
						case GTID_BB_CHECKDF1:
						case GTID_BB_CHECKDF2:
						case GTID_BB_CHECKDF3:
						case GTID_BB_CHECKDMS:
							xDisableWindow( BBWindow );
							
							if( xShowBootblock( code - GTID_BB_CHECKDF0 + DRV_DF0, NULL, TRUE ) )
								xCheckBootblockProcess( code - GTID_BB_CHECKDF0 + DRV_DF0, NULL );
							
							if( pathcheck )
								{
								xPathCheckDisk( pathcheck - 1 );
								
								pathcheck = 0;
								}
							
							xEnableWindow( BBWindow );
							break;
						
						case GTID_BB_INSTALLDF0:
						case GTID_BB_INSTALLDF1:
						case GTID_BB_INSTALLDF2:
						case GTID_BB_INSTALLDF3:
							xDisableWindow( BBWindow );
							
							unit = code - GTID_BB_INSTALLDF0 + DRV_DF0;
							
								/// Only show and analyze if install process went okay
							
							if( xInstallBootblockProcess( unit, -1 ) )
								{
								xShowBootblock( unit, NULL, TRUE );
								
								xAnalyzeAndUpdateBufferGad( unit, NULL, &t );
								}
							
							xEnableWindow( BBWindow );
							break;
						
						case GTID_BB_LOAD:
							xShowBootblock( DRV_CUSTOM, NULL, TRUE );
							break;
						
						case GTID_BB_SAVE:
							xSaveAsBinary();
							break;
						}
					break;
				
				case IDCMP_CLOSEWINDOW:
					return;
					break;
				}
			
			if( !msg )
				{
				bypass_code = 0;
				
				if( visiting )
					return;
				}
			}
		}
}

VOID xUpdateLoadSaveGadgets( VOID )
{
	if( IsListEmpty( &BBBufferList ) )
		xGhostObject( BBWindow, BBWindowGadgets[ 18 ].xg_Gad );
	else
		xUnGhostObject( BBWindow, BBWindowGadgets[ 18 ].xg_Gad );
}

VOID __stdargs xBB_UpdateStatusGadget( BOOL useoldcontents, STRPTR format, ... )
{
	static UBYTE	strbuf[ 250 ];
	
	
	if( !useoldcontents )
		RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
	
	if( BBWindow )
		GT_SetGadgetAttrs( BBWindowGadgets[ 14 ].xg_Gad, BBWindow, NULL, GTTX_Text, strbuf, TAG_END );
}

VOID __stdargs xBB_UpdateBufferGadget( BOOL useoldcontents, STRPTR format, ... )
{
	static UBYTE	strbuf[ 250 ];
	
	
	if( !useoldcontents )
		RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
	
	if( BBWindow )
		GT_SetGadgetAttrs( BBWindowGadgets[ 15 ].xg_Gad, BBWindow, NULL, GTTX_Text, strbuf, TAG_END );
}

VOID xCreateBootblockList( VOID )
{
	UBYTE	strbuf[120] = "";
	UBYTE	a;
	
	
	if( BBWindow )
		{
		GT_SetGadgetAttrs( BBWindowGadgets[ 16 ].xg_Gad, BBWindow, NULL,
			GTLV_Labels, ~0,
			TAG_END );
		
		xFreeNameNodes( &BBBufferList );
		
			/// ASCII
		
		for( a = 0; a < 16; a++ )
			{
			switch( a )
				{
				case 0:
					xSPrintF( strbuf, "$0000: " );
					break;
				
				case 1:
				case 2:
				case 3:
					xSPrintF( strbuf, "$00%2lx: ", a * 64 );
					break;
				
				default:
					xSPrintF( strbuf, "$0%3lx: ", a * 64 );
					break;
				}
			
			xConvertBBLine( XStrings->xs_Bootblock + (a * 64), strbuf );
			xAddNameNode( &BBBufferList, strbuf );
			}
		
		xAddNameNode( &BBBufferList, "" );
		
			/// Hex
		
		for( a = 0; a < 64; a++ )
			{
			switch( a )
				{
				case 0:
					xSPrintF( strbuf, "$0000: " );
					break;
				
				case 1:
				case 2:
				case 3:
					xSPrintF( strbuf, "$00%2lx: ", a * 64 );
					break;
				
				default:
					xSPrintF( strbuf, "$0%3lx: ", a * 64 );
					break;
				}
			
			xConvertBBLineToHex( XStrings->xs_Bootblock + (a * 16), strbuf );
			xAddNameNode( &BBBufferList, strbuf );
			}
		
		GT_SetGadgetAttrs( BBWindowGadgets[ 16 ].xg_Gad, BBWindow, NULL,
			GTLV_Labels, &BBBufferList,
			TAG_END );
		}
}

VOID xConvertBBLine( UBYTE *buffer, UBYTE *strbuf )
{
	UBYTE *	ptr = buffer;
	UBYTE	a = 6;		// Align right
	
	
	while( a++ < 70 )
		{
		if( (*ptr < 32) || (*ptr > 126) )
			strbuf[a] = '.';
		else
			strbuf[a] = *ptr;
		
		ptr++;
		}
	
	strbuf[a] = 0;
}

VOID xConvertBBLineToHex( UBYTE *buffer, UBYTE *strbuf )
{
	UBYTE *	ptr = buffer;
	UBYTE	a = 6;					// Align right
	
	
	for( ; a < 100; a++ )
		strbuf[a] = ' ';
	
	strbuf[a] = 0;
	
	xSPrintF( &strbuf[6], "%08lx", *((ULONG *)ptr) );
	ptr += 4;
	xSPrintF( &strbuf[15], "%08lx", *((ULONG *)ptr) );
	ptr += 4;
	xSPrintF( &strbuf[24], "%08lx", *((ULONG *)ptr) );
	ptr += 4;
	xSPrintF( &strbuf[33], "%08lx", *((ULONG *)ptr) );
	ptr += 4;
	strbuf[14] = strbuf[23] = strbuf[32] = strbuf[41] = ' ';
	
	ptr -= 16;
	a = 42;
	
	while( a++ < 58 )
		{
		if( (*ptr < 32) || (*ptr > 126) )
			strbuf[a] = '.';
		else
			strbuf[a] = *ptr;
		
		ptr++;
		}
	
	strbuf[a] = 0;
}

VOID xHandleInsertedDisk( VOID )
{
	LONG	drv;
	
	
	for( drv = 0; drv < 4; drv++ )
		{
		if( DrvUnit[drv] )
			{
			if( xIsDiskInDrive( drv ) && !DiskPre[drv] && XSettings->xs_BOO_CheckInsertedDisks )
				{
					/// Set it to current state
				
				DiskPre[ drv ] = TRUE;
				
					/// Open bootblock GUI and immediately check disk
				
				if( XSettings->xs_BOO_NoInterface )
					{
					xDisableWindow( Window );
					
					if( xShowBootblock( drv, NULL, TRUE ) )
						xCheckBootblockProcess( drv, NULL );
					
					if( XSettings->xs_FC_MIS_AutoPathCheck )
						xPathCheckDisk( drv );
					
					xEnableWindow( Window );
					}
				else
					xOpenBBWindow( GTID_BB_CHECKDF0 + drv, TRUE );
				}
			}
		}
}

VOID xLocalHandleInsertedDisk( VOID )
{
	LONG	drv;
	
	
	for( drv = 0; drv < 4; drv++ )
		{
		if( DrvUnit[drv] )
			{
			if( xIsDiskInDrive( drv ) && !DiskPre[drv] && XSettings->xs_BOO_CheckInsertedDisks )
				{
					/// Set it to current state
				
				DiskPre[ drv ] = TRUE;
				
					/// Open bootblock GUI and immediately check disk
				
				xHandleBBWindowIDCMP( GTID_BB_CHECKDF0 + drv, TRUE, FALSE );
				
				xPathCheckDisk( drv );
				}
			}
		}
}

VOID xPathCheckDisk( WORD drv )
{
	struct List	floppylist;
	
	
		/// Check the files on the disk as well ?
	
	if( XSettings->xs_FC_MIS_AutoPathCheck )
		{
		NewList( &floppylist );
		
		xAddNameNode( &floppylist, "DF%ld:", drv );
		
		xStartCheck( &floppylist, CHECKTYPE_SPECIFIED, BBWindow ? FALSE : TRUE );
		
			/// Free single node
		
		xFreeNameNodes( &floppylist );
		}
}
