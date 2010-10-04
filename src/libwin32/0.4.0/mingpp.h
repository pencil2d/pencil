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

/* why would we need to include these ??
#include <stdio.h>
*/
#include <cstring> /* for strlen used in SWFBitmap costructor */
#include <stdexcept>
#include <iostream>
#include <string>
#include <list>

#ifdef _MSC_VER
#define strcasecmp stricmp 
#endif

/* mask the c type names so that we can replace them with classes.
   weird, but it works.  (on gcc, anyway..) */

extern "C"
{
  #define SWFShape        c_SWFShape
  #define SWFMovie        c_SWFMovie
  #define SWFDisplayItem  c_SWFDisplayItem
  #define SWFFill         c_SWFFill
  #define SWFFillStyle    c_SWFFillStyle
  #define SWFCharacter    c_SWFCharacter
  #define SWFBlock        c_SWFBlock
  #define SWFSprite       c_SWFSprite
  #define SWFMovieClip    c_SWFMovieClip
  #define SWFBitmap       c_SWFBitmap
  #define SWFGradient     c_SWFGradient
  #define SWFMorph        c_SWFMorph
  #define SWFText         c_SWFText
  #define SWFFont         c_SWFFont
  #define SWFBrowserFont  c_SWFBrowserFont
  #define SWFFontCollection  c_SWFFontCollection
  #define SWFTextField    c_SWFTextField
  #define SWFAction       c_SWFAction
  #define SWFButton       c_SWFButton
  #define SWFSoundStream  c_SWFSoundStream
  #define SWFInput        c_SWFInput
  #define SWFSound        c_SWFSound
  #define SWFVideoStream  c_SWFVideoStream
  #define SWFFilter       c_SWFFilter
  #define SWFBlur         c_SWFBlur
  #define SWFShadow       c_SWFShadow
  #define SWFFilterMatrix c_SWFFilterMatrix
  #define SWFInitAction   c_SWFInitAction
  #define SWFButtonRecord c_SWFButtonRecord
  #define SWFFontCharacter c_SWFFontCharacter
  #define SWFPrebuiltClip c_SWFPrebuiltClip
  #define SWFSoundInstance c_SWFSoundInstance
  #define SWFBinaryData	  c_SWFBinaryData
  #define SWFMatrix	  c_SWFMatrix
  #define SWFCXform	  c_SWFCXform

  #include <ming.h>

/* 
 * declaration from src/blocks/fdbfont.h. 
 * internal function to maintain behavior of older ming-version
 */
SWFFont loadSWFFont_fromFdbFile(FILE *file);

  #undef SWFShape
  #undef SWFMovie
  #undef SWFDisplayItem
  #undef SWFFill
  #undef SWFFillStyle
  #undef SWFCharacter
  #undef SWFBlock
  #undef SWFSprite
  #undef SWFMovieClip
  #undef SWFBitmap
  #undef SWFGradient
  #undef SWFMorph
  #undef SWFFont
  #undef SWFBrowserFont
  #undef SWFFontCollection
  #undef SWFText
  #undef SWFTextField
  #undef SWFAction
  #undef SWFButton
  #undef SWFSoundStream
  #undef SWFInput
  #undef SWFSound
  #undef SWFFontCharacter
  #undef SWFPrebuiltClip 
  #undef SWFVideoStream
  #undef SWFFilter
  #undef SWFBlur
  #undef SWFShadow
  #undef SWFFilterMatrix
  #undef SWFInitAction
  #undef SWFButtonRecord
  #undef SWFSoundInstance
  #undef SWFBinaryData
  #undef SWFMatrix
  #undef SWFCXform
} // extern C

#define SWF_DECLAREONLY(classname) \
	private: \
	classname(const classname&); \
	const classname& operator=(const classname&)


class SWFException : public std::exception
{
public:
	SWFException(const char *m)
	{ 
		this->message = m;
	}

	virtual ~SWFException() throw () 
	{ }
		
	virtual const char *what()
	{
		return this->message.c_str();
	}	
private:
	std::string message;
};

/* SWFMatrix */
class SWFMatrix
{
 friend class SWFDisplayItem;
 public:
  c_SWFMatrix matrix;

  double getScaleX()
  { return SWFMatrix_getScaleX(this->matrix); }

  double getScaleY()
  { return SWFMatrix_getScaleY(this->matrix); }

  double getRotate0()
  { return SWFMatrix_getRotate0(this->matrix); }
 
  double getRotate1()
  { return SWFMatrix_getRotate1(this->matrix); }

  int getTranslateX()
  { return SWFMatrix_getTranslateX(this->matrix); }

  int getTranslateY()
  { return SWFMatrix_getTranslateY(this->matrix); }

 private:
  SWFMatrix(c_SWFMatrix matrix)
  {
    if(matrix == NULL)
      throw SWFException("SWFMatrix(c_SWFMatrix matrix)");
    this->matrix = matrix;
  }
 SWF_DECLAREONLY(SWFMatrix);
};

class SWFCXform
{
 public:
  c_SWFCXform cx;

  SWFCXform(int rAdd, int gAdd, int bAdd, int aAdd, float rMult, float gMult, float bMult, float aMult)
  {
    this->cx =  newSWFCXform(rAdd, gAdd, bAdd, aAdd, rMult, gMult, bMult, aMult);
    if(this->cx == NULL)
      throw SWFException("SWFCXform(int rAdd, int gAdd,...)");
  }

  void setColorAdd(int rAdd, int gAdd, int bAdd, int aAdd)
  { SWFCXform_setColorAdd(this->cx, rAdd, gAdd, bAdd, aAdd); }

  void setColorMult(float rMult, float gMult, float bMult, float aMult)
  { SWFCXform_setColorMult(this->cx, rMult, gMult, bMult, aMult); }

  ~ SWFCXform()
  { destroySWFCXform(cx); }

  static SWFCXform *AddCXForm(int rAdd, int gAdd, int bAdd, int aAdd)
  { return new SWFCXform(newSWFAddCXform(rAdd, gAdd, bAdd, aAdd));  }

  static SWFCXform *MultCXForm(float rMult, float gMult, float bMult, float aMult)
 { return new SWFCXform(newSWFMultCXform(rMult, gMult, bMult, aMult)); }

  

