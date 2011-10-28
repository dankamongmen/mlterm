/*
 *	$Id$
 */

#include  "x_color.h"

#include  <string.h>		/* memcpy,strcmp */
#include  <stdio.h>		/* sscanf */
#include  <kiklib/kik_mem.h>
#include  <kiklib/kik_debug.h>
#include  <kiklib/kik_util.h>
#include  <ml_color.h>


/* --- static functions --- */

static void
native_color_to_xcolor(
	x_color_t *  xcolor ,
	XColor *  ncolor
	)
{
	xcolor->pixel = ncolor->pixel ;
	xcolor->red = (ncolor->red >> 8) & 0xff ;
	xcolor->green = (ncolor->green >> 8) & 0xff ;
	xcolor->blue = (ncolor->blue >> 8) & 0xff ;
	xcolor->alpha = 0xff ;
}

static int
alloc_closest_xcolor_pseudo(
	x_display_t *  disp ,
	unsigned short  red ,		/* 0 to 0xffff */
	unsigned short  green ,		/* 0 to 0xffff */
	unsigned short  blue ,		/* 0 to 0xffff */
	x_color_t *  ret_xcolor
	)
{
	XColor *  all_colors ;	/* colors exist in the shared color map */
	XColor  closest_color ;

	int  closest_index = -1 ;
	unsigned long  min_diff = 0xffffffff ;
	unsigned long  diff ;
	unsigned long  diff_r = 0 , diff_g = 0 , diff_b = 0 ;
	int  ncells = disp->visual->map_entries ;
	int  i ;

	/* FIXME: When visual class is StaticColor, should not be return? */
	if ( ! disp->visual->class == PseudoColor && ! disp->visual->class == GrayScale)
	{
		return  0 ;
	}

	if( ( all_colors = malloc( ncells * sizeof( XColor))) == NULL)
	{
	#ifdef  DEBUG
		kik_warn_printf( KIK_DEBUG_TAG " malloc() failed.\n") ;
	#endif

		return  0 ;
	}

	/* get all colors from default colormap */
	for( i = 0 ; i < ncells ; i ++)
	{
		all_colors[i].pixel = i ;
	}
	XQueryColors( disp->display , disp->colormap , all_colors, ncells) ;

	/* find the closest color */
	for( i = 0 ; i < ncells ; i ++)
	{
		diff_r = (red - all_colors[i].red) >> 8 ;
		diff_g = (green - all_colors[i].green) >> 8 ;
		diff_b = (blue - all_colors[i].blue) >> 8 ;

		diff = diff_r * diff_r + diff_g * diff_g + diff_b * diff_b ;

		if ( diff < min_diff) /* closest ? */
		{
			min_diff = diff ;
			closest_index = i ;
		}
	}

	if( closest_index == -1)	/* unable to find closest color */
	{
		closest_color.red = 0 ;
		closest_color.green = 0 ;
		closest_color.blue = 0 ;
	}
	else
	{
		closest_color.red = all_colors[closest_index].red ;
		closest_color.green = all_colors[closest_index].green ;
		closest_color.blue = all_colors[closest_index].blue ;
	}

	closest_color.flags = DoRed | DoGreen | DoBlue;
	free( all_colors) ;

	if ( ! XAllocColor( disp->display , disp->colormap , &closest_color))
	{
		return 0 ;
	}

	ret_xcolor->pixel = closest_color.pixel ;
	ret_xcolor->red = (closest_color.red >> 8) & 0xff ;
	ret_xcolor->green = (closest_color.green >> 8) & 0xff ;
	ret_xcolor->blue = (closest_color.blue >> 8) & 0xff ;
	ret_xcolor->alpha = 0xff ;

	return 1 ;
}


/* --- global functions --- */

int
x_load_named_xcolor(
	x_display_t *  disp ,
	x_color_t *  xcolor ,
	char *  name
	)
{
	XColor  near ;
	XColor  exact ;
	u_int8_t  red ;
	u_int8_t  green ;
	u_int8_t  blue ;
	u_int8_t  alpha ;
	
	if( ml_color_parse_rgb_name( &red , &green , &blue , &alpha , name))
	{
		return  x_load_rgb_xcolor( disp , xcolor , red , green , blue , alpha) ;
	}

	if( ! XAllocNamedColor( disp->display , disp->colormap , name , &near , &exact))
	{
		/* try to find closest color */
		if( XParseColor( disp->display , disp->colormap , name , &exact))
		{
			return  alloc_closest_xcolor_pseudo( disp ,
					exact.red , exact.green , exact.blue , xcolor) ;
		}
		else
		{
			return  0 ;
		}
	}

	native_color_to_xcolor( xcolor , &near) ;

	return  1 ;

}

int
x_load_rgb_xcolor(
	x_display_t *  disp ,
	x_color_t *  xcolor ,
	u_int8_t  red ,
	u_int8_t  green ,
	u_int8_t  blue ,
	u_int8_t  alpha
	)
{
	if( disp->depth == 32 && alpha < 0xff)
	{
		xcolor->red = red ;
		xcolor->green = green ;
		xcolor->blue = blue ;
		xcolor->alpha = alpha ;
		/* XXX */
		xcolor->pixel = (alpha << 24) |
				(((u_int)red * (u_int)alpha / 256) << 16) |
				(((u_int)green * (u_int)alpha / 256) << 8) |
				(((u_int)blue * (u_int)alpha / 256)) ;
	}
	else
	{
		XColor  ncolor ;

		ncolor.red = (red << 8) + red ;
		ncolor.green = (green << 8) + green ;
		ncolor.blue = (blue << 8) + blue ;
		ncolor.flags = 0 ;

		if( ! XAllocColor( disp->display , disp->colormap , &ncolor))
		{
			/* try to find closest color */
			return  alloc_closest_xcolor_pseudo( disp , red , green , blue , xcolor) ;
		}

		native_color_to_xcolor( xcolor , &ncolor) ;
	}

	return  1 ;
}

int
x_unload_xcolor(
	x_display_t *  disp ,
	x_color_t *  xcolor
	)
{
#if  0
	u_long  pixel[1] ;

	pixel[0] = xcolor->pixel ;
	
	XFreeColors( disp->display , disp->colormap , pixel , 1 , 0) ;
#endif

	return  1 ;
}

int
x_get_xcolor_rgb(
	u_int8_t *  red ,
	u_int8_t *  green ,
	u_int8_t *  blue ,
	u_int8_t *  alpha ,	/* can be NULL */
	x_color_t *  xcolor
	)
{
	*red = xcolor->red ;
	*green = xcolor->green ;
	*blue = xcolor->blue ;

	if( alpha)
	{
		*alpha = xcolor->alpha ;
	}

	return  1 ;
}

int
x_xcolor_fade(
	x_display_t *  disp ,
	x_color_t *  xcolor ,
	u_int  fade_ratio
	)
{
	u_int8_t  red ;
	u_int8_t  green ;
	u_int8_t  blue ;
	u_int8_t  alpha ;

	x_get_xcolor_rgb( &red , &green , &blue , &alpha , xcolor) ;

#if  0
	kik_msg_printf( "Fading R%d G%d B%d => " , red , green , blue) ;
#endif

	red = (red * fade_ratio) / 100 ;
	green = (green * fade_ratio) / 100 ;
	blue = (blue * fade_ratio) / 100 ;

	x_unload_xcolor( disp , xcolor) ;

#if  0
	kik_msg_printf( "R%d G%d B%d\n" , red , green , blue) ;
#endif

	return  x_load_rgb_xcolor( disp , xcolor , red , green , blue , alpha) ;
}
