/*
**	$VER: Xtruder_Data.h (29.7.96)
**
**	Xtruder global data header file for the individual sources
**
**	Copyright (C) 1994-1996 Martin Wulffeld
**	All Rights Reserved
*/

extern UBYTE						ScreenTitle[];
extern UBYTE						AboutVersion[];

	/// Library bases

extern struct ExecBase *			SysBase;
extern struct Library *				DOSBase;
extern struct Library *				IntuitionBase;
extern struct Library *				GfxBase;
extern struct Library *				GadToolsBase;
extern struct Library *				UtilityBase;
extern struct Library *				CxBase;
extern struct Library *				IconBase;
extern struct Library *				WorkbenchBase;
extern struct Library *				KeymapBase;
extern struct Library *				DiskfontBase;
extern struct Library *				AmigaGuideBase;
extern struct Library *				BootblockBase;
extern struct Library *				FilevirusBase;
extern struct Library *				UnpackBase;
extern struct Library *				FileIDBase;
extern struct Library *				XtruderBase;
extern struct Library *				xfdMasterBase;
extern struct Library *				ReqToolsBase;
extern struct Library *				DataTypesBase;

	/// AmigaGuide

extern struct NewAmigaGuide			NewGuide;
extern struct Process *				GuideProcess;
extern AMIGAGUIDECONTEXT			AGuide;
extern LONG							HelpID;
extern STRPTR						ContextList[];

	/// ARexx

extern struct ARexxContext *		RexxStuff;
extern struct List *				ARexxScriptsList;
extern ULONG						LockID;
extern ULONG						ARexxSignal;

	/// Backfill

extern struct BackFillOptions		Options;
extern struct BackFillInfo			BFInfo;

	/// Bootblock

extern struct Window *				BBWindow;
extern struct Gadget *				BBWindowGadList;
extern struct xGadget *				BBWindowGadgets;
extern struct List					BBBufferList;
extern UWORD						BBBufferListViewVisible;
extern UWORD						BBBoxWidth;

extern UWORD						Virus;

	/// CheckWindow

extern struct Window *				CheckWindow;
extern struct Gadget *				CheckWindowGadList;
extern struct xGadget *				CheckWindowGadgets;
extern struct CountList				CheckWindowHistoryList;
extern struct List					CheckWindowWarningsList;
extern UWORD						HistoryLVVisible;
extern UWORD						Box_Width;
extern UWORD						MemoryBox_Top;
extern UWORD						MemoryBox_Height;
extern UWORD						CheckInfoBox_Top;
extern UWORD						CheckInfoBox_Height;
extern BOOL							StillChecking;
extern ULONG						TimeSeconds;
extern ULONG						TimeMics;

extern UWORD						GAD_CW_CHECKHISTORY;
extern UWORD						GAD_CW_WARNINGS;

	/// Disk

extern struct IOExtTD *				DiskUnit[];
extern struct MsgPort *				DiskPort[];
extern BOOL							DrvUnit[];
extern BOOL							DiskPre[];

	/// File check

extern struct PrespecifiedList *	PreList;
extern UWORD						Indent;
extern struct List *				VirusList;

	/// FileID.library

extern struct FI_FileInfo *			FInfo;

	/// Filevirus.library

extern struct FilevirusNode *		FilevirusNode;

	/// Fonts

extern struct TextFont *			ScreenFont;
extern struct TextFont *			TextFont;
extern struct TextFont *			RequesterFont;
extern UWORD						ScreenBarHeight;

extern struct TextAttr				ScreenFontAttr;
extern struct TextAttr				TextFontAttr;
extern struct TextAttr				RequesterFontAttr;
extern struct TextAttr				Topaz8Attr;

	/// Gadgets

extern struct TagItem				DisabledFalseTags[];
extern struct TagItem				DisabledTrueTags[];

	/// Graphics

