--------------------------------------------------------------------------------
-- UltraDefrag GUI Configuration file
-- This file is written in Lua programming language http://www.lua.org/
--
-- To use Unicode characters in filters and other strings, edit this file
-- in Notepad++ editor (http://www.notepad-plus-plus.org/) and save it in
-- UTF-8 (without BOM) encoding.
--
-- This file is also used to configure the Explorer's context menu handler.
-- See the IV-th section for details on how to set custom options for it.
--------------------------------------------------------------------------------

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- I. Filters

-- The main goal of defragmentation is disk access speedup. However, while
-- some fragmented files decrease its performance, another may be left as they
-- are without any noticeable system performance degradation.

-- To filter out files which take no effect on system performance, UltraDefrag
-- supports a few flexible filters.

-- in_filter and ex_filter can be used to exclude files by their paths.
-- For example, to defragment all mp3 files on disk except of these locating
-- inside temporary folders, the following pair of filters may be used:

-- in_filter = "*.mp3"
-- ex_filter = "*temp*;*tmp*"

-- Both filters support '?' and '*' wildcards. '?' character matches any one
-- character, '*' - any zero or more characters. For example, "file.mp?" matches
-- both file.mp3 and file.mp4 while "*.mp3" matches all files with mp3 extension.

-- *.* pattern matches any file with an extension. An asterisk alone (*) matches
-- anything (with or without an extension).

-- Note that you must type either full paths, or substitute the beginning of the
-- path by '*' wildcard. For example, to defragment all the books of a famous Scottish
-- novelist use "C:\\Books\\Arthur Conan Doyle\\*" or simply "*\\Arthur Conan Doyle\\*"
-- string.

-- Note that paths must be typed with double back slashes instead of the single
-- ones. For example, "C:\\MyDocs\\Music\\mp3\\Red_Hot_Chili_Peppers\\*"

-- Empty strings ("") turn off the filters.

-- Note that files marked as temporary by system are always excluded regardless
-- of filters, since these files usually take no effect on system performance.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

in_filter = ""
ex_filter = "*\\BAD\\*;*system volume information*;*temp*;*tmp*;*recycle*;*dllcache*;*ServicePackFiles*"

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- The following ..._patterns variables allow to define groups of patterns
-- for inclusion in the in/ex_filter variables.

-- For more file extensions see http://www.fileinfo.com/
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

archive_patterns = "*.7z;*.7z.*;*.arj;*.bz2;*.bzip2;*.cab;*.cpio;*.deb;*.dmg;*.gz;*.gzip;*.lha;*.lzh;*.lzma;*.rar;*.rpm;*.swm;*.tar;*.taz;*.tbz;*.tbz2;*.tgz;*.tpz;*.txz;*.xar;*.xz;*.z;*.zip"
audio_patterns = "*.aif;*.cda;*.flac;*.iff;*.kpl;*.m3u;*.m4a;*.mid;*.mp3;*.mpa;*.ra;*.wav;*.wma"
disk_image_patterns = "*.fat;*.hdd;*.hfs;*.img;*.iso;*.ntfs;*.squashfs;*.vdi;*.vhd;*.vmdk;*.wim;*.mrimg;*.spf;*.spi;*.pbd"
video_patterns = "*.3g2;*.3gp;*.asf;*.asx;*.avi;*.flv;*.mkv;*.mov;*.mp4;*.mpg;*.rm;*.srt;*.swf;*.vob;*.wmv;*.ts"

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- The following flag variables allow to specify if the pattern groups
-- defined above are to be added to the in_filter or ex_filter.

-- To add the group defined by archive_patterns to in_filter,
-- you set include_archive to 1.
-- Example: include_archive = 1

-- To add the group defined by archive_patterns to ex_filter,
-- you set exclude_archive to 1.
-- Example: exclude_archive = 1
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

include_archive = 0
exclude_archive = 0

include_audio = 0
exclude_audio = 0

include_disk_image = 0
exclude_disk_image = 0

include_video = 0
exclude_video = 0

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- The fragment size threshold filter allows to eliminate a little fragments
-- only, in other words, only fragments affecting the system performance.

-- Big fragments take no effect on the performance, because Windows needs
-- more time to read them from disk anyway and this time is over a time needed
-- to go from one fragment to another.

-- This filter is intended to avoid unnecessary data moves. It speeds up
-- the disk processing.

-- The default value is "20 Mb". Both zero value and empty string ("")
-- turn off the filter.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

fragment_size_threshold = "100 Mb"

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- The file_size_threshold filter allows to exclude big files from
-- the defragmentation. For example, when you watch a movie, it takes
-- usually 1-2 hours while time needed to move drive's head from one
-- fragment to another is about a few seconds. Therefore, you'll see
-- no difference between fragmented and not fragmented movie file.
-- By setting the file_size_threshold filter, overall disk
-- defragmentation time can be highly decreased.

-- To exclude all files greater than 100 Mb, set:

-- file_size_threshold = "100 Mb"
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

file_size_threshold = ""

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- The optimizer_file_size_threshold parameter is used to tune the disk
-- optimization. All files bigger than specified will be skipped, while
-- smaller files will be sorted out on disk by their paths to speedup
-- sequential access.

-- The default value is "20 Mb". Both zero value and empty string ("")
-- forces to use the default value, since otherwise the disk optimization
-- becomes not efficient.

-- It is not recommended to increase this parameter too much. UltraDefrag
-- uses simple algorithms (to reach highest reliability) and they require
-- larger continuous free space gaps in order to sort out bigger files.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

optimizer_file_size_threshold = "20 Mb"

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- The fragments_threshold filter allows to exclude files which have low
-- number of fragments. For example, to exclude everything with less than
-- 5 fragments, set:

-- fragments_threshold = 5

-- Both zero value and empty string ("") turn off the filter.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

fragments_threshold = 0

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- The fragmentation_threshold filter allows to avoid the disk processing
-- when the disk fragmentation level is below than specified. For example,
-- to avoid defragmentation/optimization of disks with fragmentation level
-- below 10 percents, set:

-- fragmentation_threshold = 10

-- The default value is zero (0), so all the disks are processed
-- regardless of their fragmentation level.

-- Note that this filter does not affect the MFT optimization task.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

fragmentation_threshold = 0

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- II. Miscellaneous options
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- When the specified time interval elapses the job will be terminated
-- automatically. For example, to terminate processing after 6 hours and
-- 30 minutes, set:

-- time_limit = "6h 30m"

-- Both zero value and empty string ("") turn off this option.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

time_limit = ""

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Progress refresh interval, in milliseconds. The default value is 100.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

refresh_interval = 100

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Set it to 1 (one) to disable generation of the file fragmentation reports.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

disable_reports = 0

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Set dbgprint_level to DETAILED for reporting a bug,
-- for normal operation set it to NORMAL or an empty string.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

dbgprint_level = ""

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Set log_file_path to the path and file name of the log file to be created,
-- for normal operation set it to an empty string ("").
-- For example:
--     log_file_path = "C:\\Windows\\UltraDefrag\\Logs\\ultradefrag.log"
--
--  Same as above, but uses relative path
--    log_file_path = ".\\Logs\\ultradefrag.log"
--
-- Example using environment variable:
--  Uses the temporary directory of the executing user
--    log_file_path = os.getenv("TEMP") .. "\\UltraDefrag_Logs\\ultradefrag.log"
--
-- Unicode characters cannot be included in log file paths.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

log_file_path = ".\\logs\\ultradefrag.log"

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Set dry_run parameter to 1 for defragmentation algorithm testing;
-- no actual data moves will be performed on disk in this case.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

dry_run = 0

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Seconds_for_shutdown_rejection sets the delay for the user to cancel
-- the hibernate, logoff, reboot or shutdown execution, default is 60 seconds.
-- If set to 0 (zero) the confirmation dialog will not be displayed.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

seconds_for_shutdown_rejection = 60

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Set show_menu_icons parameter to 0 if menu icons look untidy on your system.
-- Note: restart the program after this parameter adjustment.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

show_menu_icons = 1

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Set show_taskbar_icon_overlay parameter to 1 to show the taskbar icon
-- overlay indicating that the job is running on Windows 7 and more recent
-- Windows editions.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

show_taskbar_icon_overlay = 1

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Set show_progress_in_taskbar parameter to 1 to enable the progress indication
-- inside of the taskbar button on Windows 7 and more recent Windows editions.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

show_progress_in_taskbar = 1

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Set minimize_to_system_tray parameter to 1 to minimize the application's
-- window to the taskbar notification area (system tray).
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

minimize_to_system_tray = 0

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- III. Cluster map options

-- map_block_size controls the size of the block, in pixels; default value is 4.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

map_block_size = 4

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Grid line width, in pixels; default value is 1.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

grid_line_width = 1

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Grid line color, in RGB format; default value is (0;0;0),
-- all color components should be in range 0-255,
-- (0;0;0) means black; (255;255;255) - white.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

grid_color_r = 0
grid_color_g = 0
grid_color_b = 0

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Background color, in RGB format; default value is (255;255;255),
-- all color components should be in range 0-255,
-- (0;0;0) means black; (255;255;255) - white.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

free_color_r = 255
free_color_g = 255
free_color_b = 255

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- IV. Explorer's context menu handler configuration

-- To use custom configuration for the Explorer's context menu handler,
-- put custom definitions into the code block below.

-- For example, to defragment mp3 files only through the Explorer's
-- context menu add the following line:
-- in_filter = "*.mp3"
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

if shellex_flag then
    -- put custom options here 
    log_file_path = ".\\logs\\ultradefrag.log"
end

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- V. Notes
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Instead of obsolete disable_latest_version_check parameter
-- use Help -> Upgrade menu.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- Instead of obsolete restore_default_window_size parameter use
-- remove width and height parameters from gui.ini file to restore
-- default window size on the next startup.
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- this number helps to upgrade configuration file correctly, don't change it
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

version = 200

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- this code concatenates the filter variables, don't modify it
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

orig_ex_filter = ex_filter  -- for faster upgrade
if exclude_archive ~= 0 then ex_filter = ex_filter .. ";" .. archive_patterns end
if exclude_audio ~= 0 then ex_filter = ex_filter .. ";" .. audio_patterns end
if exclude_disk_image ~= 0 then ex_filter = ex_filter .. ";" .. disk_image_patterns end
if exclude_video ~= 0 then ex_filter = ex_filter .. ";" .. video_patterns end

orig_in_filter = in_filter  -- for faster upgrade
if include_archive ~= 0 then in_filter = in_filter .. ";" .. archive_patterns end
if include_audio ~= 0 then in_filter = in_filter .. ";" .. audio_patterns end
if include_disk_image ~= 0 then in_filter = in_filter .. ";" .. disk_image_patterns end
if include_video ~= 0 then in_filter = in_filter .. ";" .. video_patterns end

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- this code initializes the environment for UltraDefrag, don't modify it
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- common options
os.setenv("UD_IN_FILTER",in_filter)
os.setenv("UD_EX_FILTER",ex_filter)
os.setenv("UD_FRAGMENT_SIZE_THRESHOLD",fragment_size_threshold)
os.setenv("UD_FILE_SIZE_THRESHOLD",file_size_threshold)
os.setenv("UD_OPTIMIZER_FILE_SIZE_THRESHOLD",optimizer_file_size_threshold)
os.setenv("UD_FRAGMENTS_THRESHOLD",fragments_threshold)
os.setenv("UD_FRAGMENTATION_THRESHOLD",fragmentation_threshold)
os.setenv("UD_TIME_LIMIT",time_limit)
os.setenv("UD_REFRESH_INTERVAL",refresh_interval)
os.setenv("UD_DISABLE_REPORTS",disable_reports)
os.setenv("UD_DBGPRINT_LEVEL",dbgprint_level)
os.setenv("UD_LOG_FILE_PATH",log_file_path)
os.setenv("UD_DRY_RUN",dry_run)

-- GUI specific options
os.setenv("UD_SECONDS_FOR_SHUTDOWN_REJECTION",seconds_for_shutdown_rejection)
os.setenv("UD_SHOW_MENU_ICONS",show_menu_icons)
os.setenv("UD_SHOW_TASKBAR_ICON_OVERLAY",show_taskbar_icon_overlay)
os.setenv("UD_SHOW_PROGRESS_IN_TASKBAR",show_progress_in_taskbar)
os.setenv("UD_MINIMIZE_TO_SYSTEM_TRAY",minimize_to_system_tray)
os.setenv("UD_MAP_BLOCK_SIZE",map_block_size)
os.setenv("UD_GRID_LINE_WIDTH",grid_line_width)
os.setenv("UD_GRID_COLOR_R",grid_color_r)
os.setenv("UD_GRID_COLOR_G",grid_color_g)
os.setenv("UD_GRID_COLOR_B",grid_color_b)
os.setenv("UD_FREE_COLOR_R",free_color_r)
os.setenv("UD_FREE_COLOR_G",free_color_g)
os.setenv("UD_FREE_COLOR_B",free_color_b)

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- END OF FILE
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
