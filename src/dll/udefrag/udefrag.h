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

/*
* udefrag.dll interface header.
*/

#ifndef _UDEFRAG_H_
#define _UDEFRAG_H_

#include "udefrag-internals.h"

#if defined(__cplusplus)
extern "C" {
#endif

int udefrag_init_library(void);
void udefrag_unload_library(void);

volume_info *udefrag_get_vollist(int skip_removable);
void udefrag_release_vollist(volume_info *v);
int udefrag_validate_volume(char volume_letter,int skip_removable);
int udefrag_get_volume_information(char volume_letter,volume_info *v);

//TODO: Duplicated:
    //callbacks. (for udefrag_start_job)
typedef void  (*udefrag_progress_callback)(udefrag_progress_info *pi, void *p);
typedef int   (*udefrag_terminator)(void *p);

int udefrag_start_job(char volume_letter,udefrag_job_type job_type,int flags,
    int cluster_map_size,udefrag_progress_callback cb,udefrag_terminator t,void *p);

char *udefrag_get_results(udefrag_progress_info *pi);
void udefrag_release_results(char *results);

char *udefrag_get_error_description(int error_code);

int udefrag_set_log_file_path(void);

int convert_path_to_native(wchar_t *path, wchar_t **native_path);

// Helps extern/export defs, dont remove:
#if defined(__cplusplus)
}
#endif

#endif /* _UDEFRAG_H_ */