 private:
  SWFCXform(c_SWFCXform cx)
  {
    if(cx == NULL)
      throw SWFException("SWFCXform(c_SWFCXform cx)");

    this->cx = cx;
  }
  SWF_DECLAREONLY(SWFCXform);

};

/*  SWFInput  */

class SWFInput
{
 public:
  c_SWFInput input;

  SWFInput(FILE *f)
  { 
    this->input = newSWFInput_file(f); 
    if(this->input == NULL) 
      throw SWFException("SWFInput(FILE *f)\n");
  }

  SWFInput(unsigned char *buffer, int length)
  { 
    this->input = newSWFInput_buffer(buffer, length); 
    if(this->input == NULL)
      throw SWFException("SWFInput(unsigned char *buffer, int length)\n");
  }

  SWFInput(unsigned char *buffer, int length, int alloced)
  {
    if(alloced)
      this->input = newSWFInput_allocedBuffer(buffer, length);
    else
      this->input = newSWFInput_buffer(buffer, length);

    if(this->input == NULL)
      SWFException("SWFInput(unsigned char *buffer, int length, int alloced)\n");
  }

  virtual ~SWFInput() { destroySWFInput(this->input); }

  SWF_DECLAREONLY(SWFInput);
  SWFInput();
};


/*  SWFBlock  */

class SWFBlock
{
 public:
  virtual c_SWFBlock getBlock() = 0;
};

/*  SWFCharacter  */

class SWFCharacter : public SWFBlock
{
 friend class SWFMovie;
 friend class SWFDisplayItem;
 public:
  c_SWFCharacter character;

    float getWidth()
    { return SWFCharacter_getWidth(this->character); }

  float getHeight()
    { return SWFCharacter_getHeight(this->character); }

  virtual c_SWFBlock getBlock()
    { return (c_SWFBlock)character; }

 protected:
  SWFCharacter()
  { character = NULL; }
  
  SWFCharacter(c_SWFCharacter c)
  { character = c; }


  SWF_DECLAREONLY(SWFCharacter);
};
/*  SWFFontCharacter */
class SWFFontCharacter : public SWFCharacter
{
 friend class SWFMovie;
 public:
  c_SWFFontCharacter fontcharacter;
  
  void addChars(const char *str)
    { SWFFontCharacter_addChars(this->fontcharacter, str); }

  void addUTF8Chars(const char *str)
    { SWFFontCharacter_addUTF8Chars(this->fontcharacter, str); }

  void addAllChars()
    { SWFFontCharacter_addAllChars(this->fontcharacter); }

 private:
  SWFFontCharacter(c_SWFFontCharacter fontcharacter)
  { 
	this->fontcharacter = fontcharacter; 
	this->character = (c_SWFCharacter)fontcharacter;
  }

  virtual ~SWFFontCharacter()
    { }

  SWF_DECLAREONLY(SWFFontCharacter);
  SWFFontCharacter();
};


/*  SWFPrebuiltClip */

class SWFPrebuiltClip : public SWFBlock
{
 public:
  c_SWFPrebuiltClip prebuiltclip;

  SWFPrebuiltClip(c_SWFPrebuiltClip prebuiltclip)
  { 
    if(prebuiltclip == NULL)
      throw SWFException("new SWFPrebuiltClip: prebuiltclip == NULL)");
    this->prebuiltclip = prebuiltclip; 
  }

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
      this->prebuiltclip = NULL;
   
    if(this->prebuiltclip == NULL)
      throw SWFException("SWFPrebuiltClip(const char *name)");
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
  { 
    this->action = newSWFAction(script); 
    if(this->action == NULL)
      throw SWFException("SWFAction(const char *script)");
  }

  // movies, buttons, etc. destroy the c_SWFAction..
  virtual ~SWFAction() {}

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->action; }

  int compile(int swfVersion, int *length)
    { return SWFAction_compile(this->action, swfVersion, length); }

  unsigned char *getByteCode(int *len)
    { return SWFAction_getByteCode(this->action, len); }
  
  SWF_DECLAREONLY(SWFAction);
  SWFAction();
};

/*  SWFInitAction  */

class SWFInitAction : public SWFBlock
{
 public:
  c_SWFInitAction init;

  SWFInitAction(SWFAction *action)
  { 
    this->init = newSWFInitAction(action->action); 
    if(this->init == NULL)
      throw SWFException("SWFInitAction(SWFAction *action)");
  }

  SWFInitAction(SWFAction *action, int id)
  { 
    this->init = newSWFInitAction_withId(action->action, id); 
    if(this->init == NULL)
      throw SWFException("SWFInitAction(SWFAction *action, int id)");
  }

  virtual ~SWFInitAction() {}

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->init; }

  SWF_DECLAREONLY(SWFInitAction);
  SWFInitAction();
};


/*  SWFGradient  */

class SWFGradient
{
 public:
  c_SWFGradient gradient;

  SWFGradient()
  { 
    this->gradient = newSWFGradient(); 
    if(this->gradient == NULL)
      throw SWFGradient();
  }

  virtual ~SWFGradient()
    { destroySWFGradient(this->gradient); }

  void addEntry(float ratio, byte r, byte g, byte b, byte a=0xff)
    { SWFGradient_addEntry(this->gradient, ratio, r, g, b, a); }

  void setSpreadMode(GradientSpreadMode mode)
    { SWFGradient_setSpreadMode(this->gradient, mode); }

  void setInterpolationMode(GradientInterpolationMode mode)
    { SWFGradient_setInterpolationMode(this->gradient, mode); }

  void setFocalPoint(float focalPoint)
    { SWFGradient_setFocalPoint(this->gradient, focalPoint); }

  SWF_DECLAREONLY(SWFGradient);
};

/* SWFFilter */
class SWFBlur
{
 public:
  c_SWFBlur blur;
  
  SWFBlur(float blurX, float blurY, int passes)
  {  
    this->blur = newSWFBlur(blurX, blurY, passes); 
    if(this->blur == NULL)
      throw SWFException("SWFBlur(float blurX, float blurY, int passes)");
  }

  ~SWFBlur()
    { destroySWFBlur(blur); }

