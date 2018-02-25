#include "internal.h"

DLLIMPORTCLASSIMPLEMENT(__libiconv);

BOOL libiconvLoadSucc()
{
	if (!DLLENTRYEXIST(__libiconv, libiconv_open))
	{
		return FALSE;
	}
	if (!DLLENTRYEXIST(__libiconv, libiconv))
	{
		return FALSE;
	}
	if (!DLLENTRYEXIST(__libiconv, libiconv_close))
	{
		return FALSE;
	}
	return TRUE;
}