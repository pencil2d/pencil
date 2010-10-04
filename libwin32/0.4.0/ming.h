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
 * $Id: ming.h.in,v 1.100 2008/09/12 09:13:35 krechert Exp $
 *
 * This header file contains all declarations of functions and types that
 * are supposed to be publicly accessable.
 */

#ifndef SWF_MING_H_INCLUDED
#define SWF_MING_H_INCLUDED

#ifdef __GNUC__
	#define __deprecated __attribute__((deprecated))
#else
	#define __deprecated
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define MING_VERSION        0.4.2
#define MING_VERSION_TEXT  "0.4.2"

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
typedef struct SWFFontCollection_s *SWFFontCollection;
typedef struct SWFTextField_s *SWFTextField;
typedef struct SWFSoundStream_s *SWFSoundStream;
typedef struct SWFSound_s *SWFSound;
typedef struct SWFSoundInstance_s *SWFSoundInstance;
typedef struct SWFCXform_s *SWFCXform;
typedef struct SWFAction_s *SWFAction;
typedef struct SWFInitAction_s *SWFInitAction;
typedef struct SWFButton_s *SWFButton;
typedef struct SWFSprite_s *SWFSprite;
typedef struct SWFPosition_s *SWFPosition;
typedef struct SWFDisplayItem_s *SWFDisplayItem;
typedef struct SWFFill_s *SWFFill;
typedef struct SWFMovieClip_s *SWFMovieClip;
typedef struct SWFMovie_s *SWFMovie;
typedef struct SWFVideoStream_s *SWFVideoStream;
typedef struct SWFPrebuiltClip_s *SWFPrebuiltClip;
typedef struct SWFFilter_s * SWFFilter;
typedef struct SWFButtonRecord_s* SWFButtonRecord;
typedef struct SWFSymbolClass_s* SWFSymbolClass;
typedef struct SWFBinaryData_s* SWFBinaryData;
typedef struct SWFSceneData_s* SWFSceneData;

union SWFMovieBlockType_u {
	SWFBlock 		block;
	SWFCharacter 		character;
	SWFDBLBitmap 		dblbmp;
	SWFDBLBitmapData 	dblbmp_data;
	SWFJpegBitmap 		jpegbmp;
	SWFJpegWithAlpha        jpegbmp_alpha;
	SWFShape 		shape;
	SWFMorph		morph;
	SWFText			text;
	SWFTextField		textfield;
	SWFSound		sound;
	SWFSoundInstance	sound_instance;
	SWFAction		action;
	SWFButton		button;
	SWFSprite		sprite;
	SWFMovieClip		mc;
	SWFVideoStream		stream;
	SWFPrebuiltClip		prebuild;
	SWFBinaryData		binary;
	SWFInitAction		init;
};

typedef union SWFMovieBlockType_u SWFMovieBlockType;

#if __GNUC__ > 3 ||  \
	(__GNUC__ == 3 && (__GNUC_MINOR__ > 3 || \
                 (__GNUC_MINOR__ == 3 && __GNUC_PATCHLEVEL__ > 0)))
#define __to_movie_block(x) (union SWFMovieBlockType_u)x
#else
#define __to_movie_block(x) (union SWFMovieBlockType_u) { x }
#endif

/***** General Ming functions *****/

int Ming_init(void);
void Ming_cleanup(void);
void Ming_collectGarbage(void);
void Ming_useConstants(int flag);

/* sets the threshold error for drawing cubic beziers.  Lower is more
   accurate, hence larger file size. */
void Ming_setCubicThreshold(int num);

/* sets the overall scale, default is 20.0 */
void Ming_setScale(float scale);
float Ming_getScale(void);

/* set the version number to use */
void Ming_useSWFVersion(int version);

/*
 * Set output compression level.
 * Return previous value.
 */ 
int Ming_setSWFCompression(int level);

/*
 * Error and warning callbacks.
 */
typedef void (*SWFMsgFunc)(const char *msg, ...);

/*
 * Set the function that gets called when a warning occurs within the library
 * This function sets function to be called when a warning occurs within the
 * library. The default function prints the warning message to stdout.
 * Returns the previously-set warning function.
 */
SWFMsgFunc Ming_setWarnFunction(SWFMsgFunc);

/*
 * Set the function that gets called when an error occurs within the library
 * This function sets function to be called when an error occurs within the
 * library. The default function prints the error message to stdout and exits.
 * Returns the previously-set error function.
 */
SWFMsgFunc Ming_setErrorFunction(SWFMsgFunc);

/* a generic output method.  specific instances dump output to file,
   send to stdout, etc. */
typedef void (*SWFByteOutputMethod)(byte b, void *data);
void fileOutputMethod(byte b, void *data);


/***** SWFBlock *****/


/***** SWFMatrix *****/

