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

#ifndef SWF_MINGPP_H_INCLUDED
#define SWF_MINGPP_H_INCLUDED

#include <stdio.h>
#include <string.h>

/* mask the c type names so that we can replace them with classes.
   weird, but it works.  (on gcc, anyway..) */

extern "C"
{
  #define SWFShape        c_SWFShape
  #define SWFMovie        c_SWFMovie
  #define SWFDisplayItem  c_SWFDisplayItem
  #define SWFFill         c_SWFFill
  #define SWFCharacter    c_SWFCharacter
  #define SWFBlock        c_SWFBlock
  #define SWFSprite       c_SWFSprite
  #define SWFMovieClip    c_SWFMovieClip
  #define SWFBitmap       c_SWFBitmap
  #define SWFGradient     c_SWFGradient
  #define SWFMorph        c_SWFMorph
  #define SWFText         c_SWFText
  #define SWFFont         c_SWFFont
  #define SWFTextField    c_SWFTextField
  #define SWFAction       c_SWFAction
  #define SWFButton       c_SWFButton
  #define SWFSoundStream  c_SWFSoundStream
  #define SWFInput        c_SWFInput
  #define SWFSound        c_SWFSound

// begin minguts 2004/08/31 ((((
  #define SWFFontCharacter c_SWFFontCharacter
  #define SWFPrebuiltClip c_SWFPrebuiltClip
// )))) end minguts 2004/08/31

  #include <ming.h>

  #undef SWFShape
  #undef SWFMovie
  #undef SWFDisplayItem
  #undef SWFFill
  #undef SWFCharacter
  #undef SWFBlock
  #undef SWFSprite
  #undef SWFMovieClip
  #undef SWFBitmap
  #undef SWFGradient
  #undef SWFMorph
  #undef SWFFont
  #undef SWFText
  #undef SWFTextField
  #undef SWFAction
  #undef SWFButton
  #undef SWFSoundStream
  #undef SWFInput
  #undef SWFSound
// begin minguts 2004/08/31 ((((
  #undef SWFFontCharacter
  #undef SWFPrebuiltClip 
// )))) end minguts 2004/08/31
}

#define SWF_DECLAREONLY(classname) \
	private: \
	classname(const classname&); \
	const classname& operator=(const classname&)

/*  SWFInput  */

class SWFInput
{
 public:
  c_SWFInput input;

  SWFInput(FILE *f)
    { this->input = newSWFInput_file(f); }

  SWFInput(unsigned char *buffer, int length)
    { this->input = newSWFInput_buffer(buffer, length); }

  SWFInput(unsigned char *buffer, int length, int alloced)
  {
    if(alloced)
      this->input = newSWFInput_allocedBuffer(buffer, length);
    else
      this->input = newSWFInput_buffer(buffer, length);
  }

  virtual ~SWFInput() { destroySWFInput(this->input); }

  SWF_DECLAREONLY(SWFInput);
  SWFInput();
};


/*  SWFBlock  */

class SWFBlock
{
 public:
  virtual c_SWFBlock getBlock()
    { return NULL; }
  virtual ~SWFBlock() {}
};

/*  SWFCharacter  */

class SWFCharacter : public SWFBlock
{
 public:
  c_SWFCharacter character;

// begin minguts 2004/08/31 (needed by new class SWFFontCharacter) ((((
  SWFCharacter(c_SWFCharacter character)
    { this->character = character; }
// )))) end minguts 2004/08/31
	virtual ~SWFCharacter() {}
	
  float getWidth()
    { return SWFCharacter_getWidth(this->character); }

  float getHeight()
    { return SWFCharacter_getHeight(this->character); }

// begin minguts 2004/08/31 (removed NULL , needed by new class SWFFontCharacter) ((((
  virtual c_SWFBlock getBlock()
    { return (c_SWFBlock)this->character; }
// )))) end minguts 2004/08/31

