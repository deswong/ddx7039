/*
 * Copyright (C) 1999-2002 Free Software Foundation, Inc.
 * This file is part of the GNU LIBICONV Library.
 *
 * The GNU LIBICONV Library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * The GNU LIBICONV Library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the GNU LIBICONV Library; see the file COPYING.LIB.
 * If not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 */

/* This file defines all the converters. */


/* Our own notion of wide character, as UCS-4, according to ISO-10646-1. */
typedef unsigned int ucs4_t;

/* State used by a conversion. 0 denotes the initial state. */
typedef unsigned int state_t;

/* iconv_t is an opaque type. This is the real iconv_t type. */
typedef struct conv_struct * conv_t;

/*
 * Data type for conversion multibyte -> unicode
 */
struct mbtowc_funcs {
  int (*xxx_mbtowc) (conv_t conv, ucs4_t *pwc, unsigned char const *s, int n);
  /*
   * int xxx_mbtowc (conv_t conv, ucs4_t *pwc, unsigned char const *s, int n)
   * converts the byte sequence starting at s to a wide character. Up to n bytes
   * are available at s. n is >= 1.
   * Result is number of bytes consumed (if a wide character was read),
   * or -1 if invalid, or -2 if n too small, or -2-(number of bytes consumed)
   * if only a shift sequence was read.
   */
  int (*xxx_flushwc) (conv_t conv, ucs4_t *pwc);
  /*
   * int xxx_flushwc (conv_t conv, ucs4_t *pwc)
   * returns to the initial state and stores the pending wide character, if any.
   * Result is 1 (if a wide character was read) or 0 if none was pending.
   */
};

/* Return code if invalid. (xxx_mbtowc) */
#define RET_ILSEQ      -1
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define RET_TOOFEW(n)  (-2-(n))

/*
 * Data type for conversion unicode -> multibyte
 */
struct wctomb_funcs {
  int (*xxx_wctomb) (conv_t conv, unsigned char *r, ucs4_t wc, int n);
  /*
   * int xxx_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
   * converts the wide character wc to the character set xxx, and stores the
   * result beginning at r. Up to n bytes may be written at r. n is >= 1.
   * Result is number of bytes written, or -1 if invalid, or -2 if n too small.
   */
  int (*xxx_reset) (conv_t conv, unsigned char *r, int n);
  /*
   * int xxx_reset (conv_t conv, unsigned char *r, int n)
   * stores a shift sequences returning to the initial state beginning at r.
   * Up to n bytes may be written at r. n is >= 0.
   * Result is number of bytes written, or -2 if n too small.
   */
};

/* Return code if invalid. (xxx_wctomb) */
#define RET_ILUNI      -1
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define RET_TOOSMALL   -2

/*
 * Contents of a conversion descriptor.
 */
struct conv_struct {
  struct loop_funcs lfuncs;
  /* Input (conversion multibyte -> unicode) */
  int iindex;
  struct mbtowc_funcs ifuncs;
  state_t istate;
  /* Output (conversion unicode -> multibyte) */
  int oindex;
  struct wctomb_funcs ofuncs;
  int oflags;
  state_t ostate;
  /* Operation flags */
  int transliterate;
  int discard_ilseq;
};

/*
 * Include all the converters.
 */

#define		USE_ASCII						1

#define		USE_UTF8						1
#define		USE_UCS2						1
#define		USE_UCS2BE						1
#define		USE_UCS2LE						1
#define		USE_UCS4						1
#define		USE_UCS4BE						1
#define		USE_UCS4LE						1
#define		USE_UTF16						1
#define		USE_UTF16BE						1
#define		USE_UTF16LE						1
#define		USE_UTF32						1
#define		USE_UTF32BE						1
#define		USE_UTF32LE						1
#define		USE_UTF7						1
#define		USE_UCS2INTERNAL				1
#define		USE_UCS2SWAPPED					1
#define		USE_UCS4INTERNAL				1
#define		USE_UCS4SWAPPED					1
#define		USE_C99							0
#define		USE_JAVA						0

