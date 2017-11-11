/*
 *  ZenWINX - WIndows Native eXtended library.
 *  Copyright (c) 2007-2016 Dmitri Arkhangelski (dmitriar@gmail.com).
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

/*
* zenwinx.dll interface definitions.
*/

#ifndef _ZENWINX_H_
#define _ZENWINX_H_

#if defined(__cplusplus)
extern "C" {
#endif

//genBTC refactored the definitions into these two files
//if you need to #include some C struct primitives now you can.

//----------------------------------------------------------------------------
// Headers/Includes.                                                         |
//----------------------------------------------------------------------------
/* #defines and flags  */
#include "z-defs.h"
/* internal structures */
#include "z-structs.h"
/* debugging macros */
#include "../include/dbg.h"
//------------------------/
// Debugging Macros.      /
//------------------------/
/* prints whatever message specified */
#define trace(format,...)  winx_dbg_print(0,format,## __VA_ARGS__)

/* prints {prefix}{function name}: {specified string} */
#define etrace(format,...) winx_dbg_print(0,E "%s: " format,__FUNCTION__,## __VA_ARGS__)
#define itrace(format,...) winx_dbg_print(0,I "%s: " format,__FUNCTION__,## __VA_ARGS__)
#define dtrace(format,...) winx_dbg_print(0,D "%s: " format,__FUNCTION__,## __VA_ARGS__)

/* prints {error prefix}{function name}: {specified string}: {specified nt status and its description} */
#define strace(status,format,...) { \
    NtCurrentTeb()->LastStatusValue = status; \
    winx_dbg_print(NT_STATUS_FLAG,E "%s: " format,__FUNCTION__,## __VA_ARGS__); \
}

/* prints {error prefix}{function name}: {specified string}: {last error and its description} */
#define letrace(format,...) winx_dbg_print(LAST_ERROR_FLAG,E "%s: " format,__FUNCTION__,## __VA_ARGS__)

/* prints {error prefix}{function name}: not enough memory */
#define mtrace() etrace("not enough memory")

#define TraceEnter  { trace(D"Inside function '%s'...",__FUNCTION__); }
#define TraceExit   { trace(D"Leaving function '%s'...",__FUNCTION__); }
#define TraceSource { trace(D"Source file '%s' at line %d ...",__FILE__,__LINE__); }

/* DbgCheckN macros simplify validation of function parameters */
#define DbgCheck1(c,r) { \
    if(!(c)) {           \
        etrace("the first parameter is incorrect"); \
        return (r);      \
    }                    \
}

#define DbgCheck2(c1,c2,r) { \
    DbgCheck1(c1,r)          \
    if(!(c2)) {              \
        etrace("the second parameter is incorrect"); \
        return (r);          \
    }                        \
}

#define DbgCheck3(c1,c2,c3,r) { \
    DbgCheck2(c1,c2,r)       \
    if(!(c3)) {              \
        etrace("the third parameter is incorrect"); \
        return (r);          \
    }                        \
}

/* dbg.c - zenwinx debug functions prototypes */
void winx_set_dbg_log(wchar_t *path);
#define winx_enable_dbg_log(path) winx_set_dbg_log(path)
#define winx_disable_dbg_log()    winx_set_dbg_log(NULL)
void winx_flush_dbg_log(int flags);
void winx_dbg_print(int flags, const char *format, ...);
void winx_dbg_print_header(char ch, int width, const char *format, ...);

/* env.c */
wchar_t *winx_getenv(wchar_t *name);
int winx_setenv(wchar_t *name, wchar_t *value);

/* event.c */
int winx_create_event(wchar_t *name,int type,HANDLE *phandle);
int winx_open_event(wchar_t *name,int flags,HANDLE *phandle);
void winx_destroy_event(HANDLE h);

/* file.c */
WINX_FILE *winx_fopen(const wchar_t *filename,const char *mode);
WINX_FILE *winx_fbopen(const wchar_t *filename,const char *mode,int buffer_size);
size_t winx_fread(void *buffer,size_t size,size_t count,WINX_FILE *f);
size_t winx_fwrite(const void *buffer,size_t size,size_t count,WINX_FILE *f);
ULONGLONG winx_fsize(WINX_FILE *f);
void winx_fclose(WINX_FILE *f);
int winx_ioctl(WINX_FILE *f,
    int code,char *description,
    void *in_buffer,int in_size,
    void *out_buffer,int out_size,
    int *pbytes_returned);
int winx_fflush(WINX_FILE *f);
int winx_create_directory(const wchar_t *path);
int winx_delete_file(const wchar_t *filename);
void *winx_get_file_contents(const wchar_t *filename,size_t *bytes_read);
void winx_release_file_contents(void *contents);

typedef int  (*ftw_filter_callback)(winx_file_info *f,void *user_defined_data);
typedef void (*ftw_progress_callback)(winx_file_info *f,void *user_defined_data);
typedef int  (*ftw_terminator)(void *user_defined_data);

