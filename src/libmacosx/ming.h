/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* ming.h
 * 
 * $Id: ming.h.in,v 1.1 2005/12/06 14:32:28 strk Exp $
 *
 * This header file contains all declarations of functions and types that
 * are supposed to be publicly accessable.
 */

#ifndef SWF_MING_H_INCLUDED
#define SWF_MING_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define MING_VERSION        0.3.0
#define MING_VERSION_TEXT  "0.3.0"

/* do we enable gif / png functions? */
#include "ming_config.h"


/***** Type definitions - avoid cyclic dependencies ****/

typedef unsigned char byte;

typedef struct SWFBlock_s *SWFBlock;
typedef struct SWFMatrix_s *SWFMatrix;
typedef struct SWFInput_s *SWFInput;
typedef struct SWFCharacter_s *SWFCharacter;
typedef struct SWFDBLBitmap_s *SWFDBLBitmap;
typedef struct SWFDBLBitmapData_s *SWFDBLBitmapData;
typedef struct SWFJpegBitmap_s *SWFJpegBitmap;
typedef struct SWFJpegWithAlpha_s *SWFJpegWithAlpha;
typedef struct SWFGradient_s *SWFGradient;
typedef struct SWFFillStyle_s *SWFFillStyle;
typedef struct SWFLineStyle_s *SWFLineStyle;
typedef struct SWFShape_s *SWFShape;
typedef struct SWFMorph_s *SWFMorph;
typedef struct SWFFont_s *SWFFont;
typedef struct SWFText_s *SWFText;
typedef struct SWFBrowserFont_s *SWFBrowserFont;
typedef struct SWFFontCharacter_s *SWFFontCharacter;
typedef struct SWFTextField_s *SWFTextField;
typedef struct SWFSoundStream_s *SWFSoundStream;
typedef struct SWFSound_s *SWFSound;
typedef struct SWFSoundInstance_s *SWFSoundInstance;
typedef struct SWFCXform_s *SWFCXform;
typedef struct SWFOutputBlock_s *SWFAction;
typedef struct SWFButton_s *SWFButton;
typedef struct SWFSprite_s *SWFSprite;
typedef struct SWFPosition_s *SWFPosition;
typedef struct SWFDisplayItem_s *SWFDisplayItem;
typedef struct SWFFill_s *SWFFill;
typedef struct SWFMovieClip_s *SWFMovieClip;
typedef struct SWFMovie_s *SWFMovie;
typedef struct SWFVideoStream_s *SWFVideoStream;
typedef struct SWFPrebuiltClip_s *SWFPrebuiltClip;

/***** General Ming functions *****/

int Ming_init();
void Ming_cleanup();
void Ming_collectGarbage();

/* sets the threshold error for drawing cubic beziers.  Lower is more
   accurate, hence larger file size. */
void Ming_setCubicThreshold(int num);

/* sets the overall scale, default is 20 */
void Ming_setScale(float scale);
float Ming_getScale();

/* set the version number to use */
void Ming_useSWFVersion(int version);

/*
 * Set output compression level.
 * Return previous value.
 */ 
int Ming_setSWFCompression(int level);

/* change the error/warn behavior.  Default prints message and exits. */
void Ming_setWarnFunction(void (*warn)(const char *msg, ...));
void Ming_setErrorFunction(void (*error)(const char *msg, ...));

/* a generic output method.  specific instances dump output to file,
   send to stdout, etc. */
typedef void (*SWFByteOutputMethod)(byte b, void *data);
//void fileOutputMethod(byte b, void *data);


/***** SWFBlock *****/


/***** SWFMatrix *****/


/***** SWFInput *****/

/* A generic input object.  Wraps files, buffers and streams; replaces
   standard file funcs */

SWFInput newSWFInput_file(FILE *f);
SWFInput newSWFInput_stream(FILE *f);
SWFInput newSWFInput_buffer(unsigned char *buffer, int length);
SWFInput newSWFInput_allocedBuffer(unsigned char *buffer, int length);
void destroySWFInput(SWFInput input);

