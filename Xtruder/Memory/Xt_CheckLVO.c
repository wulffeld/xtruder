/*
**	$VER: XtruderCheckLVO.c (20.8.96)
**
**	Xtruder LVO check functions
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


struct LibCheckTab
{
	struct Library **	lt_LibBase;
	UWORD				lt_LibDefine;
};

struct LibCheckTab LCT[] =
{
	{(struct Library **)&DOSBase,			XI_LIB_DOS},
	{(struct Library **)&SysBase,			XI_LIB_EXEC},
	{(struct Library **)&GadToolsBase,		XI_LIB_GADTOOLS},
	{(struct Library **)&GfxBase,			XI_LIB_GRAPHICS},
	{(struct Library **)&IconBase,			XI_LIB_ICON},
	{(struct Library **)&IntuitionBase,		XI_LIB_INTUITION},
	{(struct Library **)&KeymapBase,		XI_LIB_KEYMAP},
	{(struct Library **)&UtilityBase,		XI_LIB_UTILITY},
	{(struct Library **)&WorkbenchBase,		XI_LIB_WORKBENCH},
};

VOID xCheckLVO( BOOL quiet_if_no_unknown )
{
	struct List *	veclist;
	UWORD			vira = 0;
	UWORD			patches = 0;
	UWORD			unknown = 0;
	UWORD			l;
	UBYTE			strbuf[ DEFAULTBUFFERLEN ];
	
	
	if( veclist = AllocVec( sizeof( struct List ), MEMF_CLEAR ) )
		{
		NewList( veclist );
		
		if( xAddNameNode( veclist, GCS( MSG_MEM_LIBRARYVECTORSINRAM ) ) )
			{
			if( xAddNameNode( veclist, "" ) )
				{
				struct Library	*	lib;
				BOOL				some = FALSE;
				
				
					/// Check exec Library for any non-standard vectors...
				
				for( l = 0; l < 9; l++ )
					{
					lib = *(LCT[ l ].lt_LibBase);
					
					if( lib )
						{
						if( xCheckLibrary( lib, -lib->lib_NegSize ) )
							{
							some = TRUE;
							
							XInfo->xi_LibBase = (ULONG *)lib;
							XInfo->xi_Library = LCT[ l ].lt_LibDefine;
							
							xAnalyseNonStandardVectors( veclist, -lib->lib_NegSize, &vira, &patches, &unknown );
							
							if( l != 8 )
								xAddNameNode( veclist, "" );
							}
						}
					}
				
				if( some )
					{
					xSPrintF( strbuf, GCS( MSG_MEM_LVOSTATISTICS ), vira, patches, unknown );
					
					xOpenListShowWindow( veclist, GCS( MSG_MEM_TL_LVOCHECK ), strbuf, Coords->xc_MemoryLVOWindow, AGH_MEMO_CHECKLVO, TRUE );
					}
				}
			xFreeNameNodes( veclist );
			}
		FreeVec( veclist );
		}
}

/*
 * Check if there are any vectors that point to an address outside the ROM
 */
BOOL xCheckLibrary( struct Library *lib, WORD lowerLVO )
{
	ULONG *	ad;
	ULONG	temp;
	WORD	o;
	
	
	for( o = -30; o >= lowerLVO; o -= 6 )
		{
		XInfo->xi_LVOOffset = o + 2;
		
		temp = (ULONG)lib + XInfo->xi_LVOOffset;
		ad = (ULONG *)temp;
		
		if( TypeOfMem( (APTR)*ad ) )
			return( TRUE );
		}
	
	return( FALSE );
}

BOOL xAnalyseNonStandardVectors( struct List *veclist, WORD lowerLVO, UWORD *vira, UWORD *patches, UWORD *unknown )
{
	ULONG *	ad;
	ULONG	temp;
	WORD	o;
	BOOL	success;
	
	
		/// Prepare library header....
	
	if( success = xPrepareLVOHeader( veclist, (struct Library *)XInfo->xi_LibBase ) )
		{
			/// Check the vectors one by one
		
		for( o = -30; o >= lowerLVO; o -= 6 )
			{
			XInfo->xi_LVOOffset = o + 2;
			
			temp = (ULONG)XInfo->xi_LibBase + XInfo->xi_LVOOffset;
			ad = (ULONG *)temp;
			temp = (ULONG)*ad;
			
			
			if( TypeOfMem( (APTR)*ad ) )
				{
				if( !xDoLVOCheck( veclist, o, temp, vira, patches, unknown ) )
					{
					success = FALSE;
					
					break;
					}
				}
			}
		}
	
	return( success );
}

BOOL xDoLVOCheck( struct List *veclist, WORD o, ULONG temp, UWORD *vira, UWORD *patches, UWORD *unknown )
{
	BOOL	added = FALSE;
	
	
	if( xlCheckLVO( XInfo ) )
		{
		switch( XInfo->xi_Result )
			{
			case XI_RES_VIRUS:
				added = xAddNameNode( veclist, "### %5ld (-$%04lx)  $%08lx  %s", o, -o, temp, XInfo->xi_Name );
				
				(*vira)++;
				break;
			
			case XI_RES_PATCH:
				added = xAddNameNode( veclist, "    %5ld (-$%04lx)  $%08lx  %-32s", o, -o, temp, XInfo->xi_Name );
				
				(*patches)++;
				break;
			}
		}
	else
		{
		added = xAddNameNode( veclist, "  * %5ld (-$%04lx)  $%08lx  %-32s", o, -o, temp, GCS( MSG_MEM_UNKNOWNPROGRAM ) );
		
		(*unknown)++;
		}
	
	return( added );
}

BOOL xPrepareLVOHeader( struct List *veclist, struct Library *lib )
{
	if( xAddNameNode( veclist, " %s: (%ld.%ld)", lib->lib_Node.ln_Name, lib->lib_Version, lib->lib_Revision ) )
		{
		if( xAddNameNode( veclist, "" ) )
			{
			if( xAddNameNode( veclist, GCS( MSG_MEM_LVOFORMAT ) ) )
				{
				if( xAddNameNode( veclist, "" ) )
					return( TRUE );
				}
			}
		}
	
	return( FALSE );
}
