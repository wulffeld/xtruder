/*
**	$VER: XtruderVirusInformation.c (6.8.96)
**
**	Xtruder functions the virus information interface
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		SPACE_BETWEEN_GADS				4


/*
 * Calculates the minimum width for the two gadgets 'Search' and 'Show all'
 */
UWORD xCalc_VITopGadgetsWidth( VOID )
{
	UWORD	max = 15 + xMaxStringLength( ScreenFont, MSG_VI_SEARCH, MSG_VI_SHOWALL );
	
	
	return( (UWORD)(max * 2 + SPACE_BETWEEN_GADS) );
}

/*
 * Calculate minimum space for leading texts
 */
UWORD xCalc_VILeadingTexts( VOID )
{
	return( (UWORD)(15 + xMaxStringLength( ScreenFont, MSG_VI_DESCRIPTIONSPATH, MSG_VI_SEARCHSTRING3 )) );
}

/*
 * Calculates the minimum height of the virus information listview
 */
UWORD xCalc_VIListViewHeight( VOID )
{
	return( (UWORD)((4 * TxtFontYSize) + 4) );
}

/*
 * Calculates the minimum width of the virus information window
 */
UWORD xCalc_VIMinWindowWidth( VOID )
{
	return( (UWORD)( 20 + xCalc_VITopGadgetsWidth() + xCalc_VILeadingTexts() ) );
}

/*
 * Calculates the minimum height of the virus information window
 */
UWORD xCalc_VIMinWindowHeight( VOID )
{
	UWORD	minheight;
	
	
	minheight = ScreenBarHeight + 2 + 6 * (GadgetHeight + 2) + 6 + 10;
	minheight += xCalc_VIListViewHeight() + 2;
	
	return( minheight );
}

VOID xRemoveVIWindowGadgets( VOID )
{
		/// Remove gadgets from window
	
	if( VIWindowGadgets )
		{
		if( VIWindowGadList )
			{
			RemoveGList( VIWindow, VIWindowGadList, -1 );
			
			FreeGadgets( VIWindowGadList );
			VIWindowGadList = NULL;
			}
		
		DisposeObject( VIWindowGadgets[8].xg_Gad );
		DisposeObject( VIWindowGadgets[7].xg_Gad );
		DisposeObject( VIWindowGadgets[3].xg_Gad );
		DisposeObject( VIWindowGadgets[1].xg_Gad );
		
		FreeVec( VIWindowGadgets );
		VIWindowGadgets = NULL;
		}
	
}

