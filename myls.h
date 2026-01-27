#ifndef MYLS_H
#define MYLS_H

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>


// structure to hold command line options (-a, -t)
typedef struct{
    bool show_all;  // -a
    bool sort_time; // -t
}options_t;

options_t parse_options(int argc, char** argv);
int gather_paths(int argc,char** argv,char** non_dirs,int* non_dir_count,char** dirs,int* dir_count);

#endif