double SWFMatrix_getScaleX(SWFMatrix m);
double SWFMatrix_getRotate0(SWFMatrix m);
double SWFMatrix_getRotate1(SWFMatrix m);
double SWFMatrix_getScaleY(SWFMatrix m);
int SWFMatrix_getTranslateX(SWFMatrix m);
int SWFMatrix_getTranslateY(SWFMatrix m);


/***** SWFInput *****/

/* A generic input object.  Wraps files, buffers and streams; replaces
   standard file funcs */
SWFInput newSWFInput_file(FILE *f);
SWFInput newSWFInput_filename(const char *);
SWFInput newSWFInput_stream(FILE *f);
SWFInput newSWFInput_buffer(unsigned char *buffer, int length);
SWFInput newSWFInput_allocedBuffer(unsigned char *buffer, int length);
SWFInput newSWFInput_bufferCopy(unsigned char *buffer, int length);
SWFInput newSWFInput_input(SWFInput in, unsigned int length);
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

typedef enum 
{
	SWF_DBL_COLORTABLE,
	SWF_DBL_RGB15,
	SWF_DBL_RGB24,
	SWF_DBL_RGB32
} SWFBitmapFmt;

typedef enum {
	SWF_RAWIMG_ARGB
} SWFRawImgFmt;


typedef SWFCharacter SWFBitmap;

SWFBitmap newSWFBitmap_fromInput(SWFInput input);
SWFBitmap newSWFBitmap_fromRawImg(unsigned char *raw, 
                                  SWFRawImgFmt srcFmt, SWFBitmapFmt dstFmt,
                                  unsigned short width, unsigned short height);

void destroySWFBitmap(SWFBitmap bitmap);

int SWFBitmap_getWidth(SWFBitmap b);
int SWFBitmap_getHeight(SWFBitmap b);


/***** SWFDBLBitmap extends SWFBitmap *****/

/* create a new DBL (define bits lossless) bitmap from the given file */
SWFDBLBitmap newSWFDBLBitmap(FILE *f);
/* create a new DBL bitmap from the given input object */
SWFDBLBitmap newSWFDBLBitmap_fromInput(SWFInput input);

SWFDBLBitmapData newSWFDBLBitmapData_fromGifFile(const char *name);
SWFDBLBitmapData newSWFDBLBitmapData_fromGifInput(SWFInput input);

SWFDBLBitmapData newSWFDBLBitmapData_fromPngFile(const char *name);
SWFDBLBitmapData newSWFDBLBitmapData_fromPngInput(SWFInput input);


/***** SWFJpegBitmap extends SWFBitmap *****/

SWFJpegBitmap newSWFJpegBitmap(FILE *f);
SWFJpegBitmap newSWFJpegBitmap_fromInput(SWFInput input);

SWFJpegWithAlpha newSWFJpegWithAlpha(FILE *f, FILE *alpha);
SWFJpegWithAlpha newSWFJpegWithAlpha_fromInput(SWFInput input, SWFInput alpha);


/***** SWFGradient *****/

typedef enum
{
        SWF_GRADIENT_PAD,
        SWF_GRADIENT_REFLECT,
        SWF_GRADIENT_REPEAT
} GradientSpreadMode;

typedef enum
{
        SWF_GRADIENT_NORMAL,
        SWF_GRADIENT_LINEAR
} GradientInterpolationMode;

SWFGradient newSWFGradient(void);
void destroySWFGradient(SWFGradient gradient);

void SWFGradient_addEntry(SWFGradient gradient,
			  float ratio, byte r, byte g, byte b, byte a);

void SWFGradient_setSpreadMode(SWFGradient gradient, GradientSpreadMode mode);
void SWFGradient_setInterpolationMode(SWFGradient gradient, GradientInterpolationMode mode);
void SWFGradient_setFocalPoint(SWFGradient gradient, float focalPoint);

/***** SWFFillStyle - a fill instance on a shape *****/

#define SWFFILL_SOLID				0x00
#define SWFFILL_GRADIENT			0x10
#define SWFFILL_LINEAR_GRADIENT 		0x10
#define SWFFILL_RADIAL_GRADIENT 		0x12
#define SWFFILL_FOCAL_GRADIENT			0x13
#define SWFFILL_BITMAP				0x40
#define SWFFILL_TILED_BITMAP			0x40
#define SWFFILL_CLIPPED_BITMAP			0x41
#define SWFFILL_NONSMOOTHED_TILED_BITMAP 	0x42
#define SWFFILL_NONSMOOTHED_CLIPPED_BITMAP 	0x43


SWFFillStyle newSWFSolidFillStyle(byte r, byte g, byte b, byte a);
SWFFillStyle newSWFGradientFillStyle(SWFGradient gradient, byte radial);
SWFFillStyle newSWFBitmapFillStyle(SWFCharacter bitmap, byte flags);

SWFMatrix SWFFillStyle_getMatrix(SWFFillStyle fill);
void destroySWFFillStyle(SWFFillStyle fill);

