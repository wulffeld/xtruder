/*
**	$VER: Xtruder_Structures.h (19.8.96)
**
**	Custom structures used in Xtruder
**
**	Copyright (C) 1994-1996 Martin Wulffeld
**	All Rights Reserved
*/


struct XtruderSettings
{
	UBYTE		xs_BOO_CustomBBFile[ DEFAULTBUFFERLEN ];
	BOOL		xs_BOO_CheckInsertedDisks;
	BOOL		xs_BOO_NoInterface;
	
	UBYTE		xs_FC_MIS_MatchFilter[ DEFAULTBUFFERLEN ];
	UBYTE		xs_FC_MIS_ArchiveFilter[ DEFAULTBUFFERLEN ];
	UWORD		xs_FC_MIS_ActionWhenVirusFound;
	UWORD		xs_FC_MIS_ActionWhenVirusCopied;
	UBYTE		xs_FC_MIS_VirusComment[ 80 ];
	UBYTE		xs_FC_MIS_VirusRenameExtension[ 32 ];
	ULONG		xs_FC_MIS_SafeMemoryLimit;
	BOOL		xs_FC_MIS_AutoDeprotect;
	BOOL		xs_FC_MIS_EnterRecursively;
	BOOL		xs_FC_MIS_AutoPathCheck;
	UWORD		xs_FC_MIS_PathIndention;
	BOOL		xs_FC_MIS_SuppressSystemMessages;
	
	UWORD		xs_FC_UNP_WhenDecrunchingUse;
	BOOL		xs_FC_UNP_ExtractArchives;
	BOOL		xs_FC_UNP_AskBeforeExtracting;
	BOOL		xs_FC_UNP_AskBeforeDeletingExtFiles;
	BOOL		xs_FC_UNP_DecrunchExecutables;
	BOOL		xs_FC_UNP_DecrunchDataFiles;
	BOOL		xs_FC_UNP_AskBeforeDecrunching;
	BOOL		xs_FC_UNP_AskBeforeDeletingDecFiles;
	BOOL		xs_FC_UNP_DecrunchEncryptedFiles;
	BOOL		xs_FC_UNP_CheckDMSBootblock;
	
	UWORD		xs_FC_ARC_LhAExtractRoutine;
	BOOL		xs_FC_ARC_ExtractLhA;
	ULONG		xs_FC_ARC_LhAMaxSize;
	UWORD		xs_FC_ARC_LZXExtractRoutine;
	BOOL		xs_FC_ARC_ExtractLZX;
	ULONG		xs_FC_ARC_LZXMaxSize;
	UBYTE		xs_FC_ARC_Output[ DEFAULTBUFFERLEN ];
	
	BOOL		xs_FC_INF_WrtInfo_Size;
	BOOL		xs_FC_INF_WrtInfo_Filetype;
	BOOL		xs_FC_INF_WrtInfo_ProtBits;
	BOOL		xs_FC_INF_WrtInfo_Date;
	
	LONG		xs_FC_PBC_PBCBits;
	BOOL		xs_FC_PBC_CheckPBCFiles;
	BOOL		xs_FC_PBC_SetPBCOnExe;
	BOOL		xs_FC_PBC_SetPBCOnArchives;
	
	BOOL		xs_FC_CHW_AutoClose;
	BOOL		xs_FC_CHW_MemoryInformation;
	BOOL		xs_FC_CHW_CheckInformation;
	BOOL		xs_FC_CHW_History;
	BOOL		xs_FC_CHW_AutoScroll;
	BOOL		xs_FC_CHW_Warnings;
	
