/*
**	$VER: Defines.h (8.7.97)
**
**	Xtruder defines for (almost) everything
**
**	Copyright (C) 1994-1997 Martin Wulffeld
**	All Rights Reserved
*/


	/// Action when virus has been found

#define		VIRUSFOUND_DISABLE				0
#define		VIRUSFOUND_DELETE				1
#define		VIRUSFOUND_RENAME				2
#define		VIRUSFOUND_COMMENT				3
#define		VIRUSFOUND_COPY					4
#define		VIRUSFOUND_REQUEST				5
#define		VIRUSFOUND_NOTHING				6

	/// Action when virus has been copied

#define		VIRUSCOPIED_DISABLE				0
#define		VIRUSCOPIED_RENAME				1
#define		VIRUSCOPIED_COMMENT				2
#define		VIRUSCOPIED_REQUEST				3
#define		VIRUSCOPIED_NOTHING				4

	/// AmigaGuide help

#define		AGH_MAIN						0
#define		AGH_MENUS						1
#define		AGH_FILECHECK					2
#define		AGH_BOOTBLOCKINTERFACE			3
#define		AGH_SETTINGS					4
#define		AGH_AREXX						5

#define		AGH_MENU_PROJECT				6
#define		AGH_MENU_FILES					7
#define		AGH_MENU_BOOTBLOCK				8
#define		AGH_MENU_MEMORY					9
#define		AGH_MENU_MISCELLANEOUS			10
#define		AGH_MENU_SETTINGS				11
#define		AGH_MENU_AREXX					12

#define		AGH_STGS_GUI					13
#define		AGH_STGS_STARTUP				14
#define		AGH_STGS_MISCELLANEOUS			15
#define		AGH_STGS_PATHS					16
#define		AGH_STGS_HISTORY				17
#define		AGH_STGS_BOOTBLOCK				18
#define		AGH_STGS_AREXX					19
#define		AGH_STGS_FCHK_PRESPECIFIED		20
#define		AGH_STGS_FCHK_MISCELLANEOUS		21
#define		AGH_STGS_FCHK_UNPACKING			22
#define		AGH_STGS_FCHK_ARCHIVES			23
#define		AGH_STGS_FCHK_INFORMATION		24
#define		AGH_STGS_FCHK_PBC				25
#define		AGH_STGS_FCHK_CHECKWINDOW		26

#define		AGH_PROJ_LKNO_BOOTBLOCKVIRA		27
#define		AGH_PROJ_LKNO_FILEBASEDVIRA		28
#define		AGH_PROJ_LKNO_CRUNCHERS			29
#define		AGH_PROJ_LKNO_PATCHES			30
#define		AGH_PROJ_LKNO_FILETYPES			31

#define		AGH_MEMO_CHECKLVO				32
#define		AGH_MEMO_SHOW_LIBRARIES			33
#define		AGH_MEMO_SHOW_DEVICES			34
#define		AGH_MEMO_SHOW_RESOURCES			35
#define		AGH_MEMO_SHOW_PORTS				36
#define		AGH_MEMO_SHOW_SEMAPHORES		37
#define		AGH_MEMO_SHOW_TASKS				38

#define		AGH_MISC_VIRUSINFORMATION		39

	/// Archive types

#define		XP_INTERNAL1EXTRACT				0
#define		XP_INTERNAL2EXTRACT				1

#define		AT_LHA							1
#define		AT_LZX							2
#define		AT_DMS							3

	/// ARexx

#define		REXX_RETURN_ERROR				((struct RexxMsg *)-1L)

	/// CheckWindow

#define		GAD_CW_PATH						0
#define		GAD_CW_FILE						1
#define		GAD_CW_PAUSE					2
#define		GAD_CW_STOP						3
#define		GAD_CW_HISTORY					4

	/// Command Line Interface

#define		USAGE							"F=FILE/M,R=PRESPECIFIED/S,V=VECTORS/S,P=PUBSCREEN/K,E=EXIT/S"

#define		ARG_FILE						0
#define		ARG_PRESPECIFIED				1
#define		ARG_VECTORS						2
#define		ARG_PUBSCREEN					3
#define		ARG_EXIT						4

#define		NUM_ARGS						5

	/// CurrentCheck return results

