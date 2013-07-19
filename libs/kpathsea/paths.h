/* paths.h.in: fallback paths for TeX file searching.  Kpathsea doesn't
   provide format-specific routines for all these, but it still
   seems best to centralize the definitions.
   
   The meanings of all these paths are described in the various
   programs' documentation.  Various environment variables override these.

   By the way, // means to search subdirectories (recursively). A
   leading %% means to look only in the ls-R db, not on the disk. See the
   Kpathsea manual for full details.
   
   See kpathsea/INSTALL for a description of how the various
   path-related files are used and created, and how to change the
   default paths.  */

/* TeX and MF inputs, and the default for eps figures.  */

/* Modified by Klaus Gebhardt, 1996 - 1999 */

#ifndef DEFAULT_TEX_PATH
#define DEFAULT_TEX_PATH "."
#endif

#ifndef DEFAULT_MF_PATH
#define DEFAULT_MF_PATH "."
#endif

#ifndef DEFAULT_FMT_PATH
#define DEFAULT_FMT_PATH "."
#endif

#ifndef DEFAULT_BASE_PATH
#define DEFAULT_BASE_PATH "."
#endif

#ifndef DEFAULT_TEXPOOL_PATH
#define DEFAULT_TEXPOOL_PATH "."
#endif

#ifndef DEFAULT_MFPOOL_PATH
#define DEFAULT_MFPOOL_PATH "."
#endif

#ifndef DEFAULT_TFM_PATH
#define DEFAULT_TFM_PATH "."
#endif

#ifndef DEFAULT_VF_PATH
#define DEFAULT_VF_PATH "."
#endif

#ifndef DEFAULT_GLYPH_PATH
#define DEFAULT_GLYPH_PATH "."
#endif

#ifndef DEFAULT_GF_PATH
#define DEFAULT_GF_PATH "."
#endif

#ifndef DEFAULT_PK_PATH
#define DEFAULT_PK_PATH "."
#endif

#ifndef DEFAULT_BIB_PATH
#define DEFAULT_BIB_PATH "."
#endif

#ifndef DEFAULT_BST_PATH
#define DEFAULT_BST_PATH "."
#endif

#ifndef DEFAULT_CONFIG_PATH
#define DEFAULT_CONFIG_PATH "."
#endif

#ifndef DEFAULT_HEADER_PATH
#define DEFAULT_HEADER_PATH "."
#endif

#ifndef DEFAULT_FIG_PATH
#define DEFAULT_FIG_PATH DEFAULT_TEX_PATH
#endif

#ifndef DEFAULT_PICT_PATH
#define DEFAULT_PICT_PATH DEFAULT_FIG_PATH
#endif

#ifndef DEFAULT_GFFONTS
#define DEFAULT_GFFONTS "."
#endif

#ifndef DEFAULT_PKFONTS
#define DEFAULT_PKFONTS "."
#endif

#ifndef DEFAULT_GLYPHFONTS
#define DEFAULT_GLYPHFONTS "."
#endif

#ifndef DEFAULT_MFBASES
#define DEFAULT_MFBASES "."
#endif

#ifndef DEFAULT_BIBINPUTS
#define DEFAULT_BIBINPUTS "."
#endif

#ifndef DEFAULT_BSTINPUTS
#define DEFAULT_BSTINPUTS "."
#endif

#ifndef DEFAULT_TEXMF
#define DEFAULT_TEXMF "."
#endif

#ifndef DEFAULT_TEXMFCNF
#define DEFAULT_TEXMFCNF "."
#endif

#ifndef DEFAULT_TEXFORMATS
#define DEFAULT_TEXFORMATS "."
#endif

#ifndef DEFAULT_MFINPUTS
#define DEFAULT_MFINPUTS "."
#endif

#ifndef DEFAULT_MFPOOL
#define DEFAULT_MFPOOL "."
#endif

#ifndef DEFAULT_TEXINPUTS
#define DEFAULT_TEXINPUTS "."
#endif

#ifndef DEFAULT_TEXPOOL
#define DEFAULT_TEXPOOL "."
#endif

#ifndef DEFAULT_TFMFONTS
#define DEFAULT_TFMFONTS "."
#endif

#ifndef DEFAULT_VFFONTS
#define DEFAULT_VFFONTS "."
#endif

#ifndef DEFAULT_TEXCONFIG
#define DEFAULT_TEXCONFIG "."
#endif

#ifndef DEFAULT_DVIPSHEADERS
#define DEFAULT_DVIPSHEADERS "."
#endif

#ifndef DEFAULT_AFMFONTS
#define DEFAULT_AFMFONTS "."
#endif

#ifndef DEFAULT_TEXMFDBS
#define DEFAULT_TEXMFDBS "."
#endif

#ifndef DEFAULT_TEXFONTMAPS
#define DEFAULT_TEXFONTMAPS "."
#endif

#ifndef DEFAULT_MPMEMS
#define DEFAULT_MPMEMS "."
#endif

#ifndef DEFAULT_MFTINPUTS
#define DEFAULT_MFTINPUTS "."
#endif

#ifndef DEFAULT_MPINPUTS
#define DEFAULT_MPINPUTS "."
#endif

#ifndef DEFAULT_MPPOOL
#define DEFAULT_MPPOOL "."
#endif

#ifndef DEFAULT_MPSUPPORT
#define DEFAULT_MPSUPPORT "."
#endif

#ifndef DEFAULT_OCPINPUTS
#define DEFAULT_OCPINPUTS "."
#endif

#ifndef DEFAULT_OFMFONTS
#define DEFAULT_OFMFONTS "."
#endif

#ifndef DEFAULT_OPLFONTS
#define DEFAULT_OPLFONTS "."
#endif

#ifndef DEFAULT_OTPINPUTS
#define DEFAULT_OTPINPUTS "."
#endif

#ifndef DEFAULT_OVFFONTS
#define DEFAULT_OVFFONTS "."
#endif

#ifndef DEFAULT_OVPFONTS
#define DEFAULT_OVPFONTS "."
#endif

#ifndef DEFAULT_TEXPSHEADERS
#define DEFAULT_TEXPSHEADERS "."
#endif

#ifndef DEFAULT_TEXDOCS
#define DEFAULT_TEXDOCS "."
#endif

#ifndef DEFAULT_TEXSOURCES
#define DEFAULT_TEXSOURCES "."
#endif

#ifndef DEFAULT_TRFONTS
#define DEFAULT_TRFONTS "."
#endif

#ifndef DEFAULT_T1FONTS
#define DEFAULT_T1FONTS "."
#endif

#ifndef DEFAULT_INDEXSTYLE
#define DEFAULT_INDEXSTYLE "."
#endif

#ifndef DEFAULT_TTFONTS
#define DEFAULT_TTFONTS "."
#endif

#ifndef DEFAULT_T42FONTS
#define DEFAULT_T42FONTS "."
#endif

#ifndef DEFAULT_WEB2C
#define DEFAULT_WEB2C "."
#endif
