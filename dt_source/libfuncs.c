#define __USE_SYSBASE

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_MEMORY_H
#include <exec/memory.h>
#endif

#ifndef GRAPHICS_GFXBASE_H
#include <graphics/gfxbase.h>
#endif

#ifndef GRAPHICS_VIEW_H
#include <graphics/view.h>
#endif

#include <intuition/icclass.h>
#include <datatypes/pictureclass.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/datatypes.h>

#include <clib/alib_protos.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <class/classbase.h>
#include "libfuncs.h"


#define MII_NAME_LENGTH    10
#define MII_SIZE           74

static UWORD mii_colors[12] =
{
 0x0F00,
 0x0F60,
 0x0FF0,
 0x06F0,
 0x00F0,
 0x000F,
 0x066F,
 0x0F06,
 0x060F,
 0x0840,
 0x0FFF,
 0x0000
};

static UWORD skin_colors[6] =
{
 0x0F80,
 0x0E70,
 0x0C50,
 0x0A30,
 0x0820,
 0x0600
};

static UWORD hair_colors[8] =
{
 0x0000,
 0x0840,
 0x0F60,
 0x0FF0,
 0x0F00,
 0x0FFF,
 0x0F80,
 0x0660
};

static UWORD eye_colors[6] =
{
 0x0000,
 0x0660,
 0x06FF,
 0x000F,
 0x0840,
 0x0F00
};

static UWORD lip_colors[3] =
{
 0x0F06,
 0x0F00,
 0x0840
};


struct MiiData
{
 UBYTE raw[MII_SIZE];
};


static void parse_mii(struct MiiData *m, UBYTE *data)
{
 memcpy(m->raw, data, MII_SIZE);
}

static int mii_get_bit(UBYTE *data, int byte, int bit)
{
 return (data[byte] >> bit) & 1;
}

static int mii_get_bits(UBYTE *data, int byte, int bit, int count)
{
 int val = 0;
 int i;
 for(i = 0; i < count; i++)
  val |= mii_get_bit(data, byte + ((bit + i) / 8), (bit + i) % 8) << i;
 return val;
}

static int mii_day(struct MiiData *m)
{
 int d = 0;
 if(m->raw[0] & 1)   d += 8;
 if(m->raw[0] & 2)   d += 16;
 if(m->raw[1] & 32)  d += 1;
 if(m->raw[1] & 64)  d += 2;
 if(m->raw[1] & 128) d += 4;
 return d;
}

static int mii_month(struct MiiData *m)
{
 int mo = 0;
 if(m->raw[0] & 4)   mo += 1;
 if(m->raw[0] & 8)   mo += 2;
 if(m->raw[0] & 16)  mo += 4;
 if(m->raw[0] & 32)  mo += 8;
 return mo;
}

static int mii_female(struct MiiData *m)
{
 return (m->raw[0] & 64) ? 1 : 0;
}

static int mii_favColor(struct MiiData *m)
{
 int c = 0;
 if(m->raw[1] & 2)   c += 1;
 if(m->raw[1] & 4)   c += 2;
 if(m->raw[1] & 8)   c += 4;
 if(m->raw[1] & 16)  c += 8;
 return c;
}

static int mii_faceShape(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x20, 0, 3);
}

static int mii_skinColor(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x20, 3, 3);
}

static int mii_hairType(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x22, 0, 7);
}

static int mii_hairColor(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x22, 7, 3);
}

static int mii_eyeType(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x28, 0, 6);
}

static int mii_eyeColor(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x2A, 0, 3);
}

static int mii_eyeVertPos(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x29, 3, 5);
}

static int mii_noseType(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x2C, 0, 4);
}

static int mii_noseVertPos(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x2D, 4, 4);
}

static int mii_lipType(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x2E, 0, 5);
}

static int mii_lipColor(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x2E, 5, 2);
}

static int mii_eyebrowType(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x24, 0, 5);
}

static int mii_eyebrowVertPos(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x26, 7, 5);
}

static int mii_glassesType(struct MiiData *m)
{
 return mii_get_bits(m->raw, 0x30, 0, 4);
}

static int mii_mole(struct MiiData *m)
{
 return m->raw[0x34] & 1;
}