extern UWORD						ConfirmGadWidth;
extern ULONG						ScrFontYSize;
extern ULONG						TxtFontYSize;
extern ULONG						TxtFontXSize;
extern ULONG						GadgetHeight;
extern ULONG						ActionRecX0;
extern ULONG						ActionRecX1;
extern ULONG						ActionRecY0;
extern ULONG						ActionRecY1;
extern ULONG						ActionWidth;
extern ULONG						ActionHeight;
extern ULONG						ActionBoxL;
extern ULONG						ActionBoxT;
extern ULONG						ActionBoxW;
extern ULONG						ActionBoxH;
extern ULONG						MaxYChars;
extern ULONG						MaxXChars;
extern ULONG						WriteX;
extern UWORD						InitialWriteY;
extern UWORD						CurrentWriteY;
extern UWORD						MaxWriteY;

extern struct ColorSpec				ColorSpec[]; 
extern ULONG						ColorTable[];
extern UWORD __chip					WaitPtr[];
extern UWORD						Pens[];

	/// GUI

extern struct Screen *				Screen;
extern struct SignalSemaphore		GUISemaphore;
extern struct Window *				Window;
extern ULONG						WindowSignal;
extern struct Menu *				Menu;
extern struct RastPort *			RastPort;
extern struct DrawInfo *			DrawInfo;
extern APTR							VisualInfo;
extern struct AppWindow *			AppWin;
extern struct MsgPort *				AppWinPort;
extern ULONG						AppWinSignal;
extern struct Gadget *				ListGadget;
extern struct Gadget *				GadgetList;
extern struct DiskObject *			AppIconDiskObject;
extern struct MsgPort *				AppIconPort;
extern struct AppIcon *				AppIcon;
extern ULONG						AppIconSignal;
extern struct Requester				NullReq;
extern Class *						ButtonClass;
extern APTR							OldWin;
extern BOOL							OnPublic;
extern BOOL							PublicScrLocked;
extern UBYTE						PublicScrName[];
extern struct NewMenu				XtruderMenu[];
extern struct NewMenu				SearchMenu[];

	/// History

extern struct XtruderHistory *		XtruderHistory;

	/// Miscellaneous

extern struct Process *				Process;
extern struct Process *				CommodityProcess;
extern struct MsgPort *				XtruderCPort;
extern struct XtruderStrings *		XStrings;
extern ULONG						XPortSignal;
extern ULONG						KillSignal;
extern BOOL							SecondCopy;
extern UBYTE						RegName[];
extern BPTR							RegFile;
extern UWORD						KeyWord;
extern struct Hook					CursorDownHook;

	/// Reports: Check (window)

extern struct Window *				CHIWindow;
extern struct Gadget *				CHIWindowGadList;
extern struct xGadget *				CHIWindowGadgets;
extern struct CountList				CHIHistoryList;
extern UWORD						CHIHistoryListViewVisible;
extern UWORD						CHIBoxWidth;
extern UWORD						CHICheckInfoBox_Top;
extern UWORD						CHICheckInfoBox_Height;
extern BOOL							CHIStillChecking;

	/// Reports: Create (window)

extern struct Window *				CRIWindow;
extern struct Gadget *				CRIWindowGadList;
extern struct xGadget *				CRIWindowGadgets;
extern struct List					CRIErrorsList;
extern UWORD						CRIErrorsListViewVisible;
extern UWORD						CRIBoxWidth;
extern BOOL							StillCreatingReport;

	/// Reports: Main (window)

extern struct Window *				MRIWindow;
extern struct Gadget *				MRIWindowGadList;
extern struct xGadget *				MRIWindowGadgets;
extern struct List					MRIReportsList;
extern UWORD						MRIReportsListViewVisible;
extern UWORD						MRIBoxWidth;

	/// reqtools.library

extern struct rtFileRequester *		FileReq;

	/// Settings

extern struct XtruderSettings *		XSettings;
extern struct XtruderWinCoords *	Coords;
extern BOOL							SettingsChanged;

	/// Strings

extern struct AppString				AS[];

	/// unpack.library

extern struct UnpackInfo *			UInfo;
extern UWORD						UPTypes;

	/// Virus information

extern struct Window *				VIWindow;
extern struct Gadget *				VIWindowGadList;
extern struct xGadget *				VIWindowGadgets;
extern UWORD						VIListViewVisible;
extern UWORD						VIBoxWidth;
extern struct List *				DescriptionsList;

	/// xfdmaster.library

extern struct xfdBufferInfo *		XFDInfo;

	/// xtruder.library

extern struct XtruderInfo *			XInfo;
