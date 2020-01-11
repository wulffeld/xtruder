/*
**	$VER: Xt_ReportsCreateWindow.c (8.8.96)
**
**	Reports: Create interface
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		SPACE_BETWEEN_GADS				4


/*
 * Calculates the minimum width for the two gadgets 'Stop', 'Pause'.
 */
UWORD xCalc_CRITopGadgetsWidth( VOID )
{
	UWORD	max = 15 + xMaxStringLength( TextFont, MSG_CRI_GAD_STOP, MSG_CRI_GAD_PAUSE );
	
	
	return( (UWORD)(max * 2 + SPACE_BETWEEN_GADS) );
}

/*
 * Calculates the minimum height of the main report listview
 */
UWORD xCalc_CRIListViewHeight( VOID )
{
	return( (UWORD)((4 * TxtFontYSize) + 4) );
}

/*
 * Calculates the minimum width of the main report window
 */
UWORD xCalc_CRIMinWindowWidth( VOID )
{
	return( (UWORD)(20 + xCalc_CRITopGadgetsWidth()) );
}

/*
 * Calculates the minimum height of the main report window
 */
UWORD xCalc_CRIMinWindowHeight( VOID )
{
	UWORD	minheight;
	
	
	minheight = ScreenBarHeight + 2 + ((TxtFontYSize + 6) + 2) * 3 + 10;
	minheight += xCalc_CRIListViewHeight() + 2;
	
	return( minheight );
}

VOID xRemoveCRIWindowGadgets( VOID )
{
		/// Remove gadgets from window
	
	CRIBoxWidth = 0;
	
	if( CRIWindowGadgets )
		{
		if( CRIWindowGadList )
			{
			RemoveGList( CRIWindow, CRIWindowGadList, -1 );
			
			FreeGadgets( CRIWindowGadList );
			CRIWindowGadList = NULL;
			}
		
		DisposeObject( CRIWindowGadgets[ 3 ].xg_Gad );			/// Pause
		DisposeObject( CRIWindowGadgets[ 2 ].xg_Gad );			/// Stop
		
		FreeVec( CRIWindowGadgets );
		CRIWindowGadgets = NULL;
		}
}