int SWFInput_length(SWFInput input);
void SWFInput_rewind(SWFInput input);
int SWFInput_tell(SWFInput input);
void SWFInput_seek(SWFInput input, long offset, int whence);
int SWFInput_eof(SWFInput input);


/***** SWFCharacter *****/

/* a character is any sort of asset that's referenced later-
   SWFBitmap, SWFShape, SWFMorph, SWFSound, SWFSprite are all SWFCharacters */

float SWFCharacter_getWidth(SWFCharacter character);
float SWFCharacter_getHeight(SWFCharacter character);


/***** SWFBitmap *****/

typedef SWFCharacter SWFBitmap;

SWFBitmap newSWFBitmap_fromInput(SWFInput input);
void destroySWFBitmap(SWFBitmap bitmap);

int SWFBitmap_getWidth(SWFBitmap b);
int SWFBitmap_getHeight(SWFBitmap b);


/***** SWFDBLBitmap extends SWFBitmap *****/

/* create a new DBL (define bits lossless) bitmap from the given file */
SWFDBLBitmap newSWFDBLBitmap(FILE *f);
/* create a new DBL bitmap from the given input object */
SWFDBLBitmap newSWFDBLBitmap_fromInput(SWFInput input);

#if USE_GIF
SWFDBLBitmapData newSWFDBLBitmapData_fromGifFile(const char *name);
SWFDBLBitmapData newSWFDBLBitmapData_fromGifInput(SWFInput input);
#endif
#if USE_PNG
SWFDBLBitmapData newSWFDBLBitmapData_fromPngFile(const char *name);
SWFDBLBitmapData newSWFDBLBitmapData_fromPngInput(SWFInput input);
#endif


/***** SWFJpegBitmap extends SWFBitmap *****/

SWFJpegBitmap newSWFJpegBitmap(FILE *f);
SWFJpegBitmap newSWFJpegBitmap_fromInput(SWFInput input);

SWFJpegWithAlpha newSWFJpegWithAlpha(FILE *f, FILE *alpha);
SWFJpegWithAlpha newSWFJpegWithAlpha_fromInput(SWFInput input, SWFInput alpha);


/***** SWFGradient *****/

SWFGradient newSWFGradient();
void destroySWFGradient(SWFGradient gradient);

void SWFGradient_addEntry(SWFGradient gradient,
			  float ratio, byte r, byte g, byte b, byte a);


/***** SWFFillStyle - a fill instance on a shape *****/

#define SWFFILL_SOLID			0x00
#define SWFFILL_GRADIENT		0x10
#define SWFFILL_LINEAR_GRADIENT 	0x10
#define SWFFILL_RADIAL_GRADIENT 	0x12
#define SWFFILL_BITMAP			0x40
#define SWFFILL_TILED_BITMAP		0x40
#define SWFFILL_CLIPPED_BITMAP		0x41

SWFFillStyle newSWFSolidFillStyle(byte r, byte g, byte b, byte a);
SWFFillStyle newSWFGradientFillStyle(SWFGradient gradient, byte radial);
SWFFillStyle newSWFBitmapFillStyle(SWFCharacter bitmap, byte flags);

SWFMatrix SWFFillStyle_getMatrix(SWFFillStyle fill);


/***** SWFLineStyle *****/

SWFLineStyle newSWFLineStyle(unsigned short width, byte r, byte g, byte b, byte a);


/***** SWFShape *****/

SWFShape newSWFShape();
/*
 * returns a shape containing the bitmap in a filled rect
 * flag can be SWFFILL_CLIPPED_BITMAP or SWFFILL_TILED_BITMAP
 */
SWFShape newSWFShapeFromBitmap(SWFBitmap bitmap, int flag);
void destroySWFShape(SWFShape shape);

void SWFShape_end(SWFShape shape);

void SWFShape_movePenTo(SWFShape shape, float x, float y);
void SWFShape_movePen(SWFShape shape, float x, float y);

float SWFShape_getPenX(SWFShape shape);
float SWFShape_getPenY(SWFShape shape);
void SWFShape_getPen(SWFShape shape, float* penX, float* penY);

