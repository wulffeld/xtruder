/*
**	$VER: Xtruder_ProtoTypes.c (15.8.96)
**
**	Xtruder prototypes
**
**	Copyright (C) 1993-1996 Martin Wulffeld
**	All Rights Reserved
*/

	/// About.c

VOID					xAbout( UBYTE );

	/// AmigaGuide.c

VOID __saveds			xGuideServer( VOID );
BYTE					xGuideLaunch( LONG );
VOID					xGuideCleanup( VOID );
VOID 					xGuideDisplay( LONG );
VOID					xMenuHelp( UWORD );

	/// ARexx.c

struct	ARexxContext *	xInitARexx( STRPTR, STRPTR );
VOID					xFreeARexx( struct ARexxContext * );
STRPTR					xARexxName( struct ARexxContext * );
ULONG					xARexxSignal( struct ARexxContext * );
WORD					xSendARexxMsg( struct ARexxContext *, STRPTR, WORD );
struct RexxMsg *		xGetARexxMsg( struct ARexxContext * );
VOID					xReplyARexxMsg( struct ARexxContext *, struct RexxMsg *, STRPTR, LONG );
WORD					xSetARexxLastError( struct ARexxContext *, struct RexxMsg *, STRPTR );

	/// ARexxCommand.c

VOID					xHandleARexxMsg( VOID );
struct ARexxResults *	xHandleARexxCommand( LONG *, UWORD );

#pragma libcall rexxcontext->RexxSysBase CreateRexxMsg 90 09803
#pragma libcall rexxcontext->RexxSysBase CreateArgstring 7E 0802
#pragma libcall rexxcontext->RexxSysBase DeleteRexxMsg 96 801
#pragma libcall rexxcontext->RexxSysBase DeleteArgstring 84 801
#pragma libcall rexxcontext->RexxSysBase IsRexxMsg A8 801

	/// The prototypes for the few ARexx functions we will call... Functions in amiga.lib

struct RexxMsg *CreateRexxMsg( struct MsgPort *, STRPTR, STRPTR );
VOID *			CreateArgstring( char *,long );
VOID			DeleteRexxMsg( struct RexxMsg * );
VOID			DeleteArgstring( char * );
BOOL			IsRexxMsg( struct Message * );

	/// Prototypes for the RVI ARexx calls. (RexxVars.o)

__stdargs long	CheckRexxMsg( struct RexxMsg * );
__stdargs long	GetRexxVar( struct RexxMsg *, char *, char ** );
__stdargs long	SetRexxVar( struct RexxMsg *, char *, char *, long );

	/// Backfill.c

VOID					CopyTiledBitMap( struct BitMap *, WORD, WORD, WORD, WORD, struct BitMap *, struct Rectangle * );
VOID __saveds __asm		WindowPatternBackFillFunc( register __a0 struct Hook *, register __a2 struct RastPort *, register __a1 struct BackFillMsg * );
VOID					CalculateCopySizes( struct BackFillInfo * );
BOOL					LoadBackgroundImage( struct BackFillInfo *, STRPTR, struct Screen *, struct BackFillOptions * );
VOID					UnloadBackgroundImage( struct BackFillInfo * );


	/// Boopsi.c

Class *					xInitButtonGadgetClass( VOID );
ULONG ASM __saveds		xDispatchButtonGadget( REGA0 Class *, REGA2 Object *, REGA1 Msg );
VOID					xDrawPoints( struct RastPort *, WORD, WORD, WORD, WORD * );
UWORD					xStrUcpy( STRPTR, STRPTR );
ULONG					xRenderButton( Class *, struct Gadget *, struct gpRender *, struct ButtonData *, BOOL );

	/// BBAuxiliary.c

BOOL					xReadBoot( WORD );
BOOL					xReadBBBrainFile( VOID );

	/// BBBinary.c

VOID					xLoadCustomBootblock( VOID );
VOID					xSaveAsBinary( VOID );

	/// BBCheck.c

