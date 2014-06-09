/*
 *	$Id$
 */

#include  <stdio.h>
#include  <stdlib.h>

#include  <kiklib/kik_debug.h>
#include  <kiklib/kik_mem.h>

#include  "x_sb_view.h"
#include  "x_window.h"


#define  TOP_MARGIN     14
#define  BOTTOM_MARGIN  14
#define  HEIGHT_MARGIN  (TOP_MARGIN + BOTTOM_MARGIN)
#define  WIDTH          13


typedef struct  simple_sb_view
{
	x_sb_view_t  view ;

	x_color_t  black ;
	x_color_t  white ;

	int  is_transparent ;

} simple_sb_view_t ;


/* --- static variables --- */

static char *  arrow_up_src[] =
{
	"             ",
	" -----------#",
	" -----#-----#",
	" -----#-----#",
	" ----###----#",
	" ----###----#",
	" ---#####---#",
	" ---#####---#",
	" --#######--#",
	" --#######--#",
	" -#########-#",
	" -----------#",
	" -----------#",
	"#############"

} ;

static char *  arrow_down_src[] =
{
	"             ",
	" -----------#",
	" -----------#",
	" -#########-#",
	" --#######--#",
	" --#######--#",
	" ---#####---#",
	" ---#####---#",
	" ----###----#",
	" ----###----#",
	" -----#-----#",
	" -----#-----#",
	" -----------#",
	"#############"

} ;

static char *  arrow_up_dent_src[] =
{
	"             ",
	" -----------#",
	" ----- -----#",
	" ----- -----#",
	" ----   ----#",
	" ----   ----#",
	" ---     ---#",
	" ---     ---#",
	" --       --#",
	" --       --#",
	" -         -#",
	" -----------#",
	" -----------#",
	"#############"

} ;

static char *  arrow_down_dent_src[] =
{
	"             ",
	" -----------#",
	" -----------#",
	" -         -#",
	" --       --#",
	" --       --#",
	" ---     ---#",
	" ---     ---#",
	" ----   ----#",
	" ----   ----#",
	" ----- -----#",
	" ----- -----#",
	" -----------#",
	"#############"

} ;


/* --- static functions --- */

static size_t
count_char(
	char  c ,
	char *  str
	)
{
	char *  p ;

	p = str ;

	while( *p == c)
	{
		p ++ ;
	}

	return  p - str ;
}

static int
draw_icon(
	x_sb_view_t *  view ,
	int  x ,
	int  y ,
	char **  data ,
	unsigned int  width ,
	unsigned int  height
	)
{
	simple_sb_view_t *  simple_sb ;
	int  x_off ;
	int  y_off ;

	simple_sb = (simple_sb_view_t*) view ;

	if( simple_sb->is_transparent)
	{
		x_window_clear( view->win , x , y , width , height) ;
	}

	for( y_off = 0 ; y_off < height ; y_off ++)
	{
		for( x_off = 0 ; x_off < width ; )
		{
			size_t  count ;

			count = count_char( data[y_off][x_off] , data[y_off] + x_off) ;

			if( data[y_off][x_off] == '-')
			{
				if( ! simple_sb->is_transparent)
				{
					x_window_fill( view->win , x + x_off , y + y_off ,
						count , 1) ;
				}
			}
			else if( data[y_off][x_off] == '#')
			{
				x_window_fill_with( view->win , &simple_sb->black ,
					x + x_off , y + y_off , count , 1) ;
			}
			else
			{
				x_window_fill_with( view->win , &simple_sb->white ,
					x + x_off , y + y_off , count , 1) ;
			}

			x_off += count ;
		}
	}

	return  1 ;
}

static void
get_geometry_hints(
	x_sb_view_t *  view ,
	unsigned int *  width ,
	unsigned int *  top_margin ,
	unsigned int *  bottom_margin ,
	int *  up_button_y ,
	unsigned int *  up_button_height ,
	int *  down_button_y ,
	unsigned int *  down_button_height
	)
{
	*width = WIDTH ;
	*top_margin = TOP_MARGIN ;
	*bottom_margin = BOTTOM_MARGIN ;
	*up_button_y = 0 ;
	*up_button_height = TOP_MARGIN ;
	*down_button_y = -BOTTOM_MARGIN ;
	*down_button_height = BOTTOM_MARGIN ;
}

static void
get_default_color(
	x_sb_view_t *  view ,
	char **  fg_color ,
	char **  bg_color
	)
{
	*fg_color = "gray" ;
	*bg_color = "lightgray" ;
}