  SWF_DECLAREONLY(SWFBlur);
  SWFBlur();
};

class SWFShadow
{
 public:
  c_SWFShadow shadow;

  SWFShadow(float angle, float distance, float strength)
  { 
    this->shadow = newSWFShadow(angle, distance, strength); 
    if(this->shadow == NULL)
      throw SWFException("SWFShadow(float angle, float distance, float strength)");
  }

  ~SWFShadow()
    { destroySWFShadow(shadow); }

  SWF_DECLAREONLY(SWFShadow);
  SWFShadow();
};

class SWFFilterMatrix 
{
 public:
  c_SWFFilterMatrix matrix;

  SWFFilterMatrix(int cols, int rows, float *vals)
  { 
    this->matrix = newSWFFilterMatrix(cols, rows, vals); 
    if(this->matrix == NULL)
      throw SWFException("SWFFilterMatrix(int cols, int rows, float *vals)");
  }

  ~SWFFilterMatrix()
    { destroySWFFilterMatrix(matrix); }

  SWF_DECLAREONLY(SWFFilterMatrix);
  SWFFilterMatrix();
};

class SWFFilter
{
 public:
  c_SWFFilter filter;

  virtual ~SWFFilter() {}

  static SWFFilter *BlurFilter(SWFBlur *blur)
    { return new SWFFilter(newBlurFilter(blur->blur)); }

  static SWFFilter *DropShadowFilter(SWFColor color, SWFBlur *blur, 
                                    SWFShadow *shadow, int flags)
    { return new SWFFilter(newDropShadowFilter(color, blur->blur, shadow->shadow, flags)); } 

  static SWFFilter *GlowFilter(SWFColor color, SWFBlur *blur, 
                              float strength, int flags)
    { return new SWFFilter(newGlowFilter(color, blur->blur, strength, flags));}

  static SWFFilter *BevelFilter(SWFColor sColor, SWFColor hColor,
                               SWFBlur *blur, SWFShadow *shadow, int flags)
    { return new SWFFilter(newBevelFilter(sColor, hColor, blur->blur, shadow->shadow, flags)); }

  static SWFFilter *GradientGlowFilter(SWFGradient *gradient, SWFBlur *blur, SWFShadow *shadow, int flags)
    { return new SWFFilter(newGradientGlowFilter(gradient->gradient, blur->blur, shadow->shadow, flags)); }

  static SWFFilter *GradientBevelFilter(SWFGradient *gradient, SWFBlur *blur,
                                       SWFShadow *shadow, int flags)
    { return new SWFFilter(newGradientBevelFilter(gradient->gradient, blur->blur, shadow->shadow, flags)); }

  static SWFFilter *ConvolutionFilter(SWFFilterMatrix *matrix, float divisor,
                                     float bias, SWFColor color, int flags)
    { return new SWFFilter(newConvolutionFilter(matrix->matrix, divisor, bias, color, flags)); }

  static SWFFilter *ColorMatrixFilter(SWFFilterMatrix *matrix)
    { return new SWFFilter(newColorMatrixFilter(matrix->matrix)); }

private:
  SWFFilter(c_SWFFilter filter)
  {  
    this->filter = filter;
    if(this->filter == NULL)
      throw SWFException("SWFFilter(c_SWFFilter filter)");
  } 
  SWF_DECLAREONLY(SWFFilter);
  SWFFilter();
};



/*  SWFDisplayItem  */

class SWFDisplayItem
{
 friend class SWFMovie;
 friend class SWFMovieClip;
 friend class SWFSprite;
 public:
  c_SWFDisplayItem item;
 
  void rotate(double degrees)
    { SWFDisplayItem_rotate(this->item, degrees); }

  void rotateTo(double degrees)
    { SWFDisplayItem_rotateTo(this->item, degrees); }

  void getRotation(double *degrees)
    { SWFDisplayItem_getRotation(this->item, degrees); }

  void move(double x, double y)
    { SWFDisplayItem_move(this->item, x, y); }

  void moveTo(double x, double y)
    { SWFDisplayItem_moveTo(this->item, x, y); }

  void getPosition(double *x, double *y)
    { SWFDisplayItem_getPosition(this->item, x, y); }

  void scale(double xScale, double yScale)
    { SWFDisplayItem_scale(this->item, xScale, yScale); }

  void scale(double scale)
    { SWFDisplayItem_scale(this->item, scale, scale); }

  void scaleTo(double xScale, double yScale)
    { SWFDisplayItem_scaleTo(this->item, xScale, yScale); }

  void scaleTo(double scale)
    { SWFDisplayItem_scaleTo(this->item, scale, scale); }

  void getScale(double *xScale, double *yScale)
    { SWFDisplayItem_getScale(this->item, xScale, yScale); }

  void skewX(double skew)
    { SWFDisplayItem_skewX(this->item, skew); }

  void skewXTo(double skew)
    { SWFDisplayItem_skewXTo(this->item, skew); }

  void skewY(double skew)
    { SWFDisplayItem_skewY(this->item, skew); }

  void skewYTo(double skew)
    { SWFDisplayItem_skewYTo(this->item, skew); }

  void getSkew(double *xSkew, double *ySkew)
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

  void addFilter(SWFFilter *filter)
    { SWFDisplayItem_addFilter(this->item, filter->filter); }

  void cacheAsBitmap(int flag)
    { SWFDisplayItem_cacheAsBitmap(this->item, flag); }

  void setBlendMode(int mode)
    { SWFDisplayItem_setBlendMode(this->item, mode); }
  
  void setMatrix(double a, double b, double c, double d, double x, double y)
    { SWFDisplayItem_setMatrix(this->item, a, b, c, d, x, y); }

  SWFMatrix getMatrix()
    { return SWFMatrix(SWFDisplayItem_getMatrix(this->item)); }

  void setMaskLevel(int level)
    { SWFDisplayItem_setMaskLevel(this->item, level); }

  void endMask()
    { SWFDisplayItem_endMask(this->item); }
  
  void flush()
    { SWFDisplayItem_flush(this->item); }
 
  SWFCharacter *getCharacter()
    { return new SWFCharacter(SWFDisplayItem_getCharacter(this->item)); }