VOID					xCheckBootblockProcess( WORD, STRPTR );
VOID					xCheckDrive( WORD, STRPTR );
struct Bootblock *		xAnalyzeAndUpdateBufferGad( WORD, STRPTR, LONG * );
VOID					xHandleBBCheckResult( struct Bootblock *, WORD, LONG, STRPTR );

	/// BBDMS.c

BOOL					xReadDMSBootblock( STRPTR, BOOL );
BOOL __saveds			xGetDMSBootblockJumpRoutine( VOID );

	/// BBInstall.c

BOOL					xInstallBootblockProcess( WORD, WORD );
BOOL					xInstallDrive( WORD, WORD, UBYTE * );

	/// BBShow.c

BOOL					xShowBootblock( WORD, STRPTR, BOOL );

	/// BBWindow.c

UWORD					xCalc_BBTopGadgetsWidth( VOID );
UWORD					xCalc_BBListViewHeight( VOID );
UWORD					xCalc_BBMinWindowWidth( VOID );
UWORD					xCalc_BBMinWindowHeight( VOID );
VOID					xRemoveBBWindowGadgets( VOID );
BOOL					xCreateBBWindowGadgets( VOID );
VOID					xOpenBBWindow( UWORD, WORD );
VOID					xCloseBBWindow( VOID );
VOID					xHandleBBWindowIDCMP( UWORD, BOOL, WORD );
VOID					xUpdateLoadSaveGadgets( VOID );
VOID __stdargs			xBB_UpdateStatusGadget( BOOL, STRPTR, ... );
VOID __stdargs			xBB_UpdateBufferGadget( BOOL, STRPTR, ... );
VOID					xCreateBootblockList( VOID );
VOID					xConvertBBLine( UBYTE *, UBYTE * );
VOID					xConvertBBLineToHex( UBYTE *, UBYTE * );
VOID					xHandleInsertedDisk( VOID );
VOID					xLocalHandleInsertedDisk( VOID );
VOID					xPathCheckDisk( WORD );

	/// Check.c

VOID					xDoObject( struct CurrentCheck *, STRPTR );
ULONG					xCHK_HandleFileProtection( STRPTR, ULONG );
ULONG					xCHK_HandleFilePBC( STRPTR, ULONG );
VOID					xCHK_DoFile( struct CurrentCheck *, struct CurrentFile * );
UBYTE					xCHK_IsFileCrunched( struct CurrentFile *, STRPTR, UBYTE *, ULONG );
UBYTE					xCHK_CheckBuffer( STRPTR, ULONG );
UBYTE					xCHK_IsFileExecutable( UBYTE * );

	/// CheckDecrunch.c

VOID					xCHK_DecrunchExeOrDataAndCheck( struct CurrentCheck *, struct CurrentFile *, struct CurrentFile *, UBYTE );

	/// CheckExtract.c

VOID					xCHK_DoExtractedFile( struct CurrentCheck *, struct CurrentFile *, struct CurrentFile * );
BOOL					xCHK_ExtractArchive( struct CurrentCheck *, struct CurrentFile *, struct CurrentFile * );
VOID					xCHK_CheckExtractedArchive( struct CurrentCheck *, struct CurrentFile * );
BOOL					xIsFileLZX( STRPTR );
struct List *			xSetUpInternal_1_ArchiveList( STRPTR, UWORD, struct CurrentCheck * );
struct List *			xSetUpInternal_2_ArchiveList( STRPTR, UWORD, struct CurrentCheck * );
VOID					xSortArchiveNodes( struct List * );
VOID					xAddArchiveNode( struct List *, STRPTR, ULONG );
VOID					xFreeArchiveNodes( struct List * );

	/// CheckHistory.c

VOID __stdargs			xAddFileCheckHistoryNodes( UWORD, STRPTR, ... );

	/// CheckLVO.c

