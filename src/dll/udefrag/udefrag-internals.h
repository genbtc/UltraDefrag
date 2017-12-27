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

#ifndef _UDEFRAG_INTERNALS_H_
#define _UDEFRAG_INTERNALS_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "../zenwinx/ntndk.h"
#include "../zenwinx/zenwinx.h"

#include "ud_flags.h"
#include "ud_enums.h"
#include "ud_structs.h"
#include "udefrag.h"
#include "../../include/version.h"


//auxiliary.c
double calc_percentage(ULONGLONG x, ULONGLONG y);

//Callbacks. (for udefrag_start_job)
typedef void(*udefrag_progress_callback)(udefrag_progress_info *pi, void *p);
typedef int(*udefrag_terminator)(void *p);
//Callbacks (For udefrag_start_query)
typedef void(*udefrag_query_progress_callback)(udefrag_query_parameters *qp, void *p);
typedef int(*udefrag_termination_router)(void *p);

//Query.c
void gui_fileslist_finished(void);
void gui_query_finished(void);

// Helps extern/export defs, dont remove:
#if defined(__cplusplus)
}
#endif

/**
* \brief aka "jp->". Job Parameters = A whole littany of objects. Kitchen Sink. Passed back and forth All-The-Time. Holds State too.
*/
typedef struct _udefrag_job_parameters {
    unsigned char volume_letter;                /* volume letter */
    udefrag_job_type job_type;                  /* type of the requested job */
    udefrag_progress_callback cb;               /* progress update callback */
    udefrag_terminator t;                       /* termination callback {wxgui.exe!QueryThread::Terminator(void *)} */
    void *p;                                    /* pointer to user defined data to be passed to both callbacks */
    udefrag_termination_router termination_router;  /* address of procedure triggering job termination {udefrag.dll!query_terminator(void *)}*/
    ULONGLONG start_time;                       /* time of the job launch */
    ULONGLONG progress_refresh_time;            /* time of the last progress refresh */
    udefrag_options udo;                        /* job options */
    udefrag_progress_info pi;                   /* progress counters */
    winx_volume_information v_info;             /* basic volume information */
    fs_type_enum fs_type;                       /* type of the file system */
    int is_fat;                                 /* nonzero value indicates that the file system is a kind of FAT */
    int is_ntfs;                                /* quick indicate that it is NTFS: */
    winx_file_info *filelist;                   /* list of files */
    struct prb_table *fragmented_files;         /* list of fragmented files; does not contain filtered out files */
    winx_volume_region *free_regions;           /* list of free space regions */
    unsigned long free_regions_count;           /* number of free space regions */
    ULONGLONG clusters_at_once;                 /* number of clusters to be moved at once */
    cmap cluster_map;                           /* cluster map's internal data */
    WINX_FILE *fVolume;                         /* handle of the volume, intended for use by file moving routines */
    struct performance_counters p_counters;     /* performance counters */
    struct prb_table *file_blocks;              /* pointer to the binary tree of all file blocks found on the volume */
    struct file_counters f_counters;            /* file counters */
    NTSTATUS last_move_status;                  /* status of the last move file operation; zero by default */
    ULONGLONG already_optimized_clusters;       /* number of clusters needing no sorting in optimization */
    int progress_trigger;                       /* a trigger used for debugging purposes */
    struct _mft_zone mft_zone;                  /* initial mft zone disposition */
    int win_version;                            /* Windows version */
    udefrag_query_parameters qp;               /* Embed a pointer to Query_parameters inside this for query.c */
    udefrag_query_progress_callback qpcb;       /* query progress update callback */
} udefrag_job_parameters;

int get_options(udefrag_job_parameters *jp);
void release_options(udefrag_job_parameters *jp);

int save_fragmentation_report(udefrag_job_parameters *jp);
void remove_fragmentation_report(udefrag_job_parameters *jp);

void dbg_print_file_counters(udefrag_job_parameters *jp);

int allocate_map(int map_size,udefrag_job_parameters *jp);
void free_map(udefrag_job_parameters *jp);
void reset_cluster_map(udefrag_job_parameters *jp);
void colorize_map_region(udefrag_job_parameters *jp,
        ULONGLONG lcn, ULONGLONG length, int new_color, int old_color);