  void setCXform(SWFCXform *cx)
    { SWFDisplayItem_setCXform(this->item, cx->cx); } 
  
 private:
  SWFDisplayItem(c_SWFDisplayItem item)
  { 
    this->item = item; 
    if(this->item == NULL)
      throw SWFException("SWFDisplayItem()");
  }
  ~SWFDisplayItem() { }

  SWF_DECLAREONLY(SWFDisplayItem);
  SWFDisplayItem();
};


/*  SWFSoundStream  */

class SWFSoundStream
{
 public:
  c_SWFSoundStream sound;

  SWFSoundStream(FILE *file)
  { 
    this->sound = newSWFSoundStream(file); 
    if(this->sound == NULL)
      throw SWFException("SWFSoundStream(FILE *file)");
  }

  SWFSoundStream(SWFInput *input)
  { 
    this->sound = newSWFSoundStream_fromInput(input->input); 
    if(this->sound == NULL)
      throw SWFException("SWFSoundStream(SWFInput *input)");
  }

  SWFSoundStream(char *filename)
  { 
    this->sound = newSWFSoundStream(fopen(filename, "rb")); 
    if(this->sound == NULL)
      throw SWFException("SWFSoundStream(char *filename)");
  }

  unsigned int getDuration()
  {
    return SWFSoundStream_getDuration(this->sound);
  }

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
  { 
	filep = NULL;
	this->sound = newSWFSound(file, flags);
	if(this->sound == NULL)
		throw SWFException("SWFSound(FILE *file, int flags)");
  }

  SWFSound(SWFInput *input, int flags)
  {
	this->sound = newSWFSound_fromInput(input->input, flags); 
	filep = NULL;
	if(this->sound == NULL)
		throw SWFException("SWFSound(SWFInput *input, int flags)");
  }

  SWFSound(char *filename, int flags)
  { 
	filep = fopen(filename, "rb");
	this->sound = newSWFSound(filep, flags);
	if(this->sound == NULL)
	{
		fclose(filep);
		throw SWFException("SWFSound(char *filename, int flags)");
	}
  }
  
  SWFSound(SWFSoundStream *stream)
  { 
	this->sound = newSWFSound_fromSoundStream(stream->sound); 
	filep = NULL;
	if(this->sound == NULL)
		throw SWFException("SWFSound(SWFSoundStream *stream)");
  }

  virtual ~SWFSound()
  {
	if(filep)
		fclose(filep); 
	destroySWFSound(this->sound); 
  }
 
 private:
    FILE *filep;

  SWF_DECLAREONLY(SWFSound);
  SWFSound();
};

/*  SWFFont  */

class SWFFont : public SWFBlock
{
 friend class SWFFontCollection;
 public:
  c_SWFFont font;

  SWFFont(FILE *file) // deprecated 
  {
	std::cerr << "SWFFont(FILE *file) is deprecated and will be removed in future releases." << std::endl;
	this->font = loadSWFFont_fromFdbFile(file);
	if(this->font == NULL)
		throw SWFException("SWFFont(FILE *file)");
  }

  SWFFont(char *path)
  { 
    this->font = newSWFFont_fromFile(path); 
    if(this->font == NULL)
      throw SWFException("SWFFont(char *path)");
  }

  virtual ~SWFFont()
    { destroySWFFont(/*(c_SWFBlock)*/this->font); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->font; }

  float getStringWidth(const char *string)
    { return SWFFont_getStringWidth(this->font, string); }

  float getWidth(const char *string)
    { return SWFFont_getStringWidth(this->font, string); }

  float getUTF8StringWidth(const char *string)
    { return SWFFont_getUTF8StringWidth(this->font, string); }

  float getAscent()
    { return SWFFont_getAscent(this->font); }

  float getDescent()
    { return SWFFont_getDescent(this->font); }

  float getLeading()
    { return SWFFont_getLeading(this->font); }

  const char *getName()
    { return SWFFont_getName(this->font); }

  int getGlyphCount()
    { return SWFFont_getGlyphCount(this->font); }
  
  char *getShape(unsigned short c)
    { return SWFFont_getShape(this->font, c); }

  SWF_DECLAREONLY(SWFFont);

 private:
  SWFFont(c_SWFFont font)
  { 
    this->font = font;
    if(this->font == NULL)
      throw SWFException("SWFFont(c_SWFFont)");
  }
};

/* SWFBrowserFont */
class SWFBrowserFont : public SWFBlock
{
 public:
  c_SWFBrowserFont bfont;
  
  SWFBrowserFont(char *name)
  { 
    this->bfont = newSWFBrowserFont(name); 
    if(this->bfont == NULL)
      throw SWFException("SWFBrowserFont(char *name)");
  }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->bfont; }

  virtual ~SWFBrowserFont()
    { destroySWFBrowserFont(this->bfont); }
  
  SWF_DECLAREONLY(SWFBrowserFont);
};

class SWFFontCollection
{
 public:
  c_SWFFontCollection fc;

  SWFFontCollection(const char *filename)
  {
    this->fc = newSWFFontCollection_fromFile(filename);
    if(this->fc == NULL)
      throw SWFException("SWFFontCollection(filename)");
  }

  ~ SWFFontCollection()
    { destroySWFFontCollection(this->fc); }

  SWFFont *getFont(int index)
    { return new SWFFont(SWFFontCollection_getFont(this->fc, index)); }

  int getFontCount()
    { return SWFFontCollection_getFontCount(this->fc); }

  SWF_DECLAREONLY(SWFFontCollection);
};

class SWFSoundInstance
{
 friend class SWFMovie;
 friend class SWFMovieClip;

 public:
  c_SWFSoundInstance instance;
 
  void setNoMultiple()
  { SWFSoundInstance_setNoMultiple(this->instance); }

  void setLoopInPoint(unsigned int point)
  { SWFSoundInstance_setLoopInPoint(this->instance, point); }
  
  void setLoopOutPoint(unsigned int point)
  {  SWFSoundInstance_setLoopOutPoint(this->instance, point); }

  void setLoopCount(int count)
  {  SWFSoundInstance_setLoopCount(this->instance, count); }

