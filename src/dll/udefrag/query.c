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

/************************************************************/
/*                    The query.c code                      */
/************************************************************/

#include "udefrag-internals.h"

static int query_killer(void *p)
{
    //udefrag_job_parameters *jp = (udefrag_job_parameters *)p;
    winx_dbg_print_header(0,0,I"*");
    winx_dbg_print_header(0x20,0,I"termination requested by caller");
    winx_dbg_print_header(0,0,I"*");
    return 1;
}

static int query_terminator(void *p)
{
    udefrag_job_parameters *jp = (udefrag_job_parameters *)p;
    int result;

    /* ask caller */
    if(jp->t){
        result = jp->t(jp->p);
        if(result)
            query_killer(jp->p);
        return result;
    }

    /* continue */
    return 0;
}

BOOL isGUIqueryFinishedBoolean = FALSE;
void gui_query_finished(void)
{
    isGUIqueryFinishedBoolean = TRUE;
    ////do nothing; save this function for later.
}
static DWORD WINAPI query_wait_delete_lists_thread(LPVOID p)
{
/*    udefrag_job_parameters *jp = (udefrag_job_parameters *)p;
    int delwaitcount;
    if (jp == NULL)
    {
        etrace("Error. Trying to clear a list thats already gone. JP was null.");
        return 1;
    }
    if (&jp->qp == NULL)
    {
        etrace("Error. Trying to clear a list thats already gone. &jp->qp was null.");
        return 1;
    }
    if (jp->qp.engineFinished == TRUE) return 1;
    delwaitcount = 0;
    //wait for both threads to come back and say they're done.
    do {
        dtrace("---Sleeping 150ms waiting for deletion to complete. Wait Count = %d", delwaitcount);
        delwaitcount++;
        winx_sleep(150);
    } while (!isGUIqueryFinishedBoolean);

    destroy_lists(jp);
    jp->qp.engineFinished = TRUE;

    winx_exit_thread(0);
    return 0;*/
}


static DWORD WINAPI query_starter(LPVOID p)
{
    udefrag_job_parameters *jp = (udefrag_job_parameters *)p;
    int result = -1;
    //analyze.
    (void)winx_vflush(jp->volume_letter); /* flush all file buffers */
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
        break;
    }
    dtrace("Finished Executing Specific Query.");

    /* now it is safe to adjust the completion status */
    jp->pi.completion_status = result;
    if(jp->pi.completion_status == 0)
        jp->pi.completion_status ++; /* success */
        
    winx_exit_thread(0); /* 8k/12k memory leak here?   ???and... */
    return 0;
}

int udefrag_starts_query(char volume_letter,udefrag_query_type query_type,int flags,int cluster_map_size,
        udefrag_query_progress_callback qpcb,udefrag_terminator t,udefrag_query_parameters qp,void *p)
{
    udefrag_job_parameters jp;
    //udefrag_query_parameters qp_pass;
	int result, delwaitcount;// , callback;

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
    jp.job_type = (udefrag_job_type)query_type;
    //jp.cb = cb;
    jp.qpcb = qpcb;
    jp.t = t;
    jp.p = p;
    isGUIqueryFinishedBoolean = FALSE;
    jp.qp = qp; //establish the Query Parameters variable.

    jp.termination_router = query_terminator;

    jp.start_time = jp.p_counters.overall_time = winx_xtime();
    jp.pi.completion_status = 0;

    if(get_options(&jp) < 0)
        goto done;

    jp.udo.job_flags = flags;

    /* set additional privileges for Vista and above */
    if(jp.win_version >= WINDOWS_VISTA){
        (void)winx_enable_privilege(SE_BACKUP_PRIVILEGE);

        if(jp.win_version >= WINDOWS_7)
            (void)winx_enable_privilege(SE_MANAGE_VOLUME_PRIVILEGE);
    }

    //Create and manage the start_query Thread.
    if(winx_create_thread(query_starter,(LPVOID)&jp) < 0){
        etrace("The query Failed to start for some reason.");
        goto done;
    }
    do {
        winx_sleep(jp.udo.refresh_interval);
    } while(jp.pi.completion_status == 0);

    jp.qpcb(&jp.qp, &jp.p);

    dtrace("Cleanup in progress.");
    //destroy_file_blocks_tree(&jp);
    //free_map(&jp);
    release_options(&jp);

done:
    jp.p_counters.overall_time = winx_xtime() - jp.p_counters.overall_time;
    dbg_print_performance_counters(&jp);
    dbg_print_footer(&jp);
    winx_flush_dbg_log(0);

    /* cleanup */
    result = jp.pi.completion_status;
    delwaitcount = 0;
    do {
        dtrace("---Sleeping 150ms waiting for deletion to complete. Wait Count = %d", delwaitcount);
        delwaitcount++;
        winx_sleep(150);
    } while (!isGUIqueryFinishedBoolean);

    destroy_lists(&jp);
    jp.qp.engineFinished = TRUE;
    if(result < 0) return result;
    return result > 0 ? 0 : -1;
}


/************************************************************/
/*              Query #1                                    */
/************************************************************/
int query_get_VCNlist(udefrag_job_parameters *jp)
{
    winx_file_info *file;
    wchar_t *native_path;
    
    //dtrace("Path was: %ws",jp->qp->path);
    convert_path_to_native(jp->qp.path,&native_path);
    
    /* iterate through the filelist (no other way) */
    for(file = jp->filelist; file != NULL; file = file->next){
        if(_wcsicmp(file->path,native_path) == 0) break;
        if(file->next == jp->filelist){
            etrace("Abnormal error. Could not match path to any scanned file...");
            return -1;
        }
    }
    jp->qp.filedisp = file->disp;
    dtrace("The VCNList Query has been stored in the qp. Throwing back to QueryThread to display it..");
/*    winx_scan_disk_release(jp->filelist);
    winx_release_free_volume_regions(jp->free_regions);
    if(jp->fragmented_files) 
        prb_destroy(jp->fragmented_files,NULL);

    jp->qp.engineFinished = TRUE;    */
    /*cleanup*/
    //winx_free(file);
    winx_free(native_path);
    clear_currently_excluded_flag(jp); //again?
    winx_fclose(jp->fVolume);
    jp->fVolume = NULL;
    return 0;
}

/************************************************************/
/*               Query #2 (free space)                      */
/************************************************************/
int query_get_freeRegions(udefrag_job_parameters *jp)
{
/*
 * @param[in] jp job parameters structure.
 * @param[in] min_lcn minimum LCN of region.
 * @param[in] min_length minimum length of region, in clusters.
 * @param[out] max_length length of the biggest region found.
 * @note In case of termination request returns NULL immediately.
 */    
    //find_first_free_region()
    //find blocking clusters
    //find next free region
    //if they are larger, move the blockages out of that area
    //find_last_free_region()
    //compare ?
    
    return 0;
}