VOID					xCheckLVO( BOOL );
BOOL					xCheckLibrary( struct Library *, WORD );
BOOL					xAnalyseNonStandardVectors( struct List *, WORD, UWORD *, UWORD *, UWORD * );
BOOL					xDoLVOCheck( struct List *, WORD, ULONG, UWORD *, UWORD *, UWORD * );
BOOL					xPrepareLVOHeader( struct List *, struct Library * );

	/// CheckMiscellaneous.c

VOID					xCHK_HandleVirus( struct CurrentCheck *, struct CurrentFile * );
VOID					xCHK_HandleVirusInArchive( struct CurrentCheck *, struct CurrentFile *, struct CurrentFile * );
VOID					xCHK_HandleVirusCopied( struct CurrentCheck *, struct CurrentFile * );
VOID					xCHK_DisableInfectedFile( struct CurrentFile * );
VOID					xCHK_AddFileToDelete( struct CurrentCheck *, struct CurrentFile * );
VOID					xCHK_DeleteInfectedFiles( struct List * );
VOID					xCHK_RenameInfectedFile( struct CurrentFile * );
VOID					xCHK_CommentInfectedFile( struct CurrentFile * );
VOID __stdargs			xCHK_ATxt( BOOL, BOOL, STRPTR, ... );
LONG					xCHK_DeleteAll( STRPTR, BOOL );
VOID					xCHK_ConvertProtectionBitsToStr( ULONG, STRPTR );
STRPTR					xCHK_FormatFileDate( struct FileInfoBlock *, STRPTR );
STRPTR					xCHK_FileType( STRPTR, STRPTR );

	/// CheckStart.c

LONG					xStartCheck( struct List *, UBYTE, BOOL );
VOID					xAddWarnings( VOID );
VOID					xAddCheckInfoToHistory( struct CurrentCheck * );
VOID					xCopyList( struct List *, struct List * );
BOOL					xCheckPathsExistance( VOID );
VOID					xInformCheckEnd( struct CurrentCheck *, ULONG );
VOID					xListPrespecified( VOID );

	/// CheckVectors.c

VOID					xCheckVectors( VOID );
VOID					xClearAllVectors( VOID );

	/// CheckVirusList.c

VOID					xCHK_AddVirusToVirusList( struct CurrentFile *, struct CurrentFile * );
VOID					xCHK_FlushVirusList( VOID );
ULONG					xCHK_CountNumberViraFound( BOOL );
STRPTR					xCHK_GetVirusName( ULONG, BOOL );

	/// CheckWindow.c

UWORD					xCalc_ThreeGadgetsWidth( VOID );
UWORD					xCalc_MemoryBoxWidth( VOID );
UWORD					xCalc_MemoryBoxHeight( VOID );
UWORD					xCalc_CheckInfoBoxWidth( VOID );
UWORD					xCalc_CheckInfoBoxHeight( VOID );
UWORD					xCalc_WarningsLVHeight( VOID );
UWORD					xCalc_MinWindowWidth( VOID );
UWORD					xCalc_MinWindowHeight( VOID );
VOID					xRemoveCheckWindowGadgets( VOID );
ULONG __asm __saveds	xCheckHistoryRenderHook( REGA1 struct LVDrawMsg *, REGA2 struct HistoryNode * );
BOOL					xCreateCheckWindowGadgets( VOID );
VOID					xDrawCheckWindowBoxes( VOID );
BOOL					xOpenCheckWindow( VOID );
VOID					xCloseCheckWindow( VOID );

	/// CheckWindowUpdate.c

VOID					xCW_UpdatePathGadget( STRPTR, BOOL );
VOID					xCW_UpdateFileGadget( STRPTR, BOOL );
VOID					xCW_UpdateMemoryInfo( VOID );
VOID					xCW_UpdateCheckInfo( struct CurrentCheck * );
UWORD					xCheckWindowMsg( struct CurrentCheck * );
LONG					xHandleCheckWindowIDCMP( struct CurrentCheck * );

	/// Commodities.c

BOOL					xCreateCommodity( VOID );
VOID					xRemoveCommodity( VOID );

	/// CRC.a

ULONG ASM				xaDoCRC( REGA0 UBYTE *, REGD0 ULONG );

	/// DOS.c