#define		CCRES_OK						1
#define		CCRES_USERBREAK					2	// Ctrl-C / Stop
#define		CCRES_CHECKEND					3	// Check ended

	/// Edit hooks

#define		HOOK_CURSORDOWN					240

	/// Gadget IDs: Bootblock

#define		GTID_BB_SHOWDF0					1000
#define		GTID_BB_SHOWDF1					1001
#define		GTID_BB_SHOWDF2					1002
#define		GTID_BB_SHOWDF3					1003
#define		GTID_BB_SHOWDMS					1004
#define		GTID_BB_CHECKDF0				1005
#define		GTID_BB_CHECKDF1				1006
#define		GTID_BB_CHECKDF2				1007
#define		GTID_BB_CHECKDF3				1008
#define		GTID_BB_CHECKDMS				1009
#define		GTID_BB_INSTALLDF0				1010
#define		GTID_BB_INSTALLDF1				1011
#define		GTID_BB_INSTALLDF2				1012
#define		GTID_BB_INSTALLDF3				1013
#define		GTID_BB_STATUS					1014
#define		GTID_BB_BUFFER					1015
#define		GTID_BB_BUFFERLISTVIEW			1016
#define		GTID_BB_LOAD					1017
#define		GTID_BB_SAVE					1018

#define		GTID_DRV_DF0					25100
#define		GTID_DRV_DF1					25101
#define		GTID_DRV_DF2					25102
#define		GTID_DRV_DF3					25103
#define		GTID_DRV_DMS					25104

#define		GTID_BB_1XOFS					25200
#define		GTID_BB_13FFS					25201
#define		GTID_BB_20OFS					25202
#define		GTID_BB_20FFS					25203
#define		GTID_BB_20OFSINT				25204
#define		GTID_BB_20FFSINT				25205
#define		GTID_BB_30OFSCACHE				25206
#define		GTID_BB_30FFSCACHE				25207
#define		GTID_BB_CUSTOM					25208
#define		GTID_BB_CUSTOMSELECT			25209

	/// Gadget IDs: Check window

#define		GTID_CWIN_PATH					1000
#define		GTID_CWIN_FILE					1001
#define		GTID_CWIN_PAUSE					1002
#define		GTID_CWIN_STOP					1003
#define		GTID_CWIN_HISTORY				1004
#define		GTID_CWIN_CHECKHISTORY			1005
#define		GTID_CWIN_WARNINGS				1006

	/// Gadget IDs: Miscellaneous

#define		GTID_OKAY						50000
#define		GTID_CANCEL						60000

	/// Gadget IDs: Reports / Check interface

#define		GTID_CHI_PATH					1000
#define		GTID_CHI_FILE					1001
#define		GTID_CHI_PAUSE					1002
#define		GTID_CHI_STOP					1003
#define		GTID_CHI_SAVEHISTORY			1004
#define		GTID_CHI_LISTVIEW				1005

	/// Gadget IDs: Reports / Create interface

#define		GTID_CRI_PATH					1000
#define		GTID_CRI_FILE					1001
#define		GTID_CRI_STOP					1002
#define		GTID_CRI_PAUSE					1003
#define		GTID_CRI_ERRORSLISTVIEW			1004

	/// Gadget IDs: Reports / Main interface

#define		GTID_MRI_CREATE					1000
#define		GTID_MRI_UPDATE					1001
#define		GTID_MRI_CHECK					1002
#define		GTID_MRI_DELETE					1003
#define		GTID_MRI_REPORTSPATH			1004
#define		GTID_MRI_GETREPORTSPATH			1005
#define		GTID_MRI_REPORTSLISTVIEW		1006

	/// Gadget IDs: Settings

#define		GTID_AR_LISTVIEW				1000
#define		GTID_AR_NAME					1001
#define		GTID_AR_ADD						1002
#define		GTID_AR_REMOVE					1003
#define		GTID_AR_UP						1004
#define		GTID_AR_DOWN					1005
#define		GTID_AR_SORTALPHABETICALLY		1006
#define		GTID_AR_SCRIPT					1007
#define		GTID_AR_GETSCRIPT				1008

#define		GTID_BB_CUSTOMBOOTBLOCK			1000
#define		GTID_BB_GETCUSTOMBOOTBLOCK		1001
#define		GTID_BB_CHECKINSERTEDDISKS		1002
#define		GTID_BB_NOINTERFACE				1003

