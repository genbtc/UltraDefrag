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

#ifndef _UDEFRAG_STRUCTS_H_
#define _UDEFRAG_STRUCTS_H_

//this externs them in C
#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    char letter;
    char fsname[MAXFSNAME];
    wchar_t label[MAX_PATH + 1];
    LARGE_INTEGER total_space;
    LARGE_INTEGER free_space;
    int is_removable;
    int is_dirty;
    ULONGLONG bytes_per_cluster;
} volume_info;

typedef struct {
    unsigned long files;              /* number of files */
    unsigned long directories;        /* number of directories */
    unsigned long compressed;         /* number of compressed files */
    unsigned long fragmented;         /* number of fragmented files */
    ULONGLONG fragments;              /* number of fragments */
    ULONGLONG bad_fragments;          /* number of fragments which need to be joined together */
    ULONGLONG bad_clusters;           /* number of clusters of the bad_fragments */
    double fragmentation;             /* fragmentation percentage */
    ULONGLONG used_clusters;          /* used space amount, in clusters*/
    ULONGLONG total_space;            /* volume size, in bytes */
    ULONGLONG free_space;             /* free space amount, in bytes */
    ULONGLONG mft_size;               /* mft size, in bytes */
    udefrag_operation_type current_operation;  /* identifies the currently running operation */
    unsigned long pass_number;        /* the current disk processing pass, increases
                                      immediately after the pass completion */
    ULONGLONG clusters_to_process;    /* number of clusters to process */
    ULONGLONG processed_clusters;     /* number of already processed clusters */
    double percentage;                /* job completion percentage */
    int completion_status;            /* zero for running jobs, positive value for succeeded, negative for failed */
    char *cluster_map;                /* the cluster map */
    int cluster_map_size;             /* size of the cluster map, (the number of cells.) */
    ULONGLONG moved_clusters;         /* number of moved clusters */
    ULONGLONG total_moves;            /* number of moves by move_files_to_front/back functions */
    int isfragfileslist;             /* Bool to prove that the fragmented files list has been filled by Analyze.c */
    struct prb_table *fragmented_files_prb; /* list of fragmented files; does not contain filtered out files */
} udefrag_progress_info;

typedef struct {
    winx_patlist in_filter;     /* paths to be defragmented */
    winx_patlist ex_filter;     /* paths to be skipped */
    winx_patlist cut_filter;    /* an auxiliary filter used internally to cut off files
                                when individual files/directories are to be defragmented */
    ULONGLONG fragment_size_threshold;  /* fragment size threshold */
    ULONGLONG size_limit;       /* file size threshold */
    ULONGLONG optimizer_size_limit; /* file size threshold for the disk optimization */
    ULONGLONG fragments_limit;  /* file fragments threshold */
    ULONGLONG time_limit;       /* processing time limit, in seconds */
    int refresh_interval;       /* progress refresh interval, in milliseconds */
    int disable_reports;        /* nonzero value disables generation of the file fragmentation reports */
    int dbgprint_level;         /* controls amount of debugging output */
    int dry_run;                /* set %UD_DRY_RUN% variable to avoid actual data moving in tests */
    int job_flags;              /* flags triggering algorithm features */
    int sorting_flags;          /* flags triggering file sorting features (UD_SORT_xxx flags) */
    int algorithm_defined_fst;  /* nonzero value indicates that the fragment size threshold
                                is set by the algorithm and not by the user */
    double fragmentation_threshold; /* fragmentation level threshold */
} udefrag_options;

struct _mft_zone {
    ULONGLONG start;
    ULONGLONG length;
};

typedef struct {
    char *name; /* the file system name, in uppercase */
    fs_type_enum type; /* the type of the file system */
                       /*
                       * The FAT-formatted disks are somewhat special as the
                       * first clusters of directories are immovable there.
                       */
    int is_fat;
    int is_ntfs;
} fs_type_struct;

/*
* More info at http://www.thescripts.com/forum/thread617704.html
* ('Dynamically-allocated Multi-dimensional Arrays - C').
*/
typedef struct {
    ULONGLONG(*array)[18];
    ULONGLONG field_size;
    int map_size;
    int n_colors;
    ULONGLONG clusters_per_cell;
    ULONGLONG clusters_per_last_cell;
    BOOLEAN opposite_order; /* clusters < cells */
    ULONGLONG cells_per_cluster;
    ULONGLONG unused_cells;
} cmap;

struct performance_counters {
    ULONGLONG overall_time;               /* time spent for volume processing */
    ULONGLONG analysis_time;              /* time spent for volume analysis */
    ULONGLONG searching_time;             /* time spent for searching */
    ULONGLONG moving_time;                /* time spent for file moves */
    ULONGLONG temp_space_releasing_time;  /* time spent to release space temporarily allocated by system */
};


struct file_counters {
    unsigned long tiny_files;
    unsigned long small_files;
    unsigned long average_files;
    unsigned long big_files;
    unsigned long huge_files;
    unsigned long giant_files;
};

/*preliminary Query.Cpp definitions, needs to be in &jp def below */
/**
* \brief path,filedisp,engineFinished,startGUI
*/
typedef struct {
    const wchar_t *path;      /* Path from the GUI. What to query */
    winx_file_disposition filedisp;
    int engineFinished;
    int startGUI;
} udefrag_query_parameters;


/*
* MFT_ZONE_SPACE is special meaning -
* it is used as a marker for MFT Zone space. */
//genBTC modified.
typedef enum {
    DEFAULT_GRAY = 0,
    UNUSED_MAP_SPACE,        /* other colors have more precedence */
    FREE_SPACE,                  /* has lowest precedence */
    SYSTEM_SPACE,
    SYSTEM_OVER_LIMIT_SPACE,
    FRAGM_SPACE,
    FRAGM_OVER_LIMIT_SPACE,
    UNFRAGM_SPACE,
    UNFRAGM_OVER_LIMIT_SPACE,
    DIR_SPACE,
    DIR_OVER_LIMIT_SPACE,
    COMPRESSED_SPACE,
    COMPRESSED_OVER_LIMIT_SPACE,
    MFT_ZONE_SPACE,
    MFT_SPACE,                   /* has highest precedence */
    IN_MOVE_PROGRESS_SPACE,
    TEAL_BLUE_GREEN,
    SUPER_OBNOXIOUS_GREEN,
    SPACE_STATES /* this must always be the last */
} g_enum_SPACE_STATES;

#if defined(__cplusplus)
}
#endif

#endif /* _UDEFRAG_STRUCTS_H */
