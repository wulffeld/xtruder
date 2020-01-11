/*
**	$VER: XtruderIcon.c (24.2.95)
**
**	Xtruder icon functions
**
**	Copyright (C) 1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Handle possible WB icons
 *
 * strbuf1 is the taskname
 * strbuf2 is the path of our process
 * strbuf3 is the file we will load
 */
VOID xHandleWBArguments( struct WBStartup *argmsg, struct MsgPort *xtruderport )
{
	struct WBArg *		wb_arg = argmsg->sm_ArgList;
	struct Process *	pc;
	struct List			iconlist;
	ULONG				a;
	UBYTE				strbuf1[ DEFAULTBUFFERLEN ];
	UBYTE				strbuf2[ DEFAULTBUFFERLEN ];
	UBYTE				strbuf3[ DEFAULTBUFFERLEN ];
	
	
	NewList( &iconlist );
	
		/// Let's find the name of this program so we don't check ourselves :)
	
	if( pc = (struct Process *)FindTask( NULL ) )
		{
		if( strncpy( strbuf1, pc->pr_Task.tc_Node.ln_Name, DEFAULTBUFFERLEN ) )
			{
				/// Get path of our process
			
			if( NameFromLock( pc->pr_CurrentDir, strbuf2, DEFAULTBUFFERLEN ) )
				{
					/// Add the task name
				
				if( AddPart( strbuf2, FilePart( strbuf1 ), DEFAULTBUFFERLEN ) )
					{
						/// Step through WB arguments...
					
					for( a = 0; a < argmsg->sm_NumArgs; a++, wb_arg++ )
						{
						if( wb_arg->wa_Lock )
							{
							BPTR	olddir;
							
							
							olddir = CurrentDir( wb_arg->wa_Lock );
							
								/// Retrieve the path of the file
							
							if( NameFromLock( wb_arg->wa_Lock, strbuf3, DEFAULTBUFFERLEN ) )
								{
									/// Add the filename
								
								if( AddPart( strbuf3, wb_arg->wa_Name, DEFAULTBUFFERLEN ) )
									{
									BPTR	filelock;
									
									
									if( filelock = Lock( strbuf1, ACCESS_READ ) )
										{
										if( NameFromLock( filelock, strbuf1, DEFAULTBUFFERLEN ) )
											{
												/// Comp. with task name
											
											if( stricmp( strbuf2, strbuf3 ) )
												xAddNameNode( &iconlist, strbuf3 );
											}
										UnLock( filelock );
										}
									}
								}
							
							CurrentDir( olddir );
							}
						}
					
					if( !IsListEmpty( &iconlist ) )
						{
						xStartCheck( &iconlist, CHECKTYPE_ICONS, TRUE );
						
						xFreeNameNodes( &iconlist );
						}
					}
				}
			}
		}
}

VOID xHandleAppWindowMessages( VOID )
{
	struct AppMessage *	amsg;
	struct WBArg *		argptr;
	struct List			iconlist;
	WORD				i;
	UBYTE				strbuf[ DEFAULTBUFFERLEN ];
	
	
	
	NewList( &iconlist );
	
	while( amsg = (struct AppMessage *)GetMsg( AppWinPort ) )
		{
		//Printf( "AppMsg: Type=%ld, ID=%ld, NumArgs=%ld\n", amsg->am_Type, amsg->am_ID, amsg->am_NumArgs );
		
		argptr = amsg->am_ArgList;
		
		for( i = 0; i < amsg->am_NumArgs; i++ )
			{
			if( NameFromLock( argptr->wa_Lock, strbuf, DEFAULTBUFFERLEN ) )
				{
				AddPart( strbuf, argptr->wa_Name, DEFAULTBUFFERLEN );
				
				xAddNameNode( &iconlist, strbuf );
				}
			
			//Printf("   arg(%ld): Name='%s', Lock=%lx\n", i, argptr->wa_Name, argptr->wa_Lock );
			
			argptr++;
			}
		
		ReplyMsg( (struct Message *)amsg );
		
		xStartCheck( &iconlist, CHECKTYPE_ICONS, TRUE );
		}
}

/*
 * Create application icon...
 */
VOID xCreateAppIcon( VOID )
{
	if( !AppIconDiskObject )
		{
			/// If WB is not open we can't create an icon
		
		if( !(AppIconDiskObject = GetDiskObject( "ENV:Xtruder/Xtruder_APPICON" )) )
			if( !(AppIconDiskObject = GetDiskObject( "PROGDIR:Xtruder_APPICON" ) ) )
				AppIconDiskObject = GetDefDiskObject( WBTOOL );
		
		if( AppIconDiskObject )
			{
				/// The type must be set to NULL for a WBAPPICON
			
			AppIconDiskObject->do_Type = NULL;
			
			if( AppIconPort = CreateMsgPort() )
				{
				if( AppIcon = AddAppIconA( 0, 0, "Xtruder", AppIconPort, NULL, AppIconDiskObject, NULL ) )
					AppIconSignal = 1 << AppIconPort->mp_SigBit;
				}
			}
		}
}

/*
 * Remove application icon...
 */
VOID xRemoveAppIcon( VOID )
{
	struct Message *	msg;
	
	
	if( AppIcon )
		{
		RemoveAppIcon( AppIcon );
		
		AppIcon = NULL;
		}
	
	if( AppIconPort )
		{
		while( msg = GetMsg( AppIconPort ) )
			ReplyMsg( msg );
		
		DeleteMsgPort( AppIconPort );
		
		AppIconPort = NULL;
		
		AppIconSignal = 0;
		}
	
	if( AppIconDiskObject )
		{
		FreeDiskObject( AppIconDiskObject );
		
		AppIconDiskObject = NULL;
		}
}

VOID xHandleIconMsg( VOID )
{
	struct AppMessage *	amsg;
	BPTR				olddir;
	UWORD				x;
	UBYTE				strbuf[ DEFAULTBUFFERLEN ];
	
	
	while( amsg = (struct AppMessage *)GetMsg( AppIconPort ) )
		{
		if( amsg->am_NumArgs == 0 )
			{
				/// If NumArgs is 0 the AppIcon was activated directly
				/// Ie doubleclicked on!
			
			xPopUpGUI();
			}
		else
		if( amsg->am_NumArgs > 0 )
			{
			struct List	iconlist;
			
			
			NewList( &iconlist );
			
				/// If NumArgs is > 0 the icon was activated by
				/// having one or more icons dropped on top of it
			
			for( x = 0; x < amsg->am_NumArgs; x++ )
				{
				if( amsg->am_ArgList[x].wa_Lock != NULL )
					{
					olddir = CurrentDir( amsg->am_ArgList[x].wa_Lock );
					
						/// Retrieve the path of the file
					
					if( NameFromLock( amsg->am_ArgList[x].wa_Lock, strbuf, DEFAULTBUFFERLEN ) )
						{
						if( AddPart( strbuf, amsg->am_ArgList[x].wa_Name, DEFAULTBUFFERLEN ) )
							xAddNameNode( &iconlist, strbuf );
						}
					
					CurrentDir( olddir );
					}
				}
			
			if( !IsListEmpty( &iconlist ) )
				{
				xStartCheck( &iconlist, CHECKTYPE_ICONS, TRUE );
				
				xFreeNameNodes( &iconlist );
				}
			}
		
		ReplyMsg( (struct Message *)amsg );
		}
}