#define		GTID_FC_PR_LISTVIEW				1000
#define		GTID_FC_PR_STRING				1001
#define		GTID_FC_PR_ADD					1002
#define		GTID_FC_PR_REMOVE				1003
#define		GTID_FC_PR_SORT					1004

#define		GTID_FC_AR_LHAEXTRACTROUTINE	1000
#define		GTID_FC_AR_EXTRACTLHA			1001
#define		GTID_FC_AR_LHAMAXSIZE			1002
#define		GTID_FC_AR_LZXEXTRACTROUTINE	1003
#define		GTID_FC_AR_EXTRACTLZX			1004
#define		GTID_FC_AR_LZXMAXSIZE			1005
#define		GTID_FC_AR_OUTPUT				1006

#define		GTID_FC_CW_AUTOCLOSE			1000
#define		GTID_FC_CW_MEMORYINFORMATION	1001
#define		GTID_FC_CW_CHECKINFORMATION		1002
#define		GTID_FC_CW_HISTORY				1003
#define		GTID_FC_CW_AUTOSCROLL			1004
#define		GTID_FC_CW_WARNINGS				1005

#define		GTID_FC_MI_FILEFILTER			1000
#define		GTID_FC_MI_ARCHIVEFILTER		1001
#define		GTID_FC_MI_DOWITHVIRUSFOUND		1002
#define		GTID_FC_MI_DOWITHVIRUSCOPIED	1003
#define		GTID_FC_MI_COMMENTFORVIRUS		1004
#define		GTID_FC_MI_RENAMEEXTENSION		1005
#define		GTID_FC_MI_PATHINDENTION		1006
#define		GTID_FC_MI_AUTODEPROTECT		1007
#define		GTID_FC_MI_ENTERDIRECTORIES		1008
#define		GTID_FC_MI_AUTOPATHCHECK		1009
#define		GTID_FC_MI_SUPPRESSSYSMSGS		1010

#define		GTID_FC_UN_WHENDECRUNCHINGUSE	1000
#define		GTID_FC_UN_EXTRACTARCHIVES		1001
#define		GTID_FC_UN_ASKBEFOREEXTRACTING	1002
#define		GTID_FC_UN_ASKBEFOREDELETINGEXT	1003
#define		GTID_FC_UN_DECRUNCHEXECUTABLES	1004
#define		GTID_FC_UN_DECRUNCHDATAFILES	1005
#define		GTID_FC_UN_ASKBEFOREDECRUNCHING	1006
#define		GTID_FC_UN_ASKBEFOREDELETINGDEC	1007
#define		GTID_FC_UN_DECRUNCHENCRYPTED	1008
#define		GTID_FC_UN_CHECKDMSBOOTBLOCK	1009

#define		GTID_FC_IN_SIZE					1001
#define		GTID_FC_IN_FILETYPE				1002
#define		GTID_FC_IN_PROTECTIONBITS		1003
#define		GTID_FC_IN_DATE					1004

#define		GTID_FC_PB_PBREAD				1000
#define		GTID_FC_PB_PBWRITE				1001
#define		GTID_FC_PB_PBEXECUTABLE			1002
#define		GTID_FC_PB_PBDELETABLE			1003
#define		GTID_FC_PB_PBARCHIVE			1004
#define		GTID_FC_PB_PBSCRIPT				1005
#define		GTID_FC_PB_PBPURE				1006
#define		GTID_FC_PB_PBHIDDEN				1007
#define		GTID_FC_PB_CHECKFILESWITHPBCSET	1008
#define		GTID_FC_PB_SETPBCONEXECUTABLES	1009
#define		GTID_FC_PB_SETPBCONARCHIVES		1010

#define		GTID_GU_SCREENMODE				1000
#define		GTID_GU_GETSCREENMODE			1001
#define		GTID_GU_PUBLICSCREENNAME		1002
#define		GTID_GU_GETPUBLICSCREEN			1003
#define		GTID_GU_SCREENFONT				1004
#define		GTID_GU_GETSCREENFONT			1005
#define		GTID_GU_TEXTFONT				1006
#define		GTID_GU_GETTEXTFONT				1007
#define		GTID_GU_REQUESTERFONT			1008
#define		GTID_GU_GETREQUESTERFONT		1009
#define		GTID_GU_BACKGROUNDPATTERN		1010
#define		GTID_GU_GETBACKGROUNDPATTERN	1011
#define		GTID_GU_PALETTE					1012
#define		GTID_GU_SCROLLERWIDTH			1013
#define		GTID_GU_HIDEAPPICON				1014

