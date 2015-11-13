#ifndef _UDEFRAG_INTERNAL_FLAGS_H
#define _UDEFRAG_INTERNAL_FLAGS_H

/************************************************************/
/*             Constants affecting performance              */
/************************************************************/

/*
* Fragment size threshold for partial defragmentation.
*/
#define PART_DEFRAG_MAGIC_CONSTANT  (20 * 1024 * 1024)

/*
* Default file size threshold for disk optimization.
*/
#define OPTIMIZER_MAGIC_CONSTANT    (20 * 1024 * 1024)

/*
* A magic constant for cut_off_group_of_files routine.
*/
#define OPTIMIZER_MAGIC_CONSTANT_N  10

/*
* Another magic constant for
* cut_off_sorted_out_files routine.
*/
#define OPTIMIZER_MAGIC_CONSTANT_M  1

/************************************************************/
/*                Prototypes, constants etc.                */
/************************************************************/

#define MAX_FILE_SIZE ((ULONGLONG) -1)
#define MAX_RGN_SIZE  ((ULONGLONG) -1)
#define DEFAULT_FRAGMENT_SIZE_THRESHOLD (MAX_FILE_SIZE / 2)

/* flags for user_defined_flags member of filelist entries */
#define UD_FILE_EXCLUDED         0x1
#define UD_FILE_OVER_LIMIT       0x2

/* file status flags */
#define UD_FILE_LOCKED           0x4   /* file is locked by system */
#define UD_FILE_MOVING_FAILED    0x10  /* file moving completed with failure */
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

#endif /* _UDEFRAG_INTERNAL_FLAGS_H */
