/*
**	$VER: Xt_Commodities.c (7.4.97)
**
**	Xtruder commodities functions
**
**	Copyright (C) 1995-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


BYTE	LocalCxSignal = -1;

static VOID __saveds xCxServer( VOID )
{
	struct NewBroker	newbroker =
	{
		NB_VERSION,
		"Xtruder",
		"Copyright © 1994-1997 Martin Wulffeld",
		"The killer in me is the killer in you",
		NBU_UNIQUE | NBU_NOTIFY,
		COF_SHOW_HIDE,
		0,
		0,
		0
	};
	struct MsgPort *	brokerport;
	CxObj *				broker;
	CxObj *				filter;
	CxMsg *				cmsg;
	ULONG				msgid,msgtype;
	ULONG				sigs;
	ULONG				cxsignal;
	BOOL				done = FALSE;
	
	
	if( brokerport = CreateMsgPort() )
		{
		brokerport->mp_Node.ln_Name = newbroker.nb_Name;
		
		newbroker.nb_Port = brokerport;
		newbroker.nb_Pri = XSettings->xs_MIS_CxPriority;
		
		cxsignal = 1 << brokerport->mp_SigBit;
		
		if( broker = CxBroker( &newbroker, NULL ) )
			{
			if( filter = HotKey( XSettings->xs_MIS_CxPopkey, brokerport, 1 ) )
				{
				AttachCxObj( broker, filter );
				
				if( !CxObjError( filter ) )
					{
					ActivateCxObj( broker, 1 );
					
						/// Everything is okay so tell the main process that...
					
					Signal( (struct Task *)Process, 1L << LocalCxSignal );
					
					while( !done )
						{
						sigs = Wait( SIGBREAKF_CTRL_E | cxsignal );
						
						if( sigs & cxsignal )
							{
							while( cmsg = (CxMsg *)GetMsg( brokerport ) )
								{
								msgid = CxMsgID( cmsg );
								msgtype = CxMsgType( cmsg );
								
								ReplyMsg( (struct Message *)cmsg );
								
								switch( msgtype )
									{
									case CXM_IEVENT:
										switch( msgid )
											{
											case 1:
												ObtainSemaphore( &GUISemaphore );
												
												if( Screen )
													{
													ScreenToFront( Screen );
													
													if( OnPublic )
														WindowToFront( Window );
													
													ActivateWindow( Window );
													}
												else
													Signal( (struct Task *)Process, SIGBREAKF_CTRL_E );
												
												ReleaseSemaphore( &GUISemaphore );
												break;
											}
										break;
									
									case CXM_COMMAND:
										switch( msgid )
											{
											case CXCMD_DISABLE:
												ActivateCxObj( broker, 0 );
												break;
											
											case CXCMD_ENABLE:
												ActivateCxObj( broker, 1 );
												break;
											
											case CXCMD_KILL:
												xSendXtruderMessage( XTCMD_KILL, NULL );
												break;
											
											case CXCMD_UNIQUE:
											case CXCMD_APPEAR:
												ObtainSemaphore( &GUISemaphore );
												
												if( Screen )
													{
													ScreenToFront( Screen );
													
													if( OnPublic )
														WindowToFront( Window );
													
													ActivateWindow( Window );
													}
												else
													Signal( (struct Task *)Process, SIGBREAKF_CTRL_E );
												
												ReleaseSemaphore( &GUISemaphore );
												break;
											
											case CXCMD_DISAPPEAR:
												Signal( (struct Task *)Process, SIGBREAKF_CTRL_D );
												break;
											}
										break;
									}
								}
							}
						
							/// Kill signal received from main Xtruder = CloseSystem
						
						if( sigs & SIGBREAKF_CTRL_E )
							done = TRUE;
						}
					}
				}
			DeleteCxObjAll( broker );
			}
		
		while( cmsg = (CxMsg *)GetMsg( brokerport ) )
			ReplyMsg( (struct Message *)cmsg );
		
		DeleteMsgPort( brokerport );
		}
	
	Forbid();
	CommodityProcess = NULL;
	Permit();
	
	Signal( (struct Task *)Process, 1L << LocalCxSignal );
}

/*
 * Create a sub-process which handles the hotkeys
 */
BOOL xCreateCommodity( VOID )
{
	if( XSettings->xs_MIS_CxPopkey[0] )
		{
		if( -1 != (LocalCxSignal = AllocSignal( -1 )) )
			{
			//SetSignal( 0, 1L << LocalCxSignal );
			
			if( CommodityProcess = CreateNewProcTags(
				NP_Entry,		xCxServer,
				NP_Name,		"Xtruder Hotkey Process",
				NP_Priority,	0,
				NP_StackSize,	8192,
				NP_WindowPtr,	-1,
				TAG_DONE ) )
				{
				Wait( 1L << LocalCxSignal );
				
				FreeSignal( LocalCxSignal );
				
				return( TRUE );
				}
			else
				FreeSignal( LocalCxSignal );
			}
		
		xReqMsg( Window, GCS( MSG_REQ_ERROR ), GCS( MSG_COULDNOTCREATECOMMODITY ), GCS( MSG_GF_OKAY ) );
		}
	
	return( FALSE );
}

VOID xRemoveCommodity( VOID )
{
	Forbid();
	
	if( CommodityProcess )
		{
		Signal( (struct Task *)CommodityProcess, SIGBREAKF_CTRL_E );
		
		Wait( 1L << LocalCxSignal );
		
		CommodityProcess = NULL;
		
		FreeSignal( LocalCxSignal );
		}
	
	Permit();
}