	ULONG		xs_GUI_ScreenMode;
	UBYTE		xs_GUI_PublicScreenName[ MAXPUBSCREENNAME + 1 ];
	ULONG		xs_GUI_ScreenOverscan;
	ULONG		xs_GUI_ScreenWidth;
	ULONG		xs_GUI_ScreenHeight;
	ULONG		xs_GUI_ScreenDepth;
	ULONG		xs_GUI_AutoScroll;
	UBYTE		xs_GUI_ScreenFontName[ 32 ];
	UWORD		xs_GUI_ScreenFontSize;
	UWORD		xs_GUI_ScreenFontStyle;
	UBYTE		xs_GUI_TextFontName[ 32 ];
	UWORD		xs_GUI_TextFontSize;
	UBYTE		xs_GUI_RequesterFontName[ 32 ];
	UWORD		xs_GUI_RequesterFontSize;
	UWORD		xs_GUI_RequesterFontStyle;
	UBYTE		xs_GUI_BackgroundPattern[ DEFAULTBUFFERLEN ];
	UWORD		xs_GUI_ScrollerWidth;
	BOOL		xs_GUI_HideAppIcon;
	
	UBYTE		xs_HIS_HistoryPath[ DEFAULTPATHLEN ];
	UBYTE		xs_HIS_HistoryFile[ DEFAULTFILELEN + 1 ];
	BOOL		xs_HIS_UseDateAndTime;
	BOOL		xs_HIS_Append;
	BOOL		xs_HIS_RequestFilename;
	UWORD		xs_HIS_MaxEntries;
	UWORD		xs_HIS_SaveMethod;
	BOOL		xs_HIS_RecFileCheck;
	
	UBYTE		xs_MIS_CxPopkey[40];
	WORD		xs_MIS_CxPriority;
	BOOL		xs_MIS_CxPopup;
	BOOL		xs_MIS_StayHidden;
	
	UBYTE		xs_PAT_ArchiveExtractPath[ DEFAULTPATHLEN ];
	UBYTE		xs_PAT_FileDecrunchPath[ DEFAULTPATHLEN ];
	UBYTE		xs_PAT_VirusPath[ DEFAULTPATHLEN ];
	UBYTE		xs_PAT_RecheckPath[ DEFAULTPATHLEN ];
	BOOL		xs_PAT_AutoCreateDrawers;
	BOOL		xs_PAT_AutoDeleteDrawers;
	UBYTE		xs_PAT_GuidePath[ DEFAULTPATHLEN ];
	
	UBYTE		xs_STA_StartupScript[ DEFAULTBUFFERLEN ];
	BOOL		xs_STA_ExecuteScript;
	BOOL		xs_STA_CheckPrespecified;
	BOOL		xs_STA_CheckVectors;
	BOOL		xs_STA_ShowSystemInformation;
	
		/// Reports
	
	UBYTE		xs_REP_ReportsPath[ DEFAULTBUFFERLEN ];
	
		/// Virus information
	
	UBYTE		xs_VIN_DescriptionsPath[ DEFAULTPATHLEN ];
	UBYTE		xs_VIN_ExternalViewer[ DEFAULTBUFFERLEN ];
};

struct XtruderWinCoords
{
	UWORD		xc_MainWindow[2];
	
	UWORD		xc_BootblockWindow[4];
	UWORD		xc_BootblockSelectWindow[2];			/// Only x and y are used
	UWORD		xc_CheckWindow[4];
	UWORD		xc_HistoryWindow[4];
	UWORD		xc_ListKnownBootblockViraWindow[4];
	UWORD		xc_ListKnownFileBasedViraWindow[4];
	UWORD		xc_ListKnownCrunchersWindow[4];
	UWORD		xc_ListKnownPatchesWindow[4];
	UWORD		xc_ListKnownFileTypesWindow[4];
	UWORD		xc_ListPrespecifiedWindow[4];
	UWORD		xc_MemoryLVOWindow[4];
	UWORD		xc_MemoryVectorsWindow[4];
	UWORD		xc_MemoryLibrariesWindow[4];
	UWORD		xc_MemoryDevicesWindow[4];
	UWORD		xc_MemoryResourcesWindow[4];
	UWORD		xc_MemoryPortsWindow[4];
	UWORD		xc_MemorySemaphoresWindow[4];
	UWORD		xc_MemoryTasksWindow[4];
	UWORD		xc_ReportsMainWindow[4];
	UWORD		xc_ReportsCreateWindow[4];
	UWORD		xc_ReportsCheckWindow[4];
	UWORD		xc_SystemInformationWindow[4];
	UWORD		xc_VirusInformationWindow[4];
};

