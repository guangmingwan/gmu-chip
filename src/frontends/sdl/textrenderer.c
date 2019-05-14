/* 
 * Gmu Music Player
 *
 * Copyright (c) 2006-2012 Johannes Heimansberg (wejp.k.vu)
 *
 * File: textrenderer.c  Created: 060929
 *
 * Description: Bitmap font renderer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2 of
 * the License. See the file COPYING in the Gmu's main directory
 * for details.
 */
#include <string.h>
#include "textrenderer.h"
#include "SDL.h"
#include "SDL_image.h"
#include "charset.h"
#include "iconv.h"
FILE *HZK;
//type 0 font1, 1 font2 3 font_dispalyer
int textrenderer_init(TextRenderer *tr, char *chars_file, char *hzk_file, int chwidth, int chheight,int type)
{
	int result = 0;
	SDL_Surface *tmp = IMG_Load(chars_file);
	if (hzk_file)
	{
		if ((HZK = fopen(hzk_file, "rb")) == NULL)
		{
			printf("Can't Open %s\n", hzk_file);
			exit(0);
		}
	}
	tr->chars = NULL;
	if (tmp)
	{
		if ((tr->chars = SDL_DisplayFormatAlpha(tmp)))
		{
			tr->chwidth = chwidth;
			tr->chheight = chheight;
			if(type == 0) {
				tr->color = SDL_MapRGB(tr->chars->format, 0xff, 0xff, 0xff);
			}
			else if(type == 1) {
				tr->color = SDL_MapRGB(tr->chars->format, 0xff, 0x94, 0x3e);
			}
			else {
				tr->color = SDL_MapRGB(tr->chars->format, 0xff, 0xff, 0xff);
			}
			result = 1;
		}
		SDL_FreeSurface(tmp);
	}
	return result;
}

void textrenderer_free(TextRenderer *tr)
{
	if (tr->chars != NULL)
	{
		SDL_FreeSurface(tr->chars);
		tr->chars = NULL;
	}
	if(HZK) {
		fclose(HZK);
		HZK = NULL;
	}
}

void textrenderer_draw_asc_char(const TextRenderer *tr, UCodePoint ch, SDL_Surface *target, int target_x, int target_y)
{
	int origin_x = target_x;
	const int n = (ch - '!') * tr->chwidth;
	SDL_Rect srect, drect;
	//fprintf(stderr,"textrenderer_draw_asc_char::enter\n" );
	if(ch == "\n") { //new line
		target_y +=16;
		target_x = origin_x;
	}
	else if(ch < 0x7F) {
		if (n >= 0)
		{
			srect.x = 1 + n;
			srect.y = 1;
			srect.w = tr->chwidth;
			srect.h = tr->chheight;

			drect.x = target_x;
			drect.y = target_y;
			drect.w = 1;
			drect.h = 1;

			//fprintf(stderr,"textrenderer_draw_asc_char: 0x%2x,%c\n", ch ,ch );
			SDL_BlitSurface(tr->chars, &srect, target, &drect);
			
			//textrenderer_draw_asc_char(tr, str[i], target, target_x + i * (tr->chwidth + 1), target_y);
		}
		
	}
	else {
		textrenderer_draw_cjk_char(tr, ch, target,target_x , target_y);
	}
	//fprintf(stderr,"textrenderer_draw_asc_char::exit\n" );

	
}

void textrenderer_draw_string_codepoints(const TextRenderer *tr, const UCodePoint *str, int str_len, SDL_Surface *target, int target_x, int target_y)
{
	int i;
	int origin_x = target_x;
	for (i = 0; i < str_len && str[i]; i++)
	if(str[i] == "\n") { //new line
		target_y +=16;
		target_x = origin_x;
	}
	else if(str[i] < 0x7F) {
		textrenderer_draw_asc_char(tr, str[i], target, target_x + i * (tr->chwidth + 1), target_y);
	}
	else {
		textrenderer_draw_cjk_char(tr, str[i], target,target_x + i * (tr->chwidth + 1), target_y);
	}
}

int u2g(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("utf-8", "gbk", inbuf, inlen, outbuf, outlen);
}

int g2u(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	return code_convert("gbk", "utf-8", inbuf, inlen, outbuf, outlen);
}


int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen,
        char *outbuf, size_t outlen) {
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset, from_charset);
    if (cd == 0)
        return -1;
    memset(outbuf, 0, outlen);
    if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
        return -1;
    iconv_close(cd);
    *pout = '\0';

    return 0;
}
/*
* Return the pixel value at (x, y)
* NOTE: The surface must be locked before calling this!
*/
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch(bpp) 
    {
    case 1:
        return *p;
    case 2:
        return *(Uint16 *)p;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32 *)p;
    default:
        return 0; /* shouldn't happen, but avoids warnings */
    }
}
 
/*
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
 */
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
 
    switch(bpp) {
    case 1:
        *p = pixel;
        break;
 
    case 2:
        *(Uint16 *)p = pixel;
        break;
 
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;
 
    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}
