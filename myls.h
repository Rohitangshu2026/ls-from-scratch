#ifndef MYLS_H
#define MYLS_H

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string.h>
#include<limits.h>

#define MAX_FILES 2000
/*
 * options_t
 * ---------
 * Stores command-line flags that control ls behavior.
 *
 * Fields:
 *   show_all  (-a): include entries whose names begin with '.'
 *   sort_time (-t): sort entries by modification time
 */
typedef struct{
    bool show_all;  // -a
    bool sort_time; // -t
}options_t;

/*
 * file_info_t
 * -----------
 * Represents metadata for a single filesystem entry (file or directory)
 * collected during directory traversal.
 *
 * Fields:
 *   name   - null-terminated path or entry name (up to PATH_MAX)
 *   sec    - modification time in seconds since the Epoch
 *   nsec   - nanosecond component of modification time
 *   is_dir - indicates whether the entry is a directory
 *
 * Usage:
 *   - Used to store per-entry metadata required for sorting and display
 *   - Enables time-based sorting (-t) with nanosecond precision
 */
typedef struct{
    char name[PATH_MAX];
    long sec;   // st_mtim.tv_sec
    long nsec;  // st_mtim.tv_nsec
    bool is_dir;
}file_info_t;

/*
 * file_list_t
 * -----------
 * Container for a collection of filesystem entries and their count.
 *
 * Fields:
 *   files - fixed-size array of file_info_t structures representing
 *           files or directories collected during traversal
 *   count - number of valid entries currently stored in the array
 *
 * Usage:
 *   - Used to accumulate directory contents before sorting and printing
 *   - Provides a simple abstraction over a raw array of file_info_t
 */
typedef struct{
    file_info_t files[MAX_FILES];
    int count;
}file_list_t;

options_t parse_options(int argc, char** argv);
int gather_paths(int argc,char** argv,char** non_dirs,int* non_dir_count,char** dirs,int* dir_count);
void sort_entries(char** entries,int count);
file_list_t read_directory(const char* path, bool show_hidden);
void sort_file_list(file_list_t *flist, bool sort_time);
static int cmp_file_time(const void *a, const void *b);
static int cmp_file_lex(const void *a, const void *b);


#endif