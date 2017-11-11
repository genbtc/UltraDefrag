#pragma once
/*
*  UltraDefrag - a powerful defragmentation tool for Windows NT.
*  Copyright (c) 2007-2017 Dmitri Arkhangelski (dmitriar@gmail.com).
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _UDEFRAG_FLAGS_H
#define _UDEFRAG_FLAGS_H

//COLORMAP:
#define DEFAULT_COLOR DEFAULT_GRAY
#define UNKNOWN_SPACE DEFAULT_GRAY

/************************************************************/
/*             Constants affecting performance              */
/************************************************************/
#define DEFAULT_REFRESH_INTERVAL 100

/* fragment size threshold for partial defragmentation */
#define PART_DEFRAG_MAGIC_CONSTANT  (20 * 1024 * 1024)

/* default file size threshold for optimization */
#define OPTIMIZER_MAGIC_CONSTANT    (20 * 1024 * 1024)

/* magic constants for the cut_off_group_of_files routine */
#define OPTIMIZER_MAGIC_CONSTANT_N  10
#define OPTIMIZER_MAGIC_CONSTANT_M  1

#define DEFAULT_FRAGMENT_SIZE_THRESHOLD (MAX_FILE_SIZE / 2)
/************************************************************/
/*                Prototypes, constants etc.                */
/************************************************************/
#define MAX_DOS_DRIVES 26
#define MAXFSNAME      32  /* enough for Volume label*/
#define MAX_FILE_SIZE ((ULONGLONG) -1)
#define MAX_RGN_SIZE  ((ULONGLONG) -1)

/* should be enough for any path in UTF-8 encoding */
#define MAX_UTF8_PATH_LENGTH (256 * 1024)

/* flags for the user_defined_flags member of the filelist entries */
#define UD_FILE_EXCLUDED         0x1
#define UD_FILE_OVER_LIMIT       0x2
/* file status flags */
#define UD_FILE_LOCKED           0x4   /* file is locked by system */
#define UD_FILE_MOVING_FAILED    0x10  /* file moving failed */
#define UD_FILE_IMPROPER_STATE   0x20  /* file is in improper state (chkdsk run needed) or some bug encountered */

/*
* This flag is used to skip already processed
* files in individual volume processing tasks.
*/
#define UD_FILE_CURRENTLY_EXCLUDED      0x40

/*
* This flag is used to speed things up.
* If we'll repeatedly check the file
* we'll noticeably slow down all the
* volume procesing routines.
*/
#define UD_FILE_NOT_LOCKED              0x80

/*
* This flag is used to avoid
* repeated moves in volume optimization.
*/
#define UD_FILE_MOVED_TO_FRONT         0x100

/*
* This flag is used to mark files
* fragmented by MFT optimizer.
*/
#define UD_FILE_FRAGMENTED_BY_FILE_OPT 0x200

/*
* Some essential DOS/boot files need to
* be at fixed locations on disk, so we're
* skipping them to keep the computer bootable.
*/
#define UD_FILE_ESSENTIAL_BOOT_FILE    0x800

/* This flag is used to speed things up. */
#define UD_FILE_NOT_ESSENTIAL_FILE     0x1000

/*
* Auxiliary flag for move_files_to_front routine.
*/
#define UD_FILE_REGION_NOT_FOUND       0x2000

/* These flags are used to speed things up. */
#define UD_FILE_MFT_FILE               0x4000
#define UD_FILE_NOT_MFT_FILE           0x8000

#define is_excluded(f)               ((f)->user_defined_flags & UD_FILE_EXCLUDED)
#define is_over_limit(f)             ((f)->user_defined_flags & UD_FILE_OVER_LIMIT)
#define is_locked(f)                 ((f)->user_defined_flags & UD_FILE_LOCKED)
#define is_moving_failed(f)          ((f)->user_defined_flags & UD_FILE_MOVING_FAILED)
#define is_in_improper_state(f)      ((f)->user_defined_flags & UD_FILE_IMPROPER_STATE)
#define is_currently_excluded(f)     ((f)->user_defined_flags & UD_FILE_CURRENTLY_EXCLUDED)
#define is_moved_to_front(f)         ((f)->user_defined_flags & UD_FILE_MOVED_TO_FRONT)
#define is_fragmented_by_file_opt(f) ((f)->user_defined_flags & UD_FILE_FRAGMENTED_BY_FILE_OPT)
#define is_essential_boot_file(f)    ((f)->user_defined_flags & UD_FILE_ESSENTIAL_BOOT_FILE)
#define is_not_essential_file(f)     ((f)->user_defined_flags & UD_FILE_NOT_ESSENTIAL_FILE)
#define is_mft_file(f)               ((f)->user_defined_flags & UD_FILE_MFT_FILE)
#define is_not_mft_file(f)           ((f)->user_defined_flags & UD_FILE_NOT_MFT_FILE)
#define is_ntfs(f)                   (f == FS_NTFS)

#define is_block_excluded(b)         ((b)->length == 0)

/*
* The UD_SORT_BY_xxx flags
* are mutually exclusive.
*/
#define UD_SORT_BY_PATH               0x1
#define UD_SORT_BY_SIZE               0x2
#define UD_SORT_BY_CREATION_TIME      0x4
#define UD_SORT_BY_MODIFICATION_TIME  0x8
#define UD_SORT_BY_ACCESS_TIME        0x10
#define UD_SORT_DESCENDING            0x20

#define TINY_FILE_SIZE            0 * 1024  /* < 10 KB */
#define SMALL_FILE_SIZE          10 * 1024  /* 10 - 100 KB */
#define AVERAGE_FILE_SIZE       100 * 1024  /* 100 KB - 1 MB */
#define BIG_FILE_SIZE          1024 * 1024  /* 1 - 16 MB */
#define HUGE_FILE_SIZE    16 * 1024 * 1024  /* 16 - 128 MB */
#define GIANT_FILE_SIZE  128 * 1024 * 1024  /* > 128 MB */

/* debug print levels */
#define DBG_NORMAL     0
#define DBG_DETAILED   1
#define DBG_PARANOID   2

/* UltraDefrag error codes */
#define UDEFRAG_UNKNOWN_ERROR     (-1)
#define UDEFRAG_NO_MEM            (-4)
#define UDEFRAG_CDROM             (-5)
#define UDEFRAG_REMOTE            (-6)
#define UDEFRAG_ASSIGNED_BY_SUBST (-7)
#define UDEFRAG_REMOVABLE         (-8)
#define UDEFRAG_UDF_DEFRAG        (-9)
#define UDEFRAG_DIRTY_VOLUME      (-12)

/* flags triggering algorithm features */
#define UD_JOB_REPEAT                     0x1
/*
* 0x2, 0x4, 0x8 flags have been used
* in the past for experimental options
*/
#define UD_JOB_CONTEXT_MENU_HANDLER       0x10

#endif /* _UDEFRAG_FLAGS_H */