  SWFCharacter() {} //needed for base classing
  SWF_DECLAREONLY(SWFCharacter);
};
// begin minguts 2004/08/31: added two new classes: "SWFFontCharacter" and "SWFPrebuiltClip" ((((
/*  SWFFontCharacter */
class SWFFontCharacter : public SWFCharacter
{
 public:
  c_SWFFontCharacter fontcharacter;

  SWFFontCharacter(c_SWFFontCharacter fontcharacter)
    { this->fontcharacter = fontcharacter; }

  virtual ~SWFFontCharacter()
    { }

  SWF_DECLAREONLY(SWFFontCharacter);
  SWFFontCharacter();
};
// 

/*  SWFPrebuiltClip */

class SWFPrebuiltClip : public SWFCharacter/* SWFBlock */
{
 public:
  c_SWFPrebuiltClip prebuiltclip;

  SWFPrebuiltClip(c_SWFPrebuiltClip prebuiltclip)
    { this->prebuiltclip = prebuiltclip; }

  virtual ~SWFPrebuiltClip()
    { }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->prebuiltclip; }


  SWFPrebuiltClip(const char *name)
  {
    if(strlen(name) > 4 &&
       strcmp(name + strlen(name) - 4, ".swf") == 0)
      this->prebuiltclip = newSWFPrebuiltClip_fromFile(name);
    else
      this->prebuiltclip = 0 ; // needs to be fixed - but how ????
  }
  SWF_DECLAREONLY(SWFPrebuiltClip);
  SWFPrebuiltClip();
};
// )))) end minguts 2004/08/31


/*  SWFAction  */

class SWFAction : public SWFBlock
{
 public:
  c_SWFAction action;

  SWFAction(const char *script)
    { this->action = compileSWFActionCode(script); }

  // movies, buttons, etc. destroy the c_SWFAction..
  virtual ~SWFAction() {}

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->action; }

  SWF_DECLAREONLY(SWFAction);
  SWFAction();
};


/*  SWFDisplayItem  */

class SWFDisplayItem
{
 public:
  c_SWFDisplayItem item;

  SWFDisplayItem(c_SWFDisplayItem item)
    { this->item = item; }

  virtual ~SWFDisplayItem()
    { }

  void rotate(float degrees)
    { SWFDisplayItem_rotate(this->item, degrees); }

  void rotateTo(float degrees)
    { SWFDisplayItem_rotateTo(this->item, degrees); }

  void getRotation(float *degrees)
    { SWFDisplayItem_getRotation(this->item, degrees); }

  void move(float x, float y)
    { SWFDisplayItem_move(this->item, x, y); }

  void moveTo(float x, float y)
    { SWFDisplayItem_moveTo(this->item, x, y); }

  void getPosition(float *x, float *y)
    { SWFDisplayItem_getPosition(this->item, x, y); }

  void scale(float xScale, float yScale)
    { SWFDisplayItem_scale(this->item, xScale, yScale); }

  void scale(float scale)
    { SWFDisplayItem_scale(this->item, scale, scale); }

  void scaleTo(float xScale, float yScale)
    { SWFDisplayItem_scaleTo(this->item, xScale, yScale); }

  void scaleTo(float scale)
    { SWFDisplayItem_scaleTo(this->item, scale, scale); }

  void getScale(float *xScale, float *yScale)
    { SWFDisplayItem_getScale(this->item, xScale, yScale); }

  void skewX(float skew)
    { SWFDisplayItem_skewX(this->item, skew); }

  void skewXTo(float skew)
    { SWFDisplayItem_skewXTo(this->item, skew); }

  void skewY(float skew)
    { SWFDisplayItem_skewY(this->item, skew); }

  void skewYTo(float skew)
    { SWFDisplayItem_skewYTo(this->item, skew); }

  void getSkew(float *xSkew, float *ySkew)
    { SWFDisplayItem_getSkew(this->item, xSkew, ySkew); }