/* x,y relative to shape origin */
void SWFShape_drawLineTo(SWFShape shape, float x, float y);
void SWFShape_drawLine(SWFShape shape, float dx, float dy);

void SWFShape_drawCurveTo(SWFShape shape, float controlx, float controly,
			  float anchorx, float anchory);
void SWFShape_drawCurve(SWFShape shape, float controldx, float controldy,
			float anchordx, float anchordy);

void SWFShape_setLineStyle(SWFShape shape, unsigned short width,
			  byte r, byte g, byte b, byte a);
void SWFShape_hideLine(SWFShape shape);

SWFFillStyle SWFShape_addSolidFillStyle(SWFShape shape,
					byte r, byte g, byte b, byte a);
SWFFillStyle SWFShape_addGradientFillStyle(SWFShape shape,
					   SWFGradient gradient, byte flags);
SWFFillStyle SWFShape_addBitmapFillStyle(SWFShape shape,
					 SWFBitmap bitmap, byte flags);

void SWFShape_setLeftFillStyle(SWFShape shape, SWFFillStyle fill);
void SWFShape_setRightFillStyle(SWFShape shape, SWFFillStyle fill);


/***** SWFMorph *****/

SWFMorph newSWFMorphShape();
void destroySWFMorph(SWFMorph morph);

SWFShape SWFMorph_getShape1(SWFMorph morph);
SWFShape SWFMorph_getShape2(SWFMorph morph);


/***** SWFFont *****/

SWFFont newSWFFont();
SWFFont loadSWFFontFromFile(FILE *file);
void destroySWFFont(SWFFont font);

float SWFFont_getStringWidth(SWFFont font, const unsigned char *string);
float SWFFont_getUTF8StringWidth(SWFFont font, const unsigned char *string);

  /* deprecated? */
  float SWFFont_getWideStringWidth(SWFFont font, const unsigned short *string, int len);
  #define SWFFont_getWidth SWFFont_getStringWidth

float SWFFont_getAscent(SWFFont font);
float SWFFont_getDescent(SWFFont font);
float SWFFont_getLeading(SWFFont font);
char *SWFFont_getShape(SWFFont font, unsigned short code);


/***** SWFText *****/

SWFText newSWFText();
SWFText newSWFText2();
void destroySWFText(SWFText text);

void SWFText_setFont(SWFText text, void* font);
void SWFText_setHeight(SWFText text, float height);
void SWFText_setColor(SWFText text, byte r, byte g, byte b, byte a);

void SWFText_moveTo(SWFText text, float x, float y);

void SWFText_addString(SWFText text, const char* string, int* advance);
void SWFText_addUTF8String(SWFText text, const char* string, int* advance);
void SWFText_addWideString(SWFText text, const unsigned short* string,
                           int strlen, int* advance);

void SWFText_setSpacing(SWFText text, float spacing);

float SWFText_getStringWidth(SWFText text, const unsigned char* string);
float SWFText_getUTF8StringWidth(SWFText text, const unsigned char* string);
float SWFText_getWideStringWidth(SWFText text, const unsigned short* string);

  /* deprecated? */
  #define SWFText_getWidth SWFText_getStringWidth

float SWFText_getAscent(SWFText text);
float SWFText_getDescent(SWFText text);
float SWFText_getLeading(SWFText text);

  /* deprecated: */
  #define SWFText_setXY(t,x,y) SWFText_moveTo((t),(x),(y))


/***** SWFBrowserFont *****/

SWFBrowserFont newSWFBrowserFont(const char *name);
void destroySWFBrowserFont(SWFBrowserFont browserFont);


/***** SWFFontCharacter *****/

SWFFontCharacter SWFMovie_addFont(SWFMovie movie, SWFFont font);
void SWFFontCharacter_addChars(SWFFontCharacter font, const char *string);
void SWFFontCharacter_addUTF8Chars(SWFFontCharacter font, const char *string);
SWFFontCharacter SWFMovie_importFont(SWFMovie movie, const char *filename, const char *name);


/***** SWFTextField *****/

