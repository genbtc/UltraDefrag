/*
 *  UltraDefrag - a powerful defragmentation tool for Windows NT.
 *  Copyright (c) 2007-2013 Dmitri Arkhangelski (dmitriar@gmail.com).
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
 *
 * Ideas by Stefan Pendl <stefanpe@users.sourceforge.net>
 * and Dmitri Arkhangelski <dmitriar@gmail.com>.
*/
/* This file only was totally created by genBTC <genBTC@gmx.com> */

/**
 * @file query.c
 * @brief Backend Passthrough to Query the internals and provide
 * results back to the GUI half, query.cpp.
 * @addtogroup Query
 * @{
 * @todo
//possibly think about combining query and job. This whole splitting them apart thing is horrible at the moment.
// my hope is that it all becomes worth it eventually, as to not confuse the two functions and query should be a lot simpler and different.
 * might want to have an "Query Mode" where the analysis/lists don't get destroyed, and instead are stored in RAM

//should invent a new query structure that the GUI can figure out. did. (query_parameters)
//  this involves making the GUI aware of the already existing structures.
 */

#include "udefrag-internals.h"
//#include "query.h"
BOOL is_gui_query_finished = FALSE;      //related to gui_fileslist_finished()
BOOL wait_delete_thread_query_finished = FALSE; //related to wait_delete_lists_thread()
static int killer(void *p)
{
    winx_dbg_print_header(0,0,I"*");
    winx_dbg_print_header(0x20,0,I"termination requested by caller");
    winx_dbg_print_header(0,0,I"*");
    gui_query_finished();//must be called.
    return 1;
}

/**
 */
static int terminator(void *p)
{
    udefrag_job_parameters *jp = (udefrag_job_parameters *)p;
    int result;

    /* ask caller */
    if(jp->t){
        result = jp->t(jp->p);
        if(result)
            killer(jp->p);
        return result;
    }

    /* continue */
    return 0;
}
static DWORD WINAPI wait_delete_lists_thread(LPVOID p)
{
    while(!is_gui_query_finished)
        winx_sleep(250);
    destroy_lists((udefrag_job_parameters *)p);
    wait_delete_thread_query_finished = TRUE;
    winx_exit_thread(0);
    return 0;
}

void gui_query_finished(void){
    is_gui_query_finished = TRUE;
}

static DWORD WINAPI start_query(LPVOID p)
{
    udefrag_job_parameters *jp = (udefrag_job_parameters *)p;
    int result = 0;
    //analyze.
    result = analyze(jp);
    
//query switch/case
    switch(jp->job_type){
    case QUERY_GET_VCNLIST:
        result = query_get_VCNlist(jp);
        break;
    case QUERY_GET_FREE_REGIONS:
        result = query_get_freeRegions(jp);
        break;
    default:
        result = 0;
        break;
    }
    dtrace("Finished Executing Specific Query.");
    //cleanup.
    destroy_file_blocks_tree(jp);
    //(void)save_fragmentation_report(jp);      //dont need.

    /* now it is safe to adjust the completion status */
    jp->pi.completion_status = result;
    if(jp->pi.completion_status == 0)
        jp->pi.completion_status ++; /* success */
        
    winx_exit_thread(0); /* 8k/12k memory leak here?   ???whocares... */
    return 0;
}

/************************************************************/
/*                    The query.c code                      */
/************************************************************/


int udefrag_start_query(char volume_letter,udefrag_query_type query_type,int flags,int cluster_map_size,
        udefrag_progress_callback cb,udefrag_terminator t,udefrag_query_parameters *qp,void *p)
{
    udefrag_job_parameters jp;
    int result = -1;

    /* initialize the job */
    memset(&jp,0,sizeof(udefrag_job_parameters));
    jp.win_version = winx_get_os_version();

    /* print the header */
    dbg_print_header(&jp);

    jp.volume_letter = winx_toupper(volume_letter);    /* convert volume letter to uppercase */
    jp.filelist = NULL;
    jp.fragmented_files = NULL;
    jp.free_regions = NULL;
    jp.progress_refresh_time = 0;
    jp.job_type = query_type;
    jp.cb = cb;
    jp.t = t;
    jp.p = p;
    jp.qp = qp;        //establish the Query Parameters variable.

    jp.termination_router = terminator;

    jp.start_time = jp.p_counters.overall_time = winx_xtime();
    jp.pi.completion_status = 0;

    if(get_options(&jp) < 0)
        goto done;

    jp.udo.job_flags = flags;

    if(allocate_map(cluster_map_size,&jp) < 0){
        release_options(&jp);
        goto done;
    }

    /* set additional privileges for Vista and above */
    if(jp.win_version >= WINDOWS_VISTA){
        (void)winx_enable_privilege(SE_BACKUP_PRIVILEGE);

        if(jp.win_version >= WINDOWS_7)
            (void)winx_enable_privilege(SE_MANAGE_VOLUME_PRIVILEGE);
    }

    if(winx_create_thread(start_query,(PVOID)&jp) < 0){
        etrace("The Query Failed for some reason.");
        goto done;
    }
    do {
        winx_sleep(jp.udo.refresh_interval);
        deliver_progress_info(&jp,0); /* status = running */
    } while(jp.pi.completion_status == 0);

    /* cleanup */
    dtrace("Cleanup in progress.");
    deliver_progress_info(&jp,jp.pi.completion_status);
    free_map(&jp);
    release_options(&jp);
done:
    jp.p_counters.overall_time = winx_xtime() - jp.p_counters.overall_time;
    dbg_print_performance_counters(&jp);
    dbg_print_footer(&jp);

    /* cleanup */
    winx_flush_dbg_log(0);
    
    result = jp.pi.completion_status;

    //Code to handle the filelists deletion after GUI finishes.
    // Needs to wait until it gets a response. If this thread exits too soon
    // without clearing the memory, deletion of lists will fail/break/segfault.
    // This has to be kept in mind in the terminator also, to reset variables.
    if (result > 0){
        winx_create_thread(wait_delete_lists_thread,(PVOID)&jp);
        while(!is_gui_query_finished || !wait_delete_thread_query_finished)
            winx_sleep(333);
        is_gui_query_finished = FALSE;
        wait_delete_thread_query_finished = FALSE;
    }
    else
        destroy_lists(&jp);
    
    if(result < 0) return result;
    return (result > 0) ? 0 : (-1);
}

/************************************************************/
/*              Query #1                                    */
/************************************************************/
int query_get_VCNlist(udefrag_job_parameters *jp)
{
    winx_file_info *file;
    wchar_t *native_path;
    
    //dtrace("Path was: %ws",jp->qp->path);
    convert_path_to_native(jp->qp->path,&native_path);
    
    /* iterate through the filelist (no other way) */
    for(file = jp->filelist; file; file = file->next){
        if(_wcsicmp(file->path,native_path) == 0) break;
        if(file->next == jp->filelist){
            etrace("Abnormal error. Could not match path to any scanned file...");
            return (-1);
        }
    }
    jp->qp->filedisp = file->disp;
    //memcpy(&jp->qp->filedisp,&file->disp,sizeof(winx_file_disposition));
    dtrace("The VCNList Query itself has been Completed. Throwing back to job..");
    
    /*cleanup*/
    winx_free(file);
    winx_free(native_path);
    clear_currently_excluded_flag(jp); //again?
    winx_fclose(jp->fVolume);
    jp->fVolume = NULL;
    return 0;
}

/************************************************************/
/*               Query #2                                   */
/************************************************************/
int query_get_freeRegions(udefrag_job_parameters *jp)
{
    
    return 0;
}