  int getDepth()
    { return SWFDisplayItem_getDepth(this->item); }

  void setDepth(int depth)
    { SWFDisplayItem_setDepth(this->item, depth); }

  void remove()
    { SWFDisplayItem_remove(this->item); }

  void setName(const char *name)
    { SWFDisplayItem_setName(this->item, name); }

  void setRatio(float ratio)
    { SWFDisplayItem_setRatio(this->item, ratio); }

  void addColor(int r, int g, int b, int a=0)
    { SWFDisplayItem_setColorAdd(this->item, r, g, b, a); }

  void multColor(float r, float g, float b, float a=1.0)
    { SWFDisplayItem_setColorMult(this->item, r, g, b, a); }

  void addAction(SWFAction *action, int flags)
    { SWFDisplayItem_addAction(this->item, action->action, flags); }

	void setMatrix(float a, float b, float c, float d, float x, float y)
    { SWFDisplayItem_setMatrix(this->item, a, b, c, d, x, y); }
    
  SWF_DECLAREONLY(SWFDisplayItem);
  SWFDisplayItem();
};


/*  SWFSoundStream  */

class SWFSoundStream
{
 public:
  c_SWFSoundStream sound;

  SWFSoundStream(FILE *file)
    { this->sound = newSWFSoundStream(file); }

  SWFSoundStream(SWFInput *input)
    { this->sound = newSWFSoundStream_fromInput(input->input); }

  SWFSoundStream(char *filename)
    { this->sound = newSWFSoundStream(fopen(filename, "rb")); }

  virtual ~SWFSoundStream()
    { destroySWFSoundStream(this->sound); }
  SWF_DECLAREONLY(SWFSoundStream);
  SWFSoundStream();
};




class SWFSound
{
 public:
  c_SWFSound sound;

  SWFSound(FILE *file, int flags)
    { this->sound = newSWFSound(file, flags); }

  SWFSound(SWFInput *input, int flags)
    { this->sound = newSWFSound_fromInput(input->input, flags); }

  SWFSound(char *filename, int flags)
    { this->sound = newSWFSound(fopen(filename, "rb"), flags); }

  virtual ~SWFSound()
    { destroySWFSound(this->sound); }
  SWF_DECLAREONLY(SWFSound);
  SWFSound();
};


/*  SWFMovie  */

class SWFMovie
{
 public:
  c_SWFMovie movie;

  SWFMovie()
    { this->movie = newSWFMovie(); }

  virtual ~SWFMovie()
    { destroySWFMovie(this->movie); }

  void setRate(float rate)
    { SWFMovie_setRate(this->movie, rate); }

  void setDimension(float x, float y)
    { SWFMovie_setDimension(this->movie, x, y); }

  void setNumberOfFrames(int nFrames)
    { SWFMovie_setNumberOfFrames(this->movie, nFrames); }

  /* aka */
  void setFrames(int nFrames)
    { SWFMovie_setNumberOfFrames(this->movie, nFrames); }

  void setBackground(byte r, byte g, byte b)
    { SWFMovie_setBackground(this->movie, r, g, b); }

  void setSoundStream(SWFSoundStream *sound)
    { SWFMovie_setSoundStream(this->movie, sound->sound); /* wogl */ }

  SWFDisplayItem *add(SWFBlock *character)
    { return new SWFDisplayItem(SWFMovie_add(this->movie, character->getBlock())); }

  void remove(SWFDisplayItem *item)
    { SWFMovie_remove(this->movie, item->item); }

  void nextFrame()
    { SWFMovie_nextFrame(this->movie); }

  void labelFrame(const char *label)
    { SWFMovie_labelFrame(this->movie, label); }

  int output(int level=-1)
  {
    int oldlevel = Ming_setSWFCompression(level);
    int ret = SWFMovie_output_to_stream(this->movie, stdout);
    Ming_setSWFCompression(oldlevel);
    return ret;
  }