/***** SWFLineStyle *****/

SWFLineStyle newSWFLineStyle(unsigned short width, byte r, byte g, byte b, byte a);

/* linestyle 2 flags */

#define SWF_LINESTYLE_CAP_ROUND         (0<<14)
#define SWF_LINESTYLE_CAP_NONE          (1<<14)
#define SWF_LINESTYLE_CAP_SQUARE        (2<<14)

#define SWF_LINESTYLE_JOIN_ROUND        (0<<12)
#define SWF_LINESTYLE_JOIN_BEVEL        (1<<12)
#define SWF_LINESTYLE_JOIN_MITER        (2<<12)

#define SWF_LINESTYLE_FLAG_NOHSCALE     (1<<10)
#define SWF_LINESTYLE_FLAG_NOVSCALE     (1<<9)
#define SWF_LINESTYLE_FLAG_HINTING      (1<<8)

#define SWF_LINESTYLE_FLAG_NOCLOSE      (1<<2)

#define SWF_LINESTYLE_FLAG_ENDCAP_ROUND  (0<<0)
#define SWF_LINESTYLE_FLAG_ENDCAP_NONE   (1<<0)
#define SWF_LINESTYLE_FLAG_ENDCAP_SQUARE (2<<0)

SWFLineStyle newSWFLineStyle2(unsigned short width, byte r, byte g, byte b, byte a,
                              int flags, float miterLimit);
SWFLineStyle newSWFLineStyle2_filled(unsigned short width, 
                                     SWFFillStyle fill, int flags, 
                                     float miterLimit);

/***** SWFShape *****/

#define SWF_SHAPE3 3
#define SWF_SHAPE4 4

#define SWF_SHAPE_USESCALINGSTROKES 	(1<<0)
#define SWF_SHAPE_USENONSCALINGSTROKES	(1<<1)

SWFShape newSWFShape(void);
/*
 * returns a shape containing the bitmap in a filled rect
 * flag can be SWFFILL_CLIPPED_BITMAP or SWFFILL_TILED_BITMAP
 */
SWFShape newSWFShapeFromBitmap(SWFBitmap bitmap, int flag);
void destroySWFShape(SWFShape shape);

void SWFShape_end(SWFShape shape);
void SWFShape_useVersion(SWFShape shape, int version);
int SWFShape_getVersion(SWFShape shape);
void SWFShape_setRenderHintingFlags(SWFShape shape, int flags);

void SWFShape_movePenTo(SWFShape shape, double x, double y);
void SWFShape_movePen(SWFShape shape, double x, double y);

double SWFShape_getPenX(SWFShape shape);
double SWFShape_getPenY(SWFShape shape);
void SWFShape_getPen(SWFShape shape, double* penX, double* penY);

/* x,y relative to shape origin */
void SWFShape_drawLineTo(SWFShape shape, double x, double y);
void SWFShape_drawLine(SWFShape shape, double dx, double dy);

void SWFShape_drawCurveTo(SWFShape shape, double controlx, double controly,
			  double anchorx, double anchory);
void SWFShape_drawCurve(SWFShape shape, double controldx, double controldy,
			double anchordx, double anchordy);

void SWFShape_setLineStyle(SWFShape shape, unsigned short width,
			  byte r, byte g, byte b, byte a) __deprecated;

void SWFShape_setLineStyle2(SWFShape shape, unsigned short width,
                            byte r, byte g, byte b, byte a, 
                            int flags, float miterLimit) __deprecated;

void SWFShape_setLineStyle2filled(SWFShape shape, unsigned short width,
                                  SWFFillStyle fill,
                                  int flags, float miterLimit) __deprecated;

void SWFShape_hideLine(SWFShape shape);

SWFFillStyle SWFShape_addSolidFillStyle(SWFShape shape,
					byte r, byte g, byte b, byte a);
SWFFillStyle SWFShape_addGradientFillStyle(SWFShape shape,
					   SWFGradient gradient, byte flags);
SWFFillStyle SWFShape_addBitmapFillStyle(SWFShape shape,
					 SWFBitmap bitmap, byte flags);

void SWFShape_setLeftFillStyle(SWFShape shape, SWFFillStyle fill);
void SWFShape_setRightFillStyle(SWFShape shape, SWFFillStyle fill);

char *SWFShape_dumpOutline(SWFShape s);
/***** SWFMorph *****/

SWFMorph newSWFMorphShape(void);
void destroySWFMorph(SWFMorph morph);

SWFShape SWFMorph_getShape1(SWFMorph morph);
SWFShape SWFMorph_getShape2(SWFMorph morph);


/***** SWFFont *****/

SWFFont newSWFFont_fromFile(const char *filename);

/* pull font definition from fdb (font def block) file */
SWFFont loadSWFFontFromFile(FILE *file) __deprecated;
void destroySWFFont(SWFFont font);

