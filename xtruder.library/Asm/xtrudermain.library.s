	SECTION	code
	
	NOLIST

	INCLUDE	"exec/types.i"
	INCLUDE	"exec/memory.i"
	INCLUDE	"exec/initializers.i"
	INCLUDE	"exec/libraries.i"
	INCLUDE	"exec/lists.i"
	INCLUDE	"exec/alerts.i"
	INCLUDE	"exec/resident.i"
	INCLUDE	"libraries/dos.i"
	INCLUDE	"libraries/xtruder.i"
	INCLUDE	"LVO/exec_lib.i"
	INCLUDE	"Functions.i"

	LIST

	XDEF	InitTable   ** These don't have to be external but it helps
	XDEF	Open        ** some debuggers to have them globally visible
	XDEF	Close
	XDEF	Expunge
	XDEF	Null
	XREF	_xlAllocXtruderInfo
	XREF	_xlFreeXtruderInfo
	XREF	_xlCheckLVO
	XREF	_xlPatchListFirst
	XREF	_xlPatchListNext

	XREF	_AbsExecBase


XLVERSION	EQU	37
XLREVISION	EQU	1


Start:
	moveq	#-1,d0
	rts

RomTag:
		dc.w	RTC_MATCHWORD	; UWORD RT_MATCHWORD
		dc.l	RomTag		; APTR  RT_MATCHTAG
		dc.l	EndCode		; APTR  RT_ENDSKIP
		dc.b	RTF_AUTOINIT	; UBYTE RT_FLAGS
		dc.b	37		; UBYTE RT_VERSION  (defined in sample_rev.i)
		dc.b	NT_LIBRARY	; UBYTE RT_TYPE
		dc.b	0		; BYTE  RT_PRI
		dc.l	LibName		; APTR  RT_NAME
		dc.l	IDString	; APTR  RT_IDSTRING
		dc.l	InitTable	; APTR  RT_INIT  table for InitResident()


LibName:	dc.b	'xtruder.library',0

IDString:	dc.b	'xtruder.library 37.1 (17.12.94)',13,10,0

DName:		dc.b	'dos.library',0
GName:		dc.b	'graphics.library',0
IName:		dc.b	'intuition.library',0
UName:		dc.b	'utility.library',0

		ds.w	0

* The  romtag  specified that we were "RTF_AUTOINIT".  This means that the
* RT_INIT  structure  member  points to one of these tables below.  If the
* AUTOINIT bit was not set then RT_INIT would point to a routine to run.

InitTable:
	dc.l	XtruderBase_SIZEOF	* Size of library base data space
	dc.l	FuncTable		* Pointer to function initializers
	dc.l	DataTable		* Pointer to data initializers
	dc.l	InitRoutine		* Routine to run

FuncTable:
	dc.l	Open			* Standard system routines
	dc.l	Close
	dc.l	Expunge
	dc.l	Null

	dc.l	_xlAllocXtruderInfo
	dc.l	_xlFreeXtruderInfo
	dc.l	_xlCheckLVO
	dc.l	_xlPatchListFirst
	dc.l	_xlPatchListNext

	dc.l	-1			* Function table end marker

* The data table initializes static data structures.  The format is specified in
* exec/InitStruct routine's manual pages.  The INITBYTE/INITWORD/INITLONG routines are
* in the file "exec/initializers.i".  The first argument is the offset from the library
* base for this byte/word/long.  The second argument is the value to put in that cell.
* The table is null terminated.
* NOTE - LN_TYPE below is a correction - old example had LH_TYPE.

DataTable:
	INITBYTE	LN_TYPE,NT_LIBRARY
	INITLONG	LN_NAME,LibName
	INITBYTE	LIB_FLAGS,LIBF_SUMUSED!LIBF_CHANGED
	INITWORD	LIB_VERSION,XLVERSION
	INITWORD	LIB_REVISION,XLREVISION
	INITLONG	LIB_IDSTRING,IDString
	dc.l		0

* This  routine  gets  called  after  the library has been allocated.  The
* library  pointer  is in D0.  The segment list is in A0.  If it returns
* non-zero then the library will be linked into the library list.

InitRoutine:
	move.l	a5,-(sp)		* Get the library pointer into a
	move.l	d0,a5			* convenient A register

	move.l	a6,xl_SysLib(a5)	* Save a pointer to exec

	move.l	a0,xl_SegList(a5)	* Save a pointer to our loaded code

	moveq.l	#0,d7			* Possible error code

	move.l	#0,xl_DosLib(a5)	* Clear the pointers
	move.l	#0,xl_IntLib(a5)
	move.l	#0,xl_GfxLib(a5)
	move.l	#0,xl_UtilityLib(a5)

	lea	DName(pc),a1		* Open the DOS library
	moveq.l	#0,d0
	CALL	OpenLibrary,4.w
	move.l	d0,xl_DosLib(a5)
	bne.b	.GotDOS

	moveq.l	#1,d7
