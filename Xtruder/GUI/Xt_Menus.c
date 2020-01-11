/*
**	$VER: XtruderMenus.c (24.7.96)
**
**	Xtruder menu functions
**
**	Copyright (C) 1996 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


/*
 * Localize the menu structure and apply the arexx scripts in the end.
 */
VOID xCreateMenus( VOID )
{
	struct NewMenu *	menu = XtruderMenu;
	STRPTR				label;
	LONG				locstr = MSG_MU_TL_PROJECT;
	
	
	while( locstr <= MSG_MU_EXECUTEAREXXSCRIPT )
		{
		label = GCS( locstr );
		
		if( menu->nm_Label != NM_BARLABEL )
			{
				/// If the first character is NON-NULL we have a shortcut
			
			switch( menu->nm_Type )
				{
				case NM_TITLE:
					menu->nm_Label = label;
					break;
				
				case NM_ITEM:
				case NM_SUB:
					menu->nm_Label		= label + 2;
					menu->nm_CommKey	= label[0] == ' ' ? NULL : label;
					break;
				}
			locstr++;
			}
		menu++;
		}
	
	if( !IsListEmpty( ARexxScriptsList ) )
		{
		struct ARexxNode *	anode;
		UBYTE				m;
		
		
		menu->nm_Type = NM_ITEM;
		menu->nm_Label = NM_BARLABEL;
		
		menu++;
		
		for( anode = (struct ARexxNode *)ARexxScriptsList->lh_Head, m = 0;
			 anode->an_Node.ln_Succ;
			 anode = (struct ARexxNode *)anode->an_Node.ln_Succ, m++ )
			{
			menu->nm_Type = NM_ITEM;
			
			if( anode->an_Separator )
				{
				menu->nm_Label = NM_BARLABEL;
				menu->nm_CommKey = NULL;
				
				m--;
				}
			else
				menu->nm_Label = anode->an_MenuName;
			
			menu++;
			
			if( m == 40 )
				break;
			}
		}
	
	menu->nm_Label = NULL;
	menu->nm_Type = NM_END;
}

BOOL xSetMenus( VOID )
{
	if( Menu = CreateMenus( XtruderMenu, TAG_END ) )
		{
		if( LayoutMenus( Menu, VisualInfo,
			OnPublic ? TAG_IGNORE : GTMN_TextAttr, &ScreenFontAttr,
			GTMN_NewLookMenus,	TRUE,
			TAG_END ) )
			{
			if( SetMenuStrip( Window, Menu ) )
				return( TRUE );
			}
		
		FreeMenus( Menu );
		Menu = NULL;
		}
	
	return( FALSE );
}

VOID xRemoveMenus( VOID )
{
	if( Menu )
		{
		ClearMenuStrip( Window );
		
		FreeMenus( Menu );
		Menu = NULL;
		}
}