float SWFFont_getStringWidth(SWFFont font, const char *string);
float SWFFont_getUTF8StringWidth(SWFFont font, const char *string);

  /* deprecated? */
  float SWFFont_getWideStringWidth(SWFFont font, const unsigned short *string, int len);
  #define SWFFont_getWidth SWFFont_getStringWidth

float SWFFont_getAscent(SWFFont font);
float SWFFont_getDescent(SWFFont font);
float SWFFont_getLeading(SWFFont font);
const char* SWFFont_getName(SWFFont font);
int SWFFont_getGlyphCount(SWFFont font);

SWFFontCollection newSWFFontCollection_fromFile(const char *filename);
SWFFont *SWFFontCollection_getFonts(SWFFontCollection collection, int *count);
int SWFFontCollection_getFontCount(SWFFontCollection collection);
SWFFont SWFFontCollection_getFont(SWFFontCollection collection, int index);
void destroySWFFontCollection(SWFFontCollection collection);

char *SWFFont_getShape(SWFFont font, unsigned short c) __deprecated;

/***** SWFText *****/

SWFText newSWFText(void);
SWFText newSWFText2(void);
void destroySWFText(SWFText text);

void SWFText_setFont(SWFText text, SWFFont font);
void SWFText_setHeight(SWFText text, float height);
void SWFText_setColor(SWFText text, byte r, byte g, byte b, byte a);

void SWFText_moveTo(SWFText text, float x, float y);

void SWFText_addString(SWFText text, const char* string, int* advance);
void SWFText_addUTF8String(SWFText text, const char* string, int* advance);
void SWFText_addWideString(SWFText text, const unsigned short* string,
                           int strlen, int* advance);

void SWFText_setSpacing(SWFText text, float spacing);

float SWFText_getStringWidth(SWFText text, const char* string);
float SWFText_getUTF8StringWidth(SWFText text, const char* string);
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

void SWFFontCharacter_addChars(SWFFontCharacter font, const char *string);
void SWFFontCharacter_addUTF8Chars(SWFFontCharacter font, const char *string);
void SWFFontCharacter_addAllChars(SWFFontCharacter);

/***** SWFTextField *****/

#define SWFTEXTFIELD_HASFONT   (1<<0)   /* private */
#define SWFTEXTFIELD_HASLENGTH (1<<1)   /* private */
#define SWFTEXTFIELD_HASCOLOR  (1<<2)   /* private */ 
#define SWFTEXTFIELD_NOEDIT    (1<<3)   /* disables editing */
#define SWFTEXTFIELD_PASSWORD  (1<<4)   /* hides characters */
#define SWFTEXTFIELD_MULTILINE (1<<5)   /* multiline and scrollable */
#define SWFTEXTFIELD_WORDWRAP  (1<<6)   /* enable automatic line wrap */
#define SWFTEXTFIELD_HASTEXT   (1<<7)   /* private */
#define SWFTEXTFIELD_USEFONT   (1<<8)   /* private */
#define SWFTEXTFIELD_HTML      (1<<9)   /* renders some HTML tags*/
#define SWFTEXTFIELD_DRAWBOX   (1<<11)  /* draws a border */
#define SWFTEXTFIELD_NOSELECT  (1<<12)  /* disabled selection */
#define SWFTEXTFIELD_HASLAYOUT (1<<13)  /* private */
#define SWFTEXTFIELD_AUTOSIZE  (1<<14)  /* resizes to textlen */

typedef enum
{
  SWFTEXTFIELD_ALIGN_LEFT    = 0,
  SWFTEXTFIELD_ALIGN_RIGHT   = 1,
  SWFTEXTFIELD_ALIGN_CENTER  = 2,
  SWFTEXTFIELD_ALIGN_JUSTIFY = 3
} SWFTextFieldAlignment;

SWFTextField newSWFTextField(void);
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
int SWFSoundStream_getFrames(SWFSoundStream sound) __deprecated;
void destroySWFSoundStream(SWFSoundStream soundStream);
unsigned int SWFSoundStream_getDuration(SWFSoundStream);


/***** SWFSound *****/

SWFSound newSWFSound(FILE *file, byte flags);
/* added by David McNab to facilitate Python access */
SWFSound newSWFSoundFromFileno(int fd, byte flags);
SWFSound newSWFSound_fromInput(SWFInput input, byte flags);
SWFSound newSWFSound_fromSoundStream(SWFSoundStream stream);
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
void SWFSoundInstance_addEnvelope(SWFSoundInstance inst, unsigned int mark44, short left, short right);

/***** SWFCXform - Color transform *****/
SWFCXform newSWFCXform(int rAdd, int gAdd, int bAdd, int aAdd,
		       float rMult, float gMult, float bMult, float aMult);