struct XtruderArgs *	xGetCLIArguments( struct MsgPort * );
VOID					xHandleCLIArguments( struct XtruderArgs *, struct MsgPort * );
VOID					xFreeCLIArguments( struct XtruderArgs * );
BOOL __stdargs			xSelectFile( struct Window *, ... );
BOOL __stdargs			xSelectPath( struct Window *, ... );
BOOL					xCopyFile( STRPTR, STRPTR );
BOOL					xCopyFileFromFIB( struct FileInfoBlock *, STRPTR );
BOOL					xExists( STRPTR );
ULONG					xFileSize( STRPTR );
BOOL					xPathName( STRPTR, STRPTR, ULONG );
STRPTR					xGetFullName( STRPTR, STRPTR );
LONG					xParsePatternNoCase( STRPTR, STRPTR );
UBYTE *					xStoreFile( STRPTR, ULONG );
BOOL					xIsExecutable( STRPTR );

	/// FilesShowCRC.c

VOID					xShowCRC( STRPTR );

	/// FilesRepair.c

VOID					xRepairFile( STRPTR );

	/// Gadgets.c

UWORD					xConvertMsgRawToVan( UWORD, UWORD, APTR );
struct xGadget *		xCheckUnderscore( struct xGadget *, UWORD, UWORD, APTR );
VOID					xFindUnderscore( struct PUMGNode * );
UWORD					xConvertVanToRaw( STRPTR );
UBYTE					xIsGadSelected( struct Gadget * );
VOID					xSetGadgetString( struct Window *, struct Gadget *, STRPTR );
VOID					xGhostGadget( struct Window *, struct Gadget * );
VOID					xUnGhostGadget( struct Window *, struct Gadget * );
VOID					xGhostObject( struct Window *, struct Gadget * );
VOID					xUnGhostObject( struct Window *, struct Gadget * );
VOID					xPressButton( struct Window *, struct Gadget * );
struct xGadget *		xFindGadget( struct xGadget *, UWORD );
LONG					xListViewTop( struct Gadget *, struct Window * );
VOID					xNewListPos( struct NewLVPos *, UWORD, UWORD, LONG, LONG, LONG, LONG );
LONG					xSliderLevel( struct Gadget *, struct Window * );
LONG					xCycleActive( struct Gadget *, struct Window * );
VOID					xRefreshCycleGadget( struct Window *, struct Gadget *, UWORD, UWORD );

	/// Graphics.c

VOID __stdargs			xWTxt( struct Window *, ULONG, ULONG, UBYTE, STRPTR, ... );
VOID __stdargs			xATxt( BOOL, UBYTE, STRPTR, ... );
VOID					xClearActionBox( VOID );
VOID __stdargs			xBarTxt( struct Window *, UWORD, STRPTR, ... );
ULONG					xMaxStringLength( struct TextFont *, ULONG, ULONG );
ULONG					xTextLength( struct TextFont *, STRPTR );
ULONG					xProportionalLength( struct TextFont *, UWORD );

	/// GUI.c

UWORD					xOpenInterface( VOID );
VOID					xCloseInterface( VOID );
VOID					xCalculateConstants( VOID );
VOID					xOpenXtruderFonts( VOID );
BOOL					xPopUpGUI( VOID );

	/// History.c

VOID __stdargs			xAddHistoryNode( UWORD, STRPTR, ... );
VOID					xClearHistory( BOOL );
VOID					xFreeHistoryNodes( struct List * );
VOID					xShowHistory( VOID );
VOID					xPerformHistorySave( VOID );
BOOL					xSaveHistory( BOOL );

	/// Icon.c

VOID					xHandleWBArguments( struct WBStartup *, struct MsgPort * );
VOID					xHandleAppWindowMessages( VOID );
VOID					xCreateAppIcon( VOID );
VOID					xRemoveAppIcon( VOID );

	/// List.c

