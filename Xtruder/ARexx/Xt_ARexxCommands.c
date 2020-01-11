/*
**	$VER: Xt_ARexxCommands.c (10.4.97)
**
**	Xtruder ARexx commands
**
**	Copyright (C) 1994-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		AREXXCMD_SHOW							0
#define		AREXXCMD_HIDE							1
#define		AREXXCMD_GETFILE						2
#define		AREXXCMD_GETPATH						3
#define		AREXXCMD_REQUESTUSER					4
#define		AREXXCMD_REQUESTSTRING					5
#define		AREXXCMD_REQUESTNUMBER					6
#define		AREXXCMD_CHECKPRESPECIFIED				7
#define		AREXXCMD_CHECKSPECIFIED					8
#define		AREXXCMD_CHECKFILES						9
#define		AREXXCMD_CHECKVECTORS					10
#define		AREXXCMD_NUMBERVIRUSESFOUND				11
#define		AREXXCMD_GETVIRUSNAME					12
#define		AREXXCMD_GETVIRUSFILENAME				13
#define		AREXXCMD_FLUSHVIRUSFOUNDLIST			14


VOID xHandleARexxMsg( VOID )
{
	struct RexxMsg *	rmsg;
	struct ARexxCommand	arexxcmds[] =
	{
		{"SHOW",				""},
		{"HIDE",				""},
		
		{"GETFILE",				"T=TITLE/K,P=PATH/K,F=FILE/K"},
		{"GETPATH",				"T=TITLE/K,P=PATH/K"},
		
		{"REQUESTUSER",			"T=TITLE/K,P=PROMPT/K/A,R=RESPONSE/K"},
		{"REQUESTSTRING",		"T=TITLE/K,P=PROMPT/K,S=STRING/K,M=MAXBUFLEN/N,A=ALLOWEMPTY/S,R=RESPONSE/K"},
		{"REQUESTNUMBER",		"T=TITLE/K,P=PROMPT/K,M=MIN/N,X=MAX/N,N=NUMBER/N,S=SHOWDEFAULT/S,R=RESPONSE/K"},
		
		{"CHECKPRESPECIFIED",	""},
		{"CHECKSPECIFIED",		""},
		{"CHECKFILES",			"F=FILE/M/A"},
		{"CHECKVECTORS",		""},
		
		{"NUMBERVIRUSESFOUND",	"A=ALL/S"},
		{"GETVIRUSNAME",		"N=NUMBER/N/A"},
		{"GETVIRUSFILENAME",	"N=NUMBER/N/A"},
		{"FLUSHVIRUSFOUNDLIST",	""},
		
			/// End marker
		
		{"",					""},
	};
	
	
	while( rmsg = xGetARexxMsg( RexxStuff ) )
		{
		struct ARexxResults *	ares;
		struct RDArgs *			ra;
		struct RDArgs *			rafill;
		STRPTR					error = NULL;
		STRPTR					result = NULL;
		LONG					errlevel = 20;
		LONG *					args;
		STRPTR					rexxstring;
		UWORD					rexxstringlength = strlen( rmsg->rm_Args[0] );
		UBYTE					a;
		UBYTE					template[256];
		
		
			/// Plus 2 cuz no need to '\0' at end
		
		if( rexxstring = AllocVec( rexxstringlength + 2, MEMF_CLEAR ) )
			{
			strcpy( rexxstring, rmsg->rm_Args[0] );
			
			rexxstring[ rexxstringlength ] = '\n';
			
			if( args = AllocVec( 100 * 4, MEMF_CLEAR ) )
				{
				if( rafill = AllocDosObject( DOS_RDARGS, TAG_DONE ) )
					{
					rafill->RDA_Flags |= RDAF_NOPROMPT;
					rafill->RDA_Source.CS_Buffer = rexxstring;
					rafill->RDA_Source.CS_Length = rexxstringlength + 1;
					rafill->RDA_Source.CS_CurChr = 0;
					
					for( a = 0; arexxcmds[a].ac_CommandName[0]; a++ )
						{
						if( !strnicmp( arexxcmds[a].ac_CommandName, rexxstring, strlen( arexxcmds[a].ac_CommandName ) ) )
							{
							xSPrintF( template, "%s/S", arexxcmds[a].ac_CommandName );
							
							if( arexxcmds[a].ac_CommandTemplate[0] )
								{
								strcat( template, "," );
								strcat( template, arexxcmds[a].ac_CommandTemplate );
								}
							
							if( ra = ReadArgs( template, args, rafill ) )
								{
									/// Found the right template...
								
								if( ares = xHandleARexxCommand( args, a ) )
									{
									result = ares->ar_ResultString;
									errlevel = ares->ar_ResultCode;
									
									FreeVec( ares );
									}
								else
									{
										/// FATAL: Not enough memory
									
									errlevel = 20;
									}
								
								FreeArgs( ra );
								
								break;
								}
							}
						}
					
					FreeDosObject( DOS_RDARGS, rafill );
					}
				FreeVec( args );
				}
			FreeVec( rexxstring );
			}
		
		if( error )
			xSetARexxLastError( RexxStuff, rmsg, error );
		
		xReplyARexxMsg( RexxStuff, rmsg, result, errlevel );
		}
}

struct ARexxResults *xHandleARexxCommand( LONG *args, UWORD commandid )
{
	struct ARexxResults *	ares;
	struct List				list;
	BPTR					lock;
	LONG					a = 0;
	ULONG					maxchars;
	LONG					number;
	UBYTE **				files;
	UBYTE					strbuf1[ DEFAULTBUFFERLEN ] = "";
	static UBYTE			resultstring[ DEFAULTBUFFERLEN ];
	
	
	if( ares = AllocVec( sizeof( struct ARexxResults ), MEMF_CLEAR ) )
		{
		ares->ar_ResultCode = 0;
		ares->ar_ResultString = NULL;
		
		switch( commandid )
			{
			case AREXXCMD_GETFILE:
					/// File...
				
				strcpy( strbuf1, args[3] ? (STRPTR)args[3] : RFILE );
				
					/// Request one...
				
				rtChangeReqAttr( FileReq,
					RTFI_Dir, args[2] ? (STRPTR)args[2] : RPATH,
					TAG_END );
				
				if( rtFileRequest( FileReq, strbuf1, args[1] ? (STRPTR)args[1] : (STRPTR)GCS( MSG_TL_SELECTFILE ),
					RT_Window,		Window,
					RT_Underscore,	'_',
					RT_TextAttr,	&RequesterFontAttr,
					RTFI_OkText,	GCS( MSG_GF_SELECT ),
					TAG_END ) )
					{
					strncpy( RPATH, FileReq->Dir, DEFAULTPATHLEN );
					
					strncpy( resultstring, FileReq->Dir, DEFAULTBUFFERLEN );
					AddPart( resultstring, strbuf1, DEFAULTBUFFERLEN );
					
					ares->ar_ResultString = resultstring;
					}
				else
					ares->ar_ResultCode = 5;
				break;
			
			case AREXXCMD_GETPATH:
				rtChangeReqAttr( FileReq,
					RTFI_Dir, args[2] ? (STRPTR)args[2] : RPATH,
					TAG_END );
				
				if( rtFileRequest( FileReq, NULL, args[1] ? (STRPTR)args[1] : (STRPTR)GCS( MSG_TL_SELECTPATH ),
					RT_Window,		Window,
					RT_Underscore,	'_',
					RT_TextAttr,	&RequesterFontAttr,
					RTFI_Flags,		FREQF_NOFILES,
					RTFI_OkText,	GCS( MSG_GF_SELECT ),
					TAG_END ) )
					{
					strncpy( RPATH, FileReq->Dir, DEFAULTPATHLEN );
					
					ares->ar_ResultString = FileReq->Dir;
					}
				else
					ares->ar_ResultCode = 5;
				break;
			
			case AREXXCMD_CHECKPRESPECIFIED:
				ares->ar_ResultCode = xStartCheck( NULL, CHECKTYPE_PRESPECIFIED, TRUE );
				break;
			
			case AREXXCMD_CHECKSPECIFIED:
				ares->ar_ResultCode = xStartCheck( NULL, CHECKTYPE_SPECIFIED, TRUE );
				break;
			
			case AREXXCMD_CHECKFILES:
				NewList( &list );
				
				files = (UBYTE **)args[1];
				
				while( *files )
					{
					if( lock = Lock( *files, ACCESS_READ ) )
						UnLock( lock );
					else
						{
						ares->ar_ResultCode = a = -1;
						
						break;
						}
					
					xAddNameNode( &list, *files );
					
					files++;
					}
				
				if( !a )
					ares->ar_ResultCode = xStartCheck( &list, CHECKTYPE_SPECIFIED, TRUE );
				
				if( !ares->ar_ResultCode )
					ares->ar_ResultCode = a;
				
				xFreeNameNodes( &list );
				break;
			
			case AREXXCMD_CHECKVECTORS:
				xCheckVectors();
				break;
			
			case AREXXCMD_NUMBERVIRUSESFOUND:
				xSPrintF( resultstring, "%ld", xCHK_CountNumberViraFound( args[1] ) );
				
				ares->ar_ResultString = resultstring;
				break;
			
			case AREXXCMD_GETVIRUSNAME:
				if( args[1] )
					ares->ar_ResultString = xCHK_GetVirusName( ((LONG *)args[1])[0], TRUE );
				break;
			
			case AREXXCMD_GETVIRUSFILENAME:
				if( args[1] )
					ares->ar_ResultString = xCHK_GetVirusName( ((LONG *)args[1])[0], FALSE );
				break;
			
			case AREXXCMD_FLUSHVIRUSFOUNDLIST:
				xCHK_FlushVirusList();
				break;
			
			case AREXXCMD_SHOW:
				xPopUpGUI();
				break;
			
			case AREXXCMD_HIDE:
				xCloseInterface();
				break;
			
			case AREXXCMD_REQUESTUSER:
				a = rtEZRequestTags( args[2] ? (STRPTR)args[2] : (STRPTR)"", args[3] ? (STRPTR)args[3] : GCS( MSG_GF_OKAY ), NULL, NULL,
					RT_Window,		Window,
					RT_Underscore,	'_',
					RT_TextAttr,	&RequesterFontAttr,
					RTEZ_ReqTitle,	args[1] ? (STRPTR)args[1] : GCS( MSG_REQ_INFO ),
					TAG_END );
				
				xSPrintF( resultstring, "%ld", a );
				
				ares->ar_ResultString = resultstring;
				break;
			
			case AREXXCMD_REQUESTSTRING:
				ares->ar_ResultCode = 1;
				maxchars = args[4] ? ((LONG *)args[4])[0] : 256;
				
				if( maxchars )
					{
						/// Check for supplied buffer
					
					if( args[3] )
						strncpy( resultstring, (STRPTR)args[3], maxchars );
					else
						resultstring[0] = 0;
					
					if( rtGetString( resultstring, maxchars - 1, args[1] ? (STRPTR)args[1] : GCS( MSG_REQ_INFO ), NULL,
						RT_Window,			Window,
						RT_Underscore,		'_',
						RT_TextAttr,		&RequesterFontAttr,
						args[2] ? RTGS_TextFmt : TAG_IGNORE, args[2],
						RTGS_GadFmt,		args[6] ? (STRPTR)args[6] : GCS( MSG_GF_OKAYCANCEL ),
						RTGS_AllowEmpty,	args[5],
						TAG_END ) )
						{
						ares->ar_ResultCode = 0;
						ares->ar_ResultString = resultstring;
						}
					}
				break;
			
			case AREXXCMD_REQUESTNUMBER:
				ares->ar_ResultCode = 1;
				number = args[5] ? ((LONG *)args[5])[0] : 0;
				
				if( rtGetLong( (ULONG *)&number, args[1] ? (STRPTR)args[1] : GCS( MSG_REQ_INFO ), NULL,
					RT_Window,			Window,
					RT_Underscore,		'_',
					RT_TextAttr,		&RequesterFontAttr,
					args[2] ? RTGL_TextFmt : TAG_IGNORE, args[2],
					RTGL_GadFmt,		args[7] ? (STRPTR)args[7] : GCS( MSG_GF_OKAYCANCEL ),
					args[3] ? RTGL_Min : TAG_IGNORE, ((LONG *)args[3])[0],
					args[4] ? RTGL_Max : TAG_IGNORE, ((LONG *)args[4])[0],
					RTGL_ShowDefault,	args[6],
					TAG_END ) )
					{
					xSPrintF( resultstring, "%ld", number );
					
					ares->ar_ResultCode = 0;
					ares->ar_ResultString = resultstring;
					}
				break;
			}
		}
	
	return( ares );
}