#define SWFTEXTFIELD_ONMASK  0x2005 /* on bits */
#define SWFTEXTFIELD_OFFMASK 0x3BFF /* off bits */
/* Taken from textfield.h: #define SWFTEXTFIELD_OFFMASK 0x7BFF / off bits */

#define SWFTEXTFIELD_HASFONT   (1<<0)   /* font and size given */
#define SWFTEXTFIELD_HASLENGTH (1<<1)
#define SWFTEXTFIELD_HASCOLOR  (1<<2)
#define SWFTEXTFIELD_NOEDIT    (1<<3)
#define SWFTEXTFIELD_PASSWORD  (1<<4)
#define SWFTEXTFIELD_MULTILINE (1<<5)
#define SWFTEXTFIELD_WORDWRAP  (1<<6)
#define SWFTEXTFIELD_HASTEXT   (1<<7)   /* initial text present */
#define SWFTEXTFIELD_USEFONT   (1<<8)
#define SWFTEXTFIELD_HTML      (1<<9)
#define SWFTEXTFIELD_DRAWBOX   (1<<11)
#define SWFTEXTFIELD_NOSELECT  (1<<12)
#define SWFTEXTFIELD_HASLAYOUT (1<<13)  /* align, margin, lspace, indent */
#define SWFTEXTFIELD_AUTOSIZE  (1<<14)  /* SWF6 */

typedef enum
{
  SWFTEXTFIELD_ALIGN_LEFT    = 0,
  SWFTEXTFIELD_ALIGN_RIGHT   = 1,
  SWFTEXTFIELD_ALIGN_CENTER  = 2,
  SWFTEXTFIELD_ALIGN_JUSTIFY = 3
} SWFTextFieldAlignment;

SWFTextField newSWFTextField();
void destroySWFTextField(SWFTextField textField);

void SWFTextField_setFont(SWFTextField field, SWFBlock font);
void SWFTextField_setBounds(SWFTextField field, float width, float height);
void SWFTextField_setFlags(SWFTextField field, int flags);
void SWFTextField_setColor(SWFTextField field, byte r, byte g, byte b, byte a);
void SWFTextField_setVariableName(SWFTextField field, const char *name);

void SWFTextField_addString(SWFTextField field, const char *string);
void SWFTextField_addUTF8String(SWFTextField field, const char *string);

void SWFTextField_setHeight(SWFTextField field, float height);
void SWFTextField_setFieldHeight(SWFTextField field, float height);
void SWFTextField_setLeftMargin(SWFTextField field, float leftMargin);
void SWFTextField_setRightMargin(SWFTextField field, float rightMargin);
void SWFTextField_setIndentation(SWFTextField field, float indentation);
void SWFTextField_setLineSpacing(SWFTextField field, float lineSpacing);
void SWFTextField_setPadding(SWFTextField field, float padding);

void SWFTextField_addChars(SWFTextField field, const char *string);

  /* deprecated? */
  /*void SWFTextField_addUTF8Chars(SWFTextField field, const char *string);*/

void SWFTextField_setAlignment(SWFTextField field,
			       SWFTextFieldAlignment alignment);
void SWFTextField_setLength(SWFTextField field, int length);


/***** SWFSoundStream - only mp3 streaming implemented *****/

SWFSoundStream newSWFSoundStream(FILE *file);
/* added by David McNab <david@rebirthing.co.nz> */
SWFSoundStream newSWFSoundStreamFromFileno(int fd);
SWFSoundStream newSWFSoundStream_fromInput(SWFInput input);
int SWFSoundStream_getFrames(SWFSoundStream sound);
void destroySWFSoundStream(SWFSoundStream soundStream);


/***** SWFSound *****/

SWFSound newSWFSound(FILE *file, byte flags);
/* added by David McNab to facilitate Python access */
SWFSound newSWFSoundFromFileno(int fd, byte flags);
SWFSound newSWFSound_fromInput(SWFInput input, byte flags);
void destroySWFSound(SWFSound sound);

