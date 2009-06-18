/*
 *	$Id$
 */

#include  "ml_color.h"

#include  <stdio.h>		/* NULL */
#include  <string.h>		/* strcmp */


/* --- static variables --- */

static char *  color_name_table[] =
{
	"hl_black" ,
	"hl_red" ,
	"hl_green" ,
	"hl_yellow" ,
	"hl_blue" ,
	"hl_magenta" ,
	"hl_cyan" ,
	"hl_white" ,
} ;

static char  color_name_256[4] ;

static u_int8_t color_rgb_table[][3] =
{
	/* VT SYS COLOR */
	{ 0x00, 0x00, 0x00 },
	{ 0xff, 0x00, 0x00 },
	{ 0x00, 0xff, 0x00 },
	{ 0xff, 0xff, 0x00 },
	{ 0x00, 0x00, 0xff },
	{ 0xff, 0x00, 0xff },
	{ 0x00, 0xff, 0xff },
	{ 0xff, 0xff, 0xff },

	/* VT SYS COLOR(BOLD) */
	{ 0x00, 0x00, 0x00 },
	{ 0xff, 0x00, 0x00 },
	{ 0x00, 0xff, 0x00 },
	{ 0xff, 0xff, 0x00 },
	{ 0x00, 0x00, 0xff },
	{ 0xff, 0x00, 0xff },
	{ 0x00, 0xff, 0xff },
	{ 0xff, 0xff, 0xff },

	/* CUBE COLOR(0x10-0xe7) */
	{ 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x5f },
	{ 0x00, 0x00, 0x87 },
	{ 0x00, 0x00, 0xaf },
	{ 0x00, 0x00, 0xd7 },
	{ 0x00, 0x00, 0xff },
	{ 0x00, 0x5f, 0x00 },
	{ 0x00, 0x5f, 0x5f },
	{ 0x00, 0x5f, 0x87 },
	{ 0x00, 0x5f, 0xaf },
	{ 0x00, 0x5f, 0xd7 },
	{ 0x00, 0x5f, 0xff },
	{ 0x00, 0x87, 0x00 },
	{ 0x00, 0x87, 0x5f },
	{ 0x00, 0x87, 0x87 },
	{ 0x00, 0x87, 0xaf },
	{ 0x00, 0x87, 0xd7 },
	{ 0x00, 0x87, 0xff },
	{ 0x00, 0xaf, 0x00 },
	{ 0x00, 0xaf, 0x5f },
	{ 0x00, 0xaf, 0x87 },
	{ 0x00, 0xaf, 0xaf },
	{ 0x00, 0xaf, 0xd7 },
	{ 0x00, 0xaf, 0xff },
	{ 0x00, 0xd7, 0x00 },
	{ 0x00, 0xd7, 0x5f },
	{ 0x00, 0xd7, 0x87 },
	{ 0x00, 0xd7, 0xaf },
	{ 0x00, 0xd7, 0xd7 },
	{ 0x00, 0xd7, 0xff },
	{ 0x00, 0xff, 0x00 },
	{ 0x00, 0xff, 0x5f },
	{ 0x00, 0xff, 0x87 },
	{ 0x00, 0xff, 0xaf },
	{ 0x00, 0xff, 0xd7 },
	{ 0x00, 0xff, 0xff },
	{ 0x5f, 0x00, 0x00 },
	{ 0x5f, 0x00, 0x5f },
	{ 0x5f, 0x00, 0x87 },
	{ 0x5f, 0x00, 0xaf },
	{ 0x5f, 0x00, 0xd7 },
	{ 0x5f, 0x00, 0xff },
	{ 0x5f, 0x5f, 0x00 },
	{ 0x5f, 0x5f, 0x5f },
	{ 0x5f, 0x5f, 0x87 },
	{ 0x5f, 0x5f, 0xaf },
	{ 0x5f, 0x5f, 0xd7 },
	{ 0x5f, 0x5f, 0xff },
	{ 0x5f, 0x87, 0x00 },
	{ 0x5f, 0x87, 0x5f },
	{ 0x5f, 0x87, 0x87 },
	{ 0x5f, 0x87, 0xaf },
	{ 0x5f, 0x87, 0xd7 },
	{ 0x5f, 0x87, 0xff },
	{ 0x5f, 0xaf, 0x00 },
	{ 0x5f, 0xaf, 0x5f },
	{ 0x5f, 0xaf, 0x87 },
	{ 0x5f, 0xaf, 0xaf },
	{ 0x5f, 0xaf, 0xd7 },
	{ 0x5f, 0xaf, 0xff },
	{ 0x5f, 0xd7, 0x00 },
	{ 0x5f, 0xd7, 0x5f },
	{ 0x5f, 0xd7, 0x87 },
	{ 0x5f, 0xd7, 0xaf },
	{ 0x5f, 0xd7, 0xd7 },
	{ 0x5f, 0xd7, 0xff },
	{ 0x5f, 0xff, 0x00 },
	{ 0x5f, 0xff, 0x5f },
	{ 0x5f, 0xff, 0x87 },
	{ 0x5f, 0xff, 0xaf },
	{ 0x5f, 0xff, 0xd7 },
	{ 0x5f, 0xff, 0xff },
	{ 0x87, 0x00, 0x00 },
	{ 0x87, 0x00, 0x5f },
	{ 0x87, 0x00, 0x87 },
	{ 0x87, 0x00, 0xaf },
	{ 0x87, 0x00, 0xd7 },
	{ 0x87, 0x00, 0xff },
	{ 0x87, 0x5f, 0x00 },
	{ 0x87, 0x5f, 0x5f },
	{ 0x87, 0x5f, 0x87 },
	{ 0x87, 0x5f, 0xaf },
	{ 0x87, 0x5f, 0xd7 },
	{ 0x87, 0x5f, 0xff },
	{ 0x87, 0x87, 0x00 },
	{ 0x87, 0x87, 0x5f },
	{ 0x87, 0x87, 0x87 },
	{ 0x87, 0x87, 0xaf },
	{ 0x87, 0x87, 0xd7 },
	{ 0x87, 0x87, 0xff },
	{ 0x87, 0xaf, 0x00 },
	{ 0x87, 0xaf, 0x5f },
	{ 0x87, 0xaf, 0x87 },
	{ 0x87, 0xaf, 0xaf },
	{ 0x87, 0xaf, 0xd7 },
	{ 0x87, 0xaf, 0xff },
	{ 0x87, 0xd7, 0x00 },
	{ 0x87, 0xd7, 0x5f },
	{ 0x87, 0xd7, 0x87 },
	{ 0x87, 0xd7, 0xaf },
	{ 0x87, 0xd7, 0xd7 },
	{ 0x87, 0xd7, 0xff },
	{ 0x87, 0xff, 0x00 },
	{ 0x87, 0xff, 0x5f },
	{ 0x87, 0xff, 0x87 },
	{ 0x87, 0xff, 0xaf },
	{ 0x87, 0xff, 0xd7 },
	{ 0x87, 0xff, 0xff },
	{ 0xaf, 0x00, 0x00 },
	{ 0xaf, 0x00, 0x5f },
	{ 0xaf, 0x00, 0x87 },
	{ 0xaf, 0x00, 0xaf },
	{ 0xaf, 0x00, 0xd7 },
	{ 0xaf, 0x00, 0xff },
	{ 0xaf, 0x5f, 0x00 },
	{ 0xaf, 0x5f, 0x5f },
	{ 0xaf, 0x5f, 0x87 },
	{ 0xaf, 0x5f, 0xaf },
	{ 0xaf, 0x5f, 0xd7 },
	{ 0xaf, 0x5f, 0xff },
	{ 0xaf, 0x87, 0x00 },
	{ 0xaf, 0x87, 0x5f },
	{ 0xaf, 0x87, 0x87 },
	{ 0xaf, 0x87, 0xaf },
	{ 0xaf, 0x87, 0xd7 },
	{ 0xaf, 0x87, 0xff },
	{ 0xaf, 0xaf, 0x00 },
	{ 0xaf, 0xaf, 0x5f },
	{ 0xaf, 0xaf, 0x87 },
	{ 0xaf, 0xaf, 0xaf },
	{ 0xaf, 0xaf, 0xd7 },
	{ 0xaf, 0xaf, 0xff },
	{ 0xaf, 0xd7, 0x00 },
	{ 0xaf, 0xd7, 0x5f },
	{ 0xaf, 0xd7, 0x87 },
	{ 0xaf, 0xd7, 0xaf },
	{ 0xaf, 0xd7, 0xd7 },
	{ 0xaf, 0xd7, 0xff },
	{ 0xaf, 0xff, 0x00 },
	{ 0xaf, 0xff, 0x5f },
	{ 0xaf, 0xff, 0x87 },
	{ 0xaf, 0xff, 0xaf },
	{ 0xaf, 0xff, 0xd7 },
	{ 0xaf, 0xff, 0xff },
	{ 0xd7, 0x00, 0x00 },
	{ 0xd7, 0x00, 0x5f },
	{ 0xd7, 0x00, 0x87 },
	{ 0xd7, 0x00, 0xaf },
	{ 0xd7, 0x00, 0xd7 },
	{ 0xd7, 0x00, 0xff },
	{ 0xd7, 0x5f, 0x00 },
	{ 0xd7, 0x5f, 0x5f },
	{ 0xd7, 0x5f, 0x87 },
	{ 0xd7, 0x5f, 0xaf },
	{ 0xd7, 0x5f, 0xd7 },
	{ 0xd7, 0x5f, 0xff },
	{ 0xd7, 0x87, 0x00 },
	{ 0xd7, 0x87, 0x5f },
	{ 0xd7, 0x87, 0x87 },
	{ 0xd7, 0x87, 0xaf },
	{ 0xd7, 0x87, 0xd7 },
	{ 0xd7, 0x87, 0xff },
	{ 0xd7, 0xaf, 0x00 },
	{ 0xd7, 0xaf, 0x5f },
	{ 0xd7, 0xaf, 0x87 },
	{ 0xd7, 0xaf, 0xaf },
	{ 0xd7, 0xaf, 0xd7 },
	{ 0xd7, 0xaf, 0xff },
	{ 0xd7, 0xd7, 0x00 },
	{ 0xd7, 0xd7, 0x5f },
	{ 0xd7, 0xd7, 0x87 },
	{ 0xd7, 0xd7, 0xaf },
	{ 0xd7, 0xd7, 0xd7 },
	{ 0xd7, 0xd7, 0xff },
	{ 0xd7, 0xff, 0x00 },
	{ 0xd7, 0xff, 0x5f },
	{ 0xd7, 0xff, 0x87 },
	{ 0xd7, 0xff, 0xaf },
	{ 0xd7, 0xff, 0xd7 },
	{ 0xd7, 0xff, 0xff },
	{ 0xff, 0x00, 0x00 },
	{ 0xff, 0x00, 0x5f },
	{ 0xff, 0x00, 0x87 },
	{ 0xff, 0x00, 0xaf },
	{ 0xff, 0x00, 0xd7 },
	{ 0xff, 0x00, 0xff },
	{ 0xff, 0x5f, 0x00 },
	{ 0xff, 0x5f, 0x5f },
	{ 0xff, 0x5f, 0x87 },
	{ 0xff, 0x5f, 0xaf },
	{ 0xff, 0x5f, 0xd7 },
	{ 0xff, 0x5f, 0xff },
	{ 0xff, 0x87, 0x00 },
	{ 0xff, 0x87, 0x5f },
	{ 0xff, 0x87, 0x87 },
	{ 0xff, 0x87, 0xaf },
	{ 0xff, 0x87, 0xd7 },
	{ 0xff, 0x87, 0xff },
	{ 0xff, 0xaf, 0x00 },
	{ 0xff, 0xaf, 0x5f },
	{ 0xff, 0xaf, 0x87 },
	{ 0xff, 0xaf, 0xaf },
	{ 0xff, 0xaf, 0xd7 },
	{ 0xff, 0xaf, 0xff },
	{ 0xff, 0xd7, 0x00 },
	{ 0xff, 0xd7, 0x5f },
	{ 0xff, 0xd7, 0x87 },
	{ 0xff, 0xd7, 0xaf },
	{ 0xff, 0xd7, 0xd7 },
	{ 0xff, 0xd7, 0xff },
	{ 0xff, 0xff, 0x00 },
	{ 0xff, 0xff, 0x5f },
	{ 0xff, 0xff, 0x87 },
	{ 0xff, 0xff, 0xaf },
	{ 0xff, 0xff, 0xd7 },
	{ 0xff, 0xff, 0xff },

	/* GRAY SCALE COLOR(0xe8-0xff) */
	{ 0x08, 0x08, 0x08 },
	{ 0x12, 0x12, 0x12 },
	{ 0x1c, 0x1c, 0x1c },
	{ 0x26, 0x26, 0x26 },
	{ 0x30, 0x30, 0x30 },
	{ 0x3a, 0x3a, 0x3a },
	{ 0x44, 0x44, 0x44 },
	{ 0x4e, 0x4e, 0x4e },
	{ 0x58, 0x58, 0x58 },
	{ 0x62, 0x62, 0x62 },
	{ 0x6c, 0x6c, 0x6c },
	{ 0x76, 0x76, 0x76 },
	{ 0x80, 0x80, 0x80 },
	{ 0x8a, 0x8a, 0x8a },
	{ 0x94, 0x94, 0x94 },
	{ 0x9e, 0x9e, 0x9e },
	{ 0xa8, 0xa8, 0xa8 },
	{ 0xb2, 0xb2, 0xb2 },
	{ 0xbc, 0xbc, 0xbc },
	{ 0xc6, 0xc6, 0xc6 },
	{ 0xd0, 0xd0, 0xd0 },
	{ 0xda, 0xda, 0xda },
	{ 0xe4, 0xe4, 0xe4 },
	{ 0xee, 0xee, 0xee },
};


