/*
**	$VER: XtruderSettingsPrespecified.c (2.3.96)
**
**	Xtruder settings
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"

/**
 ---> +---------------------------------------------------------------+
      | Prespecified                                                  |
      +---------------------------------------------------------------+
      | +-----------------------------------------------------------+ |
      | |                                                           | |
      | |                                                           | |
      | |                                                           | |
      | |                                                           | |
      | |                                                           | |
      | |                                                           | |
      | +-----------------------------------------------------------+ |
      | |                                                           | |
      | +-----------------------------------------------------------+ |
      | | Add entry        | Remove entry        | Sort entries     | |
      | +-----------------------------------------------------------+ |
      +---------------------------------------------------------------+
**/

#define		PRGT_STRING							0
#define		PRGT_LISTVIEW						1
#define		PRGT_ADD							2
#define		PRGT_REMOVE							3
#define		PRGT_SORT							4
#define		PRGT_GADS							7

VOID xOpenFCPrespecifiedSettings( VOID )
{
	struct SettingsGUI *	settingsgui;
	struct Window *			win;
	struct NewGadget		ng;
	struct Gadget *			glist = NULL;
	struct xGadget *		gads;
	UWORD					lvh = ScrFontYSize * 12 + 4;
	UWORD					gtop;
	UWORD					fullwidth = Screen->Width - 40;
	UWORD					gwidth = fullwidth / 3;
	UWORD					wh,ww;
	
	
	if( gads = AllocVec( sizeof( struct xGadget ) * 11, MEMF_CLEAR ) )
		{
		gads[0].xg_Gad = CreateContext( &glist );
		
			/// First create the string gadget for the preloaded directories
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_LeftEdge		= 10;
		ng.ng_TopEdge		= ScreenBarHeight + lvh + ScrFontYSize + 4;
		ng.ng_Width			= fullwidth;
		ng.ng_Height		= GadgetHeight;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_TextAttr		= &ScreenFontAttr;
		ng.ng_GadgetText	= "";
		ng.ng_GadgetID		= GTID_FC_PR_STRING;
		
		gads[0].xg_Gad = CreateGadget( STRING_KIND, gads[0].xg_Gad, &ng,
			GA_Disabled,	TRUE,
			GTST_MaxChars,	DEFAULTBUFFERLEN,
			TAG_END );
		
		ng.ng_TopEdge		-= lvh;
		ng.ng_Height		= lvh;
		ng.ng_Flags			= PLACETEXT_IN;
		ng.ng_GadgetID		= gads[1].xg_GadgetID = GTID_FC_PR_LISTVIEW;
		
		gads[1].xg_Gad		= CreateGadget( LISTVIEW_KIND, gads[0].xg_Gad, &ng,
			GTLV_ShowSelected,	gads[0].xg_Gad,
			GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
			GTLV_Labels,		&PreList->pl_List,
			TAG_END );
		
			/// Create Add, Remove & Sort gads
		
		gads[2].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[2].xg_GadgetID = GTID_FC_PR_ADD,
			GA_Left,		10,
			GA_Top,			gads[0].xg_Gad->TopEdge + gads[0].xg_Gad->Height + 3,
			GA_Width,		gwidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[1].xg_Gad,
			BUT_Text,		gads[2].xg_GadgetText = GCS( MSG_FCPR_ADD ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[3].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[3].xg_GadgetID = GTID_FC_PR_REMOVE,
			GA_Left,		10 + gwidth,
			GA_Top,			gads[0].xg_Gad->TopEdge + gads[0].xg_Gad->Height + 3,
			GA_Width,		gwidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[2].xg_Gad,
			GA_Disabled,	TRUE,
			BUT_Text,		gads[3].xg_GadgetText = GCS( MSG_FCPR_REMOVE ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[4].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[4].xg_GadgetID = GTID_FC_PR_SORT,
			GA_Left,		10 + gwidth * 2,
			GA_Top,			gads[0].xg_Gad->TopEdge + gads[0].xg_Gad->Height + 3,
			GA_Width,		fullwidth - ((fullwidth * 2)/3),
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[3].xg_Gad,
			GA_Disabled,	IsListEmpty( &PreList->pl_List ) ? TRUE : FALSE,
			BUT_Text,		gads[4].xg_GadgetText = GCS( MSG_FCPR_SORT ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gtop = gads[0].xg_Gad->TopEdge + gads[0].xg_Gad->Height + 3 + GadgetHeight + LASTGADGETGAP + 2 + CONFIRMGAP;
		
		gads[5].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[5].xg_GadgetID = GTID_OKAY,
			GA_Left,		10,
			GA_Top,			gtop,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[4].xg_Gad,
			BUT_Text,		gads[5].xg_GadgetText = GCS( MSG_GAD_OKAY ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		gads[6].xg_Gad		= NewObject( ButtonClass, NULL,
			GA_ID,			gads[6].xg_GadgetID = GTID_CANCEL,
			GA_Left,		Screen->Width - 30 - ConfirmGadWidth,
			GA_Top,			gtop,
			GA_Width,		ConfirmGadWidth,
			GA_Height,		GadgetHeight,
			GA_RelVerify,	TRUE,
			GA_Previous,	gads[5].xg_Gad,
			BUT_Text,		gads[6].xg_GadgetText = GCS( MSG_GAD_CANCEL ),
			BUT_TextFont,	ScreenFont,
			TAG_DONE );
		
		wh = gtop + GadgetHeight + CONFIRMGAP + 3;
		ww = Screen->Width - 20;
		
		if( settingsgui = xOpenSettingsGUI( GCS( MSG_FCPR_SETTINGS ), ww, wh, glist, gads, 6 ) )
			{
			struct PrespecifiedList *	plist;
			struct PrespecifiedList *	origlist;
			struct PrespecifiedNode *	worknode;
			
			
			win = settingsgui->sg_Window;
			
			GT_RefreshWindow( win, NULL );
			
			SetFont( win->RPort, ScreenFont );
			
			xBarTxt( win, ScreenBarHeight + ScrFontYSize, GCS( MSG_FCPR_BAR_PRESPECIFIED ) );
			xBarTxt( win, gads[6].xg_Gad->TopEdge - CONFIRMGAP, "" );
			
				/// Make an exact copy of the list which holds the prespecified
				/// objects in case the user cancels his selections.
			
			if( plist = AllocVec( sizeof( struct PrespecifiedList ), MEMF_CLEAR ) )
				{
				NewList( &plist->pl_List );
				
					/// First we copy all the entries besides to plist
				
				worknode = (struct PrespecifiedNode *)&PreList->pl_List.lh_Head;
				
				while( worknode )
					{
					if( worknode->pn_Name[0] )
						xAddPrespecifiedNode( &plist->pl_List, worknode->pn_Name );
					
					worknode = (struct PrespecifiedNode *)worknode->pn_Node.ln_Succ;
					}
				
					/// origlist er en ptr til de "gamle" nodes
				
				origlist = PreList;
				
					/// PreList peger nu på den nyligt oprettede plist's nodes
				
				PreList = plist;
				PreList->pl_Entries = origlist->pl_Entries;
				
				if( xHandleFCPrespecifiedSettings( win, gads ) )
					{
					SettingsChanged = TRUE;
					
						/// Dræb de gamle nodes
					
					xFreePrespecifiedNodes( &origlist->pl_List );
					
					FreeVec( origlist );
					}
				else
					{
						/// Reset...
					
					PreList = origlist;
					
					xFreePrespecifiedNodes( &plist->pl_List );
					
					FreeVec( plist );
					}
				}
			
			xCloseSettingsGUI( settingsgui );
			}
		
		FreeGadgets( glist );
		
		DisposeObject( gads[6].xg_Gad );
		DisposeObject( gads[5].xg_Gad );
		DisposeObject( gads[4].xg_Gad );
		DisposeObject( gads[3].xg_Gad );
		DisposeObject( gads[2].xg_Gad );
		
		FreeVec( gads );
		}
}

BOOL xHandleFCPrespecifiedSettings( struct Window *win, struct xGadget *gads )
{
	struct IntuiMessage *	msg;
	struct xGadget *		tmpgad;
	struct NewLVPos			nlv = {0,0};
	APTR					ia;
	ULONG					class;
	UWORD					altcode,code,qualifier;
	LONG					curselected = -1;
	LONG					curvistop;
	LONG					visible = (gads[PRGT_LISTVIEW].xg_Gad->Height - 2) / ScrFontYSize;
	
	
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
							if( !IsListEmpty( &PreList->pl_List ) )
								{
								curvistop = xListViewTop( gads[PRGT_LISTVIEW].xg_Gad, win );
								
								xNewListPos( &nlv, qualifier, code, curselected, curvistop, visible, PreList->pl_Entries );
								
								GT_SetGadgetAttrs( gads[PRGT_LISTVIEW].xg_Gad, win, NULL,
									GTLV_Selected, nlv.nl_NewSelected,
									GTLV_Top, nlv.nl_NewTop,
									TAG_END );
								
									/// Enable the user to remove an entry
								
								SetGadgetAttrsA( gads[PRGT_REMOVE].xg_Gad, win, NULL, DisabledFalseTags );
								
								GT_SetGadgetAttrsA( gads[PRGT_STRING].xg_Gad, win, NULL, DisabledFalseTags );
								
								curselected = nlv.nl_NewSelected;
								}
							break;
						
						case KEY_RETURN:
							if( curselected > -1 )
								{
								SetGadgetAttrsA( gads[PRGT_REMOVE].xg_Gad, win, NULL, DisabledFalseTags );
								
								GT_SetGadgetAttrsA( gads[PRGT_STRING].xg_Gad, win, NULL, DisabledFalseTags );
								
								ActivateGadget( gads[PRGT_STRING].xg_Gad, win, NULL );
								}
							break;
						
						case KEY_ESCAPE:
							xPressButton( win, gads[6].xg_Gad );
							
							return( FALSE );
							break;
						
						case KEY_HELP:
							xGuideDisplay( AGH_STGS_FCHK_PRESPECIFIED );
							break;
						
						default:
							if( tmpgad = xCheckUnderscore( gads, code, qualifier, ia ) )
								{
								if( !(tmpgad->xg_Gad->Flags & GFLG_DISABLED) )
									{
									switch( tmpgad->xg_GadgetID )
										{
										case GTID_FC_PR_ADD:
										case GTID_FC_PR_REMOVE:
										case GTID_FC_PR_SORT:
										case GTID_OKAY:
										case GTID_CANCEL:
											xPressButton( win, tmpgad->xg_Gad );
											
											code = tmpgad->xg_GadgetID;
											break;
										
										default:
											code = 0;
											break;
										}
									}
								}
							else
								break;
						}
				
				case IDCMP_GADGETUP:
					switch( code )
						{
						case GTID_FC_PR_SORT:
							GT_SetGadgetAttrs( gads[PRGT_LISTVIEW].xg_Gad, win, NULL,
								GTLV_Labels, ~0,
								TAG_END );
							
							xSortNodes();
							
							GT_SetGadgetAttrs( gads[PRGT_LISTVIEW].xg_Gad, win, NULL,
								GTLV_Labels, &PreList->pl_List,
								TAG_END );
							break;
						
							/// User clicked in the listview gadget. Therefore
							/// we need activate the edit gadget and misc.
						
						case GTID_FC_PR_LISTVIEW:
								/// Enable the user to remove an entry
							
							SetGadgetAttrsA( gads[PRGT_REMOVE].xg_Gad, win, NULL, DisabledFalseTags );
							
							GT_SetGadgetAttrsA( gads[PRGT_STRING].xg_Gad, win, NULL, DisabledFalseTags );
							
								/// Activate the string gadget for conveniance to the user
							
							ActivateGadget( gads[PRGT_STRING].xg_Gad, win, NULL );
							
							curselected = altcode;
							break;
						
							/// User pressed return in edit gadget
						
						case GTID_FC_PR_STRING:
							xUpdatePrespecifiedName( win, gads[PRGT_LISTVIEW].xg_Gad, GETSTRING( gads[PRGT_STRING].xg_Gad ), curselected );
							break;
						
							/// Add some entries
						
						case GTID_FC_PR_ADD:
							if( xAddPrespecified( win, gads[PRGT_LISTVIEW].xg_Gad, curselected ) )
								{
								if( curselected == -1 )
									curselected = 0;
								
								if( !IsListEmpty( &PreList->pl_List ) )
									SetGadgetAttrsA( gads[PRGT_SORT].xg_Gad, win, NULL, DisabledFalseTags );
								
								SetGadgetAttrsA( gads[PRGT_REMOVE].xg_Gad, win, NULL, DisabledFalseTags );
								
								GT_SetGadgetAttrsA( gads[PRGT_STRING].xg_Gad, win, NULL, DisabledFalseTags );
								
								GT_SetGadgetAttrs( gads[PRGT_LISTVIEW].xg_Gad, win, NULL,
									GTLV_Selected, curselected,
									TAG_END );
								}
							break;
						
							/// Remove an entry
						
						case GTID_FC_PR_REMOVE:
							if( !IsListEmpty( &PreList->pl_List ) )
								{
								if( curselected > -1 )
									{
									BOOL	rem;
									
									
									GT_SetGadgetAttrs( gads[PRGT_LISTVIEW].xg_Gad, win, NULL,
										GTLV_Labels, ~0,
										TAG_END );
									
									rem = xRemovePrespecified( curselected );
									
									GT_SetGadgetAttrs( gads[PRGT_LISTVIEW].xg_Gad, win, NULL,
										GTLV_Labels, &PreList->pl_List,
										TAG_END );
									
									if( rem )
										{
										PreList->pl_Entries--;
										
										if( !IsListEmpty( &PreList->pl_List ) )
											{
											if( curselected > PreList->pl_Entries - 1 )
												curselected--;
											
											GT_SetGadgetAttrs( gads[PRGT_LISTVIEW].xg_Gad, win, NULL,
												GTLV_Selected, curselected,
												TAG_END );
											}
										else
											{
											SetGadgetAttrsA( gads[PRGT_SORT].xg_Gad, win, NULL, DisabledTrueTags );
											SetGadgetAttrsA( gads[PRGT_REMOVE].xg_Gad, win, NULL, DisabledTrueTags );
											GT_SetGadgetAttrsA( gads[PRGT_STRING].xg_Gad, win, NULL, DisabledTrueTags );
											
											curselected = -1;
											}
										}
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

VOID xSortNodes( VOID )
{
	struct PrespecifiedNode *	pos;
	struct PrespecifiedNode *	min;
	struct PrespecifiedNode *	nn;
	UBYTE						temp[DEFAULTBUFFERLEN] = "";
	
	
	pos = (struct PrespecifiedNode *)PreList->pl_List.lh_Head; /* pos starter i 1    */
	
		/// Så længe pos ikke er i slutningen af filen
	
	while( (struct PrespecifiedNode *)pos->pn_Node.ln_Succ )
		{
		nn = pos;
		min = pos;
		
		while( (struct PrespecifiedNode *)nn->pn_Node.ln_Succ )
			{
				/// min > nn  -> min = nn
			
			if( stricmp( min->pn_Name, nn->pn_Name ) > 0 )
				min = nn;
			
			nn = (struct PrespecifiedNode *)nn->pn_Node.ln_Succ;
			}
		
			/// min og pos bytter plads
		
		nn = pos;
		
			/// I stedet for at bruge endnu en temp pointer!
		
		strcpy( temp, pos->pn_Name );
		strcpy( nn->pn_Name, min->pn_Name );
		strcpy( min->pn_Name, temp );
		
		pos = (struct PrespecifiedNode *)pos->pn_Node.ln_Succ;
		}
}

/*
 * Add some objects to PreList.. 
 */
BOOL xAddPrespecified( struct Window *win, struct Gadget *lvgad, LONG curselected )
{
	struct rtFileList *	plist;
	struct rtFileList *	tlist;
	BPTR				lock;
	BOOL				added = FALSE;
	UBYTE				strbuf1[ DEFAULTBUFFERLEN ] = "";
	UBYTE				strbuf2[ DEFAULTBUFFERLEN ] = "";
	
	
	rtChangeReqAttr( FileReq, RTFI_Dir, RPATH, TAG_END );
	
	if( plist = rtFileRequest( FileReq, RFILE, GCS( MSG_TL_SELECTFILESDRAWERS ),
		RT_Window, win,
		RT_Underscore, '_',
		RT_TextAttr, &RequesterFontAttr,
		RTFI_Flags, FREQF_SELECTDIRS | FREQF_MULTISELECT,
		RTFI_OkText, GCS( MSG_GF_SELECT ),
		TAG_END ) )
		{
		GT_SetGadgetAttrs( lvgad, win, NULL,
			GTLV_Labels, ~0,
			TAG_END );
		
		tlist = plist;
		
			/// Add all the paths and files
		
		while( plist )
			{
			strncpy( strbuf1, FileReq->Dir, DEFAULTPATHLEN );
			AddPart( strbuf1, plist->Name, DEFAULTBUFFERLEN );
			
			if( lock = Lock( strbuf1, ACCESS_READ ) )
				{
				if( NameFromLock( lock, strbuf2, DEFAULTBUFFERLEN ) )
					{
					if( xAddPrespecifiedNode( &PreList->pl_List, strbuf2 ) )
						{
						added = TRUE;
						
						PreList->pl_Entries++;
						}
					}
				UnLock( lock );
				}
			
			plist = plist->Next;
			}
		
			/// Reattach the modified list to the gadget and set the selected one
		
		GT_SetGadgetAttrs( lvgad, win, NULL,
			GTLV_Labels, &PreList->pl_List,
			TAG_END );
		
		strncpy( RPATH, FileReq->Dir, DEFAULTPATHLEN );
		
		rtFreeFileList( tlist );
		}
	
	return( added );
}

BOOL xRemovePrespecified( WORD number )
{
	struct Node *	worknode;
	BOOL			success = FALSE;
	
	
	worknode = PreList->pl_List.lh_Head;
	
	while( worknode && number )
		{
		number--;
		
		worknode = worknode->ln_Succ;
		}
	
	if( worknode )
		{
		Remove( worknode );
		
		FreeVec( worknode );
		
		success = TRUE;
		}
	
	return( success );
}

VOID xUpdatePrespecifiedName( struct Window *win, struct Gadget *lvgad, STRPTR name, ULONG nr )
{
	struct PrespecifiedNode *	pnode;
	UWORD						c = 0;
	
	
	GT_SetGadgetAttrs( lvgad, win, NULL,
		GTLV_Labels, ~0,
		TAG_END );
	
	pnode = (struct PrespecifiedNode *)PreList->pl_List.lh_Head;
	
		/// Find the right buffer
	
	while( pnode )
		{
		if( c == nr )
			break;
		
		c++;
		pnode = (struct PrespecifiedNode *)pnode->pn_Node.ln_Succ;
		}
	
		/// Copy the new name
	
	if( pnode )
		strncpy( pnode->pn_Name, name, DEFAULTBUFFERLEN );
	
		/// Reattach the modified list to the gadget
	
	GT_SetGadgetAttrs( lvgad, win, NULL,
		GTLV_Labels,	&PreList->pl_List,
		GTLV_Selected,	nr,
		TAG_END );
}

BOOL xAddPrespecifiedNode( struct List *l, STRPTR name )
{
	struct PrespecifiedNode *	pn;
	
	
	if( pn = AllocVec( sizeof( struct PrespecifiedNode ), MEMF_CLEAR ) )
		{
		strncpy( pn->pn_Name, name, DEFAULTBUFFERLEN );
		
		pn->pn_Node.ln_Name = pn->pn_Name;
		
		AddTail( l, (struct Node *)pn );
		
		return( TRUE );
		}
	
	return( FALSE );
}

VOID xFreePrespecifiedNodes( struct List *l )
{
	if( l && !IsListEmpty( l ) )
		{
		struct Node *	worknode;
		struct Node *	nextnode;
		
		
		worknode = l->lh_Head;
		
		while( nextnode = worknode->ln_Succ )
			{
			FreeVec( worknode );
			
			worknode = nextnode;
			}
		}
}