static void draw_pixel(UBYTE *buf, UWORD width, UWORD x, UWORD y, UBYTE color)
{
 if(x >= width) return;
 buf[y * width + x] = color & 0x0F;
}


static int in_ellipse(int x, int y, int cx, int cy, int rx, int ry)
{
 LONG dx = x - cx;
 LONG dy = y - cy;
 return (dx * dx * ry * ry + dy * dy * rx * rx) <= (rx * rx * ry * ry);
}


static void mii_render_face(UBYTE *bm, UWORD width, UWORD height, struct MiiData *m)
{
 UWORD cx = width / 2;
 UWORD cy = height / 2;
 int shape = mii_faceShape(m);
 int skin  = mii_skinColor(m);
 int rx = 14;
 int ry = 20;

 UBYTE skin_col = 2;
 UBYTE outline_col = 1;
 UBYTE eye_col = 3;
 UBYTE hair_col = 4;
 UBYTE lip_col = 5;

 int x, y;

 switch(skin)
 {
  case 0: case 1: skin_col = 2; outline_col = 1; break;
  case 2: skin_col = 6; outline_col = 1; break;
  case 3: skin_col = 7; outline_col = 1; break;
  case 4: skin_col = 8; outline_col = 1; break;
  case 5: skin_col = 9; outline_col = 11; break;
 }

 for(y = 0; y < height; y++)
  for(x = 0; x < width; x++)
  {
   if(in_ellipse(x, y, cx, cy - 2, rx, ry))
   {
    UBYTE c = skin_col;
    if((x == cx - rx || x == cx + rx || y == cy - 2 - ry || y == cy - 2 + ry))
    {
     if(in_ellipse(x, y, cx, cy - 2, rx, ry))
      c = outline_col;
    }
    if(in_ellipse(x, y, cx, cy - 2, rx - 1, ry - 1) &&
       (!in_ellipse(x, y, cx, cy - 2, rx - 2, ry - 2)))
    {
     c = outline_col;
    }
    draw_pixel(bm, width, x, y, c);
   }
  }

 for(x = cx - rx; x <= cx + rx; x++)
 {
  if(in_ellipse(x, cy - 2 - ry, cx, cy - 2, rx, ry))
   draw_pixel(bm, width, x, cy - 2 - ry, outline_col);
  if(in_ellipse(x, cy - 2 + ry, cx, cy - 2, rx, ry))
   draw_pixel(bm, width, x, cy - 2 + ry, outline_col);
 }

 int eye_type = mii_eyeType(m);
 int eye_vert = mii_eyeVertPos(m);
 int eye_y = cy - 4 + (eye_vert - 12) / 3;

 int eye_w = 3 + (eye_type % 4);
 int eye_h = 2 + ((eye_type / 8) % 2);

 int left_eye_x = cx - 5;
 int right_eye_x = cx + 5 - eye_w;

 for(y = eye_y; y < eye_y + eye_h; y++)
  for(x = left_eye_x; x < left_eye_x + eye_w; x++)
   draw_pixel(bm, width, x, y, 10);
 for(y = eye_y; y < eye_y + eye_h; y++)
  for(x = right_eye_x; x < right_eye_x + eye_w; x++)
   draw_pixel(bm, width, x, y, 10);

 int pupil_off = eye_w / 2;
 draw_pixel(bm, width, left_eye_x + pupil_off, eye_y + eye_h / 2, 11);
 draw_pixel(bm, width, right_eye_x + pupil_off, eye_y + eye_h / 2, 11);

 int nose_type = mii_noseType(m);
 int nose_y = cy + 2 + (mii_noseVertPos(m) - 9) / 3;
 int nose_w = 2 + (nose_type % 3);

 for(x = cx - nose_w; x <= cx + nose_w; x++)
  draw_pixel(bm, width, x, nose_y, outline_col);
 draw_pixel(bm, width, cx, nose_y + 1, outline_col);

 int lip_type = mii_lipType(m);
 int lip_y = cy + 8;
 int lip_w = 4 + (lip_type % 4);

 for(x = cx - lip_w; x <= cx + lip_w; x++)
  draw_pixel(bm, width, x, lip_y, lip_col);
 draw_pixel(bm, width, cx - lip_w, lip_y - 1, lip_col);
 draw_pixel(bm, width, cx + lip_w, lip_y - 1, lip_col);

 int hair_type = mii_hairType(m);
 int hair_y = cy - 2 - ry - 4;

 for(y = hair_y; y < cy - 2 - ry; y++)
  for(x = cx - rx - 2; x <= cx + rx + 2; x++)
  {
   int in_hair = 0;
   if(x >= cx - rx && x <= cx + rx) in_hair = 1;
   else if(x >= cx - rx - 2 && x <= cx - rx && y < cy - 2 - ry + 4) in_hair = 1;
   else if(x >= cx + rx && x <= cx + rx + 2 && y < cy - 2 - ry + 4) in_hair = 1;

   if(in_hair)
   {
    int skip = 0;
    if(hair_type > 15 && y >= hair_y + 2) skip = 1;
    if(hair_type > 12 && hair_type <= 15 && y >= hair_y + 3) skip = 1;
    if(hair_type > 10 && hair_type <= 12 && y == hair_y) skip = 1;

    if(!skip)
     draw_pixel(bm, width, x, y, hair_col);
   }
  }

 int brow_type = mii_eyebrowType(m);
 int brow_y = eye_y - 3 - (mii_eyebrowVertPos(m) - 10) / 3;
 int brow_w = 4 + (brow_type % 3);

 for(x = left_eye_x - 1; x > left_eye_x - 1 - brow_w && x >= 0; x--)
  draw_pixel(bm, width, x, brow_y, 1);
 for(x = right_eye_x + eye_w + 1; x < right_eye_x + eye_w + 1 + brow_w && x < width; x++)
  draw_pixel(bm, width, x, brow_y, 1);

 int glasses = mii_glassesType(m);
 if(glasses > 0)
 {
  for(y = eye_y - 1; y < eye_y + eye_h + 1; y++)
  {
   draw_pixel(bm, width, left_eye_x - 2, y, glasses);
   draw_pixel(bm, width, left_eye_x + eye_w + 1, y, glasses);
   draw_pixel(bm, width, right_eye_x - 2, y, glasses);
   draw_pixel(bm, width, right_eye_x + eye_w + 1, y, glasses);
  }
  for(x = left_eye_x - 2; x <= left_eye_x + eye_w + 1; x++)
  {
   draw_pixel(bm, width, x, eye_y - 2, glasses);
   draw_pixel(bm, width, x, eye_y + eye_h, glasses);
  }
  for(x = right_eye_x - 2; x <= right_eye_x + eye_w + 1; x++)
  {
   draw_pixel(bm, width, x, eye_y - 2, glasses);
   draw_pixel(bm, width, x, eye_y + eye_h, glasses);
  }
  draw_pixel(bm, width, cx, eye_y - 3, glasses);
 }

 if(mii_mole(m))
 {
  int mx = cx + 4;
  int my = cy + 12;
  draw_pixel(bm, width, mx, my, 11);
 }
}