#define SWF_SOUND_COMPRESSION       0xf0
#define SWF_SOUND_NOT_COMPRESSED    (0<<4)
#define SWF_SOUND_ADPCM_COMPRESSED  (1<<4)
#define SWF_SOUND_MP3_COMPRESSED    (2<<4)
#define SWF_SOUND_NOT_COMPRESSED_LE (3<<4)
#define SWF_SOUND_NELLY_COMPRESSED  (6<<4)

#define SWF_SOUND_RATE              0x0c
#define SWF_SOUND_5KHZ              (0<<2)
#define SWF_SOUND_11KHZ             (1<<2)
#define SWF_SOUND_22KHZ             (2<<2)
#define SWF_SOUND_44KHZ             (3<<2)

#define SWF_SOUND_BITS              0x02
#define SWF_SOUND_8BITS             (0<<1)
#define SWF_SOUND_16BITS            (1<<1)

#define SWF_SOUND_CHANNELS          0x01
#define SWF_SOUND_MONO              (0<<0)
#define SWF_SOUND_STEREO            (1<<0)


/***** SWFSoundInstance *****/

/* created from SWFMovie[Clip]_startSound,
   lets you change the parameters of the sound event (loops, etc.) */

void SWFSoundInstance_setNoMultiple(SWFSoundInstance instance);
void SWFSoundInstance_setLoopInPoint(SWFSoundInstance instance, unsigned int point);
void SWFSoundInstance_setLoopOutPoint(SWFSoundInstance instance, unsigned int point);
void SWFSoundInstance_setLoopCount(SWFSoundInstance instance, int count);
void SWFSoundInstance_setNoMultiple(SWFSoundInstance instance);


/***** SWFCXform - Color transform *****/

/* create a new color transform with the given parameters */
SWFCXform newSWFCXform(int rAdd, int gAdd, int bAdd, int aAdd,
		       float rMult, float gMult, float bMult, float aMult);
/* create a new color transform with the given additive parameters and
   default multiplicative */
SWFCXform newSWFAddCXform(int rAdd, int gAdd, int bAdd, int aAdd);
/* create a new color transform with the given multiplicative parameters
   and default additive */
SWFCXform newSWFMultCXform(float rMult, float gMult, float bMult, float aMult);
void destroySWFCXform(SWFCXform cXform);

/* set the additive part of the color transform to the given parameters */
void SWFCXform_setColorAdd(SWFCXform cXform,
			   int rAdd, int gAdd, int bAdd, int aAdd);
/* set the multiplicative part of the color transform to the given
   parameters */
void SWFCXform_setColorMult(SWFCXform cXform,
			    float rMult, float gMult, float bMult, float aMult);


/***** SWFAction *****/

SWFAction compileSWFActionCode(const char *script);
void destroySWFAction(SWFAction action);


/***** SWFButton *****/

#define SWFBUTTON_HIT    (1<<3)
#define SWFBUTTON_DOWN   (1<<2)
#define SWFBUTTON_OVER   (1<<1)
#define SWFBUTTON_UP     (1<<0)

  /* deprecated: */

  #define SWFBUTTONRECORD_HITSTATE    (1<<3)
  #define SWFBUTTONRECORD_DOWNSTATE   (1<<2)
  #define SWFBUTTONRECORD_OVERSTATE   (1<<1)
  #define SWFBUTTONRECORD_UPSTATE     (1<<0)


#define SWFBUTTON_KEYPRESS(c)     (((c)&0x7f)<<9)
#define SWFBUTTON_ONKEYPRESS(c)     (((c)&0x7f)<<9)

#define SWFBUTTON_OVERDOWNTOIDLE    (1<<8)
#define SWFBUTTON_IDLETOOVERDOWN    (1<<7)
#define SWFBUTTON_OUTDOWNTOIDLE     (1<<6)
#define SWFBUTTON_OUTDOWNTOOVERDOWN (1<<5)
#define SWFBUTTON_OVERDOWNTOOUTDOWN (1<<4)
#define SWFBUTTON_OVERDOWNTOOVERUP  (1<<3)
#define SWFBUTTON_OVERUPTOOVERDOWN  (1<<2)
#define SWFBUTTON_OVERUPTOIDLE      (1<<1)
#define SWFBUTTON_IDLETOOVERUP      (1<<0)