#define		USE_ISO8859_1					1
#define		USE_ISO8859_2					1
#define		USE_ISO8859_3					1
#define		USE_ISO8859_4					1
#define		USE_ISO8859_5					1
#define		USE_ISO8859_6					1
#define		USE_ISO8859_7					1
#define		USE_ISO8859_8					1
#define		USE_ISO8859_9					1
#define		USE_ISO8859_10					1
#define		USE_ISO8859_13					1
#define		USE_ISO8859_14					1
#define		USE_ISO8859_15					1
#define		USE_ISO8859_16					1
#define		USE_KOI8_R						0
#define		USE_KOI8_U						0
#define		USE_KOI8_RU						0
#define		USE_CP1250						0
#define		USE_CP1251						1
#define		USE_CP1252						0
#define		USE_CP1253						0
#define		USE_CP1254						0
#define		USE_CP1255						0
#define		USE_CP1256						0
#define		USE_CP1257						0
#define		USE_CP1258						0
#define		USE_CP850						0
#define		USE_CP862						0
#define		USE_CP866						0
#define		USE_MAC_ROMAN					0
#define		USE_MAC_CENTRALEUROPE			0
#define		USE_MAC_ICELAND					0
#define		USE_MAC_CROATIAN				0
#define		USE_MAC_ROMANIA					0
#define		USE_MAC_CYRILLIC				0
#define		USE_MAC_UKRAINE					0
#define		USE_MAC_GREEK					0
#define		USE_MAC_TURKISH					0
#define		USE_MAC_HEBREW					0
#define		USE_MAC_ARABIC					0
#define		USE_MAC_THAI					0
#define		USE_HP_ROMAN8					0
#define		USE_NEXTSTEP					0
#define		USE_ARMSCII_8					0
#define		USE_GEORGIAN_ACADEMY			0
#define		USE_GEORGIAN_PS					0
#define		USE_KOI8_T						0
#define		USE_MULELAO						0
#define		USE_CP1133						0
#define		USE_TIS620						0
#define		USE_CP874						0
#define		USE_VISCII						0
#define		USE_TCVN						0

#define		USE_ISO646_JP					0
#define		USE_JISX0201					0
#define		USE_JISX0208					0
#define		USE_JISX0212					0

#define		USE_ISO646_CN					0
#define		USE_GB2312						0
#define		USE_ISOIR165					0
#define		USE_GBK							0
#define		USE_CNS11643_1					0
#define		USE_CNS11643_2					0
#define		USE_CNS11643_3					0
#define		USE_CNS11643_4					0
#define		USE_CNS11643_5					0
#define		USE_CNS11643_6					0
#define		USE_CNS11643_7					0
#define		USE_CNS11643_15					0
#define		USE_CNS11643_inv				0
#define		USE_BIG5						0

#define		USE_KSC5601						0
#define		USE_JOHAB_HANGUL				0

#define		USE_EUC_JP						0
#define		USE_SJIS						1
#define		USE_CP932						0
#define		USE_ISO2022_JP					0
#define		USE_ISO2022_JP1					0
#define		USE_ISO2022_JP2					0

#define		USE_EUC_CN						0
#define		USE_CES_GBK						0
#define		USE_GB18030						0
#define		USE_ISO2022_CN					0
#define		USE_ISO2022_CNEXT				0
#define		USE_HZ							0
#define		USE_EUC_TW						0
#define		USE_CES_BIG5					0
#define		USE_CP950						0
#define		USE_BIG5HKSCS					0

#define		USE_EUC_KR						0
#define		USE_CP949						0
#define		USE_JOHAB						0
#define		USE_ISO2022_KR					0


#include "ascii.h"

/* General multi-byte encodings */
#include "utf8.h"
#include "ucs2.h"
#include "ucs2be.h"
#include "ucs2le.h"
#include "ucs4.h"
#include "ucs4be.h"
#include "ucs4le.h"
#include "utf16.h"
#include "utf16be.h"
#include "utf16le.h"
#include "utf32.h"
#include "utf32be.h"
#include "utf32le.h"
#include "utf7.h"
#include "ucs2internal.h"
#include "ucs2swapped.h"
#include "ucs4internal.h"
#include "ucs4swapped.h"
#include "c99.h"
#include "java.h"