SWFCXform newSWFAddCXform(int rAdd, int gAdd, int bAdd, int aAdd);
SWFCXform newSWFMultCXform(float rMult, float gMult, float bMult, float aMult);
void destroySWFCXform(SWFCXform cXform);
void SWFCXform_setColorAdd(SWFCXform cXform,
			   int rAdd, int gAdd, int bAdd, int aAdd);
void SWFCXform_setColorMult(SWFCXform cXform,
			    float rMult, float gMult, float bMult, float aMult);

/***** SWFAction *****/
SWFAction newSWFAction(const char *script);
SWFAction newSWFAction_fromFile(const char *filename);
int SWFAction_compile(SWFAction action, int swfVersion, int *length);
SWFAction compileSWFActionCode(const char *script) __deprecated;
void destroySWFAction(SWFAction action);
byte *SWFAction_getByteCode(SWFAction action, int *length);
int SWFAction_setDebug(SWFAction action, int debug);

SWFInitAction newSWFInitAction(SWFAction action);
void destroySWFInitAction(SWFInitAction init);
/* use with care */
SWFInitAction newSWFInitAction_withId(SWFAction action, int id);



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

SWFButton newSWFButton(void);
void destroySWFButton(SWFButton button);

void SWFButton_addShape(SWFButton button, SWFCharacter character, byte flags) __deprecated;
SWFButtonRecord SWFButton_addCharacter(SWFButton, SWFCharacter, byte flags);
void SWFButton_addAction(SWFButton button, SWFAction action, int flags);
SWFSoundInstance SWFButton_addSound(SWFButton button, SWFSound action, byte flags);
void SWFButton_setMenu(SWFButton button, int flag);
void SWFButton_setScalingGrid(SWFButton b, int x, int y, int w, int h);
void SWFButton_removeScalingGrid(SWFButton b);

void SWFButtonRecord_setDepth(SWFButtonRecord b, int depth);
void SWFButtonRecord_addFilter(SWFButtonRecord b, SWFFilter f);
void SWFButtonRecord_setBlendMode(SWFButtonRecord b, int mode);
void SWFButtonRecord_move(SWFButtonRecord record, double x, double y);
void SWFButtonRecord_moveTo(SWFButtonRecord record, double x, double y);
void SWFButtonRecord_rotate(SWFButtonRecord record, double deg);
void SWFButtonRecord_rotateTo(SWFButtonRecord record, double deg);
void SWFButtonRecord_scale(SWFButtonRecord record, double scaleX, double scaleY);
void SWFButtonRecord_scaleTo(SWFButtonRecord record, double scaleX, double scaleY);
void SWFButtonRecord_skewX(SWFButtonRecord record, double skewX);
void SWFButtonRecord_skewXTo(SWFButtonRecord record, double skewX);
void SWFButtonRecord_skewY(SWFButtonRecord record, double skewY);
void SWFButtonRecord_skewYTo(SWFButtonRecord record, double skewY);

/****** SWFVideo ******/

#define SWFVIDEOSTREAM_MODE_AUTO 	0
#define SWFVIDEOSTREAM_MODE_MANUAL 	1

void destroySWFVideoStream(SWFVideoStream stream);
SWFVideoStream newSWFVideoStream_fromFile(FILE *f);
SWFVideoStream newSWFVideoStream_fromInput(SWFInput input);
SWFVideoStream newSWFVideoStream(void);
void SWFVideoStream_setDimension(SWFVideoStream stream, int width, int height);
int SWFVideoStream_getNumFrames(SWFVideoStream stream);
int SWFVideoStream_hasAudio(SWFVideoStream stream);
int SWFVideoStream_setFrameMode(SWFVideoStream stream, int mode);
int SWFVideoStream_nextFrame(SWFVideoStream stream);
int SWFVideoStream_seek(SWFVideoStream stream, int frame, int whence);

/***** SWFSprite *****/

SWFSprite newSWFSprite(void);
void destroySWFSprite(SWFSprite sprite);

void SWFSprite_addBlock(SWFSprite sprite, SWFBlock block);


/***** SWFPosition *****/

SWFPosition newSWFPosition(SWFMatrix matrix);
void destroySWFPosition(SWFPosition position);

void SWFPosition_skewX(SWFPosition position, double x);
void SWFPosition_skewXTo(SWFPosition position, double x);
void SWFPosition_skewY(SWFPosition position, double y);
void SWFPosition_skewYTo(SWFPosition position, double y);

void SWFPosition_scaleX(SWFPosition position, double x);
void SWFPosition_scaleXTo(SWFPosition position, double x);
void SWFPosition_scaleY(SWFPosition position, double y);
void SWFPosition_scaleYTo(SWFPosition position, double y);
void SWFPosition_scaleXY(SWFPosition position, double x, double y);
void SWFPosition_scaleXYTo(SWFPosition position, double x, double y);