BOOL __stdargs			xAddNameNode( struct List *, STRPTR, ... );
VOID					xFreeNameNodes( struct List * );
VOID					xSortNameNodeList( struct List * );
struct Node *			xFindCurrentSelectedNode( struct List *, UWORD );
UWORD					xCountListEntries( struct List * );
struct Node *			xSelectListEntry( struct List *, ULONG, STRPTR, STRPTR, struct Screen * );
LONG					xHandleListIDCMP( struct Window *, ULONG );
VOID					xOpenListShowWindow( struct List *, STRPTR, STRPTR, UWORD *, UWORD, BOOL );
struct Gadget *			xCreateListShowGadgets( struct Window *, struct List *, BOOL, BOOL );

	/// LoadSettings.c

VOID					xLoadSettings( BOOL );
VOID					xFillSettingsStructure( UBYTE *, ULONG );
STRPTR					xFindString( UBYTE *, ULONG, STRPTR, STRPTR );
LONG					xFindNumber( UBYTE *, ULONG, STRPTR, LONG );
LONG					xFindNumberLimits( UBYTE *, ULONG, STRPTR, LONG, LONG, LONG );
LONG					xCmpEntry( STRPTR, STRPTR, LONG );
VOID					xFindCoords( UBYTE *, ULONG, UWORD *, UBYTE, STRPTR );

	/// Menus.c

VOID					xCreateMenus( VOID );
BOOL					xSetMenus( VOID );
VOID					xRemoveMenus( VOID );
VOID					xHandleMainWinMenuIDCMP( UWORD code );

	/// Miscellaneous.c

VOID					xDisableWindow( struct Window * );
VOID					xEnableWindow( struct Window * );
LONG __stdargs			xReqMsg( struct Window *, STRPTR, STRPTR, STRPTR, ... );
STRPTR					xGetCatalogStr( LONG );
VOID __stdargs			xSPrintF( STRPTR, STRPTR, ... );
VOID					xSendXtruderMessage( UWORD, STRPTR );
ULONG __interrupt __asm __saveds	xCursorDownPatchFunc( register __a0 struct Hook *, register __a2 struct SGWork *, register __a1 ULONG * );


	/// ReportsCheck

VOID					xStartCheckReport( STRPTR );
VOID					xCheckReport( STRPTR );
VOID					xCheckReportObject( struct CurrentReport *, struct FileReport * );
BOOL					xGetCopyToPath( struct CurrentReport * );
VOID __stdargs			xAddReportCheckHistoryNode( UWORD, STRPTR, ... );
BOOL					xCHI_CopyFile( STRPTR, STRPTR );

	/// ReportsCheckWindow

UWORD					xCalc_CHIThreeGadgetsWidth( VOID );
UWORD					xCalc_CHICheckInfoBoxWidth( VOID );
UWORD					xCalc_CHICheckInfoBoxHeight( VOID );
UWORD					xCalc_CHIMinWindowWidth( VOID );
UWORD					xCalc_CHIHistoryLVHeight( VOID );
UWORD					xCalc_CHIMinWindowHeight( VOID );
VOID					xRemoveCHIWindowGadgets( VOID );
ULONG __asm __saveds	xCHIHistoryRenderHook( REGA1 struct LVDrawMsg *, REGA2 struct HistoryNode * );
BOOL					xCreateCHIGadgets( VOID );
VOID					xDrawCHIWindowBoxes( VOID );
BOOL					xOpenCHIWindow( VOID );
VOID					xCloseCHIWindow( VOID );
VOID					xCHI_UpdatePathGadget( STRPTR );
VOID					xCHI_UpdateFileGadget( STRPTR );
VOID					xCHI_UpdateCheckInfo( struct CurrentReport * );
UWORD					xCheckCHIWindowMsg( struct CurrentReport * );
LONG					xHandleCHIWindowIDCMP( struct CurrentReport * );
VOID					xSaveReportCheckHistory( VOID );

	/// ReportsCreate