/* 8-bit encodings */
#include "iso8859_1.h"
#include "iso8859_2.h"
#include "iso8859_3.h"
#include "iso8859_4.h"
#include "iso8859_5.h"
#include "iso8859_6.h"
#include "iso8859_7.h"
#include "iso8859_8.h"
#include "iso8859_9.h"
#include "iso8859_10.h"
#include "iso8859_13.h"
#include "iso8859_14.h"
#include "iso8859_15.h"
#include "iso8859_16.h"
#include "koi8_r.h"
#include "koi8_u.h"
#include "koi8_ru.h"
#include "cp1250.h"
#include "cp1251.h"
#include "cp1252.h"
#include "cp1253.h"
#include "cp1254.h"
#include "cp1255.h"
#include "cp1256.h"
#include "cp1257.h"
#include "cp1258.h"
#include "cp850.h"
#include "cp862.h"
#include "cp866.h"
#include "mac_roman.h"
#include "mac_centraleurope.h"
#include "mac_iceland.h"
#include "mac_croatian.h"
#include "mac_romania.h"
#include "mac_cyrillic.h"
#include "mac_ukraine.h"
#include "mac_greek.h"
#include "mac_turkish.h"
#include "mac_hebrew.h"
#include "mac_arabic.h"
#include "mac_thai.h"
#include "hp_roman8.h"
#include "nextstep.h"
#include "armscii_8.h"
#include "georgian_academy.h"
#include "georgian_ps.h"
#include "koi8_t.h"
#include "mulelao.h"
#include "cp1133.h"
#include "tis620.h"
#include "cp874.h"
#include "viscii.h"
#include "tcvn.h"

/* CJK character sets [CCS = coded character set] [CJKV.INF chapter 3] */

typedef struct {
  unsigned short indx; /* index into big table */
  unsigned short used; /* bitmask of used entries */
} Summary16;

#include "iso646_jp.h"
#include "jisx0201.h"
#include "jisx0208.h"
#include "jisx0212.h"

#include "iso646_cn.h"
#include "gb2312.h"
#include "isoir165.h"
/*#include "gb12345.h"*/
#include "gbk.h"
#include "cns11643.h"
#include "big5.h"

#include "ksc5601.h"
#include "johab_hangul.h"

/* CJK encodings [CES = character encoding scheme] [CJKV.INF chapter 4] */

#include "euc_jp.h"
#include "sjis.h"
#include "cp932.h"
#include "iso2022_jp.h"
#include "iso2022_jp1.h"
#include "iso2022_jp2.h"

#include "euc_cn.h"
#include "ces_gbk.h"
#include "gb18030.h"
#include "iso2022_cn.h"
#include "iso2022_cnext.h"
#include "hz.h"
#include "euc_tw.h"
#include "ces_big5.h"
#include "cp950.h"
#include "big5hkscs.h"

#include "euc_kr.h"
#include "cp949.h"
#include "johab.h"
#include "iso2022_kr.h"

/* Encodings used by system dependent locales. */

#ifdef USE_AIX
#include "cp856.h"
#include "cp922.h"
#include "cp943.h"
#include "cp1046.h"
#include "cp1124.h"
#include "cp1129.h"
#include "cp1161.h"
#include "cp1162.h"
#include "cp1163.h"
#endif

#ifdef USE_OSF1
#include "dec_kanji.h"
#include "dec_hanyu.h"
#endif

#ifdef USE_DOS
#include "cp437.h"
#include "cp737.h"
#include "cp775.h"
#include "cp852.h"
#include "cp853.h"
#include "cp855.h"
#include "cp857.h"
#include "cp858.h"
#include "cp860.h"
#include "cp861.h"
#include "cp863.h"
#include "cp864.h"
#include "cp865.h"
#include "cp869.h"
#include "cp1125.h"
#endif

#ifdef USE_EXTRA
#include "euc_jisx0213.h"
#include "shift_jisx0213.h"
#include "iso2022_jp3.h"
#include "tds565.h"
#include "riscos1.h"
#endif