SWFMatrix SWFPosition_getMatrix(SWFPosition p);
void SWFPosition_setMatrix(SWFPosition p, double a, double b, double c, double d,
			   double x, double y);

void SWFPosition_rotate(SWFPosition position, double degrees);
void SWFPosition_rotateTo(SWFPosition position, double degrees);

void SWFPosition_move(SWFPosition position, double x, double y);
void SWFPosition_moveTo(SWFPosition position, double x, double y);

double SWFPosition_getRotation(SWFPosition position);
double SWFPosition_getX(SWFPosition position);
double SWFPosition_getY(SWFPosition position);

void SWFPosition_getXY(SWFPosition position, double* outX, double* outY);

double SWFPosition_getXScale(SWFPosition position);
double SWFPosition_getYScale(SWFPosition position);

void SWFPosition_getXYScale(SWFPosition position, double* outXScale, double* outYScale);

double SWFPosition_getXSkew(SWFPosition position);
double SWFPosition_getYSkew(SWFPosition position);

void SWFPosition_getXYSkew(SWFPosition position, double* outXSkew, double* outYSkew);


/***** SWFFilter ***********/

#define FILTER_MODE_INNER     (1<<7)
#define FILTER_MODE_KO        (1<<6)
#define FILTER_MODE_COMPOSITE (1<<5)
#define FILTER_MODE_ONTOP     (1<<4)

#define FILTER_FLAG_CLAMP          (1<<1)
#define FILTER_FLAG_PRESERVE_ALPHA (1<<0)

typedef enum
{
	SWFFILTER_TYPE_DROPSHADOW,
	SWFFILTER_TYPE_BLUR,
	SWFFILTER_TYPE_GLOW,
	SWFFILTER_TYPE_BEVEL,
	SWFFILTER_TYPE_GRADIENTGLOW,
	SWFFILTER_TYPE_CONVOLUTION,
	SWFFILTER_TYPE_COLORMATRIX,
	SWFFILTER_TYPE_GRADIENTBEVEL
} SWFFilterFmt;

typedef struct SWFColor {
        unsigned char    red;
        unsigned char    green;
        unsigned char    blue;
        unsigned char    alpha;
} SWFColor;

typedef struct Shadow_s *SWFShadow;
SWFShadow newSWFShadow(float angle, float distance, float strength);
void destroySWFShadow(SWFShadow s);

typedef struct Blur_s *SWFBlur;
SWFBlur newSWFBlur(float blurX, float blurY, int passes);
void destroySWFBlur(SWFBlur b);

typedef struct FilterMatrix_s *SWFFilterMatrix;
SWFFilterMatrix newSWFFilterMatrix(int cols, int rows, float *vals);
void destroySWFFilterMatrix(SWFFilterMatrix m);

void destroySWFFilter(SWFFilter filter);
SWFFilter newColorMatrixFilter(SWFFilterMatrix matrix);
SWFFilter newConvolutionFilter(SWFFilterMatrix matrix, float divisor,
                               float bias, SWFColor color, int flags);

SWFFilter newGradientBevelFilter(SWFGradient gradient, SWFBlur blur,
                                 SWFShadow shadow, int flags);

SWFFilter newGradientGlowFilter(SWFGradient gradient, SWFBlur blur,
                                SWFShadow shadow, int flags);

SWFFilter newBevelFilter(SWFColor shadowColor, SWFColor highlightColor, 
                         SWFBlur blur, SWFShadow shadow, int flags);

SWFFilter newGlowFilter(SWFColor color, SWFBlur blur, 
                        float strength, int flags);

SWFFilter newBlurFilter(SWFBlur blur);
SWFFilter newDropShadowFilter(SWFColor color, SWFBlur blur, 
                              SWFShadow shadow, int flags);

/***** SWFDisplayItem *****/

SWFCharacter SWFDisplayItem_getCharacter(SWFDisplayItem item);
void SWFDisplayItem_endMask(SWFDisplayItem item);

void SWFDisplayItem_move(SWFDisplayItem item, double x, double y);
void SWFDisplayItem_moveTo(SWFDisplayItem item, double x, double y);
void SWFDisplayItem_rotate(SWFDisplayItem item, double degrees);
void SWFDisplayItem_rotateTo(SWFDisplayItem item, double degrees);
void SWFDisplayItem_scale(SWFDisplayItem item, double xScale, double yScale);
void SWFDisplayItem_scaleTo(SWFDisplayItem item, double xScale, double yScale);
void SWFDisplayItem_skewX(SWFDisplayItem item, double x);
void SWFDisplayItem_skewXTo(SWFDisplayItem item, double x);
void SWFDisplayItem_skewY(SWFDisplayItem item, double y);
void SWFDisplayItem_skewYTo(SWFDisplayItem item, double y);

