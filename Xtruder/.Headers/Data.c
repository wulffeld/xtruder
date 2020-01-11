/*
**	$VER: Data.c (24.8.97)
**
**	Xtruder global data
**
**	Copyright (C) 1994-1997 Martin Wulffeld
**	All Rights Reserved
*/

#include	"Xtruder_Header.h"


	/// Version strings

UBYTE						ScreenTitle[]		= "Xtruder 3.7 - Copyright © 1994-1997 Martin Wulffeld";
UBYTE						AboutVersion[]		= "3.7";

	/// Library bases

extern struct ExecBase *	SysBase;
extern struct Library *		DOSBase;
struct Library *			IntuitionBase;
struct Library *			GfxBase;
struct Library *			GadToolsBase;
struct Library *			UtilityBase;
struct Library *			CxBase;
struct Library *			IconBase;
struct Library *			WorkbenchBase;
struct Library *			KeymapBase;
struct Library *			DiskfontBase;
struct Library *			AmigaGuideBase;
struct Library *			BootblockBase;
struct Library *			FilevirusBase;
struct Library *			UnpackBase;
struct Library *			xfdMasterBase;
struct Library *			FileIDBase;
struct Library *			XtruderBase;
struct Library *			ReqToolsBase;
struct Library *			DataTypesBase;

	/// AmigaGuide

struct NewAmigaGuide		NewGuide;
struct Process *			GuideProcess;
AMIGAGUIDECONTEXT			AGuide;
LONG						HelpID;

STRPTR						ContextList[] =
{
		/// Help menu
	
	"Main",
	"TheMenus",
	"FileCheck",
	"Menus.Bootblock.Interface",
	"Menus.Settings",
	"ARexxInterface",
	
		/// Menuhelp IDCMP
	
	"Menus.Project",
	"Menus.Files",
	"Menus.Bootblock",
	"Menus.Memory",
	"Menus.Miscellaneous",
	"Menus.Settings",
	"Menus.ARexx",
	
		/// Windows
	
	"Menus.Settings.GUI",
	"Menus.Settings.Startup",
	"Menus.Settings.Miscellaneous",
	"Menus.Settings.Paths",
	"Menus.Settings.History",
	"Menus.Settings.Bootblock",
	"Menus.Settings.ARexx",
	"Menus.Settings.FileCheck.Prespecified",
	"Menus.Settings.FileCheck.Miscellaneous",
	"Menus.Settings.FileCheck.Unpacking",
	"Menus.Settings.FileCheck.Archives",
	"Menus.Settings.FileCheck.Information",
	"Menus.Settings.FileCheck.PBC",
	"Menus.Settings.FileCheck.CheckWindow",
	"Menus.Project.ListKnown.BootblockVira",
	"Menus.Project.ListKnown.FileBasedVira",
	"Menus.Project.ListKnown.Crunchers",
	"Menus.Project.ListKnown.Patches",
	"Menus.Project.ListKnown.FileTypes",
	"Menus.Memory.CheckLVO",
	"Menus.Memory.Show.Libraries",
	"Menus.Memory.Show.Devices",
	"Menus.Memory.Show.Resources",
	"Menus.Memory.Show.Ports",
	"Menus.Memory.Show.Semaphores",
	"Menus.Memory.Show.Tasks",
	"Menus.Miscellaneous.VirusInformation",
	NULL
};

	/// ARexx

struct ARexxContext *		RexxStuff;
struct List *				ARexxScriptsList;
ULONG						LockID;
ULONG						ARexxSignal;

	/// Backfill

struct BackFillOptions		Options =
{
	256,256,
	// !!! 0,0,
	TRUE,FALSE,
	0,0,
	TRUE
};

struct BackFillInfo			BFInfo;

	/// Bootblock

struct Window *				BBWindow;
struct Gadget *				BBWindowGadList;
struct xGadget *			BBWindowGadgets;
struct List					BBBufferList;
UWORD						BBBufferListViewVisible;
UWORD						BBBoxWidth;

UWORD						Virus;

	/// CheckWindow

