/*
 *	$Id$
 */

#include  "ml_picture_dep.h"

#include  <X11/Xatom.h>			/* XA_PIXMAP */
#include  <kiklib/kik_debug.h>


/* --- global functions --- */

int
ml_picdep_display_opened(
	Display *  display
	)
{
	return  1 ;
}

int
ml_picdep_display_closed(
	Display *  display
	)
{
	return  1 ;
}

Pixmap
ml_picdep_load_file(
	ml_window_t *  win ,
	char *  file_path ,
	ml_picture_modifier_t *  pic_mod
	)
{
	return  None ;
}

int
ml_picdep_root_pixmap_available(
	Display *   display
	)
{
	if( XInternAtom( display , "_XROOTPMAP_ID" , True))
	{
		return  1 ;
	}

	return  0 ;
}

Pixmap
ml_picdep_load_background(
	ml_window_t *  win ,
	ml_picture_modifier_t *  pic_mod
	)
{
	Atom id ;
	int  x ;
	int  y ;
	int  _x ;
	int  _y ;
	u_int  width ;
	u_int  height ;
	Pixmap  pixmap ;
	Atom act_type ;
	int  act_format ;
	u_long  nitems ;
	u_long  bytes_after ;
	u_char *  prop ;

	if( pic_mod && pic_mod->brightness != 100)
	{
		/*
		 * XXX
		 * image is never modified without Imlib.
		 */
		
		return  None ;
	}
	
	if( ( id = XInternAtom( win->display , "_XROOTPMAP_ID" , True)) == None)
	{
		return  None ;
	}
	
	if( ! ml_window_get_visible_geometry( win , &x , &y , &_x , &_y , &width , &height))
	{
		return  None ;
	}
	
	if( XGetWindowProperty( win->display , DefaultRootWindow(win->display) , id , 0 , 1 , False ,
		XA_PIXMAP , &act_type , &act_format , &nitems , &bytes_after , &prop) != Success ||
		prop == NULL)
	{
		return  None ;
	}
	
	if( ! *prop)
	{
		XFree( prop) ;

		return  None ;
	}
	
#ifdef  __DEBUG
	kik_debug_printf( KIK_DEBUG_TAG " root pixmap %d found.\n" , *prop) ;
#endif

	pixmap = XCreatePixmap( win->display , win->my_window , ACTUAL_WIDTH(win) , ACTUAL_HEIGHT(win) ,
			DefaultDepth( win->display , win->screen)) ;

	XCopyArea( win->display , (*(Drawable*)prop) , pixmap , win->gc ,
		x , y , width , height , _x , _y) ;

	return  pixmap ;
}
