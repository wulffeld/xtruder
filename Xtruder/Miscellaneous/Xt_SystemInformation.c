/*
**	$VER: Xt_SystemInformation.c (5.7.97)
**
**	Xtruder system information functions
**
**	Copyright (C) 1996-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


struct Window *				SysInfoWindow;
struct Gadget *				SysInfoWindowGadList;
struct xGadget *			SysInfoWindowGadgets;
struct Hook					SysInfoRenderHook;
struct List					SysInfoList;
UWORD						SysInfoListViewVisible;


#define		SPACE_BETWEEN_GADS				4


/*
 * Calculates the minimum height of the listview
 */
UWORD xCalc_SysInfoListViewHeight( VOID )
{
	return( (UWORD)((8 * TxtFontYSize) + 4) );
}

/*
 * Calculates the minimum width of the about window
 */
UWORD xCalc_SysInfoMinWindowWidth( VOID )
{
	return( (UWORD)(20 + 20 * TxtFontXSize) );
}

/*
 * Calculates the minimum height of the about window
 */
UWORD xCalc_SysInfoMinWindowHeight( VOID )
{
	return( (UWORD)(ScreenBarHeight + 2 + xCalc_SysInfoListViewHeight() + 2 + 10) );
}

VOID xRemoveSysInfoWindowGadgets( VOID )
{
		/// Remove gadgets from window
	
	if( SysInfoWindowGadgets )
		{
		if( SysInfoWindowGadList )
			{
			RemoveGList( SysInfoWindow, SysInfoWindowGadList, -1 );
			
			FreeGadgets( SysInfoWindowGadList );
			SysInfoWindowGadList = NULL;
			}
		
		FreeVec( SysInfoWindowGadgets );
		SysInfoWindowGadgets = NULL;
		}
	
}

ULONG __asm __saveds xSysInfoRenderHook( REGA1 struct LVDrawMsg *msg, REGA2 struct SysInfoNode *node )
{
	struct RastPort *	rp;
	struct TextExtent	extent;
	UBYTE				state;
	ULONG				fit;
	WORD				x,y;
	WORD				slack;
	ULONG				apen = node->sn_Color;
	ULONG				bpen;
	UWORD *				pens;
	STRPTR				name = node->sn_Node.ln_Name;
	
	
	if( msg->lvdm_MethodID != LV_DRAW )
		return( LVCB_UNKNOWN );
	
	rp    = msg->lvdm_RastPort;
	state = msg->lvdm_State;
	pens  = msg->lvdm_DrawInfo->dri_Pens;
	
	bpen = pens[ FILLPEN ];
	
	if( (state == LVR_NORMAL) || (state == LVR_NORMALDISABLED) )
		bpen = pens[ BACKGROUNDPEN ];
	
	fit = TextFit( rp, name, strlen( name ), &extent, NULL, 1,
					msg->lvdm_Bounds.MaxX - msg->lvdm_Bounds.MinX - 3,
					msg->lvdm_Bounds.MaxY - msg->lvdm_Bounds.MinY + 1 );
	
	slack = (msg->lvdm_Bounds.MaxY - msg->lvdm_Bounds.MinY) - (extent.te_Extent.MaxY - extent.te_Extent.MinY);
	
	x = msg->lvdm_Bounds.MinX - extent.te_Extent.MinX + 2;
	y = msg->lvdm_Bounds.MinY - extent.te_Extent.MinY + ((slack+1) / 2);
	
	extent.te_Extent.MinX += x;
	extent.te_Extent.MaxX += x;
	extent.te_Extent.MinY += y;
	extent.te_Extent.MaxY += y;
	
	SetABPenDrMd( rp, apen, bpen, JAM2 );
	
	Move( rp, x, y );
	Text( rp, name, fit );
	
	SetAPen( rp, bpen );
	xFillOldExtent( rp, &msg->lvdm_Bounds, &extent.te_Extent );
	
	if( (state == LVR_NORMALDISABLED) || (state == LVR_SELECTEDDISABLED) )
		xGhost( rp, pens[BLOCKPEN], msg->lvdm_Bounds.MinX, msg->lvdm_Bounds.MinY, msg->lvdm_Bounds.MaxX, msg->lvdm_Bounds.MaxY );
	
	return( LVCB_OK );
}

