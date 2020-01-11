/*
**	$VER: XtruderSystem.c (29.10.95)
**
**	Xtruder system functions
**
**	Copyright (C) 1994-1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Handle all the listings
 */
VOID xListHandler( UWORD listwhat )
{
	struct List *	list = NULL;
	UBYTE			bartitle[ 150 ];
	STRPTR			wintitle = NULL;
	UWORD *			coords = NULL;
	ULONG			guidehelpcontextid = 0;
	BOOL			nopropfont = 0;
	
	
	switch( listwhat )
		{
		case LIST_KNOWNBOOTBLOCKS:
			wintitle = GCS( MSG_LIKN_TL_KNOWNBOOTBLOCKVIRUSES );
			coords = Coords->xc_ListKnownBootblockViraWindow;
			guidehelpcontextid = AGH_PROJ_LKNO_BOOTBLOCKVIRA;
			nopropfont = FALSE;
			
			list = xGenerateBootblockViraList( bartitle );
			break;
		
		case LIST_KNOWNFILEANDLINKVIRA:
			wintitle = GCS( MSG_LIKN_TL_KNOWNFILEBASEDVIRUSES );
			coords = Coords->xc_ListKnownFileBasedViraWindow;
			guidehelpcontextid = AGH_PROJ_LKNO_FILEBASEDVIRA;
			nopropfont = FALSE;
			
			list = xGenerateFileAndLinkViraList( bartitle );
			break;
		
		case LIST_KNOWNDECRUNCHERS:
			wintitle = GCS( MSG_LIKN_TL_KNOWNCRUNCHERS );
			coords = Coords->xc_ListKnownCrunchersWindow;
			guidehelpcontextid = AGH_PROJ_LKNO_CRUNCHERS;
			nopropfont = FALSE;
			
			list = xGenerateCruncherList( bartitle );
			break;
		
		case LIST_KNOWNPATCHES:
			wintitle = GCS( MSG_LIKN_TL_KNOWNPATCHES );
			coords = Coords->xc_ListKnownPatchesWindow;
			guidehelpcontextid = AGH_PROJ_LKNO_PATCHES;
			nopropfont = TRUE;
			
			list = xGeneratePatchList( bartitle );
			break;
		
		case LIST_KNOWNFILETYPES:
			wintitle = GCS( MSG_LIKN_TL_KNOWNFILETYPES );
			coords = Coords->xc_ListKnownFileTypesWindow;
			guidehelpcontextid = AGH_PROJ_LKNO_FILETYPES;
			nopropfont = FALSE;
			
			list = xGenerateFiletypeList( bartitle );
			break;
		
		case LIST_LIBRARIES:
			wintitle = GCS( MSG_MESW_TL_LIBRARIES );
			coords = Coords->xc_MemoryLibrariesWindow;
			guidehelpcontextid = AGH_MEMO_SHOW_LIBRARIES;
			nopropfont = TRUE;
			
			list = xGenerateLibraryList( bartitle );
			break;
		
		case LIST_DEVICES:
			wintitle = GCS( MSG_MESW_TL_DEVICES );
			coords = Coords->xc_MemoryDevicesWindow;
			guidehelpcontextid = AGH_MEMO_SHOW_DEVICES;
			nopropfont = TRUE;
			
			list = xGenerateDeviceList( bartitle );
			break;
		
		case LIST_RESOURCES:
			wintitle = GCS( MSG_MESW_TL_RESOURCES );
			coords = Coords->xc_MemoryResourcesWindow;
			guidehelpcontextid = AGH_MEMO_SHOW_RESOURCES;
			nopropfont = TRUE;
			
			list = xGenerateResourceList( bartitle );
			break;
		
		case LIST_PORTS:
			wintitle = GCS( MSG_MESW_TL_PORTS );
			coords = Coords->xc_MemoryPortsWindow;
			guidehelpcontextid = AGH_MEMO_SHOW_PORTS;
			nopropfont = TRUE;
			
			list = xGeneratePortList( bartitle );
			break;
		
		case LIST_SEMAPHORES:
			wintitle = GCS( MSG_MESW_TL_SEMAPHORES );
			coords = Coords->xc_MemorySemaphoresWindow;
			guidehelpcontextid = AGH_MEMO_SHOW_SEMAPHORES;
			nopropfont = TRUE;
			
			list = xGenerateSemaphoreList( bartitle );
			break;
		
		case LIST_TASKS:
			wintitle = GCS( MSG_MESW_TL_TASKS );
			coords = Coords->xc_MemoryTasksWindow;
			guidehelpcontextid = AGH_MEMO_SHOW_TASKS;
			nopropfont = TRUE;
			
			list = xGenerateTaskList( bartitle );
			break;
		}
	
	if( list && !IsListEmpty( list ) )
		{
		xDisableWindow( Window );
		
			/// Show it
		
		xOpenListShowWindow( list, wintitle, bartitle, coords, guidehelpcontextid, nopropfont );
		
		xEnableWindow( Window );
		
			/// Free it
		
		xFreeNameNodes( list );
		
		FreeVec( list );
		}
}