#define		GTID_HI_HISTORYPATH				1000
#define		GTID_HI_GETHISTORYPATH			1001
#define		GTID_HI_HISTORYFILE				1002
#define		GTID_HI_GETHISTORYFILE			1003
#define		GTID_HI_USEDATEANDTIME			1004
#define		GTID_HI_APPEND					1005
#define		GTID_HI_REQUESTFILENAME			1006
#define		GTID_HI_MAXENTRIES				1007
#define		GTID_HI_SAVEMETHOD				1008
#define		GTID_HI_FILECHECK				1009

#define		GTID_MI_POPKEY					1000
#define		GTID_MI_PRIORITY				1001
#define		GTID_MI_POPUPATSTART			1002
#define		GTID_MI_STAYHIDDEN				1003

#define		GTID_PA_ARCHIVEEXTRACTPATH		1000
#define		GTID_PA_GET_ARCHIVEEXTRACTPATH	1001
#define		GTID_PA_FILEDECRUNCHPATH		1002
#define		GTID_PA_GET_FILEDECRUNCHPATH	1003
#define		GTID_PA_VIRUSPATH				1004
#define		GTID_PA_GET_VIRUSPATH			1005
#define		GTID_PA_RECHECKPATH				1006
#define		GTID_PA_GET_RECHECKPATH			1007
#define		GTID_PA_AUTOCREATEABOVEPATHS	1008
#define		GTID_PA_AUTODELETEABOVEPATHS	1009
#define		GTID_PA_GUIDEPATH				1010
#define		GTID_PA_GET_GUIDEPATH			1011
#define		GTID_PA_DESCRIPTIONSPATH		1012
#define		GTID_PA_GET_DESCRIPTIONSPATH	1013

#define		GTID_ST_STARTUPSCRIPT			1000
#define		GTID_ST_GETSTARTUPSCRIPT		1001
#define		GTID_ST_EXECUTESTARTUPSCRIPT	1002
#define		GTID_ST_CHECKPRESPECIFIED		1003
#define		GTID_ST_CHECKVECTORS			1004
#define		GTID_ST_SHOWSYSTEMINFORMATION	1005

	/// Gadget IDs: Virus information

#define		GTID_VI_DESCRIPTIONSPATH		1000
#define		GTID_VI_GETDESCRIPTIONSPATH		1001
#define		GTID_VI_EXTERNALVIEWER			1002
#define		GTID_VI_GETEXTERNALVIEWER		1003
#define		GTID_VI_SEARCHSTRING1			1004
#define		GTID_VI_SEARCHSTRING2			1005
#define		GTID_VI_SEARCHSTRING3			1006
#define		GTID_VI_SEARCH					1007
#define		GTID_VI_SHOWALL					1008
#define		GTID_VI_DESCRIPTIONS			1009

	/// Gadget creation

#define		GETGADWIDTH						20
#define		CONFIRMGAP						3
#define		LASTGADGETGAP					5

	/// xHandleCheckWindowIDCMP() return defines

#define		CW_NOTHING						1
#define		CW_STOP							2

	/// xHandleBootblockSelectIDCMP return defines

#define		BB_1XOFS						1
#define		BB_13FFS						2
#define		BB_20OFS						3
#define		BB_20FFS						4
#define		BB_20OFSINT						5
#define		BB_20FFSINT						6
#define		BB_30OFSCACHE					7
#define		BB_30FFSCACHE					8
#define		BB_CUSTOM						20
#define		BB_CUSTOMSELECT					21

	/// xHandleDriveSelectIDCMP return defines...

#define		DRV_DF0							0
#define		DRV_DF1							1
#define		DRV_DF2							2
#define		DRV_DF3							3
#define		DRV_DMS							4
#define		DRV_CUSTOM						5

	/// xHandleFileProtection() return defines

