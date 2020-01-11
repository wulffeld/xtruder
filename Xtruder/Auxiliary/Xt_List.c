/*
**	$VER: XtruderList.c (11.7.95)
**
**	Xtruder list & node functions
**
**	Copyright (C) 1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


BOOL __stdargs xAddNameNode( struct List *l, STRPTR format, ... )
{
	struct NameNode *	nn;
	UBYTE				strbuf[ DEFAULTBUFFERLEN ];
	
	
	if( nn = AllocVec( sizeof( struct NameNode ), MEMF_CLEAR ) )
		{
		RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
		
		if( nn->nn_Name = AllocVec( strlen( strbuf ) + 1, MEMF_CLEAR ) )
			{
			strcpy( nn->nn_Name, strbuf );
			
			nn->nn_Node.ln_Name = nn->nn_Name;
			
			AddTail( l, (struct Node *)nn );
			
			return( TRUE );
			}
		FreeVec( nn );
		}
	
	return( FALSE );
}

VOID xFreeNameNodes( struct List *l )
{
	if( l && !IsListEmpty( l ) )
		{
		struct NameNode *	worknode;
		struct NameNode *	nextnode;
		
		
		worknode = (struct NameNode *)l->lh_Head;
		
		while( nextnode = (struct NameNode *)worknode->nn_Node.ln_Succ )
			{
			FreeVec( worknode->nn_Name );
			
			FreeVec( worknode );
			
			worknode = nextnode;
			}
		
		NewList( l );
		}
}

/*
 * Sort a list which nodes has the following form:
 *
 * struct NameNode
 * {
 *  struct Node	nn_Node;
 *  UBYTE *		nn_Name;
 * };
 */
VOID xSortNameNodeList( struct List *list )
{
	struct NameNode *	pos;
	struct NameNode *	min;
	struct NameNode *	nn;
	UBYTE *				temp;
	
	
	pos = (struct NameNode *)list->lh_Head;
	
	while( (struct NameNode *)pos->nn_Node.ln_Succ )
		{
		nn = pos;
		min = pos;
		
		while( (struct NameNode *)nn->nn_Node.ln_Succ )
			{
			if( stricmp( min->nn_Node.ln_Name, nn->nn_Node.ln_Name ) > 0 )
				min = nn;
			
			nn = (struct NameNode *)nn->nn_Node.ln_Succ;
			}
		
		nn = pos;
		
		temp                 = pos->nn_Node.ln_Name;
		nn->nn_Node.ln_Name  = min->nn_Node.ln_Name;
		min->nn_Node.ln_Name = temp;
		
		pos = (struct NameNode *)pos->nn_Node.ln_Succ;
		}
}

/*
 * Returns a pointer to the current selected node in a listview
 */
struct Node *xFindCurrentSelectedNode( struct List *list, UWORD curselected )
{
	struct Node *	node;
	
	
	if( !IsListEmpty( list ) )
		{
		for( node = list->lh_Head;
			 node->ln_Succ;
			 node = node->ln_Succ, curselected-- )
			{
			if( !curselected )
				return( node );
			}
		}
	
	return( NULL );
}

UWORD xCountListEntries( struct List *list )
{
	struct Node *	node;
	UWORD			n = 0;
	
	
	for( node = list->lh_Head;
		 node->ln_Succ;
		 node = node->ln_Succ )
		n++;
	
	return( n );
}

struct Node *xSelectListEntry( struct List *list, ULONG entries, STRPTR wintitle, STRPTR bartxt, struct Screen *scr )
{
	struct Window *		win;
	struct NewGadget	ng;
	struct Gadget *		gadlist = NULL;
	struct NameNode *	fnode = NULL;
	LONG				number;
	UWORD				gh,gw;
	
	
	ListGadget = CreateContext( &gadlist );
	
	gw = xProportionalLength( ScreenFont, 35 );
	gh = 10 * ScrFontYSize + 4;
	
	ng.ng_VisualInfo	= VisualInfo;
	ng.ng_TextAttr		= &ScreenFontAttr;
	ng.ng_LeftEdge		= 10;
	ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
	ng.ng_Width			= gw;
	ng.ng_Height		= gh;
	ng.ng_Flags			= PLACETEXT_IN;
	ng.ng_GadgetText	= "";
	ng.ng_GadgetID		= 1111;
	
	ListGadget = CreateGadget( LISTVIEW_KIND, ListGadget, &ng,
		GTLV_ShowSelected,	NULL,
		GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
		GTLV_Labels,		list,
		TAG_END );
	