void colorize_file(udefrag_job_parameters *jp, winx_file_info *f, int old_color);
int get_file_color(udefrag_job_parameters *jp, winx_file_info *f);
void release_temp_space_regions(udefrag_job_parameters *jp);
//void redraw_all_temporary_system_space_as_free(udefrag_job_parameters *jp);

int analyze(udefrag_job_parameters *jp);
int defragment(udefrag_job_parameters *jp);
int optimize(udefrag_job_parameters *jp);
int optimize_mft(udefrag_job_parameters *jp);
void destroy_lists(udefrag_job_parameters *jp);
int check_fragmentation_level(udefrag_job_parameters *jp);

void dbg_print_header(udefrag_job_parameters *jp);
ULONGLONG start_timing(char *operation_name,udefrag_job_parameters *jp);
ULONGLONG stop_timing(char *operation_name,ULONGLONG start_time,udefrag_job_parameters *jp);
void dbg_print_performance_counters(udefrag_job_parameters *jp);
void dbg_print_footer(udefrag_job_parameters *jp);

int check_region(udefrag_job_parameters *jp,ULONGLONG lcn,ULONGLONG length);

int is_file_locked(winx_file_info *f,udefrag_job_parameters *jp);
int is_mft(winx_file_info *f, fs_type_enum fs_type);    //genBTC

int exclude_by_fragment_size(winx_file_info *f,udefrag_job_parameters *jp);
int exclude_by_fragments(winx_file_info *f,udefrag_job_parameters *jp);
int exclude_by_size(winx_file_info *f,udefrag_job_parameters *jp);
int expand_fragmented_files_list(winx_file_info *f,udefrag_job_parameters *jp);
void truncate_fragmented_files_list(winx_file_info *f,udefrag_job_parameters *jp);
winx_blockmap* build_fragments_list(winx_file_info *f,ULONGLONG *n_fragments);
void release_fragments_list(winx_blockmap **fragments);
void clear_currently_excluded_flag(udefrag_job_parameters *jp);

winx_volume_region *find_first_free_region(udefrag_job_parameters *jp,
    ULONGLONG min_lcn,ULONGLONG min_length,ULONGLONG *max_length);
winx_volume_region *find_last_free_region(udefrag_job_parameters *jp,
    ULONGLONG min_lcn,ULONGLONG min_length,ULONGLONG *max_length);
winx_volume_region *find_largest_free_region(udefrag_job_parameters *jp);
/*winx_volume_region *find_matching_free_region(udefrag_job_parameters *jp,
    ULONGLONG start_lcn, ULONGLONG min_length, int preferred_position);
*/

int create_file_blocks_tree(udefrag_job_parameters *jp);
int add_block_to_file_blocks_tree(udefrag_job_parameters *jp, winx_file_info *file, winx_blockmap *block);
int remove_block_from_file_blocks_tree(udefrag_job_parameters *jp, winx_blockmap *block);
void destroy_file_blocks_tree(udefrag_job_parameters *jp);
winx_blockmap *find_first_block(udefrag_job_parameters *jp,
    ULONGLONG *min_lcn, int flags, winx_file_info **first_file);

int movefile_to_start_or_end(udefrag_job_parameters *jp,int start_or_end);

void deliver_progress_info(udefrag_job_parameters *jp,int completion_status);

/*Begin Query.C definitions */
#if defined(__cplusplus)
extern "C" {
#endif
//void otherCleanupFunction(udefrag_job_parameters* jp);
int query_get_VCNlist(udefrag_job_parameters *jp);
int query_get_freeRegions(udefrag_job_parameters *jp);

int move_file(winx_file_info *f,
    ULONGLONG vcn,
    ULONGLONG length,
    ULONGLONG target,
    udefrag_job_parameters *jp
);
int can_move(winx_file_info *f, fs_type_enum fstype);   //genBTC
int can_move_entirely(winx_file_info *f, fs_type_enum fstype);

int udefrag_starts_query(char volume_letter, udefrag_query_type job_type, int flags, int cluster_map_size,
    udefrag_query_progress_callback qpcb, udefrag_terminator t, udefrag_query_parameters qp, void *p);

// Helps extern/export defs, dont remove:
#if defined(__cplusplus)
}
#endif

#endif /* _UDEFRAG_INTERNALS_H_ */
