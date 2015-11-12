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
 //has been moved to udefrag.c since some static functions are
// not defined in any .h and could not be called from here.

//possibly think about combining query and job.

//might want to have an "Query Mode" where the analysis/lists don't get destroyed, and instead are stored in RAM

//should invent a new query structure that the GUI can figure out.
//  this involves making the GUI aware of the already existing structures.
//  ie: udefrag-internals_flags.h would have not only flags but actual structs.
 */

#include "udefrag-internals.h"
#include "query.h"

/************************************************************/
/*                       Query #1                           */
/************************************************************/

void query_get_VCNlist(udefrag_job_parameters *jp,winx_file_info *file){
    //return file->disp.blockmap;
    winx_blockmap *block;
    ULONGLONG i;
    
    itrace("After: The File has #fragments: %d",file->disp.fragments);
    //file->disp.blockmap->next

    for(block = file->disp.blockmap, i = 0; block; block = block->next, i++){
        itrace("file part #%I64u start: %I64u, length: %I64u",i,block->lcn,block->length);
        if(block->next == file->disp.blockmap) break;
    }
}

