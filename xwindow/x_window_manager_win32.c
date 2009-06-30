/*
 *	$Id$
 */

#include  "x_window_manager.h"

#include  <string.h>		/* memset/memcpy */
#include  <kiklib/kik_mem.h>
#include  <kiklib/kik_debug.h>
#include  <kiklib/kik_conf_io.h>


#if  0
#define  __DEBUG
#endif


/* --- static variables --- */

static x_window_manager_t *  _win_man ;


/* --- static functions --- */

static LRESULT CALLBACK window_proc(
  	HWND hwnd,
  	UINT msg,
  	WPARAM wparam,
  	LPARAM lparam
  	)
{
	XEvent  event ;
	int  count ;

	event.window = hwnd ;
	event.msg = msg ;
	event.wparam = wparam ;
	event.lparam = lparam ;

	for( count = 0 ; count < _win_man->num_of_roots ; count ++)
	{
		if( x_window_receive_event( _win_man->roots[count] , &event))
		{
			return  0 ;
		}
	}

	return  DefWindowProc( hwnd, msg, wparam, lparam) ;
}

static void
modmap_init(
	Display *  display ,
	x_modifier_mapping_t *  modmap
	)
{
	modmap->serial = 0 ;
#ifdef  USE_WIN32GUI
	modmap->map = NULL ;
#else
	modmap->map = XGetModifierMapping( display) ;
#endif
}

static void
modmap_final(
	x_modifier_mapping_t *  modmap
	)
{
	if( modmap->map)
	{
#ifndef  USE_WIN32GUI
		XFreeModifiermap( modmap->map);
#endif
	}
}


/* --- global functions --- */

int
x_window_manager_init(
	x_window_manager_t *  win_man ,
	Display *  display
	)
{
	WNDCLASS  wc ;
	
	if( _win_man != NULL)
	{
		/* win_man is singleton in win32 .*/

		return  0 ;
	}

	_win_man = win_man ;

  	/* Prepare window class */
  	ZeroMemory(&wc,sizeof(WNDCLASS)) ;
  	wc.lpfnWndProc = window_proc ;
	wc.style = CS_HREDRAW | CS_VREDRAW ;
  	wc.hInstance = display->hinst ;
  	wc.hIcon = 0 ;
  	wc.hCursor = LoadCursor(NULL,IDC_ARROW) ;
  	wc.hbrBackground = 0 ;
  	wc.lpszClassName = "MLTERM" ;

	if( ! RegisterClass(&wc))
	{
		MessageBox(NULL,"Failed to register class", NULL, MB_ICONSTOP) ;

		return  0 ;
	}

	win_man->display = display ;
	
	if( ( win_man->gc = x_gc_new( display)) == NULL)
	{
		return  0 ;
	}

	win_man->screen = 0 ;
	win_man->my_window = None ;
	win_man->group_leader = None ;

	win_man->icon_path = NULL;
	win_man->icon = None ;
	win_man->mask = None ;
	win_man->cardinal = NULL ;

	win_man->roots = NULL ;
	win_man->num_of_roots = 0 ;

	win_man->selection_owner = NULL ;

	modmap_init( display, &(win_man->modmap)) ;

	return  1 ;
}

int
x_window_manager_final(
	x_window_manager_t *  win_man
	)	
{
	int  count ;

	x_gc_delete( win_man->gc) ;
	modmap_final( &(win_man->modmap)) ;

	free(  win_man->icon_path);

#ifndef  USE_WIN32GUI
	if( win_man->icon)
	{
		XFreePixmap( win_man->display, win_man->icon) ;
	}
	if( win_man->mask)
	{
		XFreePixmap( win_man->display, win_man->mask) ;
	}
#endif

	free( win_man->cardinal) ;

	for( count = 0 ; count < win_man->num_of_roots ; count ++)
	{
		x_window_unmap( win_man->roots[count]) ;
		x_window_final( win_man->roots[count]) ;
	}

	free( win_man->roots) ;

	return  1 ;
}

int
x_window_manager_show_root(
	x_window_manager_t *  win_man ,
	x_window_t *  root ,
	int  x ,
	int  y ,
	int  hint ,
	char *  app_name
	)
{
	void *  p ;

	if( ( p = realloc( win_man->roots , sizeof( x_window_t*) * (win_man->num_of_roots + 1))) == NULL)
	{
	#ifdef  DEBUG
		kik_warn_printf( KIK_DEBUG_TAG " realloc failed.\n") ;
	#endif

		return  0 ;
	}

	win_man->roots = p ;

	root->win_man = win_man ;
	root->parent = NULL ;
	root->parent_window = win_man->my_window ;
	root->display = win_man->display ;
	root->screen = win_man->screen ;
	root->gc = win_man->gc ;
	root->x = x ;
	root->y = y ;

	if( app_name)
	{
		root->app_name = app_name ;
	}

	win_man->roots[win_man->num_of_roots++] = root ;

	return  x_window_show( root , hint) ;
}