#define		FILEPROT_NOTPROTECTED			1
#define		FILEPROT_HANDLED				2
#define		FILEPROT_PROTECTED				3
#define		FILEPROT_FAIL					4

	/// xHandleFilePBC() return defines

#define		FILEPBC_CHECKIT					1
#define		FILEPBC_SKIPIT					2

	/// Handy macros

#define		GETSTRING( g )					((( struct StringInfo * )g->SpecialInfo )->Buffer )
#define		GETNUMBER( g )					((( struct StringInfo * )g->SpecialInfo )->LongInt )
#define		GCS( n )						xGetCatalogStr( n )

	/// History save method

#define		HISSM_AFTEREACHCHECK			0
#define		HISSM_AFTEREACHCHECKFLUSH		1
#define		HISSM_DONTSAVE					2

	/// Library determination

#define		LCRU_UNPACK						1
#define		LCRU_XFDMASTER					2

	/// Menu IDs

#define		MU_PROJECT						0
#define		MU_PT_LISTKNOWN						0
#define		MU_PT_LK_BOOTBLOCKVIRA					0
#define		MU_PT_LK_FILEBASEDVIRA					1
#define		MU_PT_LK_CRUNCHERS						2
#define		MU_PT_LK_PATCHES						3
#define		MU_PT_LK_FILETYPES						4
#define		MU_PT_HELP							2
#define		MU_PT_HP_INDEX							0
#define		MU_PT_HP_MENUS							2
#define		MU_PT_HP_FILECHECK						3
#define		MU_PT_HP_BOOTBLOCKINTERFACE				4
#define		MU_PT_HP_SETTINGS						5
#define		MU_PT_HP_AREXX							6
#define		MU_PT_SYSTEMINFORMATION				4
#define		MU_PT_HIDE							6
#define		MU_PT_ABOUT							7
#define		MU_PT_AB_XTRUDER						0
#define		MU_PT_AB_AUTHOR							1
#define		MU_PT_QUIT							9

#define		MU_FILES						1
#define		MU_FS_CHECKPRESPECIFIED				0
#define		MU_FS_LISTPRESPECIFIED				1
#define		MU_FS_CHECKFILES					3
#define		MU_FS_REPAIR						4
#define		MU_FS_SHOWCRC						5
#define		MU_FS_REPORTS						7

#define		MU_BOOTBLOCK					2
#define		MU_BK_INTERFACE						0
#define		MU_BK_CHECK							2
#define		MU_BK_CK_DF0							0
#define		MU_BK_CK_DF1							1
#define		MU_BK_CK_DF2							2
#define		MU_BK_CK_DF3							3
#define		MU_BK_CK_DMS							5
#define		MU_BK_INSTALL						3
#define		MU_BK_IN_DF0							0
#define		MU_BK_IN_DF1							1
#define		MU_BK_IN_DF2							2
#define		MU_BK_IN_DF3							3
#define		MU_BK_SHOWASCII						4
#define		MU_BK_SH_DF0							0
#define		MU_BK_SH_DF1							1
#define		MU_BK_SH_DF2							2
#define		MU_BK_SH_DF3							3
#define		MU_BK_SH_DMS							5

#define		MU_MEMORY						3
#define		MU_MY_CHECKVECTORS					0
#define		MU_MY_CHECKLVO						1
#define		MU_MY_CLEAR							3
#define		MU_MY_CR_ALL							0
#define		MU_MY_CR_COOLCAPTURE					2
#define		MU_MY_CR_COLDCAPTURE					3
#define		MU_MY_CR_WARMCAPTURE					4
#define		MU_MY_CR_KICKTAGPTR						5
#define		MU_MY_CR_KICKMEMPTR						6
#define		MU_MY_SHOW							5
#define		MU_MY_SW_LIBRARIES						0
#define		MU_MY_SW_DEVICES						1
#define		MU_MY_SW_RESOURCES						2
#define		MU_MY_SW_PORTS							3
#define		MU_MY_SW_SEMAPHORES						4
#define		MU_MY_SW_TASKS							5

#define		MU_MISCELLANEOUS				4
#define		MU_MS_VIRUSINFORMATION				0
#define		MU_MS_HISTORY						2
#define		MU_MS_HY_CLEAR							0
#define		MU_MS_HY_SHOW							1
#define		MU_MS_HY_SAVE							3