  int save(const char *filename, int level=-1)
  {
    int oldlevel = Ming_setSWFCompression(level);
    int result = SWFMovie_save(this->movie,filename);
    Ming_setSWFCompression(oldlevel);
    return result;
  }

  void startSound(SWFSound *sound)
    { SWFMovie_startSound(this->movie, sound->sound); }
  void stopSound(SWFSound *sound)
    { SWFMovie_stopSound(this->movie, sound->sound); }

// begin minguts 2004/08/31 ((((
	SWFCharacter *importCharacter(const char *filename, const char *name)
	{ return new SWFCharacter(SWFMovie_importCharacter(this->movie, filename, name));}
	SWFFontCharacter *importFont(const char *filename, const char *name)
	{ return new SWFFontCharacter(SWFMovie_importFont(this->movie, filename, name)); }
// )))) end minguts 2004/08/31

  SWF_DECLAREONLY(SWFMovie);
};


/*  SWFFill  */

class SWFFill
{
 public:
  c_SWFFill fill;

  SWFFill(c_SWFFill fill)
    { this->fill = fill; }

  // shape destroys c_SWFFill object
  virtual ~SWFFill() 
	{ destroySWFFill(this->fill); }

  void skewX(float x)
    { SWFFill_skewX(this->fill, x); }

  void skewXTo(float x)
    { SWFFill_skewXTo(this->fill, x); }

  void skewY(float y)
    { SWFFill_skewY(this->fill, y); }

  void skewYTo(float y)
    { SWFFill_skewYTo(this->fill, y); }

  void scaleX(float x)
    { SWFFill_scaleX(this->fill, x); }

  void scaleXTo(float x)
    { SWFFill_scaleXTo(this->fill, x); }

  void scaleY(float y)
    { SWFFill_scaleY(this->fill, y); }

  void scaleYTo(float y)
    { SWFFill_scaleYTo(this->fill, y); }

  void scale(float x, float y)
    { SWFFill_scaleXY(this->fill, x, y); }

  void scale(float scale)
    { SWFFill_scaleXY(this->fill, scale, scale); }

  void scaleTo(float x, float y)
    { SWFFill_scaleXYTo(this->fill, x, y); }

  void scaleTo(float scale)
    { SWFFill_scaleXYTo(this->fill, scale, scale); }

  void rotate(float degrees)
    { SWFFill_rotate(this->fill, degrees); }

  void rotateTo(float degrees)
    { SWFFill_rotateTo(this->fill, degrees); }

  void move(float x, float y)
    { SWFFill_move(this->fill, x, y); }

  void moveTo(float x, float y)
    { SWFFill_move(this->fill, x, y); }
  SWF_DECLAREONLY(SWFFill);
  SWFFill();
};


/*  SWFGradient  */

class SWFGradient
{
 public:
  c_SWFGradient gradient;

  SWFGradient()
    { this->gradient = newSWFGradient(); }

  virtual ~SWFGradient()
    { destroySWFGradient(this->gradient); }

  void addEntry(float ratio, byte r, byte g, byte b, byte a=0xff)
    { SWFGradient_addEntry(this->gradient, ratio, r, g, b, a); }
  SWF_DECLAREONLY(SWFGradient);
};


/*  SWFBitmap  */

class SWFBitmap : public SWFBlock
{
 public:
  c_SWFBitmap bitmap;

// begin minguts (added because caused windows to crash) 2004/08/31 ((((
  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->bitmap; }
