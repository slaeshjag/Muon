/* Muon - a new kind of rts
 * Copyright 2012 Steven Arnow <s@rdw.se> and Axel Isaksson (mr3d/h4xxel)
 * 
 * This file is part of Muon.
 * 
 * Muon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Muon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Muon.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SUPERWEAPON_H__
#define	__SUPERWEAPON_H__

#define	SUPERWEAPONS_DEFINED	2

#define	SUPERWEAPON_FOWHOLE	0
#define	SUPERWEAPON_NUKE	1


typedef struct SUPERWEAPON {
	unsigned int		type;
	unsigned int		time_request;
	unsigned int		time_deploy;
	unsigned int		time_end;
} SUPERWEAPON;

#endif