void SWFDisplayItem_getPosition(SWFDisplayItem item, double * x, double * y);
void SWFDisplayItem_getRotation(SWFDisplayItem item, double * degrees);
void SWFDisplayItem_getScale(SWFDisplayItem item, double * xScale, double * yScale);
void SWFDisplayItem_getSkew(SWFDisplayItem item, double * xSkew, double * ySkew);

SWFMatrix SWFDisplayItem_getMatrix(SWFDisplayItem item);
void SWFDisplayItem_setMatrix(SWFDisplayItem i, double a, double b,
			      double c, double d, double x, double y);

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

void SWFDisplayItem_flush(SWFDisplayItem item);

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
#define SWFACTION_INIT       (1<<9)
#define SWFACTION_PRESS              (1<<10)
#define SWFACTION_RELEASE     (1<<11)
#define SWFACTION_RELEASEOUTSIDE (1<<12)
#define SWFACTION_ROLLOVER    (1<<13)
#define SWFACTION_ROLLOUT     (1<<14)
#define SWFACTION_DRAGOVER    (1<<15)
#define SWFACTION_DRAGOUT     (1<<16)
#define SWFACTION_KEYPRESS    (1<<17)
#define SWFACTION_CONSTRUCT   (1<<18)

void SWFDisplayItem_addAction(SWFDisplayItem item, SWFAction action, int flags);

void SWFDisplayItem_cacheAsBitmap(SWFDisplayItem item, int flag);

enum {
	SWFBLEND_MODE_NULL,
	SWFBLEND_MODE_NORMAL,
	SWFBLEND_MODE_LAYER,
	SWFBLEND_MODE_MULT,
	SWFBLEND_MODE_SCREEN,
	SWFBLEND_MODE_LIGHTEN,
	SWFBLEND_MODE_DARKEN,
	SWFBLEND_MODE_DIFF,
	SWFBLEND_MODE_ADD,
	SWFBLEND_MODE_SUB,
	SWFBLEND_MODE_INV,
	SWFBLEND_MODE_ALPHA,
	SWFBLEND_MODE_ERASE,
	SWFBLEND_MODE_OVERLAY,
	SWFBLEND_MODE_HARDLIGHT
};

void SWFDisplayItem_setBlendMode(SWFDisplayItem item, int mode);
void SWFDisplayItem_addFilter(SWFDisplayItem item, SWFFilter filter);
/***** SWFFill *****/

/* adds a position object to manipulate SWFFillStyle's matrix */

SWFFill newSWFFill(SWFFillStyle fillstyle);
void destroySWFFill(SWFFill fill);
SWFFillStyle SWFFill_getFillStyle(SWFFill fill);

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

void SWFShape_setLine2Filled(SWFShape shape, unsigned short width,
                             SWFFillStyle fill,
                             int flags, float miterLimit);

void SWFShape_setLine2(SWFShape shape, unsigned short width, 
                       byte r, byte g, byte b, byte a,
                       int flags, float miterLimit);

SWFFill SWFShape_addSolidFill(SWFShape shape, byte r, byte g, byte b, byte a);
SWFFill SWFShape_addGradientFill(SWFShape shape, SWFGradient gradient, byte flags);
SWFFill SWFShape_addBitmapFill(SWFShape shape, SWFBitmap bitmap, byte flags);

void SWFShape_setLeftFill(SWFShape shape, SWFFill fill);
void SWFShape_setRightFill(SWFShape shape, SWFFill fill);

void SWFShape_drawArc(SWFShape shape, double r, double startAngle, double endAngle);
void SWFShape_drawCircle(SWFShape shape, double r);

/* draw character c from font font into shape shape at size size */
void SWFShape_drawGlyph(SWFShape shape, SWFFont font, unsigned short c);
void SWFShape_drawSizedGlyph(SWFShape shape, SWFFont font, unsigned short c, int size);

  /* Deprecated: */
  #define SWFShape_drawFontGlyph(s,f,c) SWFShape_drawGlyph(s,f,c)

/* approximate a cubic bezier with quadratic segments */
/* returns the number of segments used */
int SWFShape_drawCubic(SWFShape shape, double bx, double by,
		       double cx, double cy, double dx, double dy);
int SWFShape_drawCubicTo(SWFShape shape, double bx, double by,
			 double cx, double cy, double dx, double dy);
void SWFShape_drawCharacterBounds(SWFShape shape, SWFCharacter character);


/***** SWFMovieClip *****/

SWFMovieClip newSWFMovieClip(void);
void destroySWFMovieClip(SWFMovieClip movieClip);

void SWFMovieClip_setNumberOfFrames(SWFMovieClip clip, int frames);
void SWFMovieClip_nextFrame(SWFMovieClip clip);
void SWFMovieClip_labelFrame(SWFMovieClip clip, const char *label);

SWFDisplayItem SWFMovieClip_add(SWFMovieClip clip, SWFBlock block);
void SWFMovieClip_remove(SWFMovieClip clip, SWFDisplayItem item);