struct List *xGenerateBootblockViraList( STRPTR bartitle )
{
	struct Bootblock *	bb;
	struct List *		bblist;
	
	
	if( bblist = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( bblist );
		
		xSPrintF( bartitle, GCS( MSG_LIKN_VIRUSES ), Virus );
		
		if( bb = ListBBBF() )
			{
			while( bb = bb->Next )
				{
				if( bb->bootname[0] )
					xAddNameNode( bblist, bb->bootname );
				else
					break;
				}
			}
		}
	
	return( bblist );
}

struct List *xGenerateFileAndLinkViraList( STRPTR bartitle )
{
	struct FilevirusInfo *	finfo;
	struct List *			list;
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		for( finfo = fvShowFVInfo( FilevirusNode ); finfo; finfo = fvShowNextFVInfo( FilevirusNode ) )
			xAddNameNode( list, (STRPTR)finfo->fvi_Name );
		
		xSPrintF( bartitle, GCS( MSG_LIKN_VIRUSES ), ((struct FilevirusBase *)FilevirusBase)->fb_VInfoTotal );
		}
	
	return( list );
}

struct List *xGenerateCruncherList( STRPTR bartitle )
{
	struct List *		list;
	struct xfdSlave *	xs;
	STRPTR				str;
	UWORD				xtypes = 0;
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		xAddNameNode( list, "Unpack.library:" );
		xAddNameNode( list, "" );
		
		xAddNameNode( list, upUnpackList( UInfo ) );
		
		while( str = upUnpackListNext( UInfo ) )
			xAddNameNode( list, str );
		
		xAddNameNode( list, "" );
		xAddNameNode( list, "xfdmaster.library:" );
		xAddNameNode( list, "" );
		
		xs = ((struct xfdMasterBase *)xfdMasterBase)->xfdm_FirstSlave;
		
		do
			{
			xtypes++;
			
			xAddNameNode( list, xs->xfds_PackerName );
			}
		while( xs = xs->xfds_Next );
		
		xSPrintF( bartitle, GCS( MSG_LIKN_CRUNCHERTYPES ), UPTypes + xtypes );
		}
	
	return( list );
}

struct List *xGeneratePatchList( STRPTR bartitle )
{
	struct List *	list;
	STRPTR			str;
	UWORD			a = 0;
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		if( str = xlPatchListFirst( XInfo ) )
			{
			xAddNameNode( list, str );
			
			a = 1;
			
			while( str = xlPatchListNext( XInfo ) )
				{
				xAddNameNode( list, str );
				
				a++;
				}
			}
		
		xSPrintF( bartitle, GCS( MSG_LIKN_PATCHES ), a );
		}
	
	return( list );
}

struct List *xGenerateFiletypeList( STRPTR bartitle )
{
	struct List *	list;
	UWORD			num,n;
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		num = FIGetHighID();
		
		for( n = 1; n < num; n++ )
			xAddNameNode( list, FIGetIDString( n ) );
		
		xSPrintF( bartitle, GCS( MSG_LIKN_FILETYPES ), n );
		}
	
	return( list );
}