VOID					xStartCreateReport( STRPTR, BOOL );
VOID					xCreateReport( STRPTR );
VOID					xUpdateReport( STRPTR );
VOID					xCreateObject( struct CurrentReport *, STRPTR );
VOID __stdargs			xAddReportCreateErrorNode( UWORD, STRPTR, ... );
VOID					xFreeErrorNodes( VOID );

	/// ReportsCreateWindow

UWORD					xCalc_CRITopGadgetsWidth( VOID );
UWORD					xCalc_CRIListViewHeight( VOID );
UWORD					xCalc_CRIMinWindowWidth( VOID );
UWORD					xCalc_CRIMinWindowHeight( VOID );
VOID					xRemoveCRIWindowGadgets( VOID );
BOOL					xCreateCRIWindowGadgets( VOID );
BOOL					xOpenCRIWindow( VOID );
VOID					xCloseCRIWindow( VOID );
VOID					xCRI_UpdatePathGadget( STRPTR );
VOID					xCRI_UpdateFileGadget( STRPTR );
UWORD					xCheckCRIWindowMsg( struct CurrentReport * );
LONG					xHandleCRIWindowIDCMP( struct CurrentReport * );

	/// ReportsMain

VOID					xRereadReportsList( VOID );
VOID					xAddReportNode( STRPTR, struct DateStamp * );
VOID					xSortReportNodes( struct List * );
VOID					xFreeReportNodes( VOID );
BOOL					xIsFileReport( STRPTR );

	/// ReportsMainWindow

UWORD					xCalc_MRITopGadgetsWidth( VOID );
UWORD					xCalc_MRIListViewHeight( VOID );
UWORD					xCalc_MRIMinWindowWidth( VOID );
UWORD					xCalc_MRIMinWindowHeight( VOID );
VOID					xRemoveMRIWindowGadgets( VOID );
ULONG __asm __saveds	xReportsListViewRenderHook( REGA1 struct LVDrawMsg *, REGA2 struct ReportNode * );
BOOL					xCreateMRIWindowGadgets( VOID );
VOID					xOpenMRIWindow( VOID );
VOID					xCloseMRIWindow( VOID );
VOID					xHandleMRIWindowIDCMP( VOID );
VOID					xUpdateMRIGadgets( LONG );

	/// ReportsUpdate

VOID					xUpdateReport( STRPTR );
VOID					xUpdateObjects( struct CurrentReport *, BPTR );

	/// SaveSettings.c

VOID					xSaveSettings( BOOL, BOOL );

	/// Search.a

UBYTE *				ASM	xaSearch( REGA0 STRPTR, REGA1 STRPTR, REGA2 STRPTR, REGD0 ULONG, REGD1 ULONG );

	/// SelectBootblock.c

WORD					xOpenBootblockSelectWindow( UWORD *, WORD );
WORD					xHandleBootblockSelectIDCMP( struct Window *, struct xGadget * );

	/// SelectDrive.c

WORD					xOpenDriveSelectWindow( UWORD *, BOOL );
WORD					xHandleDriveSelectIDCMP( struct Window *, struct xGadget * );

	/// SelectNode.c

struct Node *			xOpenSelectNodeWindow( struct List *list, STRPTR, STRPTR, UWORD, UWORD * );
struct Node *			xHandleSelectNodeIDCMP( struct Window *, struct List *, STRPTR, UWORD );
struct Gadget *			xCreateSelectNodeGadgets( struct Window *, struct List *, BOOL, WORD );

	/// SSARexx.c

