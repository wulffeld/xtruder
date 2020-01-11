/*
**	$VER: XtruderAmigaGuide.c (21.12.96)
**
**	Xtruder AmigaGuide functions
**
**	Copyright (C) 1995-1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


UBYTE	GuideFileName[ DEFAULTBUFFERLEN ];

/*
 * Calculate the node from the menu code given and show/launch the guide.
 */
VOID xMenuHelp( UWORD code )
{
	UWORD	menunum = MENUNUM( code );
	
	
	if( menunum == NOMENU )
		xGuideDisplay( AGH_MAIN );
	else
		xGuideDisplay( AGH_MENU_PROJECT + menunum );
}

/*
 * Try to display an AmigaGuide help node.
 */
VOID xGuideDisplay( LONG contextid )
{
	if( xGuideLaunch( contextid ) )
		{
		SetAmigaGuideContextA( AGuide, contextid, NULL );
		
		SendAmigaGuideContextA( AGuide, NULL );
		}
}

/*
 * Launch the AmigaGuide server.
 */
BYTE xGuideLaunch( LONG contextid )
{
	BPTR	lock,oldcd,file;
	
	
	if( Screen && AmigaGuideBase )
		{
			/// Is the help file server already running?
		
		if( !GuideProcess )
			{
				/// Guide must exist! Check if it does.
			
			if( lock = Lock( XSettings->xs_PAT_GuidePath, ACCESS_READ ) )
				{
				oldcd = CurrentDir( lock );
				
				if( file = Open( "Xtruder.guide", MODE_OLDFILE ) )
					{
					Close( file );
					
					NewGuide.nag_Name		= "Xtruder.guide";
					NewGuide.nag_Context	= ContextList;
					NewGuide.nag_Screen		= Screen;
					
						/// Launch the server process and wait for reply.
					
					if( GuideProcess = CreateNewProcTags(
						NP_Entry,	xGuideServer,
						NP_Name,	"Xtruder AmigaGuide Process",
						TAG_DONE ) )
						{
//						SetSignal( 0, SIGF_SINGLE );
						
						Wait( SIGF_SINGLE );
						}
					}
				
				CurrentDir( oldcd );
				
				UnLock( lock );
				}
			}
		
			/// Pop the screen where the guide is to the front
		
		if( GuideProcess )
			{
			ScreenToFront( Screen );
			
			return( TRUE );
			}
		else
			{
			DisplayBeep( Screen );
			
			return( FALSE );
			}
		}
	
	return( FALSE );
}

/*
 * AmigaGuide server, handles all the signal processing.
 */
VOID __saveds xGuideServer( VOID )
{
	struct AmigaGuideMsg *	gmsg;
	ULONG					guidemask,signals;
	BYTE					close = FALSE;
	
	
		/// Open the help text file asynchronously...
	
	if( AGuide = OpenAmigaGuideAsync( &NewGuide, TAG_DONE ) )
		{
			/// Pick up the signal notification mask
		
		guidemask = AmigaGuideSignal( AGuide );
		
			/// Clear the AmigaGuide context
		
		SetAmigaGuideContextA( AGuide, 0, NULL );
		
			/// Flag the main process to continue.
		
		Signal( (struct Task *)Process, SIGF_SINGLE );
		
		while( !close )
			{
			signals = Wait( SIGBREAKF_CTRL_C | guidemask );
			
			if( signals & SIGBREAKF_CTRL_C )
				close = TRUE;
			
			if( signals & guidemask )
				{
				while( gmsg = GetAmigaGuideMsg( AGuide ) )
					{
					switch( gmsg->agm_Type )
						{
						case ActiveToolID:
								/// AmigaGuide is ready for us
								
								/// Show the first node. We've already called SetAmigaGuideContextA()
								/// with the proper node to show.
							
							SendAmigaGuideContextA( AGuide, NULL );
							break;
		
						case ShutdownMsgID:
							close = TRUE;
							break;
						}
					
					if( gmsg->agm_Pri_Ret )
						close = TRUE;

					ReplyAmigaGuideMsg( gmsg );
					}
				}
			}
		
		CloseAmigaGuide( AGuide );
		}
	
	AGuide = NULL;
	GuideProcess = NULL;
	
	Signal( (struct Task *)Process, SIGF_SINGLE );
}

/*
 * Terminate the AmigaGuide server and free the
 * associated resources.
 */
VOID xGuideCleanup( VOID )
{
	Forbid();
	
	if( GuideProcess )
		{
		Signal( (struct Task *)GuideProcess, SIGBREAKF_CTRL_C );
		
//		SetSignal( 0, SIGF_SINGLE );
		
		Wait( SIGF_SINGLE );
		}
	
	Permit();
}