VOID xHandleMainWinMenuIDCMP( UWORD code )
{
	ULONG					menunum = MENUNUM( code );
	ULONG					itemnum = ITEMNUM( code );
	ULONG					subnum = SUBNUM( code );
	UBYTE					strbuf[ DEFAULTBUFFERLEN ];
	
	
	switch( menunum )
		{
		case MU_PROJECT:
			switch( itemnum )
				{
				case MU_PT_LISTKNOWN:
					switch( subnum )
						{
						case MU_PT_LK_BOOTBLOCKVIRA:
							xListHandler( LIST_KNOWNBOOTBLOCKS );
							break;
						
						case MU_PT_LK_FILEBASEDVIRA:
							xListHandler( LIST_KNOWNFILEANDLINKVIRA );
							break;
						
						case MU_PT_LK_CRUNCHERS:
							xListHandler( LIST_KNOWNDECRUNCHERS );
							break;
						
						case MU_PT_LK_PATCHES:
							xListHandler( LIST_KNOWNPATCHES );
							break;
						
						case MU_PT_LK_FILETYPES:
							xListHandler( LIST_KNOWNFILETYPES );
							break;
						}
					break;
				
				case MU_PT_HELP:
					switch( subnum )
						{
						case MU_PT_HP_INDEX:
							xGuideDisplay( AGH_MAIN );
							break;
						
						case MU_PT_HP_MENUS:
							xGuideDisplay( AGH_MENUS );
							break;
						
						case MU_PT_HP_FILECHECK:
							xGuideDisplay( AGH_FILECHECK );
							break;
						
						case MU_PT_HP_BOOTBLOCKINTERFACE:
							xGuideDisplay( AGH_BOOTBLOCKINTERFACE );
							break;
						
						case MU_PT_HP_SETTINGS:
							xGuideDisplay( AGH_SETTINGS );
							break;
						
						case MU_PT_HP_AREXX:
							xGuideDisplay( AGH_AREXX );
							break;
						}
					break;
						
				case MU_PT_SYSTEMINFORMATION:
					xOpenSysInfoWindow();
					break;
				
				case MU_PT_HIDE:
					xCloseInterface();
					break;
				
				case MU_PT_ABOUT:
					switch( subnum )
						{
						case MU_PT_AB_XTRUDER:
						case MU_PT_AB_AUTHOR:
							xAbout( subnum - MU_PT_AB_XTRUDER );
							break;
						}
					break;
				
				case MU_PT_QUIT:
					xCloseSystem( 0, 0, QUIT_ASK_SETTINGS | QUIT_RETURN_POSSIBLE );
					break;
				}
			break;
		
		case MU_FILES:
			switch( itemnum )
				{
				case MU_FS_CHECKPRESPECIFIED:
					xStartCheck( NULL, CHECKTYPE_PRESPECIFIED, TRUE );
					break;
				
				case MU_FS_LISTPRESPECIFIED:
					xListPrespecified();
					break;
				
				case MU_FS_CHECKFILES:
					xStartCheck( NULL, CHECKTYPE_SPECIFIED, TRUE );
					break;
				
				case MU_FS_REPAIR:
					xRepairFile( NULL );
					break;
				
				case MU_FS_SHOWCRC:
					xShowCRC( NULL );
					break;
				
				case MU_FS_REPORTS:
					xOpenMRIWindow();
					break;
				}
			break;
		
		case MU_BOOTBLOCK:
			switch( itemnum )
				{
				case MU_BK_INTERFACE:
					xDisableWindow( Window );
					
					xOpenBBWindow( 0, FALSE );
					
					xEnableWindow( Window );
					break;
				
				case MU_BK_CHECK:
					switch( subnum )
						{
						case MU_BK_CK_DF0:
						case MU_BK_CK_DF1:
						case MU_BK_CK_DF2:
						case MU_BK_CK_DF3:
							xDisableWindow( Window );
							
							xOpenBBWindow( subnum - MU_BK_CK_DF0 + GTID_BB_CHECKDF0, FALSE );
							
							xEnableWindow( Window );
							break;
						
						case MU_BK_CK_DMS:
							xDisableWindow( Window );
							
							xOpenBBWindow( GTID_BB_CHECKDMS, FALSE );
							
							xEnableWindow( Window );
							break;
						}
					break;
				
				case MU_BK_INSTALL:
					switch( subnum )
						{
						case MU_BK_IN_DF0:
						case MU_BK_IN_DF1:
						case MU_BK_IN_DF2:
						case MU_BK_IN_DF3:
							xDisableWindow( Window );
							
							xOpenBBWindow( subnum - MU_BK_IN_DF0 + GTID_BB_INSTALLDF0, FALSE );
							
							xEnableWindow( Window );
							break;
						}
					break;
				
				case MU_BK_SHOWASCII:
					switch( subnum )
						{
						case MU_BK_SH_DF0:
						case MU_BK_SH_DF1:
						case MU_BK_SH_DF2:
						case MU_BK_SH_DF3:
							xDisableWindow( Window );
							
							xOpenBBWindow( subnum - MU_BK_SH_DF0 + GTID_BB_SHOWDF0, FALSE );
							
							xEnableWindow( Window );
							break;
						
						case MU_BK_SH_DMS:
							xDisableWindow( Window );
							
							xOpenBBWindow( GTID_BB_SHOWDMS, FALSE );
							
							xEnableWindow( Window );
							break;
						}
					break;
				}
			break;
		
		case MU_MEMORY:
			switch( itemnum )
				{
				case MU_MY_CHECKVECTORS:
					xCheckVectors();
					break;
				
				case MU_MY_CHECKLVO:
					xCheckLVO( FALSE );
					break;
				
				case MU_MY_CLEAR:
					switch( subnum )
						{
						case MU_MY_CR_ALL:
							xClearAllVectors();
							break;
						
						case MU_MY_CR_COOLCAPTURE:
							SysBase->CoolCapture = 0;
							break;
						
						case MU_MY_CR_COLDCAPTURE:
							SysBase->ColdCapture = 0;
							break;
						
						case MU_MY_CR_WARMCAPTURE:
							SysBase->WarmCapture = 0;
							break;
						
						case MU_MY_CR_KICKTAGPTR:
							SysBase->KickTagPtr = 0;
							break;
						
						case MU_MY_CR_KICKMEMPTR:
							SysBase->KickMemPtr = 0;
							break;
						}
					break;
				
				case MU_MY_SHOW:
					switch( subnum )
						{
						case MU_MY_SW_LIBRARIES:
							xListHandler( LIST_LIBRARIES );
							break;
						
						case MU_MY_SW_DEVICES:
							xListHandler( LIST_DEVICES );
							break;
						
						case MU_MY_SW_RESOURCES:
							xListHandler( LIST_RESOURCES );
							break;
						
						case MU_MY_SW_PORTS:
							xListHandler( LIST_PORTS );
							break;
						
						case MU_MY_SW_SEMAPHORES:
							xListHandler( LIST_SEMAPHORES );
							break;
						
						case MU_MY_SW_TASKS:
							xListHandler( LIST_TASKS );
							break;
						}
					break;
				}
			break;
		
		case MU_MISCELLANEOUS:
			switch( itemnum )
				{
				case MU_MS_VIRUSINFORMATION:
					xOpenVirusInformationWindow();
					break;
				
				case MU_MS_HISTORY:
					switch( subnum )
						{
						case MU_MS_HY_CLEAR:
							xClearHistory( TRUE );
							break;
						
						case MU_MS_HY_SHOW:
							xDisableWindow( Window );
							
							xShowHistory();
							
							xEnableWindow( Window );
							break;
						
						case MU_MS_HY_SAVE:
							xSaveHistory( TRUE );
							break;
						}
					break;
				}
			break;
		
		case MU_SETTINGS:
			switch( itemnum )
				{
				case MU_SS_GUI:
					if( xOpenGUISettings() )
						{
						xCloseInterface();
						
						xPopUpGUI();
						}
					break;
				
				case MU_SS_STARTUP:
					xOpenStartupSettings();
					break;
				
				case MU_SS_MISCELLANEOUS:
					xOpenMiscellaneousSettings();
					break;
				
				case MU_SS_PATHS:
					xOpenPathsSettings();
					break;
				
				case MU_SS_HISTORY:
					xOpenHistorySettings();
					break;
				
				case MU_SS_BOOTBLOCK:
					xOpenBootblockSettings();
					break;
				
				case MU_SS_AREXX:
					xRemoveMenus();
					
					xOpenARexxSettings();
					
					xCreateMenus();
					
					xSetMenus();
					break;
				
				case MU_SS_FILECHECK:
					switch( subnum )
						{
						case MU_SSFC_PRESPECIFIED:
							xOpenFCPrespecifiedSettings();
							break;
						
						case MU_SSFC_MISCELLANEOUS:
							xOpenFCMiscellaneousSettings();
							break;
						
						case MU_SSFC_UNPACKING:
							xOpenFCUnpackingSettings();
							break;
						
						case MU_SSFC_ARCHIVES:
							xOpenFCArchivesSettings();
							break;
						
						case MU_SSFC_INFORMATION:
							xOpenFCInformationSettings();
							break;
						
						case MU_SSFC_PBC:
							xOpenFCPBCSettings();
							break;
						
						case MU_SSFC_CHECKWINDOW:
							xOpenFCCheckWindowSettings();
							break;
						}
					break;
				
				case MU_SS_LOAD:
					xLoadSettings( TRUE );
					break;
				
				case MU_SS_SAVE:
					xSaveSettings( FALSE, TRUE );
					break;
				
				case MU_SS_SAVEAS:
					xSaveSettings( TRUE, FALSE );
					break;
				}
			break;
		
		case MU_AREXX:
			if( itemnum == MU_AREXX_EXECUTE )
				{
				if( xSelectFile( Window,
					SELFP_Dir,		REXXPATH,
					SELFP_File,		REXXFILE,
					SELFP_DestFile,	strbuf,
					TAG_END ) )
					{
					xSendARexxMsg( RexxStuff, strbuf, FALSE );
					}
				}
			else
			if( itemnum > 1 && itemnum < 50 )
				{
				struct ARexxNode *	anode;
				
				
				if( anode = xFindARexxNode( itemnum - 2 ) )
					xSendARexxMsg( RexxStuff, anode->an_FileName, FALSE );
				}
			break;
		}
}