  void addEnvelope(unsigned int mark44, short left, short right)
  { SWFSoundInstance_addEnvelope(this->instance, mark44, left, right); }

 private:
  SWFSoundInstance(c_SWFSoundInstance inst)
  {
    if(inst == NULL)
      throw SWFException("SWFSoundInstance(c_SWFSoundInstance inst)");

    this->instance = inst;
  }
 SWF_DECLAREONLY(SWFSoundInstance);
};

/*  SWFMovie  */
class SWFMovie
{
 public:
  c_SWFMovie movie;

  SWFMovie()
  { 
    this->movie = newSWFMovie();
    if(this->movie == NULL)
      throw SWFException("SWFMovie()");
  }

  SWFMovie(int version)
  { 
    this->movie = newSWFMovieWithVersion(version);
    if(this->movie == NULL)
      throw SWFException("SWFMovie(int version)");
  }

  virtual ~SWFMovie()
    { destroySWFMovie(this->movie); }

  void setRate(float rate)
    { SWFMovie_setRate(this->movie, rate); }

  float getRate()
    { return SWFMovie_getRate(this->movie); }

  void setDimension(float x, float y)
    { SWFMovie_setDimension(this->movie, x, y); }

  void setNumberOfFrames(int nFrames)
    { SWFMovie_setNumberOfFrames(this->movie, nFrames); }

  /* aka */
  void setFrames(int nFrames)
    { SWFMovie_setNumberOfFrames(this->movie, nFrames); }

  void setBackground(byte r, byte g, byte b)
    { SWFMovie_setBackground(this->movie, r, g, b); }

  void setSoundStream(SWFSoundStream *sound, float skip=0.0)
  {
    if(skip > 0)
      SWFMovie_setSoundStreamAt(this->movie, sound->sound, skip);
    else
      SWFMovie_setSoundStream(this->movie, sound->sound); 
  }

  SWFDisplayItem *add(SWFBlock *character)
  {
    SWFMovieBlockType ublock;
    ublock.block = character->getBlock();
    c_SWFDisplayItem item = SWFMovie_add_internal(this->movie, ublock);
    if(item == NULL)
      return NULL;
    SWFDisplayItem *_item_ = new SWFDisplayItem(item);
    itemList.push_back(_item_);
    return _item_; 
  }

  void addExport(SWFBlock *exp, char *name)
    {  SWFMovie_addExport(this->movie, exp->getBlock(), name); }

  void remove(SWFDisplayItem *item)
    { SWFMovie_remove(this->movie, item->item); }

  bool replace(SWFDisplayItem *item, SWFBlock *character)
  { 
    SWFMovieBlockType ublock;
    ublock.block = character->getBlock();
    return SWFMovie_replace_internal(this->movie, item->item, ublock); 
  }

  void nextFrame()
    { SWFMovie_nextFrame(this->movie); }

  void labelFrame(const char *label)
    { SWFMovie_labelFrame(this->movie, label); }

  void namedAnchor(const char *label)
    { SWFMovie_namedAnchor(this->movie, label); }

  int output(int level=-1)
  {
    int oldlevel = Ming_setSWFCompression(level);
    int ret = SWFMovie_output_to_stream(this->movie, stdout);
    cleanUp();
    Ming_setSWFCompression(oldlevel);
    return ret;
  }

  int save(const char *filename, int level=-1)
  {
    int oldlevel = Ming_setSWFCompression(level);
    int result = SWFMovie_save(this->movie,filename);
    Ming_setSWFCompression(oldlevel);
    cleanUp();
    return result;
  }

  SWFSoundInstance *startSound(SWFSound *sound)
    { return new SWFSoundInstance(SWFMovie_startSound(this->movie, sound->sound)); }
  void stopSound(SWFSound *sound)
    { SWFMovie_stopSound(this->movie, sound->sound); }

  SWFCharacter *importCharacter(const char *filename, const char *name)
    { return new SWFCharacter(SWFMovie_importCharacter(this->movie, filename, name)); }

  SWFFontCharacter *importFont(const char *filename, const char *name)
  { return new SWFFontCharacter(SWFMovie_importFont(this->movie, filename, name)); }

  SWFFontCharacter *addFont(SWFFont *font)
  { return new SWFFontCharacter(SWFMovie_addFont(this->movie, font->font)); }

  void protect()
  { SWFMovie_protect(this->movie, NULL);}

  void protect(char *password)
  { SWFMovie_protect(this->movie,password); }
  
  void addMetadata(char *xml)
    { SWFMovie_addMetadata(this->movie, xml); }
  
  void setNetworkAccess(int flag)
    { SWFMovie_setNetworkAccess(this->movie, flag); }
  
  void setScriptLimits(int maxRecursion, int timeout)
    { SWFMovie_setScriptLimits(this->movie, maxRecursion, timeout); }

  void setTabIndex(int depth, int index)
    { SWFMovie_setTabIndex(this->movie, depth, index); }

  void assignSymbol(SWFCharacter *character, char *name)
    { SWFMovie_assignSymbol(this->movie, (c_SWFCharacter)character->getBlock(), name); }

  void defineScene(unsigned int offset, const char *name)
    { SWFMovie_defineScene(this->movie, offset, name); }

  void writeExports()
    { SWFMovie_writeExports(this->movie); }

  SWF_DECLAREONLY(SWFMovie);
 private:

  void cleanUp()
  {
    std::list<SWFDisplayItem *>::iterator iter = itemList.begin();
    for(; iter != itemList.end(); iter++)
      delete (*iter);
  }
  std::list<SWFDisplayItem *> itemList;
};





/*  SWFBitmap  */

