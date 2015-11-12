-------------------------------------------------------------------------------
-- Ultra Defragmenter report options
-- This file is written in Lua programming language http://www.lua.org/
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Set this parameter to zero to disable HTML reports generation.
-------------------------------------------------------------------------------
produce_html_report = 1

-------------------------------------------------------------------------------
-- Set this parameter to 1 to enable generation of plain text reports.
-------------------------------------------------------------------------------
produce_plain_text_report = 0

-------------------------------------------------------------------------------
-- All the following options were primarily designed to achieve better
-- compatibility with old web browsers.
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Set enable_sorting to zero if your web browser is too old
-- and you have error messages about invalid javascript code.
-------------------------------------------------------------------------------
enable_sorting = 1

-------------------------------------------------------------------------------
-- Set this parameter to 1 if you prefer to look at filenames split
-- into few short lines. If you prefer to use fullscreen mode of your
-- web browser then set this parameter to zero.
-------------------------------------------------------------------------------
split_long_names = 0

-------------------------------------------------------------------------------
-- Set here maximum number of characters per line in filename cells.
-------------------------------------------------------------------------------
max_chars_per_line = 50

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- this number helps to upgrade configuration file correctly, don't change it
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
version = 1

-------------------------------------------------------------------------------
-- The web page style can be set through udreport.css style sheet.
-------------------------------------------------------------------------------