/* easier to remember: */
#define SWFBUTTON_MOUSEUPOUTSIDE  SWFBUTTON_OUTDOWNTOIDLE
#define SWFBUTTON_DRAGOVER        (SWFBUTTON_OUTDOWNTOOVERDOWN | SWFBUTTON_IDLETOOVERDOWN)
#define SWFBUTTON_DRAGOUT         (SWFBUTTON_OVERDOWNTOOUTDOWN | SWFBUTTON_OVERDOWNTOIDLE)
#define SWFBUTTON_MOUSEUP         SWFBUTTON_OVERDOWNTOOVERUP
#define SWFBUTTON_MOUSEDOWN       SWFBUTTON_OVERUPTOOVERDOWN
#define SWFBUTTON_MOUSEOUT        SWFBUTTON_OVERUPTOIDLE
#define SWFBUTTON_MOUSEOVER       SWFBUTTON_IDLETOOVERUP

SWFButton newSWFButton();
void destroySWFButton(SWFButton button);

void SWFButton_addShape(SWFButton button, SWFCharacter character, byte flags);
void SWFButton_addAction(SWFButton button, SWFAction action, int flags);
SWFSoundInstance SWFButton_addSound(SWFButton button, SWFSound action, byte flags);
void SWFButton_setMenu(SWFButton button, int flag);


/****** SWFVideo ******/

SWFVideoStream newSWFVideoStream_fromFile(FILE *f);
SWFVideoStream newSWFVideoStream_fromInput(SWFInput input);
SWFVideoStream newSWFVideoStream(void);
void SWFVideoStream_setDimension(SWFVideoStream stream, int width, int height);
int SWFVideoStream_getNumFrames(SWFVideoStream stream);



/***** SWFSprite *****/

SWFSprite newSWFSprite();
void destroySWFSprite(SWFSprite sprite);

void SWFSprite_addBlock(SWFSprite sprite, SWFBlock block);


/***** SWFPosition *****/

struct _swfPosition
{
  int x;
  int y;
  float xScale;
  float yScale;
  float xSkew;
  float ySkew;
  float rot;
  SWFMatrix matrix;
};

#define SWF_POSITION_SIZE sizeof(struct _swfPosition)

SWFPosition newSWFPosition(SWFMatrix matrix);
void destroySWFPosition(SWFPosition position);

void SWFPosition_skewX(SWFPosition position, float x);
void SWFPosition_skewXTo(SWFPosition position, float x);
void SWFPosition_skewY(SWFPosition position, float y);
void SWFPosition_skewYTo(SWFPosition position, float y);

void SWFPosition_scaleX(SWFPosition position, float x);
void SWFPosition_scaleXTo(SWFPosition position, float x);
void SWFPosition_scaleY(SWFPosition position, float y);
void SWFPosition_scaleYTo(SWFPosition position, float y);
void SWFPosition_scaleXY(SWFPosition position, float x, float y);
void SWFPosition_scaleXYTo(SWFPosition position, float x, float y);

void SWFPosition_setMatrix(SWFPosition p, float a, float b, float c, float d,
			   float x, float y);

void SWFPosition_rotate(SWFPosition position, float degrees);
void SWFPosition_rotateTo(SWFPosition position, float degrees);

void SWFPosition_move(SWFPosition position, float x, float y);
void SWFPosition_moveTo(SWFPosition position, float x, float y);


/***** SWFDisplayItem *****/

void SWFDisplayItem_move(SWFDisplayItem item, float x, float y);
void SWFDisplayItem_moveTo(SWFDisplayItem item, float x, float y);
void SWFDisplayItem_rotate(SWFDisplayItem item, float degrees);
void SWFDisplayItem_rotateTo(SWFDisplayItem item, float degrees);
void SWFDisplayItem_scale(SWFDisplayItem item, float xScale, float yScale);
void SWFDisplayItem_scaleTo(SWFDisplayItem item, float xScale, float yScale);
void SWFDisplayItem_skewX(SWFDisplayItem item, float x);
void SWFDisplayItem_skewXTo(SWFDisplayItem item, float x);
void SWFDisplayItem_skewY(SWFDisplayItem item, float y);
void SWFDisplayItem_skewYTo(SWFDisplayItem item, float y);