struct Window *				CheckWindow;
struct Gadget *				CheckWindowGadList;
struct xGadget *			CheckWindowGadgets;
struct CountList			CheckWindowHistoryList;
struct List					CheckWindowWarningsList;
UWORD						HistoryLVVisible;
UWORD						Box_Width;
UWORD						MemoryBox_Top;
UWORD						MemoryBox_Height;
UWORD						CheckInfoBox_Top;
UWORD						CheckInfoBox_Height;
BOOL						StillChecking;

ULONG						TimeSeconds;
ULONG						TimeMics;

UWORD						GAD_CW_CHECKHISTORY = GAD_CW_HISTORY + 1;
UWORD						GAD_CW_WARNINGS		= GAD_CW_HISTORY + 1;

	/// Disk

struct IOExtTD *			DiskUnit[4];
struct MsgPort *			DiskPort[4];
BOOL						DrvUnit[4]; // Which drives are present
BOOL						DiskPre[4]; // Which drives has a disk inserted

	/// File check

struct PrespecifiedList *	PreList;
UWORD						Indent;
struct List *				VirusList;

	/// FileID.library

struct FI_FileInfo *		FInfo;

	/// filevirus.library

struct FilevirusNode *		FilevirusNode;

	/// Fonts

struct TextFont *			ScreenFont;
struct TextFont *			TextFont;
struct TextFont *			RequesterFont;

struct TextAttr				ScreenFontAttr		= {"topaz.font",8,FS_NORMAL,FPF_ROMFONT};
struct TextAttr				TextFontAttr		= {"topaz.font",8,FS_NORMAL,FPF_ROMFONT};
struct TextAttr				RequesterFontAttr	= {"topaz.font",8,FS_NORMAL,FPF_ROMFONT};
struct TextAttr				Topaz8Attr			= {"topaz.font",8,FS_NORMAL,FPF_ROMFONT};

	/// Gadgets

struct TagItem				DisabledFalseTags[]	= {{GA_Disabled, FALSE},{TAG_END}};
struct TagItem				DisabledTrueTags[]	= {{GA_Disabled, TRUE},{TAG_END}};

	/// Graphics

UWORD						ScreenBarHeight;
UWORD						ConfirmGadWidth;
ULONG						ScrFontYSize;
ULONG						TxtFontYSize;
ULONG						TxtFontXSize;
ULONG						GadgetHeight;
ULONG						ActionRecX0;
ULONG						ActionRecX1;
ULONG						ActionRecY0;
ULONG						ActionRecY1;
ULONG						ActionWidth;
ULONG						ActionHeight;
ULONG						ActionBoxL;
ULONG						ActionBoxT;
ULONG						ActionBoxW;
ULONG						ActionBoxH;
ULONG						MaxYChars;
ULONG						MaxXChars;
ULONG						WriteX;
UWORD						InitialWriteY;
UWORD						CurrentWriteY;
UWORD						MaxWriteY;
UWORD						KeyWord;

struct ColorSpec			ColorSpec[]			= 
{
	{0, 0xA, 0xA, 0xA},
	{1, 0x0, 0x0, 0x0},
	{2, 0xF, 0xF, 0xF},
	{3, 0x6, 0x8, 0xA},
	{-1,0x0, 0x0, 0x0}
};

ULONG						ColorTable[14]		=
{
	0x00040000,
	0xa8a8a8a8,0xaaaaaaaa,0xaeaeaeae,
	0x00000000,0x00000000,0x00000000,
	0xffffffff,0xffffffff,0xffffffff,
	0x60606060,0x7f7f7f7f,0xafafafaf,
	0x00000000
};

UWORD __chip				WaitPtr[]			=
{
	0x0000,0x0000,
	0x0400,0x07C0,
	0x0000,0x07C0,
	0x0100,0x0380,
	0x0000,0x07E0,
	0x07C0,0x1FF8,
	0x1FF0,0x3FEC,
	0x3FF8,0x7FDE,
	0x3FF8,0x7FBE,
	0x7FFC,0xFF7F,
	0x7EFC,0xFFFF,
	0x7FFC,0xFFFF,
	0x3FF8,0x7FFE,
	0x3FF8,0x7FFE,
	0x1FF0,0x3FFC,
	0x07C0,0x1FF8,
	0x0000,0x07E0,
	0x0000,0x0000,
};

UWORD						Pens[]				= { ~0 };

	/// GUI