	if( ListGadget )
		{
		gw += 20;
		gh = gh + ScrFontYSize + 4 + (ScrFontYSize + 3) + 4 + ((GadToolsBase->lib_Version > 38) ? 4 : ScrFontYSize + 5);
		
		scr = scr ? scr : Screen;
		
		if( win = OpenWindowTags( NULL,
			WA_CustomScreen,	scr,
			WA_Title,			wintitle,
			WA_Left,			(scr->Width - gw) / 2,
			WA_Top,				(scr->Height - gh) / 2,
			WA_Width,			gw,
			WA_Height,			gh,
			WA_IDCMP,			IDCMP_REFRESHWINDOW | BUTTONIDCMP |
								IDCMP_MOUSEBUTTONS | LISTVIEWIDCMP |
								IDCMP_CLOSEWINDOW | IDCMP_RAWKEY,
			WA_Gadgets,			gadlist,
			WA_DragBar,			TRUE,
			WA_CloseGadget,		TRUE,
			WA_DepthGadget,		TRUE,
			WA_Activate,		TRUE,
			WA_SmartRefresh,	TRUE,
			WA_RMBTrap,			TRUE,
			TAG_END ) )
			{
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			SetAPen( win->RPort, 1 );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, bartxt );
			
			number = xHandleListIDCMP( win, entries );
			
				/// Find the node of the selected number
			
			if( number != -1 )
				{
				fnode = (struct NameNode *)list->lh_Head;
				
				while( fnode && number > 0 )
					{
					number--;
					
					fnode = (struct NameNode *)fnode->nn_Node.ln_Succ;
					}
				}
			
			CloseWindow( win );
			}
		}
	
	FreeGadgets( gadlist );
	
	return( (struct Node *)fnode );
}

LONG xHandleListIDCMP( struct Window *win, ULONG entries )
{
	struct IntuiMessage *	msg;
	struct NewLVPos			nlv = {0,0};
	LONG					curselected = -1,oldsel = -1;
	LONG					curvistop;
	LONG					visible = ListGadget->Height / ScrFontYSize;
	ULONG					class;
	ULONG					seconds, micros,sec1 = 0,mic1 = 0,sec2,mic2;
	UWORD					qual,code,altcode;
	BOOL					done = FALSE;
	
	
	while( !done )
		{
		WaitPort( win->UserPort );
		
		while( msg = GT_GetIMsg( win->UserPort ) )
			{
			class = msg->Class;
			altcode = code = msg->Code;
			qual = msg->Qualifier;
			seconds = msg->Seconds;
			micros = msg->Micros;
			
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
						case CURSORUP:
						case CURSORDOWN:
							curvistop = xListViewTop( ListGadget, win );
							
							xNewListPos( &nlv, qual, code, curselected, curvistop, visible, entries );
							
							GT_SetGadgetAttrs( ListGadget, win, NULL,
								GTLV_Selected, nlv.nl_NewSelected,
								GTLV_Top, nlv.nl_NewTop,
								TAG_END );
							
							curselected = nlv.nl_NewSelected;
							break;
						
						case KEY_ESCAPE:
							curselected = -1;
							
							done = TRUE;
							break;
						
						case KEY_RETURN:
							done = TRUE;
							break;
						}
					break;
				
				case IDCMP_GADGETUP:
					switch( code )
						{
						case 1111:
							curselected = altcode;
							
								/// Save the old time
							
							sec2 = sec1;
							mic2 = mic1;
							
								/// Get the new time
							
							sec1 = seconds;
							mic1 = micros;
							
							if( curselected == oldsel && DoubleClick( sec2, mic2, sec1, mic1 ) )
								{
								done = TRUE;
								
									/// Reset the values
								
								sec1 = 0;
								mic1 = 0;
								}
							
							oldsel = curselected;
							break;
						}
					break;
				
				case IDCMP_CLOSEWINDOW:
					done = TRUE;
					
					curselected = -1;
					break;
				}
			}
		}
	
	return( curselected );
}

/*
 * Open a sizable window with a listview
 */
