/*
 *  (C) Copyright 2008 ZeXx86 (zexx86@gmail.com)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef _DEFAULTS_H_
#define _DEFAULTS_H_

/********************************************************************\
|			      DEFINES				     |
\********************************************************************/


#define	  VERSION				"0.0.0.3"

/* 		      PLATFORM SPEC.			*/
#ifdef __WIN32__
 #define   PLATFORM				"Windows"
#else
 #ifdef __FreeBSD__
  #define   PLATFORM				"FreeBSD"
 #endif
 #ifdef __NetBSD__
  #define   PLATFORM				"NetBSD"
 #endif
 #ifdef __OpenBSD__
  #define   PLATFORM				"OpenBSD"
 #endif
 #ifdef __linux__
  #define   PLATFORM				"Linux"
 #endif
#endif


#define 	DEFAULT_PORT			6800

#define		DEFAULT_REFRESH_INTERVAL	60

#endif