Class * __saveds __asm ObtainPicClass ( register __a6 struct ClassBase *cb)
{
 return (cb->cb_Class);
}

ULONG setdtattrs (struct ClassBase * cb, Object * o, ULONG data,...)
{
 return (SetDTAttrsA (o, NULL, NULL, (struct TagItem *) & data));
}

ULONG getdtattrs (struct ClassBase * cb, Object * o, ULONG data,...)
{
 return (GetDTAttrsA (o, (struct TagItem *) & data));
}

extern char __aligned ExLibName [];

Class *initClass (struct ClassBase * cb)
{
 Class *cl;

 if (cl = MakeClass (&ExLibName[0], PICTUREDTCLASS, NULL, NULL, 0L))
  {
   cl->cl_Dispatcher.h_Entry = (HOOKFUNC) Dispatch;
   cl->cl_UserData = (ULONG) cb;
   AddClass (cl);
  }

 return (cl);
}

ULONG __saveds __asm Dispatch ( register __a0 Class * cl, register __a2 Object * o, register __a1 Msg msg)
{
 struct ClassBase *cb = (struct ClassBase *) cl->cl_UserData;
 ULONG retval = NULL;

 switch (msg->MethodID)
  {
   case OM_NEW:
    {
     if (retval = DoSuperMethodA (cl, o, msg))
      {
       if (!GetGfxData(cb, cl, (Object *) retval, ((struct opSet *) msg)->ops_AttrList))
        {
         CoerceMethod (cl, (Object *) retval, OM_DISPOSE);
         return NULL;
        }
      }
     break;
    }
   default:
    {
     retval = (ULONG) DoSuperMethodA (cl, o, msg);
     break;
    }
  }

 return(retval);
}