/* --- global functions --- */

char *
ml_get_color_name(
	ml_color_t  color
	)
{
	if( IS_VTSYS_COLOR(color))
	{
		if( color & ML_BOLD_COLOR_MASK)
		{
			return  color_name_table[color & ~ML_BOLD_COLOR_MASK] ;
		}
		else
		{
			return  color_name_table[color] + 3 ;
		}
	}
	else if( IS_256_COLOR(color))
	{
		/* !Notice! Not reentrant */
		
		snprintf( color_name_256, sizeof( color_name_256), "%d", color) ;

		return  color_name_256 ;
	}
	else
	{
		return  NULL ;
	}
}

ml_color_t
ml_get_color(
	char *  name
	)
{
	ml_color_t  color ;

	for( color = ML_BLACK ; color <= ML_WHITE ; color++)
	{
		if( strcmp( name, color_name_table[color] + 3) == 0)
		{
			return  color ;
		}
		else if( strcmp( name, color_name_table[color]) == 0)
		{
			return  color | ML_BOLD_COLOR_MASK ;
		}
	}

	return  ML_UNKNOWN_COLOR ;
}

int
ml_get_color_rgb(
	ml_color_t  color,
	u_short *  red,
	u_short *  green,
	u_short *  blue
	)
{
	if( ! IS_VALID_COLOR(color))
	{
		return  0 ;
	}
	
	*red = color_rgb_table[color][0] ;
	*green = color_rgb_table[color][1] ;
	*blue = color_rgb_table[color][2] ;
	
	return 1 ;
}

int
ml_change_color_rgb(
	ml_color_t  color,
	u_short  red,
	u_short  green,
	u_short  blue
	)
{
	if( ! IS_VALID_COLOR(color))
	{
		return  0 ;
	}

	color_rgb_table[color][0] = red ;
	color_rgb_table[color][1] = green ;
	color_rgb_table[color][2] = blue ;
	
	return 1 ;
}
