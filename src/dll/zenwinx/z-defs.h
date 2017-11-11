#pragma once

/*
* The maximum length of the file system type name.
* The default value specified is more than enough.
*/
#define MAX_FS_NAME_LENGTH 31

#define DEFAULT_TAB_WIDTH 2
#define DEFAULT_PAGING_PROMPT_TO_HIT_ANY_KEY "      Hit any key to display next page,\n" \

#define WINDOWS_NT     40
#define WINDOWS_2K     50
#define WINDOWS_XP     51
#define WINDOWS_2K3    52 /* and Server 2003 R2 and XP x64 */
#define WINDOWS_VISTA  60 /* and Server 2008 */
#define WINDOWS_7      61 /* and Server 2008 R2 */
#define WINDOWS_8      62 /* and Server 2012 */
#define WINDOWS_81     63 /* Win 8.1 */
#define WINDOWS_10     100                  "          ESC or Break to abort..."

/* flags for winx_flush_dbg_log */
#define FLUSH_ALREADY_SYNCHRONIZED 0x1 /* for internal use only */
#define FLUSH_IN_OUT_OF_MEMORY     0x2 /* flush in the out of memory condition */

/* dbg.c */
#define DEFAULT_DBG_PRINT_DECORATION_CHAR  '-'
#define DEFAULT_DBG_PRINT_HEADER_WIDTH     64

/* ftw.c */
/* winx_ftw flags */
#define WINX_FTW_RECURSIVE              0x1 /* scan all subdirectories recursively */
#define WINX_FTW_DUMP_FILES             0x2 /* fill winx_file_disposition structures */
#define WINX_FTW_ALLOW_PARTIAL_SCAN     0x4 /* admit partially gathered information */
#define WINX_FTW_SKIP_RESIDENT_STREAMS  0x8 /* skip files of zero length and files located inside MFT */

/* file.c */
#define is_readonly(f)            ((f)->flags & FILE_ATTRIBUTE_READONLY)
#define is_hidden(f)              ((f)->flags & FILE_ATTRIBUTE_HIDDEN)
#define is_system(f)              ((f)->flags & FILE_ATTRIBUTE_SYSTEM)
#define is_directory(f)           ((f)->flags & FILE_ATTRIBUTE_DIRECTORY)
#define is_archive(f)             ((f)->flags & FILE_ATTRIBUTE_ARCHIVE)
#define is_device(f)              ((f)->flags & FILE_ATTRIBUTE_DEVICE)
#define is_normal(f)              ((f)->flags & FILE_ATTRIBUTE_NORMAL)
#define is_temporary(f)           ((f)->flags & FILE_ATTRIBUTE_TEMPORARY)
#define is_sparse(f)              ((f)->flags & FILE_ATTRIBUTE_SPARSE_FILE)
#define is_reparse_point(f)       ((f)->flags & FILE_ATTRIBUTE_REPARSE_POINT)
#define is_compressed(f)          ((f)->flags & FILE_ATTRIBUTE_COMPRESSED)
#define is_offline(f)             ((f)->flags & FILE_ATTRIBUTE_OFFLINE)
#define is_not_content_indexed(f) ((f)->flags & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)
#define is_encrypted(f)           ((f)->flags & FILE_ATTRIBUTE_ENCRYPTED)
#define are_valid_flags(f)        ((f)->flags & FILE_ATTRIBUTE_VALID_FLAGS)
#define are_valid_set_flags(f)    ((f)->flags & FILE_ATTRIBUTE_VALID_SET_FLAGS)
#define is_fragmented(f)          ((f)->disp.fragments > 1)

#define winx_fileno(f) ((f)->hFile)

#define WINX_OPEN_FOR_DUMP       0x1 /* open for FSCTL_GET_RETRIEVAL_POINTERS */
#define WINX_OPEN_FOR_BASIC_INFO 0x2 /* open for NtQueryInformationFile(FILE_BASIC_INFORMATION) */
#define WINX_OPEN_FOR_MOVE       0x4 /* open for FSCTL_MOVE_FILE */

/* flags for winx_heap_alloc */
#define MALLOC_ABORT_ON_FAILURE 0x1

/* privilege.c */
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE  0x3
#define SE_AUDIT_PRIVILEGE               0x15
#define SE_BACKUP_PRIVILEGE              0x11
#define SE_CREATE_PAGEFILE_PRIVILEGE     0x0f
#define SE_CREATE_PERMANENT_PRIVILEGE    0x10
#define SE_CREATE_TOKEN_PRIVILEGE        0x2
#define SE_DEBUG_PRIVILEGE               0x14
#define SE_IMPERSONATE_PRIVILEGE
#define SE_INC_BASE_PRIORITY_PRIVILEGE   0x0e
#define SE_INCREASE_QUOTA_PRIVILEGE      0x5
#define SE_LOAD_DRIVER_PRIVILEGE         0x0a
#define SE_LOCK_MEMORY_PRIVILEGE         0x4
#define SE_MANAGE_VOLUME_PRIVILEGE       0x1c
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE 0x0d
#define SE_RESTORE_PRIVILEGE             0x12
#define SE_SECURITY_PRIVILEGE            0x8
#define SE_SHUTDOWN_PRIVILEGE            0x13
#define SE_SYSTEM_PROFILE_PRIVILEGE      0x0b
#define SE_SYSTEMTIME_PRIVILEGE          0x0c
#define SE_TAKE_OWNERSHIP_PRIVILEGE      0x9
#define SE_TCB_PRIVILEGE                 0x7

/* string.c */
#define WINX_PAT_ICASE  0x1 /* compile patterns for case insensitive search */

/* winx_get_free_volume_regions flags */
#define WINX_GVR_ALLOW_PARTIAL_SCAN  0x1

/* volume.c */
#define DRIVE_ASSIGNED_BY_SUBST_COMMAND 1200