struct List *xGenerateLibraryList( STRPTR bartitle )
{
	struct Library *	lib;
	struct List *		list;
	UWORD				n = 0;
	UBYTE				strbuf[ DEFAULTBUFFERLEN ];
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		Forbid();
		
		xAddNameNode( list, "%-32s%10s %4s %10s", GCS( MSG_MESW_NAME ), GCS( MSG_MESW_ADDRESS ), GCS( MSG_MESW_PRIORITY ), GCS( MSG_MESW_VERSION ) );
		xAddNameNode( list, "" );
		
		for( lib = (struct Library *)(((struct ExecBase *)SysBase)->LibList.lh_Head);
			 lib != (struct Library *)(((struct ExecBase *)SysBase)->LibList.lh_TailPred->ln_Succ);
			 lib = (struct Library *)lib->lh_Node.ln_Succ)
			{
			if( lib->lh_Node.ln_Name )
				{
				n++;
				
				xSPrintF( strbuf, "%ld.%-ld", lib->lib_Version, lib->lib_Revision );
				
				xAddNameNode( list, "%-32s $%08lx %4ld %10s", lib->lib_Node.ln_Name, &lib->lib_Node, lib->lib_Node.ln_Pri, strbuf );
				}
			}
		
		Permit();
		
		xSPrintF( bartitle, GCS( MSG_MESW_TOTAL ), n );
		}
	
	return( list );
}

struct List *xGenerateDeviceList( STRPTR bartitle )
{
	struct Library *	lib;
	struct List *		list;
	UWORD				n = 0;
	UBYTE				strbuf[ DEFAULTBUFFERLEN ];
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		Forbid();
		
		xAddNameNode( list, "%-32s%10s %4s %10s", GCS( MSG_MESW_NAME ), GCS( MSG_MESW_ADDRESS ), GCS( MSG_MESW_PRIORITY ), GCS( MSG_MESW_VERSION ) );
		xAddNameNode( list, "" );
		
		for( lib = (struct Library *)(((struct ExecBase *)SysBase)->DeviceList.lh_Head);
			 lib != (struct Library *)(((struct ExecBase *)SysBase)->DeviceList.lh_TailPred->ln_Succ);
			 lib = (struct Library *)lib->lh_Node.ln_Succ)
			{
			if( lib->lh_Node.ln_Name )
				{
				n++;
				
				xSPrintF( strbuf, "%ld.%-ld", lib->lib_Version, lib->lib_Revision );
				
				xAddNameNode( list, "%-32s $%08lx %4ld %10s", lib->lib_Node.ln_Name, &lib->lib_Node, lib->lib_Node.ln_Pri, strbuf );
				}
			}
		
		Permit();
		
		xSPrintF( bartitle, GCS( MSG_MESW_TOTAL ), n );
		}
	
	return( list );
}

struct List *xGenerateResourceList( STRPTR bartitle )
{
	struct Library *	lib;
	struct List *		list;
	UWORD				n = 0;
	UBYTE				strbuf[ DEFAULTBUFFERLEN ];
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		Forbid();
		
		xAddNameNode( list, "%-32s%10s %4s %10s", GCS( MSG_MESW_NAME ), GCS( MSG_MESW_ADDRESS ), GCS( MSG_MESW_PRIORITY ), GCS( MSG_MESW_VERSION ) );
		xAddNameNode( list, "" );
		
		for( lib = (struct Library *)(((struct ExecBase *)SysBase)->ResourceList.lh_Head);
			 lib != (struct Library *)(((struct ExecBase *)SysBase)->ResourceList.lh_TailPred->ln_Succ );
			 lib = (struct Library *)lib->lh_Node.ln_Succ)
			{
			if( lib->lh_Node.ln_Name )
				{
				n++;
				
				xSPrintF( strbuf, "%ld.%-ld", lib->lib_Version, lib->lib_Revision );
				
				xAddNameNode( list, "%-32s $%08lx %4ld %10s", lib->lib_Node.ln_Name, &lib->lib_Node, lib->lib_Node.ln_Pri, strbuf );
				}
			}
		
		Permit();
		
