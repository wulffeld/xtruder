/*
**	$VER: Xtruder_Boopsi.h (29.11.94)
**
**	Xtruder support data for boopsi functions
**
**	Copyright (C) 1994 Martin Wulffeld
**	All Rights Reserved
*/


/*
 * Userdata structure for boopsi button gadgets...
 */
struct ButtonUserData
{
	UWORD	bud_Rawkeycode;						// Calculated by ()
	BOOL	bud_Selected;						// Is it being held down?
};

// ****** Boopsi button gadget structure ******
struct ButtonData
{
	BOOL				bd_LastState;		// Refresh only if state changes
	BOOL				bd_Otherunderscore;	// 'Nico' underscore
	UWORD				bd_Textcolor;		// Text color
	UWORD				bd_Backcolor;		// Background color
	UWORD				bd_Selbackcolor;	// Selected background color
	ULONG				bd_Type;			// Currently only DRAWER
	STRPTR				bd_Text;			// Text for gadget
	struct Image *		bd_Image;			// Image for gadget
	struct Image *		bd_SImage;			// Select Image for gadget
	struct TextFont *	bd_TextFont;
};

// ****** Button gadget tags ******

//  Text in gadget									Def: NULL
//  STRPTR
#define		BUT_Text				(TAG_USER + 1)

//  Color of text in gadget							Def: 1 (Black)
//  UWORD
#define		BUT_TextColor			(TAG_USER + 2)

//  Color deselected gadget's background				Def: 0 (Grey)
//  UWORD
#define		BUT_BackColor			(TAG_USER + 3)

//  Color selected gadget's background				Def: 3 (Blue)
//  UWORD
#define		BUT_SelectedBackColor	(TAG_USER + 4)

//  Font to use for gadget							Def: NULL
//  struct TextFont *
#define		BUT_TextFont			(TAG_USER + 5)

//  Image for deselected state of gadget				Def: NULL
//  struct Image *
#define		BUT_Image				(TAG_USER + 6)

//  Image for selected state of gadget				Def: NULL
//  struct Image *
#define		BUT_SelectImage			(TAG_USER + 7)

//  Alternative type of gadget						Def: 0 (Normal button)
//  ULONG
#define		BUT_Type				(TAG_USER + 8)

//  Place underscore 1 pixel lower					Def: FALSE
//  BOOL
#define		BUT_OtherUnderscore		(TAG_USER + 10)

// ** Types of alternative gadgets **
#define		BUTTYPE_DRAWER			1
#define		BUTTYPE_FILE			2
#define		BUTTYPE_ARROW			3


// ****** Boopsi progress gadget structure *******
struct ProgressData
{
	ULONG				prd_Min;
	ULONG				prd_Max;
	ULONG				prd_Current;
	BOOL				prd_ShowPercent;
	struct TextFont *	prd_TextFont;
};

// ****** Progress gadget tags *******

//  Minimum value									Def: 0
//  ULONG
#define		PRO_Min					(TAG_USER + 1)

//  Maximum value									Def: 100
//  ULONG
#define		PRO_Max					(TAG_USER + 2)

//  Current value									Def: 0
//  ULONG
#define		PRO_Current				(TAG_USER + 3)

//  Show percentage in the gadget					Def: FALSE
//  BOOL
#define		PRO_ShowPercent			(TAG_USER + 4)

//  Font to use for gadget							Def: NULL
//  struct TextFont *
#define		PRO_TextFont			(TAG_USER + 5)
