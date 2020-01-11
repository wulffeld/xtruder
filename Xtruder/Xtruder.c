/*
**	Start: (19.9.93)
**	$VER: Xtruder.c 3.8 (29.8.97)
**
**	Xtruder main()
**
**	Copyright (C) 1994-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


long __stack = 16000;

static const char xtruder_version_string[] = "$VER: Xtruder 3.7 (28.8.97)";

VOID main( WORD argc, UBYTE *argv[] )
{
	struct XtruderArgs *	xtargs = NULL;
	struct MsgPort *		xtruderport;
	
	
		/// Check if we really have kickstart 2.04
	
	if( DOSBase->lib_Version < 37 )
		{
		Write( Output(), "Xtruder requires at least Kickstart 2.04!\n", strlen( "Xtruder requires at least Kickstart 2.04!\n" ) );
		
		exit( RETURN_FAIL );
		}
	
		/// For the hotkey process
	
	Process = (struct Process *)FindTask( NULL );
	
		/// Is another copy of Xtruder already open?
	
	if( xtruderport = FindPort( "XTRUDER.1" ) )
		{
		SecondCopy = TRUE;
		
			/// Notify first Xtruder?
		
		if( ReqToolsBase = (struct Library *)OpenLibrary( "reqtools.library", 38 ) )
			{
			if( rtEZRequestTags( GCS( MSG_XTRUDERALREADYRUNNING ), GCS( MSG_GF_YESNO ), NULL, NULL, RT_Underscore,	'_', RTEZ_ReqTitle,	GCS( MSG_REQ_INFO ), TAG_END ) == 1 )
				{
				Forbid();
				
				if( xtruderport = FindPort( "XTRUDER.1" ) )
					Signal( xtruderport->mp_SigTask, SIGBREAKF_CTRL_E );
				
				Permit();
				}
			
			CloseLibrary( (struct Library *)ReqToolsBase );
			ReqToolsBase = NULL;
			}
		
			/// Open required resources
		
		xOpenSmallSystem();
		}
	else
		{
			/// Create a port QUICKLY in case 2 Xtruder's have been run...
		
		if( XtruderCPort = CreateMsgPort() )
			{
			XtruderCPort->mp_Node.ln_Name = "XTRUDER.1";
			XtruderCPort->mp_Node.ln_Pri = 0;
			
			AddPort( XtruderCPort );
			
			XPortSignal = 1 << XtruderCPort->mp_SigBit;
			
				/// Get resources
			
			xOpenFullSystem();
			}
		else
			exit( RETURN_FAIL );
		}
	
		/// Show GUI if no port found otherwise close down
	
	if( !xtruderport )
		{
			/// Check if there were any CLI arguments
		
		xtargs = xGetCLIArguments( xtruderport );
		
		if( XSettings->xs_MIS_CxPopup )
			xPopUpGUI();
		}
	else
		xCloseSystem( 0, 0, 0 );
	
		/// Now the GUI have had a chance to appear so let's handle the CLI arguments
	
	if( xtargs )
		{
		xHandleCLIArguments( xtargs, xtruderport );
		
		xFreeCLIArguments( xtargs );
		}
	
		/// Check for things we need to do at startup. Execute rexx script?
	
	if( XSettings->xs_STA_ExecuteScript && XSettings->xs_STA_StartupScript[0] )
		xSendARexxMsg( RexxStuff, XSettings->xs_STA_StartupScript, FALSE );
	
		/// Check prespecified?
	
	if( XSettings->xs_STA_CheckPrespecified )
		xStartCheck( NULL, CHECKTYPE_PRESPECIFIED, TRUE );
	
		/// Check exec vectors - NOT IMPLEMENTED
	
	if( XSettings->xs_STA_CheckVectors )
		;
	
		/// If we were run from an icon we will have to parse possible icon
		/// arguments...
	
	if( !argc )
		xHandleWBArguments( (struct WBStartup *)argv, xtruderport );
	
		/// System information
							
	if( XSettings->xs_STA_ShowSystemInformation && Window )
		xOpenSysInfoWindow();
	
	xWaitMainWin();
}

VOID xWaitMainWin( VOID )
{
	struct IntuiMessage *	msg;
	struct XtruderMsg *		xmsg;
	ULONG					mpsignal;
	ULONG					class;
	UWORD					code;
	UBYTE					help = 0;
	
	
	while( 1 )
		{
		mpsignal = Wait( WindowSignal | XPortSignal | ARexxSignal | AppIconSignal | AppWinSignal | SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_F );
		
			/// Hide GUI
		
		if( mpsignal & SIGBREAKF_CTRL_D )
			xCloseInterface();
		
			/// Show GUI
		
		if( mpsignal & SIGBREAKF_CTRL_E )
			xPopUpGUI();
		
			/// GUI was closed by hotkey process
		
		if( mpsignal & SIGBREAKF_CTRL_F )
			;
		
			/// Message to main window
		
		if( (mpsignal & WindowSignal) && Window )
			{
			while( (Window) && (msg = GT_GetIMsg( Window->UserPort )) )
				{
				class = msg->Class;
				code = msg->Code;
				
				if( (class == IDCMP_GADGETUP) || (class == IDCMP_GADGETDOWN) )
					code = ((struct Gadget *)msg->IAddress)->GadgetID;
				
				GT_ReplyIMsg( msg );
				
				if( help )
					help--;
				
				switch( class )
					{
					case IDCMP_REFRESHWINDOW:
						GT_BeginRefresh( Window );
						GT_EndRefresh( Window, TRUE );
						break;
					
					case IDCMP_DISKINSERTED:
						xHandleInsertedDisk();
						
						xFindPresentDisks();
						break;
					
					case IDCMP_DISKREMOVED:
						xFindPresentDisks();
						break;
					
					case IDCMP_CLOSEWINDOW:
						xCloseInterface();
						break;
					
					case IDCMP_RAWKEY:
						if( code == KEY_HELP && !help )
							xGuideDisplay( AGH_MAIN );
						break;
					
					case IDCMP_MENUHELP:
						xMenuHelp( code );
						
						help = 2;
						break;
					
					case IDCMP_MENUPICK:
						xHandleMainWinMenuIDCMP( code );
						break;
					}
				}
			}
		
		if( mpsignal & XPortSignal )
			{
			if( xmsg = (struct XtruderMsg *)GetMsg( XtruderCPort ) )
				{
				switch( xmsg->xt_Command )
					{
					case XTCMD_KILL:
						ReplyMsg( (struct Message *)xmsg );
						
						xCloseSystem( 0, 0, QUIT_ASK_SETTINGS );
						break;
					}
				
				ReplyMsg( (struct Message *)xmsg );
				}
			}
		
		if( mpsignal & ARexxSignal )
			xHandleARexxMsg();
		
		if( mpsignal & AppIconSignal )
			xHandleIconMsg();
		
		if( mpsignal & AppWinSignal )
			xHandleAppWindowMessages();
		}
}
