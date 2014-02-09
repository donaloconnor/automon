#ifndef DIAL_MATH_SUPPORT_H
#define DIAL_MATH_SUPPORT_H

#ifndef Q_WS_X11
#define Q_WS_X11
#endif
/*
 * math-support.h - part of DialWidget
 * Copyright © 2007 Michael Pyne <michael.pyne@kdemail.net>
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
 */


template <class T>
int sgn(T t)
{
    return t < 0 ? -1 : (t > 0 ? 1 : 0);
}

template<class T>
T clamp(const T &value, const T& l, const T& r)
{
    if(value < l)
	return l;
    else if(value > r)
	return r;

    return value;
}

#endif