void SWFDisplayItem_getPosition(SWFDisplayItem item, float * x, float * y);
void SWFDisplayItem_getRotation(SWFDisplayItem item, float * degrees);
void SWFDisplayItem_getScale(SWFDisplayItem item, float * xScale, float * yScale);
void SWFDisplayItem_getSkew(SWFDisplayItem item, float * xSkew, float * ySkew);

void SWFDisplayItem_setMatrix(SWFDisplayItem i, float a, float b,
			      float c, float d, float x, float y);

int SWFDisplayItem_getDepth(SWFDisplayItem item);
void SWFDisplayItem_setDepth(SWFDisplayItem item, int depth);
void SWFDisplayItem_remove(SWFDisplayItem item);
void SWFDisplayItem_setName(SWFDisplayItem item, const char *name);
void SWFDisplayItem_setMaskLevel(SWFDisplayItem item, int masklevel);
void SWFDisplayItem_setRatio(SWFDisplayItem item, float ratio);
void SWFDisplayItem_setCXform(SWFDisplayItem item, SWFCXform cXform);
void SWFDisplayItem_setColorAdd(SWFDisplayItem item,
				int r, int g, int b, int a);
void SWFDisplayItem_setColorMult(SWFDisplayItem item,
				 float r, float g, float b, float a);

#define SWFDisplayItem_addColor SWFDisplayItem_setColorAdd
#define SWFDisplayItem_multColor SWFDisplayItem_setColorMult

#define SWFACTION_ONLOAD      (1<<0)
#define SWFACTION_ENTERFRAME  (1<<1)
#define SWFACTION_UNLOAD      (1<<2)
#define SWFACTION_MOUSEMOVE   (1<<3)
#define SWFACTION_MOUSEDOWN   (1<<4)
#define SWFACTION_MOUSEUP     (1<<5)
#define SWFACTION_KEYDOWN     (1<<6)
#define SWFACTION_KEYUP       (1<<7)
#define SWFACTION_DATA        (1<<8)

void SWFDisplayItem_addAction(SWFDisplayItem item, SWFAction action, int flags);


/***** SWFFill *****/

/* adds a position object to manipulate SWFFillStyle's matrix */

SWFFill newSWFFill(SWFFillStyle fillstyle);
void destroySWFFill(SWFFill fill);

void SWFFill_skewX(SWFFill fill, float x);
void SWFFill_skewXTo(SWFFill fill, float x);
void SWFFill_skewY(SWFFill fill, float y);
void SWFFill_skewYTo(SWFFill fill, float y);

void SWFFill_scaleX(SWFFill fill, float x);
void SWFFill_scaleXTo(SWFFill fill, float x);
void SWFFill_scaleY(SWFFill fill, float y);
void SWFFill_scaleYTo(SWFFill fill, float y);
void SWFFill_scaleXY(SWFFill fill, float x, float y);
void SWFFill_scaleXYTo(SWFFill fill, float x, float y);

  /* Deprecated? */
  #define SWFFill_scale    SWFFill_scaleXY
  #define SWFFill_scaleTo  SWFFill_scaleXYTo

void SWFFill_rotate(SWFFill fill, float degrees);
void SWFFill_rotateTo(SWFFill fill, float degrees);

void SWFFill_move(SWFFill fill, float x, float y);
void SWFFill_moveTo(SWFFill fill, float x, float y);

void SWFFill_setMatrix(SWFFill fill, float a, float b,
		       float c, float d, float x, float y);


/***** shape_util.h *****/

void SWFShape_setLine(SWFShape shape, unsigned short width,
		      byte r, byte g, byte b, byte a);

SWFFill SWFShape_addSolidFill(SWFShape shape, byte r, byte g, byte b, byte a);
SWFFill SWFShape_addGradientFill(SWFShape shape, SWFGradient gradient, byte flags);
SWFFill SWFShape_addBitmapFill(SWFShape shape, SWFBitmap bitmap, byte flags);