class SWFBitmap : public SWFCharacter
{
 public:
  c_SWFBitmap bitmap;

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->bitmap; }
  
  SWFBitmap(const char *filename, const char *alpha=NULL)
  {
    if(strlen(filename) > 4)
    {
      if(strcasecmp(filename+strlen(filename)-4, ".dbl") == 0)
	this->bitmap = (c_SWFBitmap) newSWFDBLBitmap(fopen(filename, "rb"));

      else if(strcasecmp(filename+strlen(filename)-4, ".gif") == 0)
	this->bitmap = (c_SWFBitmap) newSWFDBLBitmapData_fromGifFile(filename);

      else if(strcasecmp(filename+strlen(filename)-4, ".png") == 0)
 		this->bitmap =   (c_SWFBitmap) newSWFDBLBitmapData_fromPngFile( filename );

      else if(strcasecmp(filename+strlen(filename)-4, ".jpg") == 0 ||
	(strlen(filename) > 5 && (strcasecmp(filename+strlen(filename)-5, ".jpeg") == 0)))
      {
	if(alpha != NULL)
	  this->bitmap = (c_SWFBitmap) newSWFJpegWithAlpha(fopen(filename, "rb"),
					     fopen(alpha, "rb"));
	else
	  this->bitmap = (c_SWFBitmap) newSWFJpegBitmap(fopen(filename, "rb"));
      }

      else
	bitmap = NULL;
    }
    if ( ! this->bitmap ) 
      throw SWFException("SWFBitmap(const char *filename, const char *alpha=NULL)");

    this->character = (c_SWFCharacter)bitmap;
  }
  
  SWFBitmap(unsigned char *raw, SWFRawImgFmt srcFmt, SWFBitmapFmt dstFmt, 
            unsigned short width, unsigned short height)
  {
    this->bitmap = newSWFBitmap_fromRawImg(raw, srcFmt, dstFmt, width, height);
    if ( ! this->bitmap ) 
      throw SWFException("SWFBitmap(const char *filename, const char *alpha=NULL)");
    this->character = (c_SWFCharacter)bitmap;
  }
  
  SWFBitmap(SWFInput *input)
  { 
    this->bitmap = newSWFBitmap_fromInput(input->input);
    if(this->bitmap == NULL)
      throw SWFException("SWFBitmap(SWFInput *input)");
    this->character = (c_SWFCharacter)bitmap;
  }

  virtual ~SWFBitmap()
    { destroySWFBitmap(this->bitmap); }
  
  int getWidth()
    { return SWFBitmap_getWidth(this->bitmap); }

  int getHeight()
    { return SWFBitmap_getHeight(this->bitmap); }

  SWF_DECLAREONLY(SWFBitmap);
  SWFBitmap();
};

class SWFFillStyle
{
 friend class SWFFill;
 public:
  c_SWFFillStyle fill;
  
  virtual ~SWFFillStyle() { }

  static SWFFillStyle *SolidFillStyle(byte r, byte g, byte b, byte a=255)
    { return new SWFFillStyle(newSWFSolidFillStyle(r, g, b, a)); }

  static SWFFillStyle *GradientFillStyle(SWFGradient *gradient, byte flags)
    { return new SWFFillStyle(newSWFGradientFillStyle(gradient->gradient, flags)); }

  static SWFFillStyle *BitmapFillStyle(SWFBitmap *bitmap, byte flags)
    { return new SWFFillStyle(newSWFBitmapFillStyle(bitmap->bitmap, flags)); }

 private:
  SWFFillStyle(c_SWFFillStyle fill)
  { 
    this->fill = fill; 
    if(this->fill == NULL)
      throw SWFException("SWFFillStyle");
  }
  SWF_DECLAREONLY(SWFFillStyle);
  SWFFillStyle();
};   

/*  SWFFill  */
class SWFFill
{
 friend class SWFShape;
 public:
  c_SWFFill fill;

  SWFFill(SWFFillStyle *fs)
  {
	this->fill = newSWFFill(fs->fill);
	if(this->fill == NULL)
		throw SWFException("SWFFill");
  }

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

  void setMatrix( float a, float b, float c, float d, float x, float y)
    { SWFFill_setMatrix(this->fill, a, b, c, d, x, y); }

  SWFFillStyle *getFillStyle()
    { return new SWFFillStyle(SWFFill_getFillStyle(this->fill)); }

  SWF_DECLAREONLY(SWFFill);
  SWFFill(); 

private:
  SWFFill(c_SWFFill fill)
  { 
    this->fill = fill; 
    if(this->fill == NULL)
      throw SWFException("SWFFill");
  }
};


/*  SWFShape  */
class SWFShape : public SWFCharacter
{
 public:
  c_SWFShape shape;

  SWFShape()
  { 
    this->shape = newSWFShape();
    if(this->shape == NULL)
      throw SWFException("SWFShape()");

    this->character = (c_SWFCharacter)shape;
  }

  SWFShape(c_SWFShape shape)
  { 
    this->shape = shape;
    if(this->shape == NULL)
      throw SWFException("SWFShape(c_SWFShape shape)");
  }

  virtual ~SWFShape()
    { destroySWFShape(this->shape); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->shape; }

  void movePen(double x, double y)
    { SWFShape_movePen(this->shape, x, y); }

  void movePenTo(double x, double y)
    { SWFShape_movePenTo(this->shape, x, y); }

  void drawLine(double x, double y)
    { SWFShape_drawLine(this->shape, x, y); }

  void drawLineTo(double x, double y)
    { SWFShape_drawLineTo(this->shape, x, y); }

  void drawCurve(double cx, double cy, double ax, double ay)
    { SWFShape_drawCurve(this->shape, cx, cy, ax, ay); }

  void drawCurveTo(double cx, double cy, double ax, double ay)
    { SWFShape_drawCurveTo(this->shape, cx, cy, ax, ay); }

  void drawCubic(double ax, double ay, double bx, double by, double cx, double cy)
    { SWFShape_drawCubic(this->shape, ax, ay, bx, by, cx, cy); }

  void drawCubicTo(double ax, double ay, double bx, double by, double cx, double cy)
    { SWFShape_drawCubicTo(this->shape, ax, ay, bx, by, cx, cy); }

  void getPen(double *x, double *y)
    { SWFShape_getPen(this->shape, x, y); }

  void end()
    { SWFShape_end(this->shape); }

  SWFFill *addSolidFill(byte r, byte g, byte b, byte a=0xff)
    { return new SWFFill(SWFShape_addSolidFill(this->shape, r, g, b, a)); }

  SWFFill *addGradientFill(SWFGradient *gradient, byte flags=0x10)
    { return new SWFFill(SWFShape_addGradientFill(this->shape, gradient->gradient, flags)); }