BOOL xCreateSysInfoWindowGadgets( VOID )
{
	struct NewGadget	ng;
	BOOL				success = TRUE;
	
	
		/// Remove previous gadgets
	
	xRemoveSysInfoWindowGadgets();
	
		/// Refresh window so it's as neat as a baby's bottom
	
	SetAPen( SysInfoWindow->RPort, 0 );
	RectFill( SysInfoWindow->RPort, SysInfoWindow->BorderLeft, SysInfoWindow->BorderTop, SysInfoWindow->Width - SysInfoWindow->BorderRight - 1,  SysInfoWindow->Height - SysInfoWindow->BorderBottom - 1 );
	SetAPen( SysInfoWindow->RPort, 1 );
	
	RefreshWindowFrame( SysInfoWindow );
	
		/// Start creating those gadgets
	
	if( SysInfoWindowGadgets = AllocVec( sizeof( struct xGadget ) * 1, MEMF_CLEAR ) )
		{
		SysInfoWindowGadgets[0].xg_Gad = CreateContext( &SysInfoWindowGadList );
		
		ng.ng_VisualInfo	= VisualInfo;
		ng.ng_TextAttr		= &TextFontAttr;
		ng.ng_Flags			= PLACETEXT_LEFT;
		ng.ng_LeftEdge		= 10;
		ng.ng_TopEdge		= ScreenBarHeight + 2;
		ng.ng_Width			= SysInfoWindow->Width - 20;
		ng.ng_Height		= SysInfoWindow->Height - SysInfoWindow->BorderBottom - 2 - ng.ng_TopEdge;
		ng.ng_Height		= (((ng.ng_Height - 4) / TxtFontYSize) * TxtFontYSize) + 4;
		ng.ng_GadgetText	= SysInfoWindowGadgets[6].xg_GadgetText = "";
		ng.ng_GadgetID		= SysInfoWindowGadgets[6].xg_GadgetID = 1000;
		
		SysInfoRenderHook.h_Entry = (HOOKFUNC)xSysInfoRenderHook;
		
		SysInfoWindowGadgets[0].xg_Gad = CreateGadget( LISTVIEW_KIND, SysInfoWindowGadgets[0].xg_Gad, &ng,
			GTLV_Labels,		&SysInfoList,
			GTLV_ReadOnly,		TRUE,
			GTLV_ScrollWidth,	XSettings->xs_GUI_ScrollerWidth,
			GTLV_CallBack,		&SysInfoRenderHook,
			TAG_END );
		
			/// Check if all gadgets were created
		
		if( !SysInfoWindowGadgets[ 0 ].xg_Gad )
			success = FALSE;
		
		if( success )
			{
			AddGList( SysInfoWindow, SysInfoWindowGadList, -1, -1, NULL );
			RefreshGList( SysInfoWindowGadList, SysInfoWindow, NULL, -1 );
			
			GT_RefreshWindow( SysInfoWindow, NULL );
			
			return( TRUE );
			}
		}
	
		/// Free what we DID create...
	
	xRemoveSysInfoWindowGadgets();
	
	return( FALSE );
}

/*
 * Report window uses TextFont!
 */
VOID xOpenSysInfoWindow( VOID )
{
	UWORD	minheight = xCalc_SysInfoMinWindowHeight();
	
	
	if( Screen )
		{
		xDisableWindow( Window );
		
		if( SysInfoWindow = OpenWindowTags( NULL,
			WA_Left,			Coords->xc_SystemInformationWindow[0],
			WA_Top,				Coords->xc_SystemInformationWindow[1],
			WA_Width,			__builtin_max( xCalc_SysInfoMinWindowWidth() + 20, Coords->xc_SystemInformationWindow[2] ),
			WA_Height,			__builtin_max( minheight, Coords->xc_SystemInformationWindow[3] ),
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
			WA_Title,			GCS( MSG_SYIN_TL_SYSTEMINFORMATION ),
			WA_Activate,		TRUE,
			WA_CloseGadget,		TRUE,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_SizeGadget,		TRUE,
			WA_SizeBBottom,		TRUE,
			WA_SmartRefresh,	TRUE,
			WA_MinWidth,		xCalc_SysInfoMinWindowWidth() + 20,
			WA_MinHeight,		minheight,
			WA_MaxWidth,		-1,
			WA_MaxHeight,		-1,
			TAG_END ) )
			{
			xGenerateSystemInformationList();
			
			if( xCreateSysInfoWindowGadgets() )
				{
				SetFont( SysInfoWindow->RPort, TextFont );
				
				xHandleSysInfoWindowIDCMP();
				
				xFreeSysInfoNodes();
				}
			
			xCloseSysInfoWindow();
			}
		
		xEnableWindow( Window );
		}
}