struct XtruderArgs
{
	struct RDArgs *	xa_RArgs;
	LONG			xa_Args[NUM_ARGS];
};

/*
 * A structure for the ARexx handler context
 * This is *VERY* *PRIVATE* and should not be touched...
 */
struct ARexxContext
{
	struct MsgPort *	ARexxPort;		// The port messages come in at...
	struct Library *	RexxSysBase;	// We will hide the library pointer here...
	LONG				Outstanding;	// The count of outstanding ARexx messages...
	char				PortName[24];	// The port name goes here...
	char				ErrorName[28];	// The name of the <base>.LASTERROR...
	char				Extension[8];	// Default file name extension...
};

struct ARexxResults
{
	STRPTR	ar_ResultString;
	LONG	ar_ResultCode;
};

struct ARexxCommand
{
	STRPTR	ac_CommandName;
	STRPTR	ac_CommandTemplate;
};

struct ARexxNode
{
	struct Node	an_Node;
	UBYTE		an_MenuName[40];
	UBYTE		an_FileName[DEFAULTBUFFERLEN];
//	UBYTE		an_Output[100];			// For later :-)
	BOOL		an_Separator;
};

	/// Custom 'C' message structure

#define		XTCMD_KILL					1
#define		XTCMD_CHECKPRESPECIFIED		2

struct XtruderMsg
{
	struct Message	xt_Msg;
	ULONG			xt_Command;
	ULONG			xt_Value;
	UBYTE			xt_String[ DEFAULTBUFFERLEN ];
	ULONG			xt_ReturnValue;
};

struct AppString
{
	LONG	as_ID;
	STRPTR	as_Str;
};

struct CountList
{
	struct List	cl_List;
	ULONG		cl_Entries;
};

struct SettingsGUI
{
	struct Screen *	sg_Screen;
	struct Window *	sg_Window;
	struct Window *	sg_OldProcessWin;
};

struct xGadget
{
	struct xGadget *	xg_NextGad;
	struct Gadget *		xg_Gad;
	UWORD				xg_GadgetID;
	STRPTR				xg_GadgetText;
};

struct NewLVPos
{
	UWORD	nl_NewTop;
	UWORD	nl_NewSelected;
};

struct PrespecifiedList
{
	struct List	pl_List;
	UWORD		pl_Entries;
};

struct PrespecifiedNode
{
	struct Node	pn_Node;
	UBYTE		pn_Name[ DEFAULTBUFFERLEN ];
};

struct ArchiveNode
{
	struct Node	an_Node;
	UBYTE		an_Name[ DEFAULTBUFFERLEN ];
	ULONG		an_ExtractedSize;
};

struct NameNode
{
	struct Node	nn_Node;
	UBYTE *		nn_Name;
};

struct SysInfoNode
{
	struct Node	sn_Node;
	UBYTE		sn_Name[ 300 ];
	UWORD		sn_Color;
};

/*
 * Used in the main report window listview
 */
struct ReportNode
{
	struct Node			rn_Node;
	UBYTE				rn_Filename[ DEFAULTBUFFERLEN ];
	UBYTE				rn_ListViewName[ DEFAULTBUFFERLEN + 20 ];
};

/*
 * Used in VirusList
 */
struct VirusFoundNode
{
	struct Node	vn_Node;
	UBYTE		vn_VirusName[ 50 ];
	UBYTE		vn_FileName[ 256 ];
};

struct CurrentReport
{
	UWORD			cr_Result;						// Result of check
	ULONG			cr_HandleMismatch;				// How to handle a mismatch
	ULONG			cr_PreviousHandleMismatch;		// For Copy all to
	UBYTE			cr_CopyToPath[ DEFAULTBUFFERLEN ];	// Copy to path
	ULONG			cr_FilesChecked;				// How many files did we check
	ULONG			cr_MismatchesFound;
	UBYTE			cr_ReportFileName[ 33 ];		// The name of the report file to save to
	BPTR			cr_ReportFile;					// Filehandle to the report file
	UBYTE *			cr_ReportBuffer;				// Buffer to the report file
	UBYTE			cr_ReportPath[ DEFAULTBUFFERLEN ];	// Leading path for the files
};