		xSPrintF( bartitle, GCS( MSG_MESW_TOTAL ), n );
		}
	
	return( list );
}

struct List *xGeneratePortList( STRPTR bartitle )
{
	struct MsgPort *	lib;
	struct List *		list;
	UWORD				n = 0;
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		Forbid();
		
		xAddNameNode( list, "%-32s%10s %4s", GCS( MSG_MESW_NAME ), GCS( MSG_MESW_ADDRESS ), GCS( MSG_MESW_PRIORITY ) );
		xAddNameNode( list, "" );
		
		for( lib = (struct MsgPort *)(((struct ExecBase *)SysBase)->PortList.lh_Head);
			 lib != (struct MsgPort *)(((struct ExecBase *)SysBase)->PortList.lh_TailPred->ln_Succ );
			 lib = (struct MsgPort *)lib->mp_Node.ln_Succ)
			{
			if( lib->mp_Node.ln_Name )
				{
				n++;
				
				xAddNameNode( list, "%-32s $%08lx %4ld", lib->mp_Node.ln_Name, &lib->mp_Node, lib->mp_Node.ln_Pri );
				}
			}
		
		Permit();
		
		xSPrintF( bartitle, GCS( MSG_MESW_TOTAL ), n );
		}
	
	return( list );
}

struct List *xGenerateSemaphoreList( STRPTR bartitle )
{
	struct SignalSemaphore *	lib;
	struct List *				list;
	UWORD						n = 0;
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		Forbid();
		
		xAddNameNode( list, "%-32s%10s %4s", GCS( MSG_MESW_NAME ), GCS( MSG_MESW_ADDRESS ), GCS( MSG_MESW_PRIORITY ) );
		xAddNameNode( list, "" );
		
		for( lib = (struct SignalSemaphore *)(((struct ExecBase *)SysBase)->SemaphoreList.lh_Head);
			 lib != (struct SignalSemaphore *)(((struct ExecBase *)SysBase)->SemaphoreList.lh_TailPred->ln_Succ );
			 lib = (struct SignalSemaphore *)lib->ss_Link.ln_Succ)
			{
			n++;
			
			xAddNameNode( list, "%-32s $%08lx %4ld", lib->ss_Link.ln_Name, &lib->ss_Link, lib->ss_Link.ln_Pri );
			}
		
		Permit();
		
		xSPrintF( bartitle, GCS( MSG_MESW_TOTAL ), n );
		}
	
	return( list );
}

struct List *xGenerateTaskList( STRPTR bartitle )
{
	struct Task *	task;
	struct List *	list;
	UWORD			n = 0;
	
	
	if( list = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( list );
		
		xAddNameNode( list, "%-32s %-11s %9s%4s", GCS( MSG_MESW_NAME ), GCS( MSG_MESW_TYPE ), GCS( MSG_MESW_ADDRESS ), GCS( MSG_MESW_PRIORITY ) );
		xAddNameNode( list, "" );
		
		Disable();
		
		for(task  = (struct Task *)(((struct ExecBase *)SysBase)->TaskWait.lh_Head );
			task != (struct Task *)(((struct ExecBase *)SysBase)->TaskWait.lh_TailPred->ln_Succ );
			task  = (struct Task *)task->tc_Node.ln_Succ )
			{
			if( task->tc_Node.ln_Name )
				{
				n++;
				
				xAddNameNode( list, "%-32s %-11s $%08lx %4ld",
					task->tc_Node.ln_Name,
					xGetNodeType( task->tc_Node.ln_Type ),
					&task->tc_Node,
					task->tc_Node.ln_Pri );
				}
			}
		
		Enable();
		
		xSPrintF( bartitle, GCS( MSG_MESW_TOTAL ), n );
		}
	
	return( list );
}

STRPTR xGetNodeType( UBYTE type )
{
	switch( type )
		{
		case NT_TASK:
			return( GCS( MSG_MESW_TASK ) );
			break;
		
		case NT_PROCESS:
			return( GCS( MSG_MESW_PROCESS ) );
			break;
		
		default:
			return( GCS( MSG_MESW_UNKNOWN ) );
			break;
		}
}
