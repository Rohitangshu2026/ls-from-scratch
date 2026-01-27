/*
1. Parse arguments -> -a, -t
2. for directories ls -> ls dir1
3. sort everything alphabetically or by time
4. print
5. free
*/

#include "myls.h"

int main(int argc, char** argv){
    // 1. Parse options
    options_t opts = parse_options(argc,argv);
    printf("Options: %d, %d\n",opts.show_all, opts.sort_time);
    
    // store directories and paths
    char* dirs[argc];
    char* non_dirs[argc];
    int dir_count = 0;
    int non_dir_count = 0;
    
    // 2. Gather paths
    int total = gather_paths(argc,argv,non_dirs,&non_dir_count,dirs,&dir_count);
    printf("total: %d\n",total);
    printf("dirs: %s\n",dirs[0]);


    return 0;
}
options_t parse_options(int argc, char** argv){

    options_t opts;
    opts.show_all = false;
    opts.sort_time =false;

    // scan all arguments for flags
    for(int i = 0; i < argc; ++i)
        if(argv[i][0] == '-')
            for(int j = 1; argv[i][j] != '\0'; ++j){
                if(argv[i][j] == 'a')
                    opts.show_all = true;
                else if(argv[i][j] == 't')
                    opts.sort_time = true;
                else{
                    printf("myls: invalid option -- %c\n", argv[i][j]);
                    exit(1);
                }    

            }
    
    return opts;
}

int gather_paths(int argc,char** argv,char** non_dirs,int* non_dir_count,char** dirs,int* dir_count){
    int total = 0;
    *dir_count = 0;
    *non_dir_count = 0;

    // skip argv[0] --> ./myls
    for(int i = 1; i < argc; ++i){
        if(argv[i][0] == '-')
            continue;
        
        // try opening as a directory
        DIR *temp_dir = opendir(argv[i]);
        if(temp_dir){
            // valid directory
            closedir(temp_dir);
            dirs[*dir_count] = argv[i];
            (*dir_count)++;
            total++;
        }
        else{
            // not a directory --> file / invalid
            struct stat st; // stat --> display file or file system status
            if(!lstat(argv[i],&st)){
                // existing file
                non_dirs[*non_dir_count] = argv[i];
                (*non_dir_count)++;
                total++;
            }
            else{
                // invalid
                printf("myls: cannot access -- %s\n",argv[i]);
            }
        }

        // no valid non-options args provided, default
    }
    if(total == 0){
        dirs[0] = ".";
        *dir_count = 1;
        total = 1;
    }
    return total;
}