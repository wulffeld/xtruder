/*
**	$VER: Xt_ARexx.c (29.10.95)
**
**	Xtruder ARexx functions
**
**	Copyright (C) 1994-1995 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * This routine initializes an ARexx port for your process
 * This should only be done once per process.  You must call it
 * with a valid application name and you must use the handle it
 * returns in all other calls...
 *
 * NOTE:  The AppName should not have spaces in it...
 *        Example AppNames:  "MyWord" or "FastCalc" etc...
 *        The name *MUST* be less that 16 characters...
 *        If it is not, it will be trimmed...
 *        The name will also be UPPER-CASED...
 *
 * NOTE:  The Default file name extension, if NULL will be
 *        "rexx"  (the "." is automatic)
 */
struct ARexxContext *xInitARexx( STRPTR appname, STRPTR extension )
{
	struct ARexxContext *	rexxcontext;
	WORD					loop;
	STRPTR					tmp;
	
	
	if( rexxcontext = AllocVec( sizeof(struct ARexxContext), MEMF_PUBLIC | MEMF_CLEAR ) )
		{
		if( rexxcontext->RexxSysBase = OpenLibrary( "rexxsyslib.library", 0 ) )
			{
				/// Set up the extension...
			
			if( !extension )
				extension = "rexx";
			
			tmp = rexxcontext->Extension;
			
			for( loop = 0; (loop < 7) && (extension[loop]); loop++ )
				*tmp++ = extension[loop];
			
			*tmp = '\0';
			
				/// Set up a port name...
			
			tmp = rexxcontext->PortName;
			
			for( loop = 0; (loop < 16) && (appname[loop]); loop++ )
				*tmp++ = ToUpper( appname[loop] );
			
			*tmp = '\0';
			
				/// Set up the last error RVI name...
				///
				/// This is <appname>.LASTERROR
			
			strncpy( rexxcontext->ErrorName, rexxcontext->PortName, 28 );
			strncat( rexxcontext->ErrorName, ".LASTERROR", 28 );
			
				/// We need to make a unique port name...
			
			Forbid();
			
			if( !(FindPort( rexxcontext->PortName )) )
				rexxcontext->ARexxPort = CreatePort( rexxcontext->PortName, NULL );
			
			Permit();
			}
		
		if( (!( rexxcontext->RexxSysBase ) ) || ( !(rexxcontext->ARexxPort) ) )
			{
			xFreeARexx( rexxcontext );
			
			rexxcontext = NULL;
			}
		}
	
	ARexxSignal = xARexxSignal( RexxStuff );
	
	return( rexxcontext );
}

/*
 * This function closes down the ARexx context that was opened
 * with xInitARexx...
 */
VOID xFreeARexx( struct ARexxContext *rexxcontext )
{
	struct RexxMsg *	rmsg;
	
	
	if( rexxcontext )
		{
			/// Clear port name so it can't be found...
		
		rexxcontext->PortName[0] = '\0';
		
			/// Clean out any outstanding messages we had sent out...
		
		while( rexxcontext->Outstanding )
			{
			WaitPort( rexxcontext->ARexxPort );
			
			while( rmsg = xGetARexxMsg( rexxcontext ) )
				{
				if( rmsg != REXX_RETURN_ERROR )
					{
						/// Any messages that come now are blown away...
					
					xSetARexxLastError( rexxcontext, rmsg, "PORT CLOSED" );
					
					xReplyARexxMsg( rexxcontext, rmsg, "", 100 );
					}
				}
			}
		
			/// Clean up the port and delete it...
		
		if( rexxcontext->ARexxPort )
			{
			while( rmsg = xGetARexxMsg( rexxcontext ) )
				{
					/// Any messages that still are coming in are
					/// "dead"  We just set the LASTERROR and
					/// reply an error of 100...
				
				xSetARexxLastError( rexxcontext, rmsg, "PORT CLOSED" );
				
				xReplyARexxMsg( rexxcontext, rmsg, "", 100 );
				}
			
			DeletePort( rexxcontext->ARexxPort );
			}
		
			/// Make sure we close the library...
		
		if( rexxcontext->RexxSysBase )
			CloseLibrary( rexxcontext->RexxSysBase );
		
			/// Free the memory of the rexxcontext
		
		FreeVec( rexxcontext );
		}
}

STRPTR xARexxName( struct ARexxContext *rexxcontext )
{
	if( rexxcontext )
		return( rexxcontext->PortName );
	
	return( NULL );
}

/*
 * This function returns the signal mask that the Rexx port is
 * using.  It returns NULL if there is no signal...
 *
 * Use this signal bit in your Wait() loop...
 */
ULONG xARexxSignal( struct ARexxContext *rexxcontext )
{
	if( rexxcontext )
		return( (ULONG)1 << (rexxcontext->ARexxPort->mp_SigBit) );
	
	return( 0 );
}