VOID					xGhost( struct RastPort *, UWORD, UWORD, UWORD, UWORD, UWORD );
VOID					xFillOldExtent( struct RastPort *, struct Rectangle *, struct Rectangle * );
ULONG __asm __saveds	xRenderHook( REGA1 struct LVDrawMsg *, REGA2 struct ARexxNode * );
VOID					xOpenARexxSettings( VOID );
BOOL					xHandleARexxSettings( struct Window *, struct xGadget * );
VOID					xUpdateStringGads( struct Window *, struct xGadget *, LONG );
VOID					xHandleARexxGadgets( struct Window *, struct xGadget *, UWORD, BOOL );
VOID					xSwapARexxNodes( LONG, BOOL );
BOOL					xAddARexxScript( struct Window *, struct xGadget *, LONG );
BOOL					xAddARexxScriptNode( struct List *, STRPTR, STRPTR, BOOL );
BOOL					xRemoveARexxScriptNode( WORD );
VOID					xFreeARexxScriptNodes( struct List * );
VOID					xSortARexxScriptNodes( VOID );
VOID					xUpdateARexxScriptNode( struct Window *, struct xGadget *, STRPTR, STRPTR, ULONG );
struct ARexxNode *		xFindARexxNode( LONG );

	/// SSBootblock.c

VOID					xOpenBootblockSettings( VOID );
BOOL					xHandleBootblockSettings( struct Window *, struct xGadget * );

	/// SSFCArchives.c

VOID					xOpenFCArchivesSettings( VOID );
BOOL					xHandleFCArchivesSettings( struct Window *, struct xGadget * );

	/// SSFCCheckWindow.c

VOID					xOpenFCCheckWindowSettings( VOID );
BOOL					xHandleFCCheckWindowSettings( struct Window *, struct xGadget * );

	/// SSFCInformation.c

VOID					xOpenFCInformationSettings( VOID );
BOOL					xHandleFCInformationSettings( struct Window *, struct xGadget * );

	/// SSFCMiscellaneous.c

VOID					xOpenFCMiscellaneousSettings( VOID );
BOOL					xHandleFCMiscellaneousSettings( struct Window *, struct xGadget * );

	/// SSFCPBC.c

VOID					xOpenFCPBCSettings( VOID );
BOOL					xHandleFCPBCSettings( struct Window *, struct xGadget * );

	/// SSFCPrespecified.c

VOID					xOpenFCPrespecifiedSettings( VOID );
BOOL					xHandleFCPrespecifiedSettings( struct Window *, struct xGadget * );
VOID					xSortNodes( VOID );
BOOL					xAddPrespecified( struct Window *, struct Gadget *, LONG );
BOOL					xRemovePrespecified( WORD );
VOID					xUpdatePrespecifiedName( struct Window *, struct Gadget *, STRPTR, ULONG );
BOOL					xAddPrespecifiedNode( struct List *, STRPTR );
VOID					xFreePrespecifiedNodes( struct List * );

	/// SSFCUnpacking.c

VOID					xOpenFCUnpackingSettings( VOID );
BOOL					xHandleFCUnpackingSettings( struct Window *, struct xGadget * );

	/// SSGUI.c

BOOL					xOpenGUISettings( VOID );
BOOL					xHandleGUISettings( struct Screen *, struct Window *, struct xGadget *, BOOL * );
struct List *			xBuildScreenList( VOID );

	/// SSHistory.c

VOID					xOpenHistorySettings( VOID );
BOOL					xHandleHistorySettings( struct Window *, struct xGadget * );

	/// SSMiscellaneous.c

VOID					xOpenMiscellaneousSettings( VOID );
BOOL					xHandleMiscellaneousSettings( struct Window *, struct xGadget * );

	/// SSPaths.c

VOID					xOpenPathsSettings( VOID );
BOOL					xHandlePathsSettings( struct Window *, struct xGadget * );
VOID					xCreatePaths( VOID );

	/// SSStartup.c

VOID					xOpenStartupSettings( VOID );
BOOL					xHandleStartupSettings( struct Window *, struct xGadget * );

	/// SS_GUISupport.c

struct SettingsGUI *	xOpenSettingsGUI( STRPTR, UWORD, UWORD, struct Gadget *, struct xGadget *, UWORD );
VOID					xCloseSettingsGUI( struct SettingsGUI * );

	/// StartAndQuit.c