BOOL xCreateVirusInfoWindowGadgets( VOID )
{
	struct NewGadget	ng;
	UWORD				g;
	UWORD				leftedge,gadwidth;
	BOOL				success = TRUE;
	
	
		/// Remove previous gadgets
	
	xRemoveVIWindowGadgets();
	
		/// Refresh window so it's as neat as a baby's bottom
	
	SetAPen( VIWindow->RPort, 0 );
	RectFill( VIWindow->RPort, VIWindow->BorderLeft, VIWindow->BorderTop, VIWindow->Width - VIWindow->BorderRight - 1,  VIWindow->Height - VIWindow->BorderBottom - 1 );
	SetAPen( VIWindow->RPort, 1 );
	
	RefreshWindowFrame( VIWindow );
	
		/// Calculate something
	
	leftedge = xCalc_VILeadingTexts();
	gadwidth = VIWindow->Width - leftedge - 10;

		/// Start creating those gadgets
	
	if( VIWindowGadgets = AllocVec( sizeof( struct xGadget ) * 10, MEMF_CLEAR ) )
		{
		VIWindowGadgets[0].xg_Gad = CreateContext( &VIWindowGadList );
		
			/// Descriptions path
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_LeftEdge		= leftedge;
		ng.ng_TopEdge		= ScreenBarHeight + 2;
		ng.ng_Width			= gadwidth - GETGADWIDTH;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_GadgetText	= VIWindowGadgets[0].xg_GadgetText = GCS( MSG_VI_DESCRIPTIONSPATH );
		ng.ng_GadgetID		= VIWindowGadgets[0].xg_GadgetID = GTID_VI_DESCRIPTIONSPATH;
		
		VIWindowGadgets[0].xg_Gad = CreateGadget( STRING_KIND, VIWindowGadgets[0].xg_Gad, &ng,
			GTST_String,		XSettings->xs_VIN_DescriptionsPath,
			GTST_MaxChars,		DEFAULTPATHLEN,
			GTIN_EditHook,		&CursorDownHook,
			GT_Underscore,		'_',
			TAG_END );
		
			/// Get descriptions path
		
		VIWindowGadgets[1].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			VIWindowGadgets[1].xg_GadgetID = GTID_VI_GETDESCRIPTIONSPATH,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		GETGADWIDTH,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	VIWindowGadgets[0].xg_Gad,
			BUT_Type,		BUTTYPE_DRAWER,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
			/// External viewer
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_GadgetText	= VIWindowGadgets[2].xg_GadgetText = GCS( MSG_VI_EXTERNALVIEWER );
		ng.ng_GadgetID		= VIWindowGadgets[2].xg_GadgetID = GTID_VI_EXTERNALVIEWER;
		
		VIWindowGadgets[2].xg_Gad = CreateGadget( STRING_KIND, VIWindowGadgets[1].xg_Gad, &ng,
			GTST_String,		XSettings->xs_VIN_ExternalViewer,
			GTST_MaxChars,		DEFAULTBUFFERLEN,
			GTIN_EditHook,		&CursorDownHook,
			GT_Underscore,		'_',
			TAG_END );
		
			/// Get descriptions path
		
		VIWindowGadgets[3].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			VIWindowGadgets[3].xg_GadgetID = GTID_VI_GETEXTERNALVIEWER,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width,
			GA_Width,		GETGADWIDTH,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	VIWindowGadgets[2].xg_Gad,
			BUT_Type,		BUTTYPE_FILE,
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
			/// Search string 1
		
		ng.ng_TopEdge		+= ng.ng_Height + 4;
		ng.ng_Width			= gadwidth;
		ng.ng_GadgetText	= VIWindowGadgets[4].xg_GadgetText = GCS( MSG_VI_SEARCHSTRING1 );
		ng.ng_GadgetID		= VIWindowGadgets[4].xg_GadgetID = GTID_VI_SEARCHSTRING1;
		
		VIWindowGadgets[4].xg_Gad = CreateGadget( STRING_KIND, VIWindowGadgets[3].xg_Gad, &ng,
			GTST_String,		XStrings->xs_SearchStrings[0],
			GTST_MaxChars,		100,
			GT_Underscore,		'_',
			TAG_END );
		
			/// Search string 2
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= gadwidth;
		ng.ng_GadgetText	= VIWindowGadgets[5].xg_GadgetText = GCS( MSG_VI_SEARCHSTRING2 );
		ng.ng_GadgetID		= VIWindowGadgets[5].xg_GadgetID = GTID_VI_SEARCHSTRING2;
		
		VIWindowGadgets[5].xg_Gad = CreateGadget( STRING_KIND, VIWindowGadgets[4].xg_Gad, &ng,
			GTST_String,		XStrings->xs_SearchStrings[1],
			GTST_MaxChars,		100,
			GT_Underscore,		'_',
			TAG_END );
		
			/// Search string 3
		
		ng.ng_TopEdge		+= ng.ng_Height + 2;
		ng.ng_Width			= gadwidth;
		ng.ng_GadgetText	= VIWindowGadgets[6].xg_GadgetText = GCS( MSG_VI_SEARCHSTRING3 );
		ng.ng_GadgetID		= VIWindowGadgets[6].xg_GadgetID = GTID_VI_SEARCHSTRING3;
		
		VIWindowGadgets[6].xg_Gad = CreateGadget( STRING_KIND, VIWindowGadgets[5].xg_Gad, &ng,
			GTST_String,		XStrings->xs_SearchStrings[2],
			GTST_MaxChars,		100,
			GT_Underscore,		'_',
			TAG_END );
		
			/// Search
		
		VIWindowGadgets[7].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			VIWindowGadgets[7].xg_GadgetID = GTID_VI_SEARCH,
			GA_Top,			ng.ng_TopEdge += ng.ng_Height + 4,
			GA_Left,		ng.ng_LeftEdge,
			GA_Width,		gadwidth / 2 - 2,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	VIWindowGadgets[6].xg_Gad,
			BUT_Text,		VIWindowGadgets[7].xg_GadgetText = GCS( MSG_VI_SEARCH ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
			/// Show all
		
		VIWindowGadgets[8].xg_Gad = NewObject( ButtonClass, NULL,
			GA_ID,			VIWindowGadgets[8].xg_GadgetID = GTID_VI_SHOWALL,
			GA_Top,			ng.ng_TopEdge,
			GA_Left,		ng.ng_LeftEdge + ng.ng_Width - (gadwidth / 2) + 2,
			GA_Width,		gadwidth / 2 - 2,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	VIWindowGadgets[7].xg_Gad,
			BUT_Text,		VIWindowGadgets[8].xg_GadgetText = GCS( MSG_VI_SHOWALL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
			/// Listview
		
		ng.ng_TopEdge		+= ng.ng_Height + 4;
		ng.ng_Height		= VIWindow->Height - VIWindow->BorderBottom - 2 - ng.ng_TopEdge;
		ng.ng_Height		= (((ng.ng_Height - 4) / ScrFontYSize) * ScrFontYSize) + 4;
		ng.ng_GadgetText	= "";
		ng.ng_GadgetID		= VIWindowGadgets[9].xg_GadgetID = GTID_VI_DESCRIPTIONS;
		
		VIWindowGadgets[9].xg_Gad = CreateGadget( LISTVIEW_KIND, VIWindowGadgets[8].xg_Gad, &ng,
			GTLV_ShowSelected,	NULL,
			GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
			GT_Underscore,	'_',
			TAG_END );
		
		VIListViewVisible = (ng.ng_Height - 4) / TxtFontYSize;
		
			/// Link gadgets
		
		for( g = 0; g < 9; g++ )
			{
			if( !VIWindowGadgets[ g ].xg_Gad || !VIWindowGadgets[ g + 1 ].xg_Gad )
				{
				success = FALSE;
				
				break;
				}
			
			VIWindowGadgets[ g ].xg_NextGad = &VIWindowGadgets[ g + 1 ];
			}
		
		VIWindowGadgets[ g ].xg_NextGad = NULL;
		
		if( success )
			{
			AddGList( VIWindow, VIWindowGadList, -1, -1, NULL );
			RefreshGList( VIWindowGadList, VIWindow, NULL, -1 );
			
			GT_RefreshWindow( VIWindow, NULL );
			
			return( TRUE );
			}
		}
	
		/// Free what we DID create...
	
	xRemoveMRIWindowGadgets();
	
	return( FALSE );
	
}

VOID xOpenVirusInformationWindow( VOID )
{
	UWORD	minheight = xCalc_VIMinWindowHeight();
	
	
	if( Screen )
		{
		xDisableWindow( Window );
		
		if( DescriptionsList = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
			{
			NewList( DescriptionsList );
			
			if( VIWindow = OpenWindowTags( NULL,
				WA_Left,			Coords->xc_VirusInformationWindow[0],
				WA_Top,				Coords->xc_VirusInformationWindow[1],
				WA_Width,			__builtin_max( xCalc_VIMinWindowWidth() + 20, Coords->xc_VirusInformationWindow[2] ),
				WA_Height,			__builtin_max( minheight, Coords->xc_VirusInformationWindow[3] ),
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
				WA_Title,			GCS( MSG_VI_TL_VIRUSINFORMATION ),
				WA_DragBar,			TRUE,
				WA_CloseGadget,		TRUE,
				WA_Activate,		TRUE,
				WA_DepthGadget,		TRUE,
				WA_RMBTrap,			TRUE,
				WA_SizeGadget,		TRUE,
				WA_SizeBBottom,		TRUE,
				WA_SmartRefresh,	TRUE,
				WA_MinWidth,		xCalc_VIMinWindowWidth() + 20,
				WA_MinHeight,		minheight,
				WA_MaxWidth,		-1,
				WA_MaxHeight,		-1,
				TAG_END ) )
				{
				if( xCreateVirusInfoWindowGadgets() )
					{
					SetFont( VIWindow->RPort, ScreenFont );
					
						/// Preread all the descriptions
					
					xFindDescriptions( FALSE );
					
						/// Put list on
					
					xUnFreezeDescriptionsListGadget();
					
					xHandleVirusInformationIDCMP();
					
						/// Store options
					
					strcpy( XSettings->xs_VIN_DescriptionsPath, GETSTRING( VIWindowGadgets[0].xg_Gad ) );
					strcpy( XSettings->xs_VIN_ExternalViewer, GETSTRING( VIWindowGadgets[2].xg_Gad ) );
					strcpy( XStrings->xs_SearchStrings[0], GETSTRING( VIWindowGadgets[4].xg_Gad ) );
					strcpy( XStrings->xs_SearchStrings[1], GETSTRING( VIWindowGadgets[5].xg_Gad ) );
					strcpy( XStrings->xs_SearchStrings[2], GETSTRING( VIWindowGadgets[6].xg_Gad ) );
						
						/// Flush nodes
					
					xFreeNameNodes( DescriptionsList );
					}
				else
					xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_COULDNOTOPENWINDOW ), GCS( MSG_GF_OKAY ) );
				
				xCloseVirusInformationWindow();
				}
			FreeVec( DescriptionsList );
			}
		
		xEnableWindow( Window );
		}
}

VOID xCloseVirusInformationWindow( VOID )
{
	if( VIWindow )
		{
		xRemoveVIWindowGadgets();
		
			/// Store position of window
		
		Coords->xc_VirusInformationWindow[0] = VIWindow->LeftEdge;
		Coords->xc_VirusInformationWindow[1] = VIWindow->TopEdge;
		Coords->xc_VirusInformationWindow[2] = VIWindow->Width;
		Coords->xc_VirusInformationWindow[3] = VIWindow->Height;
		
			/// Close check window
		
		CloseWindow( VIWindow );
		VIWindow = NULL;
		}
}

VOID xHandleVirusInformationIDCMP( VOID )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	struct NewLVPos			nlv = {0,0};
	struct Node *			node;
	LONG					curselected = -1,oldsel = -1;
	LONG					curvistop,entries;
	ULONG					seconds, micros,sec1 = 0,mic1 = 0,sec2,mic2;
	APTR					ia;
	ULONG					class;
	UWORD					altcode,code,qualifier;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ] = "";
	
	
	while( 1 )
		{
		WaitPort( VIWindow->UserPort );
		
		while( msg = GT_GetIMsg( VIWindow->UserPort ) )
			{
			class = msg->Class;
			altcode = code = msg->Code;
			qualifier = msg->Qualifier;
			ia = msg->IAddress;
			seconds = msg->Seconds;
			micros = msg->Micros;
			
			if( (class == IDCMP_GADGETUP) || (class == IDCMP_GADGETDOWN) )
				code = ((struct Gadget *)msg->IAddress)->GadgetID;
			
			GT_ReplyIMsg( msg );
			
			switch( class )
				{
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( VIWindow );
					GT_EndRefresh( VIWindow, TRUE );
					break;
				
				case IDCMP_NEWSIZE:
						/// Store options
					
					strcpy( XSettings->xs_VIN_DescriptionsPath, GETSTRING( VIWindowGadgets[0].xg_Gad ) );
					strcpy( XSettings->xs_VIN_ExternalViewer, GETSTRING( VIWindowGadgets[2].xg_Gad ) );
					strcpy( XStrings->xs_SearchStrings[0], GETSTRING( VIWindowGadgets[4].xg_Gad ) );
					strcpy( XStrings->xs_SearchStrings[1], GETSTRING( VIWindowGadgets[5].xg_Gad ) );
					strcpy( XStrings->xs_SearchStrings[2], GETSTRING( VIWindowGadgets[6].xg_Gad ) );
						
						/// Recreate gadgets
					
					if( !xCreateVirusInfoWindowGadgets() )
						return;
					
						/// Put list back on
					
					xUnFreezeDescriptionsListGadget();
					
						/// Set last selected
					
					if( curselected != -1 )
						GT_SetGadgetAttrs( VIWindowGadgets[ 9 ].xg_Gad, VIWindow, NULL,
							GTLV_Selected,	curselected,
							GTLV_Top,		nlv.nl_NewTop,
							TAG_END );
					break;
				
				case IDCMP_RAWKEY:
					switch( code )
						{
						case KEY_ESCAPE:
							return;
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_MISC_VIRUSINFORMATION );
							break;
						
						case KEY_RETURN:
							if( node = xFindCurrentSelectedNode( DescriptionsList, curselected ) )
								{
								strcpy( strbuf, GETSTRING( VIWindowGadgets[0].xg_Gad ) );
								AddPart( strbuf, node->ln_Name, DEFAULTBUFFERLEN );
								
								xDisableWindow( VIWindow );
								
								strcpy( XSettings->xs_VIN_ExternalViewer, GETSTRING( VIWindowGadgets[2].xg_Gad ) );
								xViewDescription( Screen, strbuf );
								
								xEnableWindow( VIWindow );
								}
							break;
						
						case CURSORUP:
						case CURSORDOWN:
							if( entries = xCountListEntries( DescriptionsList ) )
								{
								curvistop = xListViewTop( VIWindowGadgets[ 9 ].xg_Gad, VIWindow );
								
								xNewListPos( &nlv, qualifier, code, curselected, curvistop, VIListViewVisible, entries );
								
								GT_SetGadgetAttrs( VIWindowGadgets[ 9 ].xg_Gad, VIWindow, NULL,
									GTLV_Selected,	nlv.nl_NewSelected,
									GTLV_Top,		nlv.nl_NewTop,
									TAG_END );
								
								curselected = nlv.nl_NewSelected;
								}
							break;
						
						default:
							if( tmpgad = xCheckUnderscore( VIWindowGadgets, code, qualifier, ia ) )
								{
								switch( tmpgad->xg_GadgetID )
									{
									case GTID_VI_SEARCH:
									case GTID_VI_SHOWALL:
										xPressButton( VIWindow, tmpgad->xg_Gad );
										
										code = tmpgad->xg_GadgetID;
										break;
									
									case GTID_VI_DESCRIPTIONSPATH:
									case GTID_VI_EXTERNALVIEWER:
									case GTID_VI_SEARCHSTRING1:
									case GTID_VI_SEARCHSTRING2:
									case GTID_VI_SEARCHSTRING3:
										ActivateGadget( tmpgad->xg_Gad, VIWindow, NULL );
										
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
						case GTID_VI_DESCRIPTIONSPATH:
							if( altcode != HOOK_CURSORDOWN )
								break;
						
						case GTID_VI_GETDESCRIPTIONSPATH:
							xDisableWindow( VIWindow );
							
								/// Select a directory
							
							if( xSelectPath( VIWindow,
								SELFP_DestDir,	strbuf,
								SELFP_Gadget,	VIWindowGadgets[0].xg_Gad,
								TAG_END ) )
								{
								xSetGadgetString( VIWindow, VIWindowGadgets[0].xg_Gad, strbuf );
								}
							
							xEnableWindow( VIWindow );
							break;
						
						case GTID_VI_EXTERNALVIEWER:
							if( altcode != HOOK_CURSORDOWN )
								break;
						
						case GTID_VI_GETEXTERNALVIEWER:
							xDisableWindow( VIWindow );
							
								/// Select a file
							
							if( xSelectFile( VIWindow,
								SELFP_DestFile,		strbuf,
								SELFP_Gadget,		VIWindowGadgets[2].xg_Gad,
								SELFP_StripFormats,	TRUE,
								TAG_END ) )
								{
								xSetGadgetString( VIWindow, VIWindowGadgets[2].xg_Gad, strbuf );
								}
							
							xEnableWindow( VIWindow );
							break;
						
						case GTID_VI_SEARCH:
							xUpdateDescriptions( TRUE );
							
							GT_SetGadgetAttrs( VIWindowGadgets[ 9 ].xg_Gad, VIWindow, NULL,
								GTLV_Selected,	IsListEmpty( DescriptionsList ) ? (curselected = -1) : (curselected = 0),
								GTLV_Top,		0,
								TAG_END );
							break;
						
						case GTID_VI_SHOWALL:
							xUpdateDescriptions( FALSE );
							
							GT_SetGadgetAttrs( VIWindowGadgets[ 9 ].xg_Gad, VIWindow, NULL,
								GTLV_Selected,	IsListEmpty( DescriptionsList ) ? (curselected = -1) : (curselected = 0),
								GTLV_Top,		0,
								TAG_END );
							break;
						
						case GTID_VI_DESCRIPTIONS:
							curselected = altcode;
							
								/// Save the old time
							
							sec2 = sec1;
							mic2 = mic1;
							
								/// Get the new time
							
							sec1 = seconds;
							mic1 = micros;
							
							if( curselected == oldsel && DoubleClick( sec2, mic2, sec1, mic1 ) )
								{
								if( node = xFindCurrentSelectedNode( DescriptionsList, curselected ) )
									{
									strcpy( strbuf, GETSTRING( VIWindowGadgets[0].xg_Gad ) );
									AddPart( strbuf, node->ln_Name, DEFAULTBUFFERLEN );
									
									xDisableWindow( VIWindow );
									
									strcpy( XSettings->xs_VIN_ExternalViewer, GETSTRING( VIWindowGadgets[2].xg_Gad ) );
									xViewDescription( Screen, strbuf );
									
									xEnableWindow( VIWindow );
									}
								
									/// Reset the values
								
								sec1 = 0;
								mic1 = 0;
								}
							
							oldsel = curselected;
							break;
						}
					break;
				
				case IDCMP_CLOSEWINDOW:
					return;
					break;
				}
			}
		}
}

VOID xUpdateDescriptions( BOOL search )
{
	xFreezeDescriptionsListGadget();
	
	xFreeNameNodes( DescriptionsList );
	
	NewList( DescriptionsList );
	
	xFindDescriptions( search );
	
	xUnFreezeDescriptionsListGadget();
}

VOID xFreezeDescriptionsListGadget( VOID )
{
	GT_SetGadgetAttrs( VIWindowGadgets[ 9 ].xg_Gad, VIWindow, NULL,
		GTLV_Labels, ~0,
		TAG_END );
}

VOID xUnFreezeDescriptionsListGadget( VOID )
{
	GT_SetGadgetAttrs( VIWindowGadgets[ 9 ].xg_Gad, VIWindow, NULL,
		GTLV_Labels, DescriptionsList,
		TAG_END );
}

/*
 * Build a list with the files (descriptions) available in a directory
 */
VOID xFindDescriptions( BOOL search )
{
	struct AnchorPath *	ap;
	struct NameNode *	nn;
	ULONG				r;
	BPTR				lock,oldcd;
	
	
	if( lock = Lock( GETSTRING( VIWindowGadgets[0].xg_Gad ), ACCESS_READ ) )
		{
		oldcd = CurrentDir( lock );
		
		if( ap = AllocVec( sizeof( struct AnchorPath ) + 256, MEMF_CLEAR ) )
			{
			ap->ap_BreakBits = SIGBREAKF_CTRL_C;
			ap->ap_Strlen = 256;
			
			for( r = MatchFirst( "#?", ap );  r == 0; r = MatchNext( ap ) )
				{
				if( ap->ap_Info.fib_DirEntryType < 0 )
					{
					BOOL	add_it = TRUE;
					
					
					if( search )
						{
						add_it = xSearchCheckFile( ap );
						}
					
					if( add_it )
						{
						if( nn = AllocVec( sizeof( struct NameNode ), MEMF_CLEAR ) )
							{
							if( nn->nn_Name = AllocVec( strlen( ap->ap_Buf ) + 1, MEMF_CLEAR ) )
								{
								strcpy( nn->nn_Name, ap->ap_Buf );
								nn->nn_Node.ln_Name = nn->nn_Name;
								
								AddTail( DescriptionsList, (struct Node *)nn );
								}
							else
								FreeVec( nn );
							}
						}
					}
				}
			
			if( !IsListEmpty( DescriptionsList ) )
				xSortNameNodeList( DescriptionsList );
			
			MatchEnd( ap );
			
			FreeVec( ap );
			}
		
		CurrentDir( oldcd );
		
		UnLock( lock );
		}
}

BOOL xSearchCheckFile( struct AnchorPath *ap )
{
	BOOL	file_matches = FALSE;
	BPTR	file;
	UBYTE *	buffer;
	ULONG	sizeread;
	
	
	if( buffer = AllocVec( ap->ap_Info.fib_Size, MEMF_CLEAR ) )
		{
		if( file = Open( ap->ap_Buf, MODE_OLDFILE ) )
			{
			sizeread = Read( file, buffer, ap->ap_Info.fib_Size );
			
			if( sizeread > 0 )
				{
				if( xIsStringInBuffer( buffer, sizeread, GETSTRING( VIWindowGadgets[4].xg_Gad ) ) &&
					xIsStringInBuffer( buffer, sizeread, GETSTRING( VIWindowGadgets[5].xg_Gad ) ) &&
					xIsStringInBuffer( buffer, sizeread, GETSTRING( VIWindowGadgets[6].xg_Gad ) ) )
					{
					file_matches = TRUE;
					}
				}
			else
				DisplayBeep( NULL );
			
			Close( file );
			}
		FreeVec( buffer );
		}
	
	return( file_matches );
}

BOOL xIsStringInBuffer( UBYTE *buffer, ULONG size, STRPTR string )
{
	ULONG	i;
	UWORD	len = strlen( string );
	UBYTE *	bufend = buffer + size;
	UBYTE *	p;
	
	
	if( len == 0 )
		return( TRUE );
	
		/// Convert search string to lowercase
	
	for( p = string; p < string + len; p++ )
		*p = ToLower( *p );
	
	for( ; buffer < bufend; buffer++ )
		{
		for( i = 0; i < len; i++)
			if( string[i] != ToLower( *(buffer + i) ) )
				break;
		
		if( i == len )
			return( TRUE );
		}
	
	return( FALSE );
}

VOID xViewDescription( struct Screen *scr, STRPTR filename )
{
	if( XSettings->xs_VIN_ExternalViewer[0] )
		{
		UBYTE	strbuf[ DEFAULTBUFFERLEN ];
		
		
		xSPrintF( strbuf, XSettings->xs_VIN_ExternalViewer, filename );
		
		Execute( strbuf, NULL, NULL );
		}
	else
		xViewGuideFile( scr, filename );
}

VOID xViewGuideFile( struct Screen *scr, STRPTR filename )
{
	struct NewAmigaGuide *	nag;
	VOID *					aghandle;
	BPTR					agfile;
	
	
	if( AmigaGuideBase )
		{
		if( nag = AllocVec( sizeof( struct NewAmigaGuide ), MEMF_CLEAR ) )
			{
			if( agfile = Lock( filename, ACCESS_READ ) )
				{
				UnLock( agfile );
				
				nag->nag_Name     = filename;
				nag->nag_Screen   = scr;
				nag->nag_BaseName = "Xtruder";
				
				if( aghandle = OpenAmigaGuide( nag, NULL ) )
					CloseAmigaGuide( aghandle );
				}
			FreeVec( nag );
			}
		}
}