// )))) end minguts 2004/08/31

  SWFBitmap(const char *filename, const char *alpha=NULL)
  {
    if(strlen(filename) > 4)
    {
      if(strcmp(filename+strlen(filename)-4, ".dbl") == 0)
	this->bitmap = (c_SWFBitmap) newSWFDBLBitmap(fopen(filename, "rb"));

#if USE_GIF
      
      else if(strcmp(filename+strlen(filename)-4, ".gif") == 0)
	this->bitmap = (c_SWFBitmap) newSWFDBLBitmapData_fromGifFile(filename);

#endif

#if USE_PNG

      else if(strcmp(filename+strlen(filename)-4, ".png") == 0)
 		this->bitmap =   (c_SWFBitmap) newSWFDBLBitmapData_fromPngFile( filename );

#endif

      else if(strcmp(filename+strlen(filename)-4, ".jpg") == 0)
      {
          if(alpha != NULL)
              this->bitmap = (c_SWFBitmap) newSWFJpegWithAlpha(fopen(filename, "rb"),
                                                               fopen(alpha, "rb"));
          else
              this->bitmap = (c_SWFBitmap) newSWFJpegBitmap(fopen(filename, "rb"));
      }
      else
      {
          ; // XXX - throw exception
      }
    }
  }

  SWFBitmap(SWFInput *input)
    { this->bitmap = newSWFBitmap_fromInput(input->input); }

  virtual ~SWFBitmap()
    { destroySWFBitmap(this->bitmap); }

  int getWidth()
    { return SWFBitmap_getWidth(this->bitmap); }

  int getHeight()
    { return SWFBitmap_getHeight(this->bitmap); }
  SWF_DECLAREONLY(SWFBitmap);
  SWFBitmap();
};


/*  SWFFont  */

class SWFFont : public SWFBlock
{
 public:
  c_SWFFont font;

  SWFFont()
    { this->font = newSWFFont(); }

  SWFFont(FILE *file)
    { this->font = loadSWFFontFromFile(file); }

  SWFFont(const char *name)
  {
    if(strlen(name) > 4 &&
       strcmp(name + strlen(name) - 4, ".fdb") == 0)
      this->font = loadSWFFontFromFile(fopen(name, "rb"));
    else
      this->font = (c_SWFFont)newSWFBrowserFont(name); // minguts fix
  }

  virtual ~SWFFont()
    { destroySWFFont(/*(c_SWFBlock)*/this->font); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->font; }

  float getStringWidth(const unsigned char *string)
    { return SWFFont_getStringWidth(this->font, string); }

  float getWidth(const unsigned char *string)
    { return SWFFont_getStringWidth(this->font, string); }

  float getAscent()
    { return SWFFont_getAscent(this->font); }

  float getDescent()
    { return SWFFont_getDescent(this->font); }

  float getLeading()
    { return SWFFont_getLeading(this->font); }
  SWF_DECLAREONLY(SWFFont);
};


/*  SWFShape  */

class SWFShape : public SWFCharacter
{
 public:
  c_SWFShape shape;

  SWFShape()
    { this->shape = newSWFShape(); }

  SWFShape(c_SWFShape shape)
    { this->shape = shape; }

  virtual ~SWFShape()
    { destroySWFShape(this->shape); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->shape; }

  void movePen(float x, float y)
    { SWFShape_movePen(this->shape, x, y); }

  void movePenTo(float x, float y)
    { SWFShape_movePenTo(this->shape, x, y); }

  void drawLine(float x, float y)
    { SWFShape_drawLine(this->shape, x, y); }

  void drawLineTo(float x, float y)
    { SWFShape_drawLineTo(this->shape, x, y); }

  void drawCurve(float cx, float cy, float ax, float ay)
    { SWFShape_drawCurve(this->shape, cx, cy, ax, ay); }

  void drawCurveTo(float cx, float cy, float ax, float ay)
    { SWFShape_drawCurveTo(this->shape, cx, cy, ax, ay); }

  void drawCubic(float ax, float ay, float bx, float by, float cx, float cy)
    { SWFShape_drawCubic(this->shape, ax, ay, bx, by, cx, cy); }

  void drawCubicTo(float ax, float ay, float bx, float by, float cx, float cy)
    { SWFShape_drawCubicTo(this->shape, ax, ay, bx, by, cx, cy); }

