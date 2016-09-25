/* ANSI-C code produced by gperf version 3.0.3 */
/* Command-line: /Applications/Xcode6-Beta2.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/gperf -m 100 fcobjshash.gperf  */
/* Computed positions: -k'2-3' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif

#line 1 "fcobjshash.gperf"

#line 13 "fcobjshash.gperf"
struct FcObjectTypeInfo {
int name;
int id;
};
#include <string.h>
#include <stddef.h>
/* maximum key range = 56, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
FcObjectTypeHash (register const char *str, register unsigned int len)
{
  static const unsigned char asso_values[] =
    {
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60,  0, 35,  0,
      30, 35, 60, 20, 27,  0, 60, 60, 18,  3,
       8,  0, 18, 60,  0,  0, 25,  0, 60, 60,
      20, 10,  2, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60, 60, 60, 60, 60,
      60, 60, 60, 60, 60, 60
    };
  return len + asso_values[(unsigned char)str[2]] + asso_values[(unsigned char)str[1]];
}

struct FcObjectTypeNamePool_t
  {
    char FcObjectTypeNamePool_str4[sizeof("hash")];
    char FcObjectTypeNamePool_str5[sizeof("scale")];
    char FcObjectTypeNamePool_str6[sizeof("size")];
    char FcObjectTypeNamePool_str7[sizeof("foundry")];
    char FcObjectTypeNamePool_str8[sizeof("scalable")];
    char FcObjectTypeNamePool_str9[sizeof("family")];
    char FcObjectTypeNamePool_str10[sizeof("rasterizer")];
    char FcObjectTypeNamePool_str11[sizeof("namelang")];
    char FcObjectTypeNamePool_str12[sizeof("lang")];
    char FcObjectTypeNamePool_str13[sizeof("familylang")];
    char FcObjectTypeNamePool_str14[sizeof("postscriptname")];
    char FcObjectTypeNamePool_str15[sizeof("hinting")];
    char FcObjectTypeNamePool_str16[sizeof("minspace")];
    char FcObjectTypeNamePool_str17[sizeof("hintstyle")];
    char FcObjectTypeNamePool_str18[sizeof("fontformat")];
    char FcObjectTypeNamePool_str19[sizeof("fontversion")];
    char FcObjectTypeNamePool_str20[sizeof("fontfeatures")];
    char FcObjectTypeNamePool_str21[sizeof("dpi")];
    char FcObjectTypeNamePool_str22[sizeof("file")];
    char FcObjectTypeNamePool_str23[sizeof("slant")];
    char FcObjectTypeNamePool_str24[sizeof("aspect")];
    char FcObjectTypeNamePool_str25[sizeof("spacing")];
    char FcObjectTypeNamePool_str26[sizeof("fullname")];
    char FcObjectTypeNamePool_str27[sizeof("prgname")];
    char FcObjectTypeNamePool_str28[sizeof("capability")];
    char FcObjectTypeNamePool_str29[sizeof("pixelsize")];
    char FcObjectTypeNamePool_str30[sizeof("fullnamelang")];
    char FcObjectTypeNamePool_str31[sizeof("globaladvance")];
    char FcObjectTypeNamePool_str32[sizeof("outline")];
    char FcObjectTypeNamePool_str33[sizeof("autohint")];
    char FcObjectTypeNamePool_str34[sizeof("charset")];
    char FcObjectTypeNamePool_str35[sizeof("width")];
    char FcObjectTypeNamePool_str36[sizeof("charwidth")];
    char FcObjectTypeNamePool_str37[sizeof("charheight")];
    char FcObjectTypeNamePool_str39[sizeof("lcdfilter")];
    char FcObjectTypeNamePool_str40[sizeof("style")];
    char FcObjectTypeNamePool_str41[sizeof("weight")];
    char FcObjectTypeNamePool_str42[sizeof("antialias")];
    char FcObjectTypeNamePool_str43[sizeof("index")];
    char FcObjectTypeNamePool_str44[sizeof("stylelang")];
    char FcObjectTypeNamePool_str45[sizeof("decorative")];
    char FcObjectTypeNamePool_str46[sizeof("embolden")];
    char FcObjectTypeNamePool_str49[sizeof("verticallayout")];
    char FcObjectTypeNamePool_str52[sizeof("embeddedbitmap")];
    char FcObjectTypeNamePool_str59[sizeof("rgba")];
  };
static const struct FcObjectTypeNamePool_t FcObjectTypeNamePool_contents =
  {
    "hash",
    "scale",
    "size",
    "foundry",
    "scalable",
    "family",
    "rasterizer",
    "namelang",
    "lang",
    "familylang",
    "postscriptname",
    "hinting",
    "minspace",
    "hintstyle",
    "fontformat",
    "fontversion",
    "fontfeatures",
    "dpi",
    "file",
    "slant",
    "aspect",
    "spacing",
    "fullname",
    "prgname",
    "capability",
    "pixelsize",
    "fullnamelang",
    "globaladvance",
    "outline",
    "autohint",
    "charset",
    "width",
    "charwidth",
    "charheight",
    "lcdfilter",
    "style",
    "weight",
    "antialias",
    "index",
    "stylelang",
    "decorative",
    "embolden",
    "verticallayout",
    "embeddedbitmap",
    "rgba"
  };
#define FcObjectTypeNamePool ((const char *) &FcObjectTypeNamePool_contents)
const struct FcObjectTypeInfo *
FcObjectTypeLookup (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 45,
      MIN_WORD_LENGTH = 3,
      MAX_WORD_LENGTH = 14,
      MIN_HASH_VALUE = 4,
      MAX_HASH_VALUE = 59
    };

  static const struct FcObjectTypeInfo wordlist[] =
    {
      {-1}, {-1}, {-1}, {-1},
#line 61 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str4),FC_HASH_OBJECT},
#line 45 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str5),FC_SCALE_OBJECT},
#line 27 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str6),FC_SIZE_OBJECT},
#line 31 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str7),FC_FOUNDRY_OBJECT},
#line 42 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str8),FC_SCALABLE_OBJECT},
#line 18 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str9),FC_FAMILY_OBJECT},
#line 40 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str10),FC_RASTERIZER_OBJECT},
#line 58 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str11),FC_NAMELANG_OBJECT},
#line 50 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str12),FC_LANG_OBJECT},
#line 19 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str13),FC_FAMILYLANG_OBJECT},
#line 62 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str14),FC_POSTSCRIPT_NAME_OBJECT},
#line 34 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str15),FC_HINTING_OBJECT},
#line 46 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str16),FC_MINSPACE_OBJECT},
#line 33 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str17),FC_HINT_STYLE_OBJECT},
#line 53 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str18),FC_FONTFORMAT_OBJECT},
#line 51 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str19),FC_FONTVERSION_OBJECT},
#line 59 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str20),FC_FONT_FEATURES_OBJECT},
#line 43 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str21),FC_DPI_OBJECT},
#line 38 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str22),FC_FILE_OBJECT},
#line 24 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str23),FC_SLANT_OBJECT},
#line 28 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str24),FC_ASPECT_OBJECT},
#line 30 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str25),FC_SPACING_OBJECT},
#line 22 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str26),FC_FULLNAME_OBJECT},
#line 60 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str27),FC_PRGNAME_OBJECT},
#line 52 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str28),FC_CAPABILITY_OBJECT},
#line 29 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str29),FC_PIXEL_SIZE_OBJECT},
#line 23 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str30),FC_FULLNAMELANG_OBJECT},
#line 37 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str31),FC_GLOBAL_ADVANCE_OBJECT},
#line 41 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str32),FC_OUTLINE_OBJECT},
#line 36 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str33),FC_AUTOHINT_OBJECT},
#line 49 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str34),FC_CHARSET_OBJECT},
#line 26 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str35),FC_WIDTH_OBJECT},
#line 47 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str36),FC_CHAR_WIDTH_OBJECT},
#line 48 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str37),FC_CHAR_HEIGHT_OBJECT},
      {-1},
#line 57 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str39),FC_LCD_FILTER_OBJECT},
#line 20 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str40),FC_STYLE_OBJECT},
#line 25 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str41),FC_WEIGHT_OBJECT},
#line 32 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str42),FC_ANTIALIAS_OBJECT},
#line 39 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str43),FC_INDEX_OBJECT},
#line 21 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str44),FC_STYLELANG_OBJECT},
#line 56 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str45),FC_DECORATIVE_OBJECT},
#line 54 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str46),FC_EMBOLDEN_OBJECT},
      {-1}, {-1},
#line 35 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str49),FC_VERTICAL_LAYOUT_OBJECT},
      {-1}, {-1},
#line 55 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str52),FC_EMBEDDED_BITMAP_OBJECT},
      {-1}, {-1}, {-1}, {-1}, {-1}, {-1},
#line 44 "fcobjshash.gperf"
      {offsetof(struct FcObjectTypeNamePool_t, FcObjectTypeNamePool_str59),FC_RGBA_OBJECT}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      unsigned int key = FcObjectTypeHash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register int o = wordlist[key].name;
          if (o >= 0)
            {
              register const char *s = o + FcObjectTypeNamePool;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[key];
            }
        }
    }
  return 0;
}