int
x_window_manager_remove_root(
	x_window_manager_t *  win_man ,
	x_window_t *  root
	)
{
	int  count ;

	for( count = 0 ; count < win_man->num_of_roots ; count ++)
	{
		if( win_man->roots[count] == root)
		{
			x_window_unmap( win_man->roots[count]) ;
			x_window_final( root) ;

			win_man->num_of_roots -- ;

			if( count == win_man->num_of_roots)
			{
				memset( &win_man->roots[count] , 0 , sizeof( win_man->roots[0])) ;
			}
			else
			{
				memcpy( &win_man->roots[count] ,
					&win_man->roots[win_man->num_of_roots] ,
					sizeof( win_man->roots[0])) ;
			}

			return  1 ;
		}
	}

	return  0 ;
}

void
x_window_manager_idling(
	x_window_manager_t *  win_man
	)
{
	int  count ;

	for( count = 0 ; count < win_man->num_of_roots ; count ++)
	{
		x_window_idling( win_man->roots[count]) ;
	}
}

int
x_window_manager_receive_next_event(
	x_window_manager_t *  win_man
	)
{
	return  1 ;
}


/*
 * Folloing functions called from x_window.c
 */

int
x_window_manager_own_selection(
	x_window_manager_t *  win_man ,
	x_window_t *  win
	)
{
#ifndef  USE_WIN32GUI
	if( win_man->selection_owner)
	{
		x_window_manager_clear_selection( win_man , win_man->selection_owner) ;
	}

	win_man->selection_owner = win ;
#endif

	return  1 ;
}

int
x_window_manager_clear_selection(
	x_window_manager_t *  win_man ,
	x_window_t *  win
	)
{
#ifndef  USE_WIN32GUI
	if( win_man->selection_owner == NULL || win_man->selection_owner != win)
	{
		return  0 ;
	}

	if( win_man->selection_owner->selection_cleared)
	{
		(*win_man->selection_owner->selection_cleared)( win_man->selection_owner) ;
	}

	win_man->selection_owner = NULL ;
#endif

	return  1 ;
}

XID
x_window_manager_get_group(
	x_window_manager_t *  win_man
	)
{
	return  win_man->group_leader ;
}


XModifierKeymap *
x_window_manager_get_modifier_mapping(
	x_window_manager_t *  win_man
	)
{
	return  win_man->modmap.map ;
}

void
x_window_manager_update_modifier_mapping(
	x_window_manager_t *  win_man ,
	u_int  serial
	)
{
#ifndef  USE_WIN32GUI
	if( serial != win_man->modmap.serial)
	{
		modmap_final( &(win_man->modmap)) ;
		win_man->modmap.map = XGetModifierMapping( win_man->display) ;
		win_man->modmap.serial = serial ;
	}
#endif
}

int x_window_manager_set_icon(
	x_window_t *  win,
	char *  icon_path
	)
{
	x_window_manager_t *  win_man ;

	win_man = win->win_man ;

	if( !icon_path && !win_man->icon_path)
	{
		/* dont't need icon at all? */
		return  0 ;
	}

	if( !win_man->icon_path)
	{
		x_window_t  dummy = {NULL};

		/* register the default icon */
		if(!(win_man->icon_path = strdup( icon_path)))
		{
			return  0 ;
		}
		dummy.my_window = win_man->group_leader ;
		dummy.display = win_man->display ;
		dummy.icon_pix = None ;
		dummy.icon_mask = None ;
		dummy.icon_card = NULL ;
		x_window_set_icon_from_file( &dummy, icon_path);

		win_man->icon = dummy.icon_pix ;
		win_man->mask = dummy.icon_mask ;
		win_man->cardinal = dummy.icon_card ;

	}

	if( !icon_path || strcmp( icon_path, win_man->icon_path) ==0)
	{
		x_window_remove_icon( win) ;
		/* use a default icon from window manager */
		return x_window_set_icon( win,
					  win_man->icon,
					  win_man->mask,
					  win_man->cardinal) ;
	}
	else
	{
		/* load new icon from "icon_path" */
		return x_window_set_icon_from_file( win, icon_path);
	}
}