void SWFShape_setLeftFill(SWFShape shape, SWFFill fill);
void SWFShape_setRightFill(SWFShape shape, SWFFill fill);

void SWFShape_drawArc(SWFShape shape, float r, float startAngle, float endAngle);
void SWFShape_drawCircle(SWFShape shape, float r);

/* draw character c from font font into shape shape at size size */
void SWFShape_drawGlyph(SWFShape shape, SWFFont font, unsigned short c);
void SWFShape_drawSizedGlyph(SWFShape shape, SWFFont font, unsigned short c, int size);

  /* Deprecated: */
  #define SWFShape_drawFontGlyph(s,f,c) SWFShape_drawGlyph(s,f,c)

/* approximate a cubic bezier with quadratic segments */
/* returns the number of segments used */
int SWFShape_drawCubic(SWFShape shape, float bx, float by,
		       float cx, float cy, float dx, float dy);
int SWFShape_drawCubicTo(SWFShape shape, float bx, float by,
			 float cx, float cy, float dx, float dy);
void SWFShape_drawCharacterBounds(SWFShape shape, SWFCharacter character);


/***** SWFMovieClip *****/

SWFMovieClip newSWFMovieClip();
void destroySWFMovieClip(SWFMovieClip movieClip);

void SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int frames);
void SWFMovieClip_nextFrame(SWFMovieClip clip);
void SWFMovieClip_labelFrame(SWFMovieClip clip, const char *label);

SWFDisplayItem SWFMovieClip_add(SWFMovieClip clip, SWFBlock block);
void SWFMovieClip_remove(SWFMovieClip clip, SWFDisplayItem item);

void SWFMovieClip_setSoundStream(SWFMovieClip clip, SWFSoundStream sound, float rate);
void SWFMovie_setSoundStreamAt(SWFMovie movie, SWFSoundStream stream, float skip);
SWFSoundInstance SWFMovieClip_startSound(SWFMovieClip clip, SWFSound sound);
void SWFMovieClip_stopSound(SWFMovieClip clip, SWFSound sound);

/***** SWFPrebuiltClip ****/
SWFPrebuiltClip newSWFPrebuiltClip_fromFile(const char *filename);
SWFPrebuiltClip newSWFPrebuiltClip_fromInput(SWFInput input);

/***** SWFMovie *****/

SWFMovie newSWFMovie();
SWFMovie newSWFMovieWithVersion(int version);
void destroySWFMovie(SWFMovie movie);

void SWFMovie_setRate(SWFMovie movie, float rate);
void SWFMovie_setDimension(SWFMovie movie, float x, float y);
void SWFMovie_setNumberOfFrames(SWFMovie movie, int frames);

void SWFMovie_addExport(SWFMovie movie, SWFBlock block, const char *name);

void SWFMovie_setBackground(SWFMovie movie, byte r, byte g, byte b);

void SWFMovie_setSoundStream(SWFMovie movie, SWFSoundStream sound);
SWFSoundInstance SWFMovie_startSound(SWFMovie movie, SWFSound sound);
void SWFMovie_stopSound(SWFMovie movie, SWFSound sound);

SWFDisplayItem SWFMovie_add(SWFMovie movie, SWFBlock block);
void SWFMovie_remove(SWFMovie movie, SWFDisplayItem item);

void SWFMovie_nextFrame(SWFMovie movie);
void SWFMovie_labelFrame(SWFMovie movie, const char *label);
void SWFMovie_namedAnchor(SWFMovie movie, const char *label);

int SWFMovie_output(SWFMovie movie, SWFByteOutputMethod method, void *data);
int SWFMovie_save(SWFMovie movie, const char *filename);
int SWFMovie_output_to_stream(SWFMovie movie, FILE *fp);

  /* deprecated */
  /*int SWFMovie_outputC(SWFMovie movie, SWFByteOutputMethod method, void *data, int level);*/

SWFCharacter SWFMovie_importCharacter(SWFMovie movie, const char *filename, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* SWF_MING_H_INCLUDED */

