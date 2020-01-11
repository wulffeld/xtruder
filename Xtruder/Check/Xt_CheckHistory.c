/*
**	$VER: XtruderCheckHistory.c (30.7.96)
**
**	Xtruder check history functions
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


VOID __stdargs xAddFileCheckHistoryNodes( UWORD color, STRPTR format, ... )
{
	if( XSettings->xs_HIS_RecFileCheck || (XSettings->xs_FC_CHW_History && CheckWindow) )
		{
		UBYTE				strbuf[ DEFAULTBUFFERLEN ];
		
		
		RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
		
			/// Add to normal global history?
		
		if( XSettings->xs_HIS_RecFileCheck )
			xAddHistoryNode( color, strbuf );
		
			/// Add to local check history? Requires check window to be open.
			/// It might not be open if user uses the 'Stay hidden...' option.
		
		if( XSettings->xs_FC_CHW_History && CheckWindow )
			{
			struct HistoryNode *	hn;
			
			
				/// Remove list from ListView
			
			GT_SetGadgetAttrs( CheckWindowGadgets[ GAD_CW_CHECKHISTORY ].xg_Gad, CheckWindow, NULL,
				GTLV_Labels, ~0,
				TAG_END );
			
				/// Add information to list
			
			if( hn = AllocVec( sizeof( struct HistoryNode ), MEMF_CLEAR ) )
				{
				if( hn->hn_Name = AllocVec( strlen( strbuf ) + 1, MEMF_CLEAR ) )
					{
					strcpy( hn->hn_Name, strbuf );
					hn->hn_Node.ln_Name = hn->hn_Name;
					hn->hn_Color = color;
					CheckWindowHistoryList.cl_Entries++;
					
					AddTail( &CheckWindowHistoryList.cl_List, (struct Node *)hn );
					}
				}
			
				/// Reattach list to ListView
			
			if( XSettings->xs_FC_CHW_AutoScroll )
				{
				GT_SetGadgetAttrs( CheckWindowGadgets[ GAD_CW_CHECKHISTORY ].xg_Gad, CheckWindow, NULL,
					GTLV_Labels, &CheckWindowHistoryList.cl_List,
					GTLV_Top, __builtin_max( 0, xCountListEntries( &CheckWindowHistoryList.cl_List ) - HistoryLVVisible ),
					TAG_END );
				}
			else
				{
				GT_SetGadgetAttrs( CheckWindowGadgets[ GAD_CW_CHECKHISTORY ].xg_Gad, CheckWindow, NULL,
					GTLV_Labels, &CheckWindowHistoryList.cl_List,
					TAG_END );
				}
			}
		}
}