void textrenderer_draw_cjk_char(const TextRenderer *tr,Uint16  ch, SDL_Surface *screen,int ShowX,int ShowY)
{
   int x=ShowX;
   int y=ShowY; // 显示位置设置
   int i,j,k;   
   unsigned char incode[3]={0};    // 要读出的汉字
   incode[0]=(ch & 0xff00 ) >> 8;
   incode[1]=ch & 0x00ff; // GBK内码
   unsigned char licode[3]={0};
   licode[0] = incode[1];
   licode[1] = incode[0]; 
   unsigned char buf[10];
   g2u(incode, strlen(licode), buf, sizeof(buf));

   //fprintf(stderr,"textrenderer_draw_cjk_char: 0x%4x,%d,%d,%s\n", ch,ShowX,ShowY,buf);
   unsigned char qh,wh;   
   unsigned long offset;   
         
   char mat[32]; 
 
   // 占两个字节,取其区位号   
   qh=incode[0]-0xa0;    
   wh=incode[1]-0xa0;                
   offset=(94*(qh-1)+(wh-1))*24;  
   
   fseek(HZK,offset,SEEK_SET);   
   fread(mat,24,1,HZK);
   for(j=0;j<12;j++)
   {
        for(i=0;i<2;i++)
		{
            for(k=0;k<8;k++)
			{
				if(((mat[j*2+i]>>(7-k))&0x1)!=0)
				{
					DrawOnePoint(screen,x+6*i+k,y+j,tr);
				}
			}
		}
   }
}
 
void DrawOnePoint(SDL_Surface *screen,int x,int y, const TextRenderer *tr)
{
    Uint32 white;
    /* Map the color yellow to this display (R=0xff, G=ff, B=0x00)
       Note:  If the display is palettized, you must set the palette first.
    */
    white = SDL_MapRGB(screen->format, 0xff, 0xff, 0xff);

	Uint32 blue;
	blue = SDL_MapRGB(screen->format, 0xff, 0x94, 0x3e);
    /* Lock the screen for direct access to the pixels */
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            fprintf(stderr, "Can't lock screen: %s\n", SDL_GetError());
            return;
        }
    }
    putpixel(screen, x, y, tr->color);
    if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }
    /* Update just the part of the display that we've changed */
    SDL_UpdateRect(screen, x, y, 1, 1);
    return;
}
void textrenderer_draw_string(const TextRenderer *tr, const char *str, SDL_Surface *target, int target_x, int target_y)
{
	unsigned char gbk_str[1024];
	u2g(str, strlen(str), gbk_str, sizeof(gbk_str));
	int utf8_chars = charset_gbk_len(gbk_str) + 1;
	UCodePoint *ustr = utf8_chars > 0 ? malloc(sizeof(UCodePoint) * (utf8_chars + 1)) : NULL;

	if (ustr && charset_gbk_to_codepoints(ustr, gbk_str, utf8_chars))
	{
		textrenderer_draw_string_codepoints(tr, ustr, utf8_chars, target, target_x, target_y);
	}
	if (ustr)
		free(ustr);
}

int textrenderer_get_string_length(const char *str)
{
	int i, len = (int)strlen(str);
	int len_const = len;
	unsigned char gbk_str[1024];
	u2g(str, strlen(str), gbk_str, sizeof(gbk_str));

	int utf8_chars = charset_gbk_len(gbk_str);

	for (i = 0; i < len_const - 1; i++)
		if (str[i] == '*' && str[i + 1] == '*')
			utf8_chars--;
	return utf8_chars;
}

void textrenderer_draw_string_with_highlight(const TextRenderer *tr1, const TextRenderer *tr2,
											 const char *str, int str_offset,
											 SDL_Surface *target, int target_x, int target_y,
											 int max_length, Render_Mode rm)
{
	int highlight = 0;
	int i, j;
	int l = (int)strlen(str);
	unsigned char gbk_str[1024];
	u2g(str, strlen(str), gbk_str, sizeof(gbk_str));
	int utf8_chars = charset_gbk_len(gbk_str) + 1;
	UCodePoint *ustr = utf8_chars > 0 ? malloc(sizeof(UCodePoint) * (utf8_chars + 1)) : NULL;

	if (rm == RENDER_ARROW)
	{
		if (str_offset > 0)
			textrenderer_draw_asc_char(tr2, '<', target, target_x, target_y);
		if (textrenderer_get_string_length(str) - str_offset > max_length)
		{
			textrenderer_draw_asc_char(tr2, '>', target, target_x + (max_length - 1) * (tr2->chwidth + 1), target_y);
			max_length--;
		}
	}

	if (rm == RENDER_CROP)
	{
		if (utf8_chars > max_length)
		{
			int current_max = 0;

			for (i = 0, j = 0; j < max_length; i++, j++)
			{
				if (str[i] == '*' && i + 1 < l && str[i + 1] == '*')
					j -= 2;
				if (str[i] == ' ')
					current_max = j;
			}
			max_length = current_max;
		}
	}

	if (ustr && charset_gbk_to_codepoints(ustr, gbk_str, utf8_chars))
	{
		for (i = 0, j = 0; i < utf8_chars && j - str_offset < max_length; i++, j++)
		{
			if (ustr[i] == '*' && i + 1 < utf8_chars && ustr[i + 1] == '*')
			{
				highlight = !highlight;
				i += 2;
			}
			if (j >= str_offset && (j != str_offset || str_offset == 0))
			{
				if (!highlight)
					textrenderer_draw_asc_char(tr1, ustr[i], target,
										   target_x + (j - str_offset) * (tr1->chwidth + 1), target_y);
				else
					textrenderer_draw_asc_char(tr2, ustr[i], target,
										   target_x + (j - str_offset) * (tr2->chwidth + 1), target_y);
			}
		}
	}
	if (ustr)
		free(ustr);
}