static void
realized(
	x_sb_view_t *  view ,
	Display *  display ,
	int  screen ,
	Window  window ,
	GC  gc ,
	unsigned int  height
	)
{
	simple_sb_view_t *  simple_sb ;

	simple_sb = (simple_sb_view_t*) view ;

	view->display = display ;
	view->screen = screen ;
	view->window = window ;
	view->gc = gc ;
	view->height = height ;

	x_load_rgb_xcolor( view->win->disp , &simple_sb->black ,
		0x00 , 0x00 , 0x00 , 0xff) ;
	x_load_rgb_xcolor( view->win->disp , &simple_sb->white ,
		0xff , 0xff , 0xff , 0xff) ;
}

static void
resized(
	x_sb_view_t *  view ,
	Window  window ,
	unsigned int  height
	)
{
	view->window = window ;
	view->height = height ;
}

static void
delete(
	x_sb_view_t *  view
	)
{
	free( view) ;
}

static void
draw_arrow_up_icon(
	x_sb_view_t *  view ,
	int  is_dent
	)
{
	draw_icon( view , 0 , 0 , is_dent ? arrow_up_dent_src : arrow_up_src ,
		WIDTH , TOP_MARGIN) ;
}

static void
draw_arrow_down_icon(
	x_sb_view_t *  view ,
	int  is_dent
	)
{
	draw_icon( view , 0 , view->height - BOTTOM_MARGIN ,
		is_dent ? arrow_down_dent_src : arrow_down_src ,
		WIDTH , BOTTOM_MARGIN) ;
}

static void
draw_scrollbar(
	x_sb_view_t *  view ,
	int  bar_top_y ,
	unsigned int  bar_height
	)
{
	simple_sb_view_t *  simple_sb ;

	simple_sb = (simple_sb_view_t*) view ;

	/* drawing bar */
	if( simple_sb->is_transparent)
	{
		x_window_clear( view->win , 1 , bar_top_y , WIDTH - 1 , bar_height) ;
	}
	else
	{
		x_window_fill( view->win , 1 , bar_top_y , WIDTH - 1 , bar_height) ;
	}

	/* left side shade */
	x_window_fill_with( view->win , &simple_sb->white , 0 , bar_top_y , 1 , bar_height) ;

	/* up side shade */
	x_window_fill_with( view->win , &simple_sb->white , 0 , bar_top_y , WIDTH , 1) ;

	/* right side shade */
	x_window_fill_with( view->win , &simple_sb->black ,
		WIDTH - 1 , bar_top_y , 1 , bar_height) ;

	/* down side shade */
	x_window_fill_with( view->win , &simple_sb->black ,
		1 , bar_top_y + bar_height - 1 , WIDTH , 1) ;
}

static void
draw_background(
	x_sb_view_t *  view ,
	int  y ,
	unsigned int  height
	)
{
	x_window_clear( view->win , 0 , y , WIDTH , height) ;
}

static void
draw_up_button(
	x_sb_view_t *  view ,
	int  is_pressed
	)
{
	draw_arrow_up_icon( view , is_pressed) ;
}

static void
draw_down_button(
	x_sb_view_t *  view ,
	int  is_pressed
	)
{
	draw_arrow_down_icon( view , is_pressed) ;
}


/* --- global functions --- */

x_sb_view_t *
x_simple_sb_view_new(void)
{
	simple_sb_view_t *  simple_sb ;

	if( ( simple_sb = calloc( 1 , sizeof( simple_sb_view_t))) == NULL)
	{
		return  NULL ;
	}

	simple_sb->view.version = 1 ;

	simple_sb->view.get_geometry_hints = get_geometry_hints ;
	simple_sb->view.get_default_color = get_default_color ;
	simple_sb->view.realized = realized ;
	simple_sb->view.resized = resized ;
	simple_sb->view.delete = delete ;

	simple_sb->view.draw_scrollbar = draw_scrollbar ;
	simple_sb->view.draw_background = draw_background ;
	simple_sb->view.draw_up_button = draw_up_button ;
	simple_sb->view.draw_down_button = draw_down_button ;

	return  &simple_sb->view ;
}

x_sb_view_t *
x_simple_transparent_sb_view_new(void)
{
	simple_sb_view_t *  simple_sb ;

	if( ( simple_sb = (simple_sb_view_t*)x_simple_sb_view_new()) == NULL)
	{
		return  NULL ;
	}

	simple_sb->is_transparent = 1 ;

	return  &simple_sb->view ;
}