  void end()
    { SWFShape_end(this->shape); }

  SWFFill *addSolidFill(byte r, byte g, byte b, byte a=0xff)
    { return new SWFFill(SWFShape_addSolidFill(this->shape, r, g, b, a)); }

  SWFFill *addGradientFill(SWFGradient *gradient, byte flags=0)
    { return new SWFFill(SWFShape_addGradientFill(this->shape, gradient->gradient, flags)); }

  SWFFill *addBitmapFill(SWFBitmap *bitmap, byte flags=0)
    { return new SWFFill(SWFShape_addBitmapFill(this->shape, bitmap->bitmap, flags)); }

  void setLeftFill(SWFFill *fill)
    { SWFShape_setLeftFill(this->shape, fill->fill); }

  void setRightFill(SWFFill *fill)
    { SWFShape_setRightFill(this->shape, fill->fill); }

  void setLine(unsigned short width, byte r, byte g, byte b, byte a=0xff)
    { SWFShape_setLine(this->shape, width, r, g, b, a); }

// begin minguts 2004/08/31 ((((
  void drawCharacterBounds(SWFCharacter *character)
	{ SWFShape_drawCharacterBounds(this->shape, character->character); }
  void setLineStyle(unsigned short width, byte r, byte g, byte b, byte a=0xff) // alias for setline
	{ setLine(width, r, g, b, a); }
// )))) end minguts 2004/08/31

  void drawArc(float r, float startAngle, float endAngle)
    { SWFShape_drawArc(this->shape, r, startAngle, endAngle); }

  void drawCircle(float r)
    { SWFShape_drawCircle(this->shape, r); }

  void drawGlyph(SWFFont *font, unsigned short c, int size=0)
    { SWFShape_drawSizedGlyph(this->shape, font->font, c, size); }

  // deprecated?
  void drawFontGlyph(SWFFont *font, unsigned short c)
    { SWFShape_drawGlyph(this->shape, font->font, c); }

  SWF_DECLAREONLY(SWFShape);
};


/*  SWFSprite  */

class SWFSprite : public SWFCharacter
{
 public:
  c_SWFMovieClip clip;

  SWFSprite()
    { this->clip = newSWFMovieClip(); }

  virtual ~SWFSprite()
    { destroySWFMovieClip(this->clip); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->clip; }

  void setNumberOfFrames(int nFrames)
    { SWFMovieClip_setNumberOfFrames(this->clip, nFrames); }

  SWFDisplayItem *add(SWFBlock *character)
    { return new SWFDisplayItem(SWFMovieClip_add(this->clip, character->getBlock())); }

  void remove(SWFDisplayItem *item)
    { SWFMovieClip_remove(this->clip, item->item); }

  void nextFrame()
    { SWFMovieClip_nextFrame(this->clip); }

  void labelFrame(char *label)
    { SWFMovieClip_labelFrame(this->clip, label); }
  SWF_DECLAREONLY(SWFSprite);
};


/*  SWFMorph  */

class SWFMorph : public SWFCharacter
{
 public:
  c_SWFMorph morph;

  SWFMorph()
    { this->morph = newSWFMorphShape(); }

  virtual ~SWFMorph()
    { destroySWFMorph(this->morph); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->morph; }

  SWFShape *getShape1()
    { return new SWFShape(SWFMorph_getShape1(this->morph)); }

  SWFShape *getShape2()
    { return new SWFShape(SWFMorph_getShape2(this->morph)); }
  SWF_DECLAREONLY(SWFMorph);
};


/*  SWFText  */

class SWFText : public SWFCharacter
{
 public:
  c_SWFText text;

  SWFText()
    { this->text = newSWFText2(); }

  virtual ~SWFText()
    { destroySWFText(this->text); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->text; }

  void setFont(SWFBlock *font)
    { SWFText_setFont(this->text, font->getBlock()); }

  void setHeight(float height)
    { SWFText_setHeight(this->text, height); }

  // I don't like this..  it's too easy to confuse with displayitem::moveTo
  void moveTo(float x, float y)
    { SWFText_moveTo(this->text, x, y); }

  void setColor(byte r, byte g, byte b, byte a=0xff)
    { SWFText_setColor(this->text, r, g, b, a); }

  void addString(const char *string, int *advance=NULL)
    { SWFText_addString(this->text, string, advance); }

  void addUTF8String(const char *string, int *advance=NULL)
    { SWFText_addUTF8String(this->text, string, advance); }

  void setSpacing(float spacing)
    { SWFText_setSpacing(this->text, spacing); }

  float getStringWidth(const unsigned char *string)
    { return SWFText_getStringWidth(this->text, string); }

  float getWidth(const unsigned char *string)
    { return SWFText_getStringWidth(this->text, string); }

  float getUTF8Width(const unsigned char *string)
    { return SWFText_getUTF8StringWidth(this->text, string); }
  SWF_DECLAREONLY(SWFText);
};

/*  SWFTextField  */

class SWFTextField : public SWFCharacter
{
 public:
  c_SWFTextField textField;

  SWFTextField()
    { this->textField = newSWFTextField(); }

  virtual ~SWFTextField()
    { destroySWFTextField(this->textField); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->textField; }

  void setFont(SWFBlock *font)
    { SWFTextField_setFont(this->textField, font->getBlock()); }

  void setBounds(float width, float height)
    { SWFTextField_setBounds(this->textField, width, height); }

  void setFlags(int flags)
    { SWFTextField_setFlags(this->textField, flags); }

  void setColor(byte r, byte g, byte b, byte a=0xff)
    { SWFTextField_setColor(this->textField, r, g, b, a); }

  void setVariableName(const char *name)
    { SWFTextField_setVariableName(this->textField, name); }

  void addString(const char *string)
    { SWFTextField_addString(this->textField, string); }

  void addUTF8String(const char *string)
    { SWFTextField_addUTF8String(this->textField, string); }

  void setHeight(float height)
    { SWFTextField_setHeight(this->textField, height); }

  void setLeftMargin(float margin)
    { SWFTextField_setLeftMargin(this->textField, margin); }

  void setRightMargin(float margin)
    { SWFTextField_setRightMargin(this->textField, margin); }

  void setIndentation(float indentation)
    { SWFTextField_setIndentation(this->textField, indentation); }

  void setPadding(float padding)
    { SWFTextField_setPadding(this->textField, padding); }

  void setLineSpacing(float lineSpacing)
    { SWFTextField_setLineSpacing(this->textField, lineSpacing); }

  void setAlignment(SWFTextFieldAlignment alignment)
    { SWFTextField_setAlignment(this->textField, alignment); }

  void align(SWFTextFieldAlignment alignment)
    { SWFTextField_setAlignment(this->textField, alignment); }

  void setLength(int length)
    { SWFTextField_setLength(this->textField, length); }
  SWF_DECLAREONLY(SWFTextField);
};


/*  SWFButton  */

class SWFButton : public SWFCharacter
{
 public:
  c_SWFButton button;

  SWFButton()
    { this->button = newSWFButton(); }

  virtual ~SWFButton()
    { destroySWFButton(this->button); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->button; }

  void addShape(SWFCharacter *character, byte flags)
    { SWFButton_addShape(this->button, (c_SWFCharacter)character->getBlock(), flags); }

  void addAction(SWFAction *action, int flags)
    { SWFButton_addAction(this->button, action->action, flags); }

  void setMenu(int flag=0)
    { SWFButton_setMenu(this->button, flag); }

  void addSound(SWFSound *sound, int flags)
    { SWFButton_addSound(this->button, sound->sound, flags); }

  SWF_DECLAREONLY(SWFButton);
};

#endif /* SWF_MINGPP_H_INCLUDED */