.GotDOS:
	lea	IName(pc),a1		* Open the DOS library
	moveq.l	#37,D0			* At least veersion 37
	CALL	OpenLibrary,4.w
	move.l	d0,xl_IntLib(a5)
	bne.b	.GotIntuition

	moveq.l	#1,d7
.GotIntuition:
	lea	GName(pc),a1		* Open the Graphics library
	moveq.l	#37,d0			* At least version 37
	CALL	OpenLibrary,4.w
	move.l	d0,xl_GfxLib(a5)
	bne.b	.GotGraphics

	moveq.l	#1,D7
.GotGraphics:
	lea	UName(pc),a1		* Open the Utility library
	moveq.l	#37,d0			* At least version 37
	CALL	OpenLibrary,4.w
	move.l	d0,xl_UtilityLib(a5)
	bne.b	.GotUtility

	moveq.l	#1,d7			* Put error code
.GotUtility:
	move.l	a5,d0
	move.l	(sp)+,a5
	rts

Open:						* ( libptr:a6, version:d0 )
	addq.w	#1,LIB_OPENCNT(a6)		* mark us as having another opener
	bclr	#LIBB_DELEXP,xl_Flags(a6)	* prevent delayed expunges
	move.l	a6,d0
	rts

*********************************************************************
* There  are  two  different  things that might be returned from the Close
* routine.  If the library is no longer open and there is a delayed expunge
* then Close should return the segment list (as given to Init).  Otherwise
* close should return NULL.
*********************************************************************
Close:      ; ( libptr:a6 )
	moveq.l	#0,d0			* Set the return value
	subq.w	#1,LIB_OPENCNT(a6)	* Mark us as having one fewer openers
	bne.s	1$			* See if there is anyone left with us open

	btst	#LIBB_DELEXP,xl_Flags(a6)	* See if we have a delayed
	beq.s	1$				* expunge pending

	bsr	Expunge			* Do the expunge
1$:
	rts

*********************************************************************
* There  are  two different things that might be returned from the Expunge
* routine.  If the library is no longer open then Expunge should return the
* segment list (as given to Init). Otherwise Expunge should set the
* delayed expunge flag and return NULL.
*
* One  other  important  note:   because Expunge is called from the memory
* allocator, it may NEVER Wait() or otherwise take long time to complete.
*********************************************************************
Expunge:   ; ( libptr: a6 )
	movem.l	d2/a5/a6,-(sp)
	move.l	a6,a5
	move.l	xl_SysLib(a5),a6

	tst.w	LIB_OPENCNT(a5)		* See if anyone has us open
	beq	.ll

	bset	#LIBB_DELEXP,xl_Flags(a5)
					* It is still open.  
	moveq.l	#0,d0			* Set the delayed expunge flag
	bra	Expunge_End
.ll:
	move.l	xl_SegList(a5),d2	* Go ahead and get rid of us.
					* Store our seglist in d2
	move.l	a5,a1			* Unlink from library list
	CALL	Remove,4.w

	move.l	xl_UtilityLib(a5),a1	* Close the Utility library
	cmp.l	#0,a1
	beq.b	.CloseGfx
	CALL	CloseLibrary,4.w
.CloseGfx:
	move.l	xl_GfxLib(a5),a1	* Close the Graphics library
	cmp.l	#0,a1
	beq.b	.CloseInt
	CALL	CloseLibrary,4.w
.CloseInt:
	move.l	xl_IntLib(a5),a1	* Close the Intuition library
	cmp.l	#0,a1
	beq.b	.CloseDOS
	CALL	CloseLibrary,4.w
.CloseDOS:
	move.l	xl_DosLib(a5),a1	* Close the DOS library
	cmp.l	#0,a1
	beq.b	.EndClose
	CALL	CloseLibrary,4.w
.EndClose:
	moveq.l	#0,d0			* Free our memory
	move.l	a5,a1
	move.w	LIB_NEGSIZE(a5),d0
	sub.l	d0,a1
	add.w	LIB_POSSIZE(a5),d0
	CALL	FreeMem,4.w

	move.l	d2,d0			* Set up our return value

Expunge_End:
	movem.l	(sp)+,d2/a5/a6
	rts

*********************************************************************
Null:
	moveq.l	#0,d0
	rts

*********************************************************************
EndCode:
	rts
	
	END