  SWFFill *addBitmapFill(SWFBitmap *bitmap, byte flags=0x40)
    { return new SWFFill(SWFShape_addBitmapFill(this->shape, bitmap->bitmap, flags)); }

  void setLeftFillStyle(SWFFillStyle *fill)
    { SWFShape_setLeftFillStyle(this->shape, fill->fill); }

  void setRightFillStyle(SWFFillStyle *fill)
    { SWFShape_setRightFillStyle(this->shape, fill->fill); }

  void setLeftFill(SWFFill *fill)
    { SWFShape_setLeftFill(this->shape, fill->fill); }

  void setRightFill(SWFFill *fill)
    { SWFShape_setRightFill(this->shape, fill->fill); }

  void setLine(unsigned short width, byte r, byte g, byte b, byte a=0xff)
    { SWFShape_setLine(this->shape, width, r, g, b, a); }

  void drawCharacterBounds(SWFCharacter *character)
	{ SWFShape_drawCharacterBounds(this->shape, character->character); }

  void setLineStyle(unsigned short width, byte r, byte g, byte b, byte a=0xff) // alias for setline
	{ setLine(width, r, g, b, a); }

  void setLine2(unsigned short width, SWFFillStyle *fill, int flags, float miterLimit)
    {  SWFShape_setLine2Filled(this->shape, width, fill->fill, flags, miterLimit); }
  
  void setLine2(unsigned short width, byte r, byte g, byte b, byte a, int flags, float miterLimit)
    {  SWFShape_setLine2(this->shape, width, r, g, b, a, flags, miterLimit); }

  void drawArc(double r, double startAngle, double endAngle)
    { SWFShape_drawArc(this->shape, r, startAngle, endAngle); }

  void drawCircle(double r)
    { SWFShape_drawCircle(this->shape, r); }

  void drawGlyph(SWFFont *font, unsigned short c, int size=0)
    { SWFShape_drawSizedGlyph(this->shape, font->font, c, size); }

  void useVersion(int version)
    { SWFShape_useVersion(this->shape, version); }

  int getVersion()
    { return SWFShape_getVersion(this->shape); }

  void setRenderingHintingFlags(int flags)
    { SWFShape_setRenderHintingFlags(this->shape, flags); }

  void hideLine()
    { SWFShape_hideLine(this->shape); }

  void drawFontGlyph(SWFFont *font, unsigned short c)
    { SWFShape_drawGlyph(this->shape, font->font, c); }

  char *dumpOutline()
  { return SWFShape_dumpOutline(this->shape); }

  SWF_DECLAREONLY(SWFShape);
};


/*  SWFMovieClip  */
class SWFMovieClip : public SWFCharacter
{
 public:
  c_SWFMovieClip clip;

  SWFMovieClip()
  { 
    this->clip = newSWFMovieClip();
    if(this->clip == NULL)
      throw SWFException("SWFMovieClip()");

    this->character = (c_SWFCharacter)clip;
  }

  virtual ~SWFMovieClip()
    { destroySWFMovieClip(this->clip); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->clip; }

  void setNumberOfFrames(int nFrames)
    { SWFMovieClip_setNumberOfFrames(this->clip, nFrames); }

  SWFDisplayItem *add(SWFBlock *character)
  {
    c_SWFDisplayItem item = SWFMovieClip_add(this->clip, character->getBlock());
    if(item == NULL)
      return NULL;
    else
      return new SWFDisplayItem(item); 
  }

  void remove(SWFDisplayItem *item)
    { SWFMovieClip_remove(this->clip, item->item); }

  void nextFrame()
    { SWFMovieClip_nextFrame(this->clip); }

  void labelFrame(char *label)
    { SWFMovieClip_labelFrame(this->clip, label); }

  void addInitAction(SWFAction* a)
    { SWFMovieClip_addInitAction(this->clip, a->action); }

  void setScalingGrid(int x, int y, int w, int h)
    { SWFMovieClip_setScalingGrid(this->clip, x, y, w, h); }

  void removeScalingGrid()
    { SWFMovieClip_removeScalingGrid(this->clip); }

   SWFSoundInstance *startSound(SWFSound *sound)
    { return new SWFSoundInstance(SWFMovieClip_startSound(this->clip, sound->sound)); }

  void stopSound(SWFSound *sound)
    { SWFMovieClip_stopSound(this->clip, sound->sound); }

  void setSoundStream(SWFSoundStream *sound, float rate, float skip=0.0)
  { SWFMovieClip_setSoundStreamAt(this->clip, sound->sound, rate, skip); }

  SWF_DECLAREONLY(SWFMovieClip);
};

/*  SWFSprite  */
/* deprecated ! */
class SWFSprite : public SWFMovieClip
{
  SWF_DECLAREONLY(SWFSprite);
};


/*  SWFMorph  */

class SWFMorph : public SWFCharacter
{
 public:
  c_SWFMorph morph;

  SWFMorph()
  { 
    this->morph = newSWFMorphShape();
    if(this->morph == NULL)
      throw SWFException("SWFMorph()");

    this->character = (c_SWFCharacter)morph;
  }

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

  SWFText(int version = 2)
  { 
      if(version == 2)
        this->text = newSWFText2();
      else 
        this->text = newSWFText();
      if(this->text == NULL)
        throw SWFException("SWFText()");
      this->character = (c_SWFCharacter)text;
  }

  virtual ~SWFText()
    { destroySWFText(this->text); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->text; }

  void setFont(SWFFont *font)
    { SWFText_setFont(this->text, font->font); }

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

  void addWideString(const unsigned short *string, int len, int *advance = NULL)
    { SWFText_addWideString(this->text, string, len, advance); }

  void setSpacing(float spacing)
    { SWFText_setSpacing(this->text, spacing); }

  float getStringWidth(const char *string)
    { return SWFText_getStringWidth(this->text, string); }

  float getWidth(const char *string)
    { return SWFText_getStringWidth(this->text, string); }

  float getWideStringWidth(const unsigned short *string)
    { return SWFText_getWideStringWidth(this->text, string); }

  float getUTF8Width(const char *string)
    { return SWFText_getUTF8StringWidth(this->text, string); }

  float getAscent()
    { return SWFText_getAscent(this->text); }

  float getDescent()
    { return SWFText_getDescent(this->text); }
  
  float getLeading()
    { return SWFText_getLeading(this->text); }
  SWF_DECLAREONLY(SWFText);
};

/*  SWFTextField  */

class SWFTextField : public SWFCharacter
{
 public:
  c_SWFTextField textField;

  SWFTextField()
  { 
    this->textField = newSWFTextField();
    if(this->textField == NULL)
      throw SWFException("SWFTextField()");
    this->character = (c_SWFCharacter)textField;
  }

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

  void setFieldHeight(int height)
    { SWFTextField_setFieldHeight(this->textField, height); }

  void setLength(int length)
    { SWFTextField_setLength(this->textField, length); }

  void addChars(const char *string)
    { SWFTextField_addChars(this->textField, string); }

  SWF_DECLAREONLY(SWFTextField);
};

class SWFButtonRecord
{
 friend class SWFButton;
 public:
  c_SWFButtonRecord record;

  void addFilter(SWFFilter *f)
    { SWFButtonRecord_addFilter(this->record, f->filter); }

  void setDepth(int depth)
    { SWFButtonRecord_setDepth(this->record, depth); }

  void setBlendMode(int mode)
    { SWFButtonRecord_setBlendMode(this->record, mode); }

  void move(float x, float y)
    { SWFButtonRecord_move(this->record, x, y); }

  void moveTo(float x, float y)
    { SWFButtonRecord_moveTo(this->record, x, y); }

  void rotate(float deg)
    { SWFButtonRecord_rotate(this->record, deg); }

  void rotateTo(float deg)
    { SWFButtonRecord_rotateTo(this->record, deg); }

  void scale(float scaleX, float scaleY)
    { SWFButtonRecord_scale(this->record, scaleX, scaleY); }

  void scaleTo(float scaleX, float scaleY)
    { SWFButtonRecord_scaleTo(this->record, scaleX, scaleY); }

  void skewX(float skewX)
    { SWFButtonRecord_skewX(this->record, skewX); }

  void skewY(float skewY)
    { SWFButtonRecord_skewY(this->record, skewY); }

  void skewXTo(float skewX)
    { SWFButtonRecord_skewXTo(this->record, skewX); }

  void skewYTo(float skewY)
    { SWFButtonRecord_skewYTo(this->record, skewY); }

 private:
  SWFButtonRecord(c_SWFButtonRecord record)
  { 
    this->record = record; 
    if(this->record == NULL)
      throw SWFException("ButtonRecord");
  }
  SWF_DECLAREONLY(SWFButtonRecord);	
};

/*  SWFButton  */

class SWFButton : public SWFCharacter
{
 public:
  c_SWFButton button;

  SWFButton()
  { 
    this->button = newSWFButton();
    if(this->button == NULL)
      throw SWFException("SWFButton()");
    this->character = (c_SWFCharacter)button;
  }

  virtual ~SWFButton()
    { destroySWFButton(this->button); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->button; }

  SWFButtonRecord* addShape(SWFCharacter *character, byte flags)
    { return new SWFButtonRecord(SWFButton_addCharacter(this->button, (c_SWFCharacter)character->getBlock(), flags)); }

  void addAction(SWFAction *action, int flags)
    { SWFButton_addAction(this->button, action->action, flags); }

  void setMenu(int flag=0)
    { SWFButton_setMenu(this->button, flag); }

  void addSound(SWFSound *sound, int flags)
    { SWFButton_addSound(this->button, sound->sound, flags); }

  void setScalingGrid(int x, int y, int w, int h)
    { SWFButton_setScalingGrid(this->button, x, y, w, h); }

  void removeScalingGrid()
    { SWFButton_removeScalingGrid(this->button); }

  SWFButtonRecord* addCharacter(SWFCharacter *character, byte flags)
    { return new SWFButtonRecord(
        SWFButton_addCharacter(this->button, (c_SWFCharacter)character->getBlock(), flags)); }

  SWF_DECLAREONLY(SWFButton);
};

/* SWFBinaryData */
class SWFBinaryData : public SWFBlock
{
 public:
  c_SWFBinaryData data;

  SWFBinaryData(unsigned char *data, int length)
  {
    this->data = newSWFBinaryData(data, length);
    if(this->data == NULL)
      throw SWFException("SWFBinaryData(char *data, int length)");
  }

  virtual ~SWFBinaryData()
    { destroySWFBinaryData(this->data); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->data; }
  SWF_DECLAREONLY(SWFBinaryData);
};

/* SWFVideoStream */
class SWFVideoStream : public SWFCharacter
{
 public:
  c_SWFVideoStream stream;

  SWFVideoStream()
  { 
    this->stream = newSWFVideoStream();
    if(this->stream == NULL) 
      throw SWFException("newSWFVideoStream()");
    this->character = (c_SWFCharacter)stream;
  }

  SWFVideoStream(const char *path)
  { 
    this->stream = newSWFVideoStream_fromFile(fopen(path, "rb")); 
    if(this->stream == NULL)
      throw SWFException("SWFVideoStream(const char *path)");
  }

  SWFVideoStream(FILE *file)
  { 
    this->stream = newSWFVideoStream_fromFile(file); 
    if(this->stream == NULL)
      throw SWFException(" SWFVideoStream(FILE *file)");
  }

  virtual ~SWFVideoStream()
    { destroySWFVideoStream(this->stream); }

  void setDimension(int width, int height)
    { SWFVideoStream_setDimension(this->stream, width, height); }

  int getNumFrames()
    { return SWFVideoStream_getNumFrames(this->stream); }

  int hasAudio()
    { return SWFVideoStream_hasAudio(this->stream); }

  int setFrameMode(int mode)
    { return SWFVideoStream_setFrameMode(this->stream, mode); }

  int nextFrame()
    { return SWFVideoStream_nextFrame(this->stream); }

  int seek(int frame, int whence)
    { return SWFVideoStream_seek(this->stream, frame, whence); }

  c_SWFBlock getBlock()
    { return (c_SWFBlock)this->stream; }

  SWF_DECLAREONLY(SWFVideoStream);

};

#endif /* SWF_MINGPP_H_INCLUDED */