VOID					xOpenFullSystem( VOID );
VOID					xOpenSmallSystem( VOID );
VOID					xCloseSystem( UWORD, UWORD, UWORD );
BOOL					xOpenLibraries( VOID );
VOID					xCloseLibraries( VOID );
VOID					xInitializeFonts( VOID );
BOOL					xAllocMiscMemory( VOID );
VOID					xFreeMiscMemory( VOID );
BOOL					xInitializeMiscellaneous( VOID );
VOID					xFreeMiscellaneous( VOID );
VOID					xReadKeyfile( VOID );
VOID					xAutoDeletePaths( VOID );

	/// System.c

VOID					xListHandler( UWORD );
struct List *			xGenerateBootblockViraList( STRPTR );
struct List *			xGenerateFileAndLinkViraList( STRPTR );
struct List *			xGenerateCruncherList( STRPTR );
struct List *			xGeneratePatchList( STRPTR );
struct List *			xGenerateFiletypeList( STRPTR );
struct List *			xGenerateLibraryList( STRPTR );
struct List *			xGenerateDeviceList( STRPTR );
struct List *			xGenerateResourceList( STRPTR );
struct List *			xGeneratePortList( STRPTR );
struct List *			xGenerateSemaphoreList( STRPTR );
struct List *			xGenerateTaskList( STRPTR );
STRPTR					xGetNodeType( UBYTE );

	/// System information

UWORD					xCalc_SysInfoListViewHeight( VOID );
UWORD					xCalc_SysInfoMinWindowWidth( VOID );
UWORD					xCalc_SysInfoMinWindowHeight( VOID );
VOID					xRemoveSysInfoWindowGadgets( VOID );
ULONG __asm __saveds	xSysInfoRenderHook( REGA1 struct LVDrawMsg *, REGA2 struct SysInfoNode * );
BOOL					xCreateSysInfoWindowGadgets( VOID );
VOID					xOpenSysInfoWindow( VOID );
VOID					xCloseSysInfoWindow( VOID );
VOID					xHandleSysInfoWindowIDCMP( VOID );
VOID					xGenerateSystemInformationList( VOID );
VOID __stdargs			xAddSysInfoNode( UWORD, STRPTR, ... );
VOID					xFreeSysInfoNodes( VOID );
ULONG					xGetXtruderSize( VOID );

	/// Time.c

VOID					xCreateTimeCheck( VOID );
BOOL					xCreateTimeRequest( VOID );
VOID					xRemoveTimeRequest( VOID );
VOID					xTimeCheck( VOID );

	/// Trackdisk.c

VOID					xGetTrackUnits( VOID );
VOID					xFreeTrackUnits( VOID );
BOOL					xIsDiskInDrive( LONG );
VOID					xFindPresentDisks( VOID );
VOID					xTDError( LONG );

	/// UnpackLibrarySupport.c

STRPTR	ASM				xaListNextCruncher( REGD1 ULONG );

	/// VirusInformation.c

UWORD					xCalc_VITopGadgetsWidth( VOID );
UWORD					xCalc_VILeadingTexts( VOID );
UWORD					xCalc_VIListViewHeight( VOID );
UWORD					xCalc_VIMinWindowWidth( VOID );
UWORD					xCalc_VIMinWindowHeight( VOID );
VOID					xRemoveVIWindowGadgets( VOID );
BOOL					xCreateVirusInfoWindowGadgets( VOID );
VOID					xOpenVirusInformationWindow( VOID );
VOID					xCloseVirusInformationWindow( VOID );
VOID					xHandleVirusInformationIDCMP( VOID );
VOID					xUpdateDescriptions( BOOL );
VOID					xFreezeDescriptionsListGadget( VOID );
VOID					xUnFreezeDescriptionsListGadget( VOID );
VOID					xFindDescriptions( BOOL );
BOOL					xSearchCheckFile( struct AnchorPath * );
BOOL					xIsStringInBuffer( UBYTE *, ULONG, STRPTR );
VOID					xViewDescription( struct Screen *, STRPTR );
VOID					xViewGuideFile( struct Screen *, STRPTR );

	/// Xtruder.c

VOID					xWaitMainWin( VOID );
VOID					xHandleIconMsg( VOID );