#define		MU_SETTINGS						5
#define		MU_SS_GUI							0
#define		MU_SS_STARTUP						1
#define		MU_SS_MISCELLANEOUS					2
#define		MU_SS_PATHS							3
#define		MU_SS_HISTORY						4
#define		MU_SS_BOOTBLOCK						5
#define		MU_SS_AREXX							6
#define		MU_SS_FILECHECK						7
#define		MU_SSFC_PRESPECIFIED					0
#define		MU_SSFC_MISCELLANEOUS					1
#define		MU_SSFC_UNPACKING						2
#define		MU_SSFC_ARCHIVES						3
#define		MU_SSFC_INFORMATION						4
#define		MU_SSFC_PBC								5
#define		MU_SSFC_CHECKWINDOW						6
#define		MU_SS_LOAD							9
#define		MU_SS_SAVE							10
#define		MU_SS_SAVEAS						11

#define		MU_AREXX						6
#define		MU_AREXX_EXECUTE					0

	/// Miscellaneous

#define		FILE_NOT_INFECTED				1
#define		FILE_NOT_EXECUTABLE				2
#define		FILE_INFECTED					3
#define		COULDNT_OPEN_FILE				4
#define		COULDNT_READ_FILE				5
#define		NOT_ENOUGH_MEMORY				6

	/// Much used size definitions

#define		DEFAULTBUFFERLEN				256
#define		DEFAULTPATHLEN					DEFAULTBUFFERLEN - DEFAULTFILELEN
#define		DEFAULTFILELEN					32
#define		BOOTBLOCKSIZE					1024

	/// Quitting

#define		QUIT_ASK_SETTINGS				1
#define		QUIT_RETURN_POSSIBLE			2

	/// Rawkey codes

#define		KEY_TAB							0x42
#define		KEY_RETURN						0x44
#define		KEY_ESCAPE						0x45
#define		KEY_HELP						0x5F
#define		KEY_DELETE						0x46

	/// Reports: Checking

#define		RC_COPYTO						1
#define		RC_COPYALLTO					2
#define		RC_DELETE						3
#define		RC_IGNORE						4
#define		RC_IGNOREALL					0

	/// System

#define		LIST_KNOWNBOOTBLOCKS			0
#define		LIST_KNOWNFILEANDLINKVIRA		1
#define		LIST_KNOWNDECRUNCHERS			2
#define		LIST_KNOWNPATCHES				3
#define		LIST_KNOWNFILETYPES				4
#define		LIST_LIBRARIES					5
#define		LIST_DEVICES					6
#define		LIST_RESOURCES					7
#define		LIST_PORTS						8
#define		LIST_SEMAPHORES					9
#define		LIST_TASKS						10

	/// Tags for xSelectFile() and xSelectPath()

#define		SELFP_ReqTitle					80000001
#define		SELFP_OkText					80000002
#define		SELFP_Dir						80000003		/// Start dir for requester
#define		SELFP_File						80000004		/// Initial file
#define		SELFP_DestDir					80000005		/// Destination buffer for FileReq->Dir
#define		SELFP_DestFile					80000006		/// Destination buffer for filename
#define		SELFP_NewReqPath				80000007		/// If present copy FileReq->Dir to ti_Data
#define		SELFP_NewReqFile				80000008		/// If present copy filename to ti_Data
#define		SELFP_Gadget					80000009		/// Gadget to grab dir/file from - Overrides SELFP_Dir and SELFP_File
#define		SELFP_StripFormats				80000010		/// Strips %s

	/// Tags for xFileATxt

#define		FI_Size							80000001
#define		FI_Protection					80000002
#define		FI_Date							80000003
#define		FI_Filetype						80000004
#define		FI_ExtractedFullName			80000005

	/// Type of check

#define		CHECKTYPE_PRESPECIFIED			1
#define		CHECKTYPE_PRESPECIFIEDTIMECHECK	2
#define		CHECKTYPE_SPECIFIED				3
#define		CHECKTYPE_CLIARGS				4
#define		CHECKTYPE_ICONS					5
#define		CHECKTYPE_FLOPPY				6

	/// When decrunching use

#define		WDU_UNPACKTHENXFD				0
#define		WDU_XFDTHENUNPACK				1
#define		WDU_UNPACK						2
#define		WDU_XFD							3
