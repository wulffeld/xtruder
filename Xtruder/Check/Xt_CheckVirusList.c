/*
**	$VER: XtruderCheckVirusList.c (12.2.96)
**
**	Xtruder check virus list module
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Add virus information node to the VirusList
 */
VOID xCHK_AddVirusToVirusList( struct CurrentFile *cf, struct CurrentFile *archivecf )
{
	struct VirusFoundNode *	vnode;
	
	
	if( vnode = AllocVec( sizeof( struct VirusFoundNode ), MEMF_CLEAR ) )
		{
			/// If cf is non-NULL it's a virus we must record. Otherwise it's a blank node
		
		if( cf )
			{
			BPTR	lock;
			BOOL	gotname = FALSE;
			
			
			strcpy( vnode->vn_VirusName, archivecf ? archivecf->cf_VirusName : cf->cf_VirusName );
			
			if( lock = Lock( cf->cf_FileName, ACCESS_READ ) )
				{
				gotname = NameFromLock( lock, vnode->vn_FileName, 256 );
				
				UnLock( lock );
				}
			
			if( !gotname )
				{
				strcpy( vnode->vn_FileName, cf->cf_PathName );
				AddPart( vnode->vn_FileName, FilePart( cf->cf_FileName ), 255 );
				}
			}
		
		AddTail( VirusList, (struct Node *)vnode );
		}
}

/*
 * Removes all nodes from the VirusList
 */
VOID xCHK_FlushVirusList( VOID )
{
	if( VirusList && !IsListEmpty( VirusList ) )
		{
		struct VirusFoundNode *	worknode;
		struct VirusFoundNode *	nextnode;
		
		
		worknode = (struct VirusFoundNode *)VirusList->lh_Head;
		
		while( nextnode = (struct VirusFoundNode *)worknode->vn_Node.ln_Succ )
			{
			FreeVec( worknode );
			
			worknode = nextnode;
			}
		
		NewList( VirusList );
		}
}

/*
 * Counts and returns the number of vira found in the last check or all checks.
 */
ULONG xCHK_CountNumberViraFound( BOOL all )
{
	struct VirusFoundNode *	vnode;
	ULONG					vira = 0,last = 0;
	
	
	for( vnode = (struct VirusFoundNode *)VirusList->lh_Head;
		 vnode->vn_Node.ln_Succ;
		 vnode = (struct VirusFoundNode *)vnode->vn_Node.ln_Succ )
		{
		if( vnode->vn_FileName[0] )
			{
			last++;
			vira++;
			}
		else
			last = 0;
		}
	
	return( all ? vira : last );
}

/*
 * Return either the virusname (if virusname is TRUE) or the virus filename
 */
STRPTR xCHK_GetVirusName( ULONG number, BOOL virusname )
{
	struct VirusFoundNode *	vnode;
	STRPTR					name = "";
	ULONG					numvira = xCHK_CountNumberViraFound( TRUE );
	
	
	number = numvira - number;
	
	if( VirusList && !IsListEmpty( VirusList ) )
		{
		for( vnode = (struct VirusFoundNode *)VirusList->lh_Head;
			 vnode->vn_Node.ln_Succ;
			 vnode = (struct VirusFoundNode *)vnode->vn_Node.ln_Succ )
			{
			if( vnode->vn_FileName[0] )
				{
				if( !number )
					{
					if( virusname )
						name = vnode->vn_VirusName;
					else
						name = vnode->vn_FileName;
					
					break;
					}
				
				number--;
				}
			}
		}
	
	return( name );
}