void SWFMovieClip_setSoundStream(SWFMovieClip clip, SWFSoundStream sound, float rate);
void SWFMovieClip_setSoundStreamAt(SWFMovieClip clip, SWFSoundStream sound, float rate, float skip);
SWFSoundInstance SWFMovieClip_startSound(SWFMovieClip clip, SWFSound sound);
void SWFMovieClip_stopSound(SWFMovieClip clip, SWFSound sound);
void SWFMovieClip_setScalingGrid(SWFMovieClip clip, int x, int y, int w, int h);
void SWFMovieClip_removeScalingGrid(SWFMovieClip clip);
void SWFMovieClip_addInitAction(SWFMovieClip, SWFAction);

/***** SWFPrebuiltClip ****/

void destroySWFPrebuiltClip(SWFPrebuiltClip clip);
SWFPrebuiltClip newSWFPrebuiltClip_fromFile(const char *filename);
SWFPrebuiltClip newSWFPrebuiltClip_fromInput(SWFInput input);

/***** SWFBinaryData *****/

SWFBinaryData newSWFBinaryData(unsigned char *blob, int length);
void destroySWFBinaryData(SWFBinaryData data);

/***** SWFMovie *****/

void SWFMovie_writeExports(SWFMovie movie);
SWFMovie newSWFMovie(void);
SWFMovie newSWFMovieWithVersion(int version);
void destroySWFMovie(SWFMovie movie);
void SWFMovie_setRate(SWFMovie movie, float rate);
float SWFMovie_getRate(SWFMovie movie);
void SWFMovie_setDimension(SWFMovie movie, float x, float y);
void SWFMovie_setNumberOfFrames(SWFMovie movie, int frames);
void SWFMovie_addExport(SWFMovie movie, SWFBlock block, const char *name);
void SWFMovie_assignSymbol(SWFMovie m, SWFCharacter character, const char *name);
void SWFMovie_defineScene(SWFMovie m, unsigned int offset, const char *name);
void SWFMovie_setBackground(SWFMovie movie, byte r, byte g, byte b);
void SWFMovie_setSoundStream(SWFMovie movie, SWFSoundStream sound);
void SWFMovie_setSoundStreamAt(SWFMovie movie, SWFSoundStream stream, float skip);
SWFSoundInstance SWFMovie_startSound(SWFMovie movie, SWFSound sound);
void SWFMovie_stopSound(SWFMovie movie, SWFSound sound);
SWFFontCharacter SWFMovie_addFont(SWFMovie movie, SWFFont font);
SWFFontCharacter SWFMovie_importFont(SWFMovie movie, const char *filename, const char *name);
SWFDisplayItem SWFMovie_add_internal(SWFMovie movie, SWFMovieBlockType ublock);
#ifndef _MSC_VER
#define SWFMovie_add(__movie, __block) \
	SWFMovie_add_internal((__movie), __to_movie_block((__block)))
#else
static inline SWFDisplayItem SWFMovie_add(SWFMovie movie, SWFBlock block)
{
	SWFMovieBlockType ublock;
	ublock.block = block;
	return SWFMovie_add_internal(movie, ublock);
}
#endif

int SWFMovie_replace_internal(SWFMovie movie, SWFDisplayItem item, SWFMovieBlockType block);
#ifndef _MSC_VER
#define SWFMovie_replace(__movie, __item, __block) \
	SWFMovie_replace_internal((__movie), (__item), __to_movie_block((__block)))
#else
static inline int SWFMovie_replace(SWFMovie movie, SWFDisplayItem item, SWFBlock block)
{
	SWFMovieBlockType ublock;
	ublock.block = block;
	return SWFMovie_replace_internal(movie, item, ublock);
}
#endif

void SWFMovie_remove(SWFMovie movie, SWFDisplayItem item);
void SWFMovie_nextFrame(SWFMovie movie);
void SWFMovie_labelFrame(SWFMovie movie, const char *label);
void SWFMovie_namedAnchor(SWFMovie movie, const char *label);
int SWFMovie_output(SWFMovie movie, SWFByteOutputMethod method, void *data);
int SWFMovie_save(SWFMovie movie, const char *filename);
int SWFMovie_output_to_stream(SWFMovie movie, FILE *fp);
void SWFMovie_protect(SWFMovie movie, const char *password);
void SWFMovie_setNetworkAccess(SWFMovie movie, int flag);
void SWFMovie_addMetadata(SWFMovie movie, const char *xml);
void SWFMovie_setScriptLimits(SWFMovie movie, int maxRecursion, int timeout);
void SWFMovie_setTabIndex(SWFMovie movie, int depth, int index);

  /* deprecated */
  /*int SWFMovie_outputC(SWFMovie movie, SWFByteOutputMethod method, void *data, int level);*/

SWFCharacter SWFMovie_importCharacter(SWFMovie movie, const char *filename, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* SWF_MING_H_INCLUDED */