BOOL xCreateCRIWindowGadgets( VOID )
{
	struct NewGadget	ng;
	UWORD				n,g;
	BOOL				success = TRUE;
	
	
		/// Remove previous gadgets
	
	xRemoveCRIWindowGadgets();
	
		/// Refresh window so it's as neat as a baby's bottom
	
	SetAPen( CRIWindow->RPort, 0 );
	RectFill( CRIWindow->RPort, CRIWindow->BorderLeft, CRIWindow->BorderTop, CRIWindow->Width - CRIWindow->BorderRight - 1,  CRIWindow->Height - CRIWindow->BorderBottom - 1 );
	SetAPen( CRIWindow->RPort, 1 );
	
	RefreshWindowFrame( CRIWindow );
	
		/// Calculate something
	
	CRIBoxWidth = __builtin_max( CRIWindow->Width - 20, xCalc_CRIMinWindowWidth() );
	
		/// Start creating those gadgets
	
	if( CRIWindowGadgets = AllocVec( sizeof( struct xGadget ) * 5, MEMF_CLEAR ) )
		{
		CRIWindowGadgets[0].xg_Gad = CreateContext( &CRIWindowGadList );
		
			/// Path
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= 10;
		ng.ng_TopEdge		= ScreenBarHeight + 2;
		ng.ng_Width			= CRIBoxWidth;
		ng.ng_Height		= (TxtFontYSize + 6);
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &TextFontAttr;
		ng.ng_GadgetText	= "";
		ng.ng_GadgetID		= CheckWindowGadgets[0].xg_GadgetID = GTID_CRI_PATH;
		
		CRIWindowGadgets[0].xg_Gad = CreateGadget( TEXT_KIND, CRIWindowGadgets[0].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// File
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= "";
		ng.ng_GadgetID		= CRIWindowGadgets[1].xg_GadgetID = GTID_CRI_FILE;
		
		CRIWindowGadgets[1].xg_Gad = CreateGadget( TEXT_KIND, CRIWindowGadgets[0].xg_Gad, &ng,
			GTTX_Border,	TRUE,
			GT_Underscore,	'_',
			TAG_END );
		
			/// Stop
		
		CRIWindowGadgets[2].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			CRIWindowGadgets[2].xg_GadgetID = GTID_CRI_STOP,
			GA_Left,		10,
			GA_Top,			ng.ng_TopEdge += ng.ng_Height + 2,
			GA_Width,		n = ((CRIWindow->Width - 20) - SPACE_BETWEEN_GADS) / 2,
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	CRIWindowGadgets[1].xg_Gad,
			GA_Disabled,	!StillCreatingReport,
			BUT_Text,		CRIWindowGadgets[2].xg_GadgetText = GCS( MSG_CRI_GAD_STOP ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Pause
		
		CRIWindowGadgets[3].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			CRIWindowGadgets[3].xg_GadgetID = GTID_CRI_PAUSE,
			GA_Left,		10 + n + SPACE_BETWEEN_GADS,
			GA_Top,			ng.ng_TopEdge,
			GA_Width,		CRIBoxWidth - (n + SPACE_BETWEEN_GADS),
			GA_Height,		(TxtFontYSize + 6),
			GA_RelVerify,	TRUE,
			GA_Previous,	CRIWindowGadgets[2].xg_Gad,
			GA_Disabled,	!StillCreatingReport,
			BUT_Text,		CRIWindowGadgets[3].xg_GadgetText = GCS( MSG_CRI_GAD_PAUSE ),
			BUT_TextFont,	TextFont,
			TAG_DONE );
		
			/// Error listview
		
		ng.ng_TopEdge		+= TxtFontYSize + 6 + 2;
		ng.ng_Width			= CRIBoxWidth;
		ng.ng_Height		= CRIWindow->Height - CRIWindow->BorderBottom - 2 - ng.ng_TopEdge;
		ng.ng_Height		= (((ng.ng_Height - 4) / TxtFontYSize) * TxtFontYSize) + 4;
		ng.ng_GadgetText	= "";
		ng.ng_GadgetID		= CRIWindowGadgets[4].xg_GadgetID = GTID_CRI_ERRORSLISTVIEW;
		
		CRIWindowGadgets[4].xg_Gad = CreateGadget( LISTVIEW_KIND, CRIWindowGadgets[3].xg_Gad, &ng,
			GTLV_ReadOnly,		TRUE,
			GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
			TAG_END );
		
		CRIErrorsListViewVisible = (ng.ng_Height - 4) / TxtFontYSize;
		
			/// Check if all gadgets were created
		
		for( g = 0; g < 4; g++ )
			{
			if( !CRIWindowGadgets[ g ].xg_Gad || !CRIWindowGadgets[ g + 1 ].xg_Gad )
				success = FALSE;
			
			CRIWindowGadgets[ g ].xg_NextGad = &CRIWindowGadgets[ g + 1 ];
			}
		
		CRIWindowGadgets[ g ].xg_NextGad = NULL;
		
		if( success )
			{
			AddGList( CRIWindow, CRIWindowGadList, -1, -1, NULL );
			RefreshGList( CRIWindowGadList, CRIWindow, NULL, -1 );
			
			GT_RefreshWindow( CRIWindow, NULL );
			
			return( TRUE );
			}
		}
	
		/// Free what we DID create...
	
	xRemoveCRIWindowGadgets();
	
	return( FALSE );
}

/*
 * Report window uses TextFont!
 */
BOOL xOpenCRIWindow( VOID )
{
	UWORD	minheight = xCalc_CRIMinWindowHeight();
	
	
	if( Screen )
		{
		NewList( &CRIErrorsList );
		
		if( CRIWindow = OpenWindowTags( NULL,
			WA_Left,			Coords->xc_ReportsCreateWindow[0],
			WA_Top,				Coords->xc_ReportsCreateWindow[1],
			WA_Width,			__builtin_max( xCalc_CRIMinWindowWidth() + 20, Coords->xc_ReportsCreateWindow[2] ),
			WA_Height,			__builtin_max( minheight, Coords->xc_ReportsCreateWindow[3] ),
			WA_IDCMP,			IDCMP_REFRESHWINDOW |
								IDCMP_RAWKEY |
								IDCMP_GADGETUP |
								IDCMP_GADGETDOWN |
								IDCMP_INTUITICKS |
								IDCMP_MOUSEBUTTONS |
								IDCMP_MOUSEMOVE |
								IDCMP_CLOSEWINDOW |
								IDCMP_NEWSIZE,
			WA_PubScreen,		Screen,
			WA_Title,			GCS( MSG_CRI_TL_REPORTCREATION ),
			WA_DragBar,			TRUE,
			WA_CloseGadget,		TRUE,
			WA_Activate,		TRUE,
			WA_DepthGadget,		TRUE,
			WA_RMBTrap,			TRUE,
			WA_SizeGadget,		TRUE,
			WA_SizeBBottom,		TRUE,
			WA_SmartRefresh,	TRUE,
			WA_MinWidth,		xCalc_CRIMinWindowWidth() + 20,
			WA_MinHeight,		minheight,
			WA_MaxWidth,		-1,
			WA_MaxHeight,		-1,
			TAG_END ) )
			{
			StillCreatingReport = TRUE;
			
			if( xCreateCRIWindowGadgets() )
				{
				SetFont( CRIWindow->RPort, TextFont );
				
				return( TRUE );
				}
			
			xCloseCRIWindow();
			}
		}
	
	return( FALSE );
}

VOID xCloseCRIWindow( VOID )
{
	xRemoveCRIWindowGadgets();
	
	if( CRIWindow )
		{
			/// Store position of window
		
		Coords->xc_ReportsCreateWindow[0] = CRIWindow->LeftEdge;
		Coords->xc_ReportsCreateWindow[1] = CRIWindow->TopEdge;
		Coords->xc_ReportsCreateWindow[2] = CRIWindow->Width;
		Coords->xc_ReportsCreateWindow[3] = CRIWindow->Height;
		
			/// Close check window
		
		CloseWindow( CRIWindow );
		CRIWindow = NULL;
		}
	
		/// Free list
	
	xFreeErrorNodes();
}

VOID xCRI_UpdatePathGadget( STRPTR pathname )
{
	static UBYTE	pathnamebuf[ 250 ];
	
	
	strncpy( pathnamebuf, pathname, 249 );
	
	GT_SetGadgetAttrs( CRIWindowGadgets[ 0 ].xg_Gad, CRIWindow, NULL, GTTX_Text, pathnamebuf, TAG_END );
}

VOID xCRI_UpdateFileGadget( STRPTR filename )
{
	static UBYTE	filenamebuf[ 50 ];
	
	
	strncpy( filenamebuf, filename, 49 );
	
	GT_SetGadgetAttrs( CRIWindowGadgets[ 1 ].xg_Gad, CRIWindow, NULL, GTTX_Text, filenamebuf, TAG_END );
}

UWORD xCheckCRIWindowMsg( struct CurrentReport *cr )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	ULONG					msgclass;
	APTR					ia;
	UWORD					msgcode;
	UWORD					qualifier;
	struct NewLVPos			nlv = {0,0};
	LONG					curvistop;
	
	
	while( msg = GT_GetIMsg( CRIWindow->UserPort ) )
		{
		msgclass = msg->Class;
		msgcode = msg->Code;
		qualifier = msg->Qualifier;
		ia = msg->IAddress;
		
		if( msgclass == IDCMP_GADGETUP )
			msgcode = ((struct Gadget *)msg->IAddress)->GadgetID;
		
		GT_ReplyIMsg( msg );
		
		switch( msgclass )
			{
			case IDCMP_REFRESHWINDOW:
				GT_BeginRefresh( CRIWindow );
				GT_EndRefresh( CRIWindow, TRUE );
				break;
			
			case IDCMP_NEWSIZE:
				xCreateCRIWindowGadgets();
				break;
			
			case IDCMP_RAWKEY:
				switch( msgcode )
					{
					case CURSORUP:
					case CURSORDOWN:
						curvistop = xListViewTop( CRIWindowGadgets[ 4 ].xg_Gad, CRIWindow );
						
						xNewListPos( &nlv, qualifier, msgcode, msgcode == CURSORUP ? curvistop : curvistop + CRIErrorsListViewVisible - 1, curvistop, CRIErrorsListViewVisible, xCountListEntries( &CRIErrorsList ) );
						
						GT_SetGadgetAttrs( CRIWindowGadgets[ 4 ].xg_Gad, CRIWindow, NULL,
							GTLV_Top,		nlv.nl_NewTop,
							TAG_END );
						break;
					
					case KEY_ESCAPE:
						if( cr->cr_Result == CCRES_CHECKEND )
							return( CCRES_CHECKEND );
						break;
					
					default:
						if( tmpgad = xCheckUnderscore( CRIWindowGadgets, msgcode, qualifier, ia ) )
							{
							switch( tmpgad->xg_GadgetID )
								{
								case GTID_CRI_STOP:
								case GTID_CRI_PAUSE:
									if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
										{
										xPressButton( CRIWindow, tmpgad->xg_Gad );
										
										msgcode = tmpgad->xg_GadgetID;
										}
									break;
								}
							}
						else
							break;
					}
			
			case IDCMP_GADGETUP:
				switch( msgcode )
					{
					case GTID_CRI_STOP:
						return( CW_STOP );
						break;
					
					case GTID_CRI_PAUSE:
						xReqMsg( Window, GCS( MSG_REQ_INFO ), GCS( MSG_CW_OPERATIONPAUSED ), GCS( MSG_GF_CONTINUE ) );
						break;
					}
				break;
			
			case IDCMP_CLOSEWINDOW:
				return( CCRES_CHECKEND );
				break;
			}
		}
	
	return( CW_NOTHING );
}

LONG xHandleCRIWindowIDCMP( struct CurrentReport *cr )
{
	UWORD					res = CW_NOTHING;
	
	
	if( CRIWindow )
		{
		if( cr->cr_Result && cr->cr_Result != CCRES_CHECKEND )
			return( cr->cr_Result );
		
		if( cr->cr_Result == CCRES_CHECKEND )
			{
				/// It has finished creating the report
			
			StillCreatingReport = FALSE;
			
				/// Clear gadgets
			
			xCRI_UpdatePathGadget( "" );
			xCRI_UpdateFileGadget( "" );
			
				/// Ghost gadgets
			
			xGhostObject( CRIWindow, CRIWindowGadgets[ 2 ].xg_Gad );
			xGhostObject( CRIWindow, CRIWindowGadgets[ 3 ].xg_Gad );
			
				/// Wait for the user to close the window
			
			do
				{
				WaitPort( CRIWindow->UserPort );
				}
			while( xCheckCRIWindowMsg( cr ) != CCRES_CHECKEND );
			}
		else
			res = xCheckCRIWindowMsg( cr );
		}
	
	return( res );
}