/*
 * This function will send a string to ARexx...
 * The default host port will be that of your task...
 * If you set StringFile to TRUE, it will set that bit for the message...
 * Returns TRUE if it send the message, FALSE if it did not...
 */
WORD xSendARexxMsg( struct ARexxContext *rexxcontext, STRPTR rstring, WORD stringfile )
{
	struct MsgPort *	rexxport;
	struct RexxMsg *	rmsg;
	WORD				flag = FALSE;
	
	
	if( rexxcontext )
		{
		if( rstring )
			{
			if( rmsg = CreateRexxMsg( rexxcontext->ARexxPort, rexxcontext->Extension, rexxcontext->PortName ) )
				{
				rmsg->rm_Action = RXCOMM | (stringfile ? (1L << RXFB_STRING) : 0 );
				
				if( rmsg->rm_Args[0] = CreateArgstring( rstring, strlen( rstring ) ) )
					{
						/// We need to find the RexxPort and this needs
						/// to be done in a Forbid()
					
					Forbid();
					
					if( rexxport = FindPort( RXSDIR ) )
						{
							/// We found the port, so put the message to
							/// ARexx...
						
						PutMsg( rexxport, (struct Message *)rmsg );
						
						rexxcontext->Outstanding += 1;
						
						flag = TRUE;
						}
					else
						{
							/// No port, so clean up...
						
						DeleteArgstring( rmsg->rm_Args[0] );
						DeleteRexxMsg( rmsg );
						}
					
					Permit();
					}
				else
					DeleteRexxMsg( rmsg );
				}
			}
		}
	
	return( flag );
}

/*
 * This function returns a structure that contains the commands sent from
 * ARexx...  You will need to parse it and return the structure back
 * so that the memory can be freed...
 *
 * This returns NULL if there was no message...
 */
struct RexxMsg *xGetARexxMsg( struct ARexxContext *rexxcontext )
{
	struct RexxMsg *	tmp = NULL;
	WORD				flag;
	
	
	if( rexxcontext )
		{
		if( tmp = (struct RexxMsg *)GetMsg( rexxcontext->ARexxPort ) )
			{
			if( tmp->rm_Node.mn_Node.ln_Type == NT_REPLYMSG )
				{
					/// If we had sent a command, it would come this way...
					///
					/// Since we don't in this simple example, we just throw
					/// away anything that looks "strange"
				
				flag = FALSE;
				
				if( tmp->rm_Result1 )
					flag = TRUE;
				
					/// Free the arguments and the message...
				
				DeleteArgstring( tmp->rm_Args[0] );
				
				DeleteRexxMsg( tmp );
				
				rexxcontext->Outstanding -= 1;
				
					/// Return the error if there was one...
				
				tmp = flag ? REXX_RETURN_ERROR : NULL;
				}
			}
		}
	
	return( tmp );
}

/*
 * Use this to return a ARexx message...
 *
 * If you wish to return something, it must be in the RString.
 * If you wish to return an Error, it must be in the Error.
 * If there is an error, the RString is ignored.
 */
VOID xReplyARexxMsg( struct ARexxContext *rexxcontext, struct RexxMsg *rmsg, STRPTR rstring, LONG error )
{
	if( rexxcontext )
		{
		if( rmsg )
			{
			if( rmsg != REXX_RETURN_ERROR )
				{
				rmsg->rm_Result2 = 0;
				
				if( !(rmsg->rm_Result1 = error) )
					{
						/// If you did not have an error we return the string
					
					if( rmsg->rm_Action & (1L << RXFB_RESULT))
						if( rstring )
							rmsg->rm_Result2 = (LONG)CreateArgstring( rstring, strlen( rstring ) );
					}
				
					/// Reply the message to ARexx...
				
				ReplyMsg( (struct Message *)rmsg );
				}
			}
		}
}

/*
 * This function will set an error string for the ARexx
 * application in the variable defined as <appname>.LASTERROR
 *
 * Note that this can only happen if there is an ARexx message...
 *
 * This returns TRUE if it worked, FALSE if it did not...
 */
WORD xSetARexxLastError( struct ARexxContext *rexxcontext, struct RexxMsg *rmsg, STRPTR errorstring )
{
	WORD	okflag = FALSE;
	
	
	if( rexxcontext )
		{
		if( rmsg )
			{
			if( CheckRexxMsg( rmsg ) )
				{
					/// Note that SetRexxVar() has more than just a TRUE/FALSE
					/// return code, but for this "basic" case, we just care if
					/// it works or not.
				
				if( !SetRexxVar( rmsg, rexxcontext->ErrorName, errorstring, strlen( errorstring ) ) )
					okflag = TRUE;
				}
			}
		}
	
	return( okflag );
}