VOID xOpenListShowWindow( struct List *slist, STRPTR title, STRPTR bartitle, UWORD *coords, UWORD guidecontext, BOOL nopropfont )
{
	struct NewLVPos			nlv = {0,0};
	struct IntuiMessage *	msg;
	struct Window *			win;
	struct Gadget *			listgad;
	ULONG					class;
	UWORD					qual,code;
	UWORD					minwidth;
	UWORD					curvistop;
	UWORD					entries = xCountListEntries( slist );
	UWORD					visible;
	BOOL					done = FALSE;
	
	
	if( Screen )
		{
		minwidth = 70 + xTextLength( ScreenFont, bartitle );
		
		if( minwidth > coords[2] )
			coords[2] = minwidth;
		
		if( 120 > coords[3] )
			coords[3] = 120;
		
			/// Check bounds and adjust if necessary
		
		if( coords[2] > Screen->Width )
			coords[2] = Screen->Width;
		
		if( coords[3] > Screen->Height )
			coords[3] = Screen->Height;
		
			/// Open window
		
		if( win = OpenWindowTags( NULL,
			WA_PubScreen,		Screen,
			WA_Title,			title,
			WA_IDCMP,			IDCMP_REFRESHWINDOW | BUTTONIDCMP |
								IDCMP_MOUSEBUTTONS | LISTVIEWIDCMP |
								IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE |
								IDCMP_RAWKEY,
			WA_Left,			coords[0],
			WA_Top,				coords[1],
			WA_Width,			coords[2],
			WA_Height,			coords[3],
			WA_Activate,		TRUE,
			WA_AutoAdjust,		TRUE,
			WA_CloseGadget,		TRUE,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_SmartRefresh,	TRUE,
			WA_SizeBBottom,		TRUE,
			WA_SizeGadget,		TRUE,
			WA_MinWidth,		minwidth,
			WA_MinHeight,		120,
			WA_MaxWidth,		Screen->Width,
			WA_MaxHeight,		Screen->Height,
			TAG_END ) )
			{
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			SetAPen( win->RPort, 1 );
			
			GadgetList = NULL;
			
			if( listgad = xCreateListShowGadgets( win, slist, FALSE, nopropfont ) )
				{
				visible = listgad->Height / ScrFontYSize;
				
				xBarTxt( win, ScreenBarHeight + ScrFontYSize, bartitle );
				
				while( !done )
					{
					WaitPort( win->UserPort );
					
					while( msg = GT_GetIMsg( win->UserPort ) )
						{
						class = msg->Class;
						code = msg->Code;
						qual = msg->Qualifier;
						
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
									case CURSORUP:
									case CURSORDOWN:
										curvistop = xListViewTop( listgad, win );
										
										xNewListPos( &nlv, qual, code, code == CURSORUP ? curvistop : curvistop + visible - 1, curvistop, visible, entries );
										
										GT_SetGadgetAttrs( listgad, win, NULL,
											GTLV_Top, nlv.nl_NewTop,
											TAG_END );
										break;
									
									case KEY_HELP:
										xGuideDisplay( guidecontext );
										break;
									
									case KEY_ESCAPE:
										done = TRUE;
										break;
									}
								break;
							
							case IDCMP_NEWSIZE:
								if( !(listgad = xCreateListShowGadgets( win, slist, FALSE, nopropfont ) ) )
									done = TRUE;
								else
									{
									visible = listgad->Height / ScrFontYSize;
									
									xBarTxt( win, ScreenBarHeight + ScrFontYSize, bartitle );
									}
								break;
							
							case IDCMP_CLOSEWINDOW:
								done = TRUE;
								break;
							}
						}
					}
				
				xCreateListShowGadgets( win, slist, TRUE, nopropfont );
				
				coords[0] = win->LeftEdge;
				coords[1] = win->TopEdge;
				coords[2] = win->Width;
				coords[3] = win->Height;
				}
			CloseWindow( win );
			}
		}
}

struct Hook		HistoryRenderHook;

struct Gadget *xCreateListShowGadgets( struct Window *win, struct List *slist, BOOL quit, BOOL nopropfont )
{
	struct NewGadget	ng;
	struct Gadget *		gad;
	
	
	SetAPen( win->RPort, 0 );
	RectFill( win->RPort, 4, ScreenBarHeight, win->Width - 5,  win->Height - 8 );
	
	if( GadgetList )
		RemoveGList( win, GadgetList, -1 );
	
	FreeGadgets( GadgetList );
	GadgetList = NULL;
	
	if( quit )
		return( FALSE );
	
	RefreshWindowFrame( win );
	
	gad = CreateContext( &GadgetList );
	
		/// Create a listview gadget which fills the entire window
	
	ng.ng_VisualInfo	= VisualInfo;
	ng.ng_TextAttr		= &ScreenFontAttr;
	
	if( nopropfont )
		if( ScreenFont->tf_Flags & FPF_PROPORTIONAL )
			ng.ng_TextAttr = &TextFontAttr;
	
	ng.ng_LeftEdge		= 10;
	ng.ng_TopEdge		= ScreenBarHeight + ScrFontYSize + 4;
	ng.ng_Width			= win->Width - 20;
	ng.ng_Height		= win->Height - (win->BorderTop + win->BorderBottom) - ScrFontYSize - 4;
	ng.ng_GadgetText	= "";
	ng.ng_GadgetID		= 1001;
	ng.ng_Flags			= 0;
	
	HistoryRenderHook.h_Entry = (HOOKFUNC)xCheckHistoryRenderHook;
	
	if( gad = CreateGadget( LISTVIEW_KIND, gad, &ng,
		GTLV_Labels,		slist,
		GTLV_ReadOnly,		TRUE,
		GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
		(&XtruderHistory->his_List == slist) ? GTLV_CallBack : TAG_IGNORE, &HistoryRenderHook,
		TAG_END ) )
		{
		AddGList( win, GadgetList, -1, -1, NULL );
		RefreshGList( GadgetList, win, NULL, -1 );
		
		GT_RefreshWindow( win, NULL );
		
		return( gad );
		}
	
	FreeGadgets( GadgetList );
	
	GadgetList = NULL;
	
	return( NULL );
}
