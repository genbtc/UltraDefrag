#pragma once
//All The Structs.

/* file.c */
typedef struct _WINX_FILE {
    HANDLE hFile;             /* file handle */
    LARGE_INTEGER roffset;    /* offset for read requests */
    LARGE_INTEGER woffset;    /* offset for write requests */
    void *io_buffer;          /* for the buffered i/o */
    size_t io_buffer_size;    /* size of the buffer, in bytes */
    size_t io_buffer_offset;  /* the current offset inside io_buffer */
    LARGE_INTEGER wboffset;   /* offset for write requests in the buffered mode */
} WINX_FILE, *PWINX_FILE;


typedef struct _winx_blockmap {
    struct _winx_blockmap *next; /* pointer to the next fragment */
    struct _winx_blockmap *prev; /* pointer to the previous fragment */
    ULONGLONG vcn;               /* the virtual cluster number */
    ULONGLONG lcn;               /* the logical cluster number */
    ULONGLONG length;            /* size of the fragment, in clusters */
} winx_blockmap;

typedef struct _winx_file_disposition {
    ULONGLONG clusters;                /* total number of clusters belonging to the file */
    ULONGLONG fragments;               /* total number of file fragments, not blocks */
    winx_blockmap *blockmap;           /* map of the blocks */
} winx_file_disposition;

typedef struct _winx_file_internal_info {
    ULONGLONG BaseMftId;
    ULONGLONG ParentDirectoryMftId;
} winx_file_internal_info;

/*
* All the file access times are in the standard time format.
* That is the number of 100-nanosecond intervals since January 1, 1601.
*/
typedef struct _winx_file_info {
    struct _winx_file_info *next;      /* pointer to the next item */
    struct _winx_file_info *prev;      /* pointer to the previous item */
    wchar_t *name;                     /* the name of the file */
    wchar_t *path;                     /* the full native path */
    unsigned long flags;               /* a combination of FILE_ATTRIBUTE_xxx flags defined in winnt.h */
    winx_file_disposition disp;        /* information about file fragments and their disposition */
    unsigned long user_defined_flags;  /* a combination of flags defined by the caller */
    winx_file_internal_info internal;  /* internal information used by ftw_scan_disk support routines */
    ULONGLONG creation_time;           /* the file creation time */
    ULONGLONG last_modification_time;  /* the time of the last file modification */
    ULONGLONG last_access_time;        /* the time of the last file access */
} winx_file_info;

/* list.c */
/**
* @brief Generic structure describing a double linked list entry.
*/
typedef struct _list_entry {
    struct _list_entry *next; /* pointer to the next entry */
    struct _list_entry *prev; /* pointer to the previous entry */
} list_entry;


typedef struct _winx_history_entry {
    struct _winx_history_entry *next;
    struct _winx_history_entry *prev;
    char *string;
} winx_history_entry;

typedef struct _winx_history {
    winx_history_entry *head;
    winx_history_entry *current;
    int n_entries;
} winx_history;


typedef struct _winx_patlist {
    int count;
    wchar_t **array;
    int flags;
    wchar_t *string;
} winx_patlist;


typedef struct _winx_time {
    short year;        // range [1601...]
    short month;       // range [1..12]
    short day;         // range [1..31]
    short hour;        // range [0..23]
    short minute;      // range [0..59]
    short second;      // range [0..59]
    short milliseconds;// range [0..999]
    short weekday;     // range [0..6] == [sunday..saturday]
} winx_time;


typedef struct _winx_volume_information {
    char volume_letter;                    /* must be set by the caller! */
    char fs_name[MAX_FS_NAME_LENGTH + 1];  /* file system name */
    wchar_t label[MAX_PATH + 1];           /* volume label */
    ULONGLONG total_bytes;                 /* total volume size, in bytes */
    ULONGLONG free_bytes;                  /* amount of free space, in bytes */
    ULONGLONG total_clusters;              /* total number of clusters */
    ULONGLONG bytes_per_cluster;           /* cluster size, in bytes */
    ULONG sectors_per_cluster;             /* number of sectors in each cluster */
    ULONG bytes_per_sector;                /* sector size, in bytes */
    NTFS_DATA ntfs_data;                   /* NTFS data, valid for NTFS formatted volumes only */
    int is_dirty;                          /* nonzero value indicates that the volume is dirty and needs to be repaired */
    ULONGLONG device_capacity;             /* total device capacity (including all partitions), in bytes */
} winx_volume_information;


typedef struct _winx_volume_region {
    struct _winx_volume_region *next;  /* pointer to the next region */
    struct _winx_volume_region *prev;  /* pointer to the previous region */
    ULONGLONG lcn;                     /* the logical cluster number */
    ULONGLONG length;                  /* size of the region, in clusters */
} winx_volume_region;