ULONG __saveds __stdargs DTS_ReadIntoBitMap(struct ClassBase *cb, Object * o, Class *cl, struct TagItem * attrs);

ULONG __saveds __asm GetGfxData ( register __a6 struct ClassBase * cb, register __a0 Class * cl, register __a2 Object * o, register __a1 struct TagItem * attrs)
{
 return DTS_ReadIntoBitMap(cb, o, cl, attrs);
}

ULONG __saveds __stdargs DTS_GetBestModeID(ULONG width, ULONG height, ULONG depth);

ULONG __saveds __stdargs DTS_ReadIntoBitMap(struct ClassBase *cb, Object * o, Class *cl, struct TagItem * attrs)
{
 struct BitMapHeader *bmhd;

 BOOL success = TRUE;

 struct RastPort __aligned trp;
 struct RastPort __aligned rp;

 struct BitMap *bm, *tbm;

 ULONG i, width, height;
 UBYTE *buffer;

 UBYTE *readname;

 struct ColorRegister *cmap;
 LONG *cregs;

 BPTR fh;

 UWORD ID = (UWORD) 0;
 ULONG source_type;

  UBYTE sig_check[4];


  source_type = (ULONG) GetTagData (DTA_SourceType, DTST_FILE, attrs);
  if(source_type != DTST_FILE)
   {
    SetIoErr(ERROR_OBJECT_WRONG_TYPE);
    return(FALSE);
   }

  readname = (UBYTE *) GetTagData (DTA_Name, NULL, attrs);
  getdtattrs (cb, o, PDTA_BitMapHeader, &bmhd, TAG_DONE, NULL);


  fh = Open(readname, MODE_OLDFILE);
  if(!fh)
   {
    SetIoErr(ERROR_OBJECT_NOT_FOUND);
    return(FALSE);
   }

  FRead(fh, sig_check, 4, 1);

  if((sig_check[0] != 'R') || (sig_check[1] != 'N') ||
     ((sig_check[2] != 'C') && (sig_check[2] != 'O')) ||
     (sig_check[3] != 'D'))
   {
    Close(fh);
    SetIoErr(ERROR_OBJECT_WRONG_TYPE);
    return(FALSE);
   }

  width = 48;
  height = 56;

  bmhd->bmh_Width  = (bmhd->bmh_PageWidth  = width);
  bmhd->bmh_Height = (bmhd->bmh_PageHeight = height);
  bmhd->bmh_Depth = 4;

  setdtattrs(cb, o, PDTA_NumColors,      12,
                     TAG_DONE,            NULL);

  getdtattrs(cb, o, PDTA_ColorRegisters, (ULONG) &cmap,
                     PDTA_CRegs,          &cregs,
                     TAG_DONE,            NULL);

  if( (!cmap) || (!cregs) )
   {
    success = FALSE;
   }else
   {
    if(tbm = AllocBitMap (bmhd->bmh_Width, 1, bmhd->bmh_Depth, BMF_CLEAR, NULL))
     {
      InitRastPort (&trp);
      trp.BitMap = tbm;

      if (bm = AllocBitMap (bmhd->bmh_Width, bmhd->bmh_Height, bmhd->bmh_Depth, BMF_CLEAR, NULL))
       {
        InitRastPort (&rp);
        rp.BitMap = bm;

        buffer = (APTR) AllocVec(width * height, MEMF_CLEAR|MEMF_PUBLIC);
        if(buffer)
         {
          struct MiiData mii_data;
          UBYTE mii_raw[MII_SIZE];
          ULONG read_size;
          LONG data_start;

          data_start = (sig_check[2] == 'O') ? 4 : 6;

          Seek(fh, data_start, OFFSET_BEGINNING);
          read_size = FRead(fh, mii_raw, 1, MII_SIZE);
          while(read_size < MII_SIZE)
           mii_raw[read_size++] = 0;

          parse_mii(&mii_data, mii_raw);

         mii_render_face(buffer, width, height, &mii_data);

         for(i = 0; i < height; i++)
          {
           WritePixelLine8(&rp, 0, i, width, buffer + (i * width), &trp);
          }

         for(i = 0; i < 12; i++)
          {
           UWORD col;
           switch(i)
           {
            case 0:  col = 0x0000; break;
            case 1:  col = 0x0220; break;
            case 2:  { int s = mii_skinColor(&mii_data); col = skin_colors[s % 6]; break; }
            case 3:  { int e = mii_eyeColor(&mii_data); col = eye_colors[e % 6]; break; }
            case 4:  { int h = mii_hairColor(&mii_data); col = hair_colors[h % 8]; break; }
            case 5:  { int l = mii_lipColor(&mii_data); col = lip_colors[l % 3]; break; }
            case 6:  col = mii_colors[1]; break;
            case 7:  col = mii_colors[6]; break;
            case 8:  col = mii_colors[8]; break;
            case 9:  col = mii_colors[9]; break;
            case 10: col = mii_colors[10]; break;
            case 11: col = 0x0000; break;
            default: col = mii_colors[i]; break;
           }

           cmap->red   = ((col >> 8) & 0x0F) * 17;
           cmap->green = ((col >> 4) & 0x0F) * 17;
           cmap->blue  = (col & 0x0F) * 17;
           cmap++;

           cregs[i * 3    ] = (LONG)((col >> 8) & 0x0F) * 17 << 24;
           cregs[i * 3 + 1] = (LONG)((col >> 4) & 0x0F) * 17 << 24;
           cregs[i * 3 + 2] = (LONG)(col & 0x0F) * 17 << 24;
          }

         setdtattrs (cb, o,
                     DTA_ObjName,       readname,
                     DTA_NominalHoriz,  bmhd->bmh_Width,
                     DTA_NominalVert,   bmhd->bmh_Height,
                     PDTA_BitMap,       bm,
                     PDTA_ModeID,       DTS_GetBestModeID(bmhd->bmh_Width, bmhd->bmh_Height, 4),
                     TAG_DONE);

         FreeVec(buffer);
        }else
        {
         WaitBlit();
         FreeBitMap(bm);

         success = FALSE;
         SetIoErr (ERROR_NO_FREE_STORE);
        }

      }else
      {
       success = FALSE;
       SetIoErr (ERROR_NO_FREE_STORE);
      }

     WaitBlit();
     FreeBitMap(tbm);
    }else
    {
     success = FALSE;
     SetIoErr (ERROR_NO_FREE_STORE);
    }
  }

 Close(fh);

 return(success);
}

ULONG __saveds __stdargs DTS_GetBestModeID(ULONG width, ULONG height, ULONG depth)
{
 ULONG mode_id = NULL;

 mode_id = BestModeID(BIDTAG_NominalWidth,  width,
                      BIDTAG_NominalHeight, height,
                      BIDTAG_DesiredWidth,  width,
                      BIDTAG_DesiredHeight, height,
                      BIDTAG_Depth,         depth,
                      TAG_END);

 if(mode_id == INVALID_ID)
  {
       if((width > 724) && (depth < 3)) mode_id = SUPER_KEY;
  else if((width > 362) && (depth < 5)) mode_id = HIRES_KEY;
  else                                  mode_id = LORES_KEY;

  if(!ModeNotAvailable(mode_id | PAL_MONITOR_ID))
   {
    if(height > 283) mode_id |= LACE;

    mode_id |= PAL_MONITOR_ID;
   }else
   {
    if(!ModeNotAvailable(mode_id | NTSC_MONITOR_ID))
     {
      if(height > 241) mode_id |= LACE;

      mode_id |= NTSC_MONITOR_ID;
     }
   }
  }

 return(mode_id);
}
