#include "internal.h"

DLLIMPORTCLASSIMPLEMENT(__libiconv);
DLLIMPORTCLASSIMPLEMENT(__regedit);

BOOL libiconvLoadSucc()
{
	if (!DLLENTRYEXIST(__libiconv, libiconv_open)){
		return FALSE;
	}
	if (!DLLENTRYEXIST(__libiconv, libiconv)){
		return FALSE;
	}
	if (!DLLENTRYEXIST(__libiconv, libiconv_close)){
		return FALSE;
	}
	return TRUE;
}

BOOL RegEditLoadSucc()
{
	if (!DLLENTRYEXIST(__regedit, RegEdit_Init)){
		return FALSE;
	}
	if (!DLLENTRYEXIST(__regedit, RegEdit_GetProfileString)){
		return FALSE;
	}
	if (!DLLENTRYEXIST(__regedit, RegEdit_GetProfileInt)){
		return FALSE;
	}
	if (!DLLENTRYEXIST(__regedit, RegEdit_UInit)) {
		return FALSE;
	}
	return TRUE;
}