struct FileReport
{
	UBYTE				fr_FileName[ DEFAULTBUFFERLEN ];	// Full name
	ULONG				fr_Size;
	ULONG				fr_Protection;
	struct DateStamp	fr_Date;
	UBYTE				fr_Comment[ 80 ];
	ULONG				fr_CRC;
};

struct CurrentCheck
{
	UWORD			cc_Result;						// Result of check
	ULONG			cc_ArchivedFilesEncountered;	// How many archived files did we encounter
	ULONG			cc_ArchivedFilesExtracted;		// How many archived files did we fully extract and check
	ULONG			cc_DirsEncountered;				// How many dirs did we encounter
	ULONG			cc_FilesEncountered;			// How many files did we encounter
	ULONG			cc_FilesChecked;				// How many files did we check
	ULONG			cc_FilesCrunched;				// How many crunched files did we find
	ULONG			cc_FilesDecrunched;				// How many crunched files did we decrunch
	ULONG			cc_ViraFound;					// How many vira did we find
	struct List		cc_DeleteList;					// Files to delete at the end of a check
};

	/// Check...

struct CurrentFile
{
	UBYTE			cf_FileName[DEFAULTBUFFERLEN];	// Current ROOT filename
	UBYTE			cf_PathName[DEFAULTBUFFERLEN];	// Real path of file
	UBYTE			cf_VirusName[DEFAULTBUFFERLEN];	// The name of the virus
	UBYTE			cf_CruncherName[32];		// If file has been decrunched this will hold the crunchers name
	UBYTE *			cf_Buffer;					// If NON-NULL the file itself
	ULONG			cf_BufferSize;				// Size of 'cf_Buffer'
	UBYTE			cf_Comment[82];				// Comment
	ULONG			cf_Size;					// Size of file
	ULONG			cf_Protection;				// Protection bits of file
	UBYTE			cf_Date[50];				// Date of file
	UWORD			cf_ArchiveType;				// Defines otherwere
	
	ULONG			cf_Result;					// Miscellaneous
	
	BOOL			cf_FileActedUpon;			// Has file been renamed etc.
};

	/// History...

struct XtruderHistory
{
	struct List	his_List;
	ULONG		his_Entries;
	ULONG		his_ViraFound;
};

struct HistoryNode
{
	struct Node	hn_Node;
	UBYTE *		hn_Name;
	UWORD		hn_Color;
};

	/// Miscellaneous...

struct XtruderStrings
{
	UBYTE	xs_ReqPath[DEFAULTPATHLEN];
	UBYTE	xs_ReqFile[DEFAULTBUFFERLEN];
	
	UBYTE	xs_RexxPath[DEFAULTPATHLEN];
	UBYTE	xs_RexxFile[DEFAULTBUFFERLEN];
	
	UBYTE	xs_ReportPath[DEFAULTPATHLEN];
	UBYTE	xs_ReportFile[DEFAULTBUFFERLEN];
	
	UBYTE	xs_Bootblock[BOOTBLOCKSIZE];
	
	UBYTE	xs_RegName[ 512 ];
	
	UBYTE	xs_Strbuf[7][DEFAULTBUFFERLEN];
	
	UBYTE	xs_SearchStrings[6][100];
	
	UBYTE	xs_Indention[DEFAULTBUFFERLEN];
	
	UBYTE	xs_PrefsName[ DEFAULTBUFFERLEN ];
};

#define		RPATH			XStrings->xs_ReqPath
#define		RFILE			XStrings->xs_ReqFile

#define		REXXPATH		XStrings->xs_RexxPath
#define		REXXFILE		XStrings->xs_RexxFile

#define		STRBUF0			XStrings->xs_Strbuf[0]
#define		STRBUF1			XStrings->xs_Strbuf[1]
#define		STRBUF2			XStrings->xs_Strbuf[2]
#define		STRBUF3			XStrings->xs_Strbuf[3]
#define		STRBUF4			XStrings->xs_Strbuf[4]
#define		STRBUF5			XStrings->xs_Strbuf[5]
#define		STRBUF6			XStrings->xs_Strbuf[6]

#define		INDENT			XStrings->xs_Indention
