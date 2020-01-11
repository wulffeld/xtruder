/*
**	$VER: Xt_About.c (14.2.97)
**
**	Xtruder about functions
**
**	Copyright (C) 1994-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


#define		CY	^0xA5

VOID xAbout( UBYTE mode )
{
	struct TextAttr *	ta = RequesterFont->tf_Flags & FPF_PROPORTIONAL ? &TextFontAttr : &RequesterFontAttr;
	STRPTR				title = GCS( MSG_REQ_INFO );
	UBYTE				buffer[1024] = "";
	UBYTE				strbuf1[ DEFAULTBUFFERLEN ] = "";
	UBYTE				addy[] = { 'S'CY,'n'CY,'a'CY,'i'CY,'l'CY,':'CY,' 'CY,'M'CY,'a'CY,'r'CY,'t'CY,'i'CY,'n'CY,' 'CY,'W'CY,'u'CY,'l'CY,'f'CY,'f'CY,'e'CY,'l'CY,'d'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,'\n'CY,
								   ' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,'O'CY,'d'CY,'e'CY,'n'CY,'s'CY,'e'CY,'v'CY,'e'CY,'j'CY,' 'CY,'1'CY,'2'CY,'1'CY,','CY,' 'CY,'2'CY,' 'CY,'T'CY,'V'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,'\n'CY,
								   ' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,'4'CY,'7'CY,'0'CY,'0'CY,' 'CY,'N'CY,'a'CY,'e'CY,'s'CY,'t'CY,'v'CY,'e'CY,'d'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,'\n'CY,
								   ' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,'D'CY,'e'CY,'n'CY,'m'CY,'a'CY,'r'CY,'k'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,' 'CY,'\n'CY,
								   '\n'CY,
								   'E'CY,'-'CY,'M'CY,'a'CY,'i'CY,'l'CY,':'CY,' 'CY,'w'CY,'u'CY,'l'CY,'f'CY,'f'CY,'e'CY,'l'CY,'d'CY,'@'CY,'p'CY,'o'CY,'s'CY,'t'CY,'4'CY,'.'CY,'t'CY,'e'CY,'l'CY,'e'CY,'.'CY,'d'CY,'k'CY,' 'CY,'\0'CY,
								 };
	UWORD				c;
	
	
	xDisableWindow( Window );
	
	switch( mode )
		{
			/// About Xtruder
		
		case 0:
			if( KeyWord == 1 )
				{
				strcpy( strbuf1, GCS( MSG_REGISTEREDTO ) );
				strcat( strbuf1, RegName );
				}
			else
			if( KeyWord == 0 )
				strcpy( strbuf1, GCS( MSG_UNREGISTERED ) );
			else
				exit( 0 );
			
			xSPrintF( buffer, GCS( MSG_XTRUDER_INFO ), AboutVersion, XSettings->xs_GUI_PublicScreenName[0] ? XSettings->xs_GUI_PublicScreenName : (STRPTR)"XTRUDER", strbuf1 );
			
			rtEZRequestTags( buffer, GCS( MSG_GF_OKAY ), NULL, NULL,
				RT_Window,		Window,
				RT_ReqPos,		REQPOS_CENTERSCR,
				RT_Underscore,	'_',
				RT_TextAttr,	ta,
				RTEZ_Flags,		EZREQF_CENTERTEXT,
				RTEZ_ReqTitle,	title,
				TAG_END );
			break;
		
			/// Author/Contact
		
		case 1:
				/// Decode author info
			
			for( c = 0; c < strlen( addy ); c++ )
				addy[c] = addy[c] ^ 0xA5;
			
			xSPrintF( buffer, GCS( MSG_AUTHOR_INFO ), addy );
			
			rtEZRequestTags( buffer, GCS( MSG_GF_OKAY ), NULL, NULL,
				RT_Window,		Window,
				RT_ReqPos,		REQPOS_CENTERSCR,
				RT_Underscore,	'_',
				RT_TextAttr,	ta,
				RTEZ_Flags,		EZREQF_CENTERTEXT,
				RTEZ_ReqTitle,	title,
				TAG_END );
			break;
		}
	
	xEnableWindow( Window );
}
