/***********************************************************************
 * n:/Werk/src/csmash-0.3.3/z.h
 * $Id: z.h,v 1.2 2000/07/18 14:30:19 nan Exp $
 *
 * Copyright (C) 2000  渡辺 正泰(Watanabe Masahiro)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ***********************************************************************/
#ifndef __wata_0005__z_h__INCLUDED__
#define __wata_0005__z_h__INCLUDED__
/***********************************************************************/
#include <zlib.h>
/* __BEGIN__BEGIN__ */
gzFile gzopenx(const char *filename, const char *mode);
/* __END__END__ */
/***********************************************************************/
#endif
/***********************************************************************
 * END OF z.h
 ***********************************************************************/