VOID xCloseSysInfoWindow( VOID )
{
	xRemoveSysInfoWindowGadgets();
	
	if( SysInfoWindow )
		{
			/// Store position of window
		
		Coords->xc_SystemInformationWindow[0] = SysInfoWindow->LeftEdge;
		Coords->xc_SystemInformationWindow[1] = SysInfoWindow->TopEdge;
		Coords->xc_SystemInformationWindow[2] = SysInfoWindow->Width;
		Coords->xc_SystemInformationWindow[3] = SysInfoWindow->Height;
		
			/// Close check window
		
		CloseWindow( SysInfoWindow );
		SysInfoWindow = NULL;
		}
}

VOID xHandleSysInfoWindowIDCMP( VOID )
{
	struct IntuiMessage *	msg;
	ULONG					class;
	UWORD					code,qualifier,curvistop;
	struct NewLVPos			nlv = {0,0};
	
	
	while( 1 )
		{
		WaitPort( SysInfoWindow->UserPort );
		
		while( msg = GT_GetIMsg( SysInfoWindow->UserPort ) )
			{
			class = msg->Class;
			code = msg->Code;
			qualifier = msg->Qualifier;
			
			GT_ReplyIMsg( msg );
			
			switch( class )
				{
				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh( SysInfoWindow );
					GT_EndRefresh( SysInfoWindow, TRUE );
					break;
				
				case IDCMP_NEWSIZE:
					if( !xCreateSysInfoWindowGadgets() )
						return;
					break;
				
				case IDCMP_RAWKEY:
					switch( code )
						{
						case CURSORUP:
						case CURSORDOWN:
							curvistop = xListViewTop( SysInfoWindowGadgets[ 0 ].xg_Gad, SysInfoWindow );
							
							xNewListPos( &nlv, qualifier, code, code == CURSORUP ? curvistop : curvistop + SysInfoListViewVisible - 1, curvistop, SysInfoListViewVisible, xCountListEntries( &SysInfoList ) );
							
							GT_SetGadgetAttrs( CheckWindowGadgets[ GAD_CW_CHECKHISTORY ].xg_Gad, CheckWindow, NULL,
								GTLV_Top, nlv.nl_NewTop,
								TAG_END );
							break;
						
						case KEY_ESCAPE:
							return;
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

VOID xGenerateSystemInformationList( VOID )
{
	struct xfdSlave *	xs;
	BPTR				lock;
	UBYTE				strbuf[ 255 ];
	UBYTE				pathbuf[ 255 ];
	UWORD				p,msg;
	ULONG				adr;
	UBYTE *				paths[3];
	STRPTR				name;
	
	
		/// Prepare list
	
	NewList( &SysInfoList );
	
	paths[0] = XSettings->xs_PAT_ArchiveExtractPath;
	paths[1] = XSettings->xs_PAT_FileDecrunchPath;
	paths[2] = XSettings->xs_PAT_RecheckPath;
	
		/// Exec vectors
	
	adr = (ULONG)SysBase->CoolCapture;
	xAddSysInfoNode( adr ? 2 : 1, "Coolcapture: $%08lx", adr );
	
	adr = (ULONG)SysBase->ColdCapture;
	xAddSysInfoNode( adr ? 2 : 1, "Coldcapture: $%08lx", adr );
	
	adr = (ULONG)SysBase->WarmCapture;
	xAddSysInfoNode( adr ? 2 : 1, "Warmcapture: $%08lx", adr );
	
	adr = (ULONG)SysBase->KickTagPtr;
	xAddSysInfoNode( adr ? 2 : 1, "KickTagPtr:  $%08lx", adr );
	
	adr = (ULONG)SysBase->KickMemPtr;
	xAddSysInfoNode( adr ? 2 : 1, "KickMemPtr:  $%08lx", adr );
	
	xAddSysInfoNode( 1, "" );
	
		/// Size of Xtruder
	
	adr = xGetXtruderSize();
	
	xAddSysInfoNode( adr ? (adr != 183124 ? 2 : 1) : 1, GCS( MSG_SYIN1 ), 183124 );
	
		/// Paths
	
	xAddSysInfoNode( 1, "" );
	
	for( p = 0, msg = MSG_SYIN8; p < 3; p++, msg++ )
		{
		pathbuf[0] = 0;
		
		if( paths[ p ][0] )
			{
			if( lock = Lock( paths[ p ], ACCESS_READ ) )
				{
				if( NameFromLock( lock, strbuf, 255 ) )
					strcpy( pathbuf, strbuf );
				
				UnLock( lock );
				
				xAddSysInfoNode( 1, GCS( msg ), pathbuf );
				}
			else
				xAddSysInfoNode( 3, GCS( msg ), GCS( MSG_SYIN_NOTFOUNDPLEASECONFIG ) );
			}
		else
			xAddSysInfoNode( 3, GCS( msg ), GCS( MSG_SYIN_NOTFOUNDPLEASECONFIG ) );
		}
	
	xAddSysInfoNode( 1, "" );
	
		/// Bootblock.library
	
	if( !GetBBBFInfo( &p, strbuf ) )
		xAddSysInfoNode( 1, GCS( MSG_SYIN2 ), BootblockBase->lib_Version, BootblockBase->lib_Revision, p, strbuf );
	
		/// FileID.library
	
	xAddSysInfoNode( 1, GCS( MSG_SYIN3 ), FileIDBase->lib_Version, FileIDBase->lib_Revision, FIGetHighID() );
	
		/// filevirus.library
	
	xAddSysInfoNode( 1, GCS( MSG_SYIN4 ), ((struct FilevirusBase *)FilevirusBase)->fb_Lib.lib_Version, ((struct FilevirusBase *)FilevirusBase)->fb_Lib.lib_Revision, ((struct FilevirusBase *)FilevirusBase)->fb_VInfoTotal );
	
		/// unpack.library
	
	upUnpackList( UInfo );
	
	UPTypes = 0;
	
	do
		{
		UPTypes++;
		}
	while( name = upUnpackListNext( UInfo ) );

	xAddSysInfoNode( 1, GCS( MSG_SYIN5 ), UnpackBase->lib_Version, UnpackBase->lib_Revision, UPTypes );
	
		/// xfdmaster.library
	
	xs = ((struct xfdMasterBase *)xfdMasterBase)->xfdm_FirstSlave;
	
	p = 0;
	
	do
		{
		p++;
		}
	while( xs = xs->xfds_Next );

	xAddSysInfoNode( 1, GCS( MSG_SYIN6 ), xfdMasterBase->lib_Version, xfdMasterBase->lib_Revision, p );
	
		/// xtruder.library
	
	xAddSysInfoNode( 1, GCS( MSG_SYIN7 ), XtruderBase->lib_Version, XtruderBase->lib_Revision, XInfo->xi_KnownPatches );
}

VOID __stdargs xAddSysInfoNode( UWORD color, STRPTR format, ... )
{
	struct SysInfoNode *	sn;
	UBYTE					strbuf[ DEFAULTBUFFERLEN ] = "";
	UWORD					len;
	
	
	if( sn = AllocVec( sizeof( struct SysInfoNode ), MEMF_CLEAR ) )
		{
		RawDoFmt( format, (APTR)(&format+1), (void (*)())"\x16\xc0\x4E\x75", strbuf );
		
		if( (len = strlen( strbuf )) >= 300 )
			len = 299;
		
		strncpy( sn->sn_Name, strbuf, len );
		sn->sn_Node.ln_Name = sn->sn_Name;
		sn->sn_Color = color;
		
		AddTail( &SysInfoList, (struct Node *)sn );
		}
}

VOID xFreeSysInfoNodes( VOID )
{
	if( !IsListEmpty( &SysInfoList ) )
		{
		struct Node *	worknode;
		struct Node *	nextnode;
		
		
		worknode = SysInfoList.lh_Head;
		
		while( nextnode = worknode->ln_Succ )
			{
			FreeVec( worknode );
			
			worknode = nextnode;
			}
		}
}

ULONG xGetXtruderSize( VOID )
{
	UBYTE	xtrudername[ DEFAULTBUFFERLEN ];
	ULONG	filesize = 0;
	BPTR	file;
	
	
	if( !(GetProgramName( xtrudername, DEFAULTBUFFERLEN ) ) )
		strcpy( xtrudername, Process->pr_Task.tc_Node.ln_Name );
	
	if( file = Open( xtrudername, MODE_OLDFILE ) )
		{
		Seek( file, 0, OFFSET_END );
		
		filesize = Seek( file, 0, OFFSET_BEGINNING );
		
		Close( file );
		}
	
	return( filesize );
}
