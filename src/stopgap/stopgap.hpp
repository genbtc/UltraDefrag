#pragma once

#ifndef _STOPGAP_HPP
#define _STOPGAP_HPP

#define MAXIMUM_GAP_SIZE_DEFAULT 102400 //in KB:

struct Options {
    size_t maxSize;
    int verbose;
    char volume;
    bool aggressive;
    bool gaps;
    bool defrag;
    bool widen;

    Options()
        : maxSize(MAXIMUM_GAP_SIZE_DEFAULT), verbose(true), volume('\0'), aggressive(false), gaps(true),
        defrag(false), widen(false) {
    }

    void parseCmdLine(int argc, wchar_t **argv);

    void Reset()
    {
        this->aggressive = false;
        this->defrag = false;
        this->gaps = false;
        this->maxSize = MAXIMUM_GAP_SIZE_DEFAULT;
        this->verbose = true;
        this->volume = 'A';
        this->widen = true;
    }
};
extern "C" {
    #include "../dll/zenwinx/ntndk.h"
    #include "../dll/zenwinx/zenwinx.h"
}


void stopgap_init_run();
void stopgap_move_file(Options opts, winx_file_info *f, const winx_volume_region *g);
void stopgap_move_set(winx_volume_region &r);
void stopgap_widen_behind(const winx_volume_region *g, size_t maxMoves);
void stopgap_close_gaps(char driveLetter);
std::wstring stopgap_enumerate_gaps(char driveLetter);
void stopgap_defrag(char driveLetter);
//Guicon.cpp
void RedirectIOToConsole();
size_t stopgap_count_gaps(char driveLetter);
std::wstring stopgap_findfiles_at_LCN(char driveLetter, ULONGLONG LCN, ULONGLONG length);
#endif // _STOPGAP_HPP