winx_file_info *winx_ftw(wchar_t *path, int flags,
        ftw_filter_callback fcb, ftw_progress_callback pcb, ftw_terminator t,void *user_defined_data);

winx_file_info *winx_scan_disk(char volume_letter, int flags,
        ftw_filter_callback fcb,ftw_progress_callback pcb, ftw_terminator t,void *user_defined_data);

void winx_ftw_release(winx_file_info *filelist);
#define winx_scan_disk_release(f) winx_ftw_release(f)

int winx_ftw_dump_file(winx_file_info *f,ftw_terminator t,void *user_defined_data);

#ifdef _NTNDK_H_
NTSTATUS winx_defrag_fopen(winx_file_info *f,int action,HANDLE *phandle);
#define NtCloseSafe(h) { if(h) { NtClose(h); h = NULL; } }
void winx_defrag_fclose(HANDLE h);
#endif

/* keyboard.c */
int winx_kb_init(void);

/* ldr.c */
void *winx_get_proc_address(wchar_t *libname,char *funcname);

/* list.c */
list_entry *winx_list_insert(list_entry **phead,list_entry *prev,long size);
void winx_list_remove(list_entry **phead,list_entry *item);
void winx_list_destroy(list_entry **phead);

/* lock.c */
int winx_create_lock(wchar_t *name,HANDLE *phandle);
int winx_acquire_lock(HANDLE h,int msec);
int winx_release_lock(HANDLE h);
void winx_destroy_lock(HANDLE h);

/* mem.c */
void *winx_heap_alloc(size_t size,int flags);
void winx_heap_free(void *addr);


/*
* If small amount of memory is needed,
* call winx_malloc and don't care on the 
* returned value correctness. In case of
* allocation failure it will simply abort
* the application.
* On the other hand, if some big amount
* of memory needs to be allocated,
* winx_tmalloc needs to be called and then
* its returned value needs to be checked for
* being equal to zero.
*/

/* aborts the application in case of allocation failure */
#define winx_malloc(n) winx_heap_alloc(n,MALLOC_ABORT_ON_FAILURE)

/* this form is tolerant for allocation failures */
#define winx_tmalloc(n) winx_heap_alloc(n,0)

#define winx_free winx_heap_free

typedef int (*winx_killer)(size_t n);
void winx_set_killer(winx_killer k);

/* misc.c */
void winx_sleep(int msec);
int winx_get_os_version(void);
wchar_t *winx_get_windows_directory(void);
int winx_query_symbolic_link(wchar_t *name, wchar_t *buffer, int length);

/* process mode constants */
#define INTERNAL_SEM_FAILCRITICALERRORS 0
int winx_set_system_error_mode(unsigned int mode);

wchar_t *winx_get_windows_boot_options(void);
int winx_windows_in_safe_mode(void);

/* mutex.c */
int winx_create_mutex(wchar_t *name,HANDLE *phandle);
int winx_open_mutex(wchar_t *name,HANDLE *phandle);
int winx_release_mutex(HANDLE h);
void winx_destroy_mutex(HANDLE h);

/* path.c */
void winx_path_remove_extension(wchar_t *path);
//Removes the file name from a path. Writable.
void winx_path_remove_filename(wchar_t *path);
void winx_path_extract_filename(wchar_t *path);
wchar_t *winx_get_module_filename(void);
int winx_create_path(wchar_t *path);
int winx_enable_privilege(unsigned long luid);

/* reg.c */
int winx_bootex_check(const wchar_t *command);
int winx_bootex_register(const wchar_t *command);
int winx_bootex_unregister(const wchar_t *command);

/* stdio.c */

#ifdef _NTNDK_H_
int winx_putch(int ch);
int winx_puts(const char *string);
void winx_print(char *string);
int winx_printf(const char *format, ...);
/*
* Note: winx_scanf() can not be implemented;
* use winx_gets() and sscanf() instead.
*/
int winx_kbhit(int msec);
int winx_kb_read(KBD_RECORD *kbd_rec,int msec);
int winx_breakhit(int msec);

int winx_getch(void);
int winx_getche(void);
int winx_gets(char *string,int n);

void winx_init_history(winx_history *h);
void winx_destroy_history(winx_history *h);

int winx_prompt(char *prompt,char *string,int n,winx_history *h);

int winx_print_strings(char **strings,int line_width,
    int max_rows,char *prompt,int divide_to_pages);
#endif /* _NTNDK_H_ */

/* string.c */
/* reliable _toupper and _tolower analogs */
char winx_toupper(char c);
char winx_tolower(char c);
/* reliable towupper and towlower analogs */
wchar_t winx_towupper(wchar_t c);
wchar_t winx_towlower(wchar_t c);
/* reliable _wcsupr and _wcslwr analogs */
wchar_t *winx_wcsupr(wchar_t *s);
wchar_t *winx_wcslwr(wchar_t *s);

