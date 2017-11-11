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

#ifndef _UDEFRAG_ENUMS_H
#define _UDEFRAG_ENUMS_H

typedef enum {
    ANALYSIS_JOB = 0,
    DEFRAGMENTATION_JOB,
    FULL_OPTIMIZATION_JOB,
    QUICK_OPTIMIZATION_JOB,
    MFT_OPTIMIZATION_JOB,
    SINGLE_FILE_MOVE_FRONT_JOB,
    SINGLE_FILE_MOVE_END_JOB
} udefrag_job_type;

typedef enum {
    VOLUME_ANALYSIS = 0,     /* should be zero */
    VOLUME_DEFRAGMENTATION,
    VOLUME_OPTIMIZATION
} udefrag_operation_type;


/* flags for Auxiliary structures for analyze.c
* in the get_volume_information routine. */
typedef enum {
    FS_UNKNOWN = 0, /* ext2 and others */
    FS_FAT12,
    FS_FAT16,
    FS_FAT32,
    FS_EXFAT,
    FS_NTFS,
    FS_UDF
} fs_type_enum;

/* flags for find_matching_free_region */
enum {
    FIND_MATCHING_RGN_FORWARD,
    FIND_MATCHING_RGN_BACKWARD,
    FIND_MATCHING_RGN_ANY
};

/* flags for the find_first_block routine */
enum {
    SKIP_PARTIALLY_MOVABLE_FILES = 0x1
};

/* flags for query.cpp/query.c*/
typedef enum {
    QUERY_GET_VCNLIST = 0,
    QUERY_GET_FREE_REGIONS,
    QUERY_OPERATION_3,
    STOPGAP_OPERATION_1
} udefrag_query_type;

#endif /* _UDEFRAG_ENUMS_H */