struct Screen *				Screen;
struct SignalSemaphore		GUISemaphore;
struct Window *				Window;
ULONG						WindowSignal;
struct Menu *				Menu;
struct RastPort *			RastPort;
struct DrawInfo *			DrawInfo;
APTR						VisualInfo;
struct AppWindow *			AppWin;
struct MsgPort *			AppWinPort;
ULONG						AppWinSignal;
struct Gadget *				ListGadget;
struct Gadget *				GadgetList;
struct DiskObject *			AppIconDiskObject;
struct MsgPort *			AppIconPort;
struct AppIcon *			AppIcon;
ULONG						AppIconSignal;
struct Requester			NullReq;
Class *						ButtonClass;
APTR						OldWin;
BOOL						OnPublic;
BOOL						PublicScrLocked;
UBYTE						PublicScrName[140];

struct NewMenu				XtruderMenu[]		=
{
	{NM_TITLE,	0,				0,	0,	0,	0,},		/// Project
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	
	{NM_TITLE,	0,				0,	0,	0,	0,},		/// Files
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	
	{NM_TITLE,	0,				0,	0,	0,	0,},		/// Bootblock
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	
	{NM_TITLE,	0,				0,	0,	0,	0,},		/// Memory
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	
	{NM_TITLE,	0,				0,	0,	0,	0,},		/// Miscellaneous
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	
	{NM_TITLE,	0,				0,	0,	0,	0,},		/// Settings
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_SUB,	0,				0,	0,	0,	0,},
	{NM_ITEM,	NM_BARLABEL,	0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	
	{NM_TITLE,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_END,	0,				0,	0,	0,	0,},

	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},

	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},

	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},

	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},

	{NM_END,	0,				0,	0,	0,	0,},
};

struct NewMenu				SearchMenu[] =
{
	{NM_TITLE,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	0,	0,	0,},
	{NM_ITEM,	0,				0,	CHECKIT|MENUTOGGLE, 0, 0,},
	{NM_END,	NULL,			0,	0,	0,	0,},
};

	/// History

struct XtruderHistory *		XtruderHistory;

	/// Miscellaneous

struct Process *			Process;
struct Process *			CommodityProcess;
struct MsgPort *			XtruderCPort;
struct XtruderStrings *		XStrings;
ULONG						XPortSignal;
ULONG						KillSignal;
BOOL						SecondCopy;					/// TRUE if this is the second copy
UBYTE						RegName[ 30 ];
BPTR						RegFile;
struct Hook					CursorDownHook = { NULL, NULL, (HOOKFUNC)xCursorDownPatchFunc, NULL, NULL };

	/// Reports: Check (window)

struct Window *				CHIWindow;
struct Gadget *				CHIWindowGadList;
struct xGadget *			CHIWindowGadgets;
struct CountList			CHIHistoryList;
UWORD						CHIHistoryListViewVisible;
UWORD						CHIBoxWidth;
UWORD						CHICheckInfoBox_Top;
UWORD						CHICheckInfoBox_Height;
BOOL						CHIStillChecking;

	/// Reports: Create (window)

struct Window *				CRIWindow;
struct Gadget *				CRIWindowGadList;
struct xGadget *			CRIWindowGadgets;
struct List					CRIErrorsList;
UWORD						CRIErrorsListViewVisible;
UWORD						CRIBoxWidth;
BOOL						StillCreatingReport;

	/// Reports: Main window

struct Window *				MRIWindow;
struct Gadget *				MRIWindowGadList;
struct xGadget *			MRIWindowGadgets;
struct List					MRIReportsList;
UWORD						MRIReportsListViewVisible;
UWORD						MRIBoxWidth;

	/// reqtools.library

struct rtFileRequester *	FileReq;

	/// Settings

struct XtruderSettings *	XSettings;
struct XtruderWinCoords *	Coords;
BOOL						SettingsChanged;

	/// unpack.library

struct UnpackInfo *			UInfo;
UWORD						UPTypes;

	/// Virus information

struct Window *				VIWindow;
struct Gadget *				VIWindowGadList;
struct xGadget *			VIWindowGadgets;
UWORD						VIListViewVisible;
struct List	*				DescriptionsList;

	/// xfdmaster.library

struct xfdBufferInfo *		XFDInfo;

	/// xtruder.library

struct XtruderInfo *		XInfo;