char *winx_strdup(const char *s);
wchar_t *winx_wcsdup(const wchar_t *s);
int winx_wcsicmp(const wchar_t *s1, const wchar_t *s2);
wchar_t *winx_wcsistr(const wchar_t *s1, const wchar_t *s2);
char *winx_stristr(const char *s1, const char *s2);
int winx_wcsmatch(wchar_t *string, wchar_t *mask, int flags);
char *winx_vsprintf(const char *format,va_list arg);
char *winx_sprintf(const char *format, ...);
wchar_t *winx_vswprintf(const wchar_t *format,va_list arg);
wchar_t *winx_swprintf(const wchar_t *format, ...);

int winx_patcomp(winx_patlist *patterns,wchar_t *string,wchar_t *delim,int flags);
int winx_patfind(wchar_t *string,winx_patlist *patterns);
int winx_patcmp(wchar_t *string,winx_patlist *patterns);
void winx_patfree(winx_patlist *patterns);

int winx_bytes_to_hr(ULONGLONG bytes, int digits, char *buffer, int length);
ULONGLONG winx_hr_to_bytes(char *string);
void winx_to_utf8(char *dst,int size,wchar_t *src);

/* thread.c */
#ifdef _NTNDK_H_
int winx_create_thread(PTHREAD_START_ROUTINE start_addr,PVOID parameter);
void winx_exit_thread(NTSTATUS status);
#endif

/* time.c */
ULONGLONG winx_str2time(char *string);
int winx_time2str(ULONGLONG time,char *buffer,int size);
ULONGLONG winx_xtime(void);
#define winx_xtime_nsec() (winx_xtime() * 1000 * 1000)

int winx_get_system_time(winx_time *t);
int winx_get_local_time(winx_time *t);

/* volume.c */
int winx_get_drive_type(char letter);

#ifndef _NTNDK_H_
#pragma pack(push, 1)
typedef struct _NTFS_DATA {
    LARGE_INTEGER VolumeSerialNumber;
    LARGE_INTEGER NumberSectors;
    LARGE_INTEGER TotalClusters;
    LARGE_INTEGER FreeClusters;
    LARGE_INTEGER TotalReserved;
    ULONG BytesPerSector;
    ULONG BytesPerCluster;
    ULONG BytesPerFileRecordSegment;
    ULONG ClustersPerFileRecordSegment;
    LARGE_INTEGER MftValidDataLength;
    LARGE_INTEGER MftStartLcn;
    LARGE_INTEGER Mft2StartLcn;
    LARGE_INTEGER MftZoneStart;
    LARGE_INTEGER MftZoneEnd;
} NTFS_DATA, *PNTFS_DATA;
#pragma pack(pop)
#endif /* !_NTNDK_H_ */

int winx_get_volume_information(char volume_letter,winx_volume_information *v);
WINX_FILE *winx_vopen(char volume_letter);
int winx_vflush(char volume_letter);

typedef int (*volume_region_callback)(winx_volume_region *reg,void *user_defined_data);

winx_volume_region *winx_get_free_volume_regions(char volume_letter,
        int flags,volume_region_callback cb,void *user_defined_data);
winx_volume_region *winx_add_volume_region(winx_volume_region *rlist,
        ULONGLONG lcn,ULONGLONG length);
winx_volume_region *winx_sub_volume_region(winx_volume_region *rlist,
        ULONGLONG lcn,ULONGLONG length);
void winx_release_free_volume_regions(winx_volume_region *rlist);

/* zenwinx.c */
int winx_init_library(void);
void winx_unload_library(void);
void winx_exit(int exit_code);
void winx_reboot(void);
void winx_shutdown(void);

/* prb.h - Red-black binary trees with parent pointers */
#include "prb.h"

//genBTC time functions @ time.c
int winx_filetime2winxtime(ULONGLONG input,winx_time *output);
//cant access TIME_FIELDS from ntndk here to declare them. skip.
int winx_filetime2timefields(ULONGLONG input, TIME_FIELDS *output);
int winx_timefields2winxtime(TIME_FIELDS input, winx_time *output);

//utf8.c best modern conversion string functions (added by genBTC)
//Reason:
    //https://www.youtube.com/watch?v=ysh2B6ZgNXk
    // (“Unicode Strings: Why the Implementation Matters”)
//Source: "utf8.c" is part of the FreeDOS-32 LibFAT Unicode Support Library
int unicode_wchar_to_utf8(char *s, wchar_t wc, size_t size);
int unicode_utf8_to_wchar(wchar_t * result, const char * string, size_t size);
int unicode_utf8_len(char lead_byte);

//dont remove this, it helps with exports.
#if defined(__cplusplus)
}
#endif

#endif /* _ZENWINX_H_ */
