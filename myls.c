/*
 * Program Overview
 * ----------------
 * Implements a simplified version of the Unix `ls` command.
 *
 * Execution Flow:
 *   1. Parse command-line options:
 *        - `-a` to include hidden files
 *        - `-t` to sort by modification time
 *
 *   2. Process operands:
 *        - Separate files and directories
 *        - Default to the current directory (`.`) if no operands are provided
 *
 *   3. Sort operands:
 *        - Lexicographically by name (default)
 *        - By modification time when `-t` is specified
 *
 *   4. Display output:
 *        - Print files first, followed by directories
 *        - Format output to match standard `ls` behavior
 *
 *   5. Cleanup:
 *        - Release any acquired resources (directory streams, etc.)
 */

#include "myls.h"

int main(int argc, char** argv){
    // parse options
    options_t opts = parse_options(argc,argv);
    
    // store directories and paths
    char* dirs[argc];
    char* non_dirs[argc];
    memset(dirs,0,sizeof(dirs));
    memset(non_dirs,0,sizeof(non_dirs));
    int dir_count = 0;
    int non_dir_count = 0;
    
    // gather paths
    gather_paths(argc,argv,non_dirs,&non_dir_count,dirs,&dir_count);

    // sort non-directories lexicographically
    if(non_dir_count > 1)
        sort_entries(non_dirs, non_dir_count);

    // sort directories lexicographically
    if(dir_count > 1)
        sort_entries(dirs, dir_count);

    // print non directories
    for(int i = 0; i < non_dir_count; ++i){
        printf("%s\n",non_dirs[i]);
    }

    if(non_dir_count > 0 && dir_count > 0)
        printf("\n");

    // for each directory read, sort, print
    for(int i = 0; i < dir_count; ++i){
        if(dir_count > 1){
            printf("%s:\n",dirs[i]);
        }

        // read the directory
        file_list_t flist = read_directory(dirs[i],opts.show_all);
        sort_file_list(&flist, opts.sort_time);
        for (int j = 0; j < flist.count; ++j)
            printf("%s\n", flist.files[j].name);

        if (i < dir_count - 1)
            printf("\n");
    }
    return 0;
}

/*
 * parse_options
 * -------------
 * Parses command-line arguments and extracts flags.
 *   
 * Parameters:
 *   argc,argv - arguments passed to main()
 *
 * Returns:
 *   An options_t structure holding provided flags
 *
 * Behaviour:
 *   - Recognizes -a and -t flag
 *   - Exits with an error or invalid flag options
 */
options_t parse_options(int argc, char** argv){
    options_t opts;
    opts.show_all = false;
    opts.sort_time =false;

    // scan all arguments for flags
    for(int i = 1; i < argc; ++i)
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

/*
 * gather_paths
 * ------------
 * Process command-line operands and classify them into directories
 * and non-directory files.
 *
 * Parameters:
 *   argc, argv        - arguments passed to main()
 *   non_dirs          - output array for non-directory paths (files)
 *   non_dir_count     - output count of non-directory paths
 *   dirs              - output array for directory paths
 *   dir_count         - output count of directory paths
 *
 * Returns:
 *   The total number of valid non-option operands processed.
 *
 * Behavior:
 *   - Skips argv[0] (program name) and all option arguments (prefixed with '-')
 *   - Attempts to open each operand as a directory using opendir()
 *   - If opendir() succeeds, the operand is treated as a directory
 *   - Otherwise, uses lstat() to check if the operand refers to a valid file
 *   - Valid files are classified as non-directory paths
 *   - Invalid operands result in an error message and are ignored
 *   - If no valid non-option operands are provided, defaults to the current
 *     directory (".")
 */
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
    }
    // no valid non-options args provided, default
    if(total == 0){
        dirs[0] = ".";
        *dir_count = 1;
        total = 1;
    }
    return total;
}

/*
 * cmp_lex
 * -------
 * Comparator function for lexicographical (alphabetical) ordering of strings.
 *
 * Parameters:
 *   a, b - pointers to elements being compared by qsort()
 *
 * Returns:
 *   < 0 if the string pointed to by a comes before b
 *   > 0 if the string pointed to by a comes after b
 *   = 0 if both strings are equal
 *
 * Behavior:
 *   - Interprets input pointers as pointers to C strings (char *)
 *   - Compares strings using strcmp()
 */
static int cmp_lex(const void *a, const void *b) {
    const char *s1 = *(const char **)a;
    const char *s2 = *(const char **)b;
    return strcmp(s1, s2);
}

/*
 * sort_entries
 * ------------
 * Sort an array of directory entry names in lexicographical order.
 *
 * Parameters:
 *   entries - array of C strings representing entry names
 *   count   - number of entries in the array
 *
 * Behavior:
 *   - Sorts entries in-place using qsort()
 *   - Ordering is based on strcmp() comparison
 */
void sort_entries(char **entries, int count) {
    qsort(entries, count, sizeof(char *), cmp_lex);
}

/*
 * read_directory
 * --------------
 * Read the contents of a directory and collect metadata for each entry.
 *
 * Parameters:
 *   path        - filesystem path to the directory to be read
 *   show_hidden - when true, include entries whose names begin with '.'
 *
 * Returns:
 *   A file_list_t structure containing metadata for each directory entry.
 *   If the directory cannot be opened, an empty file_list_t is returned.
 *
 * Behavior:
 *   - Opens the directory specified by path using opendir()
 *   - Iterates over directory entries using readdir()
 *   - Skips hidden entries (names starting with '.') unless show_hidden is true
 *   - Constructs a full path for each entry and retrieves metadata via lstat()
 *   - Records the following information per entry:
 *       • entry name
 *       • modification time (seconds and nanoseconds)
 *       • whether the entry is a directory
 *   - Stores entries in a fixed-size file_list_t container, up to MAX_FILES
 *   - Prints a warning if the number of entries exceeds MAX_FILES
 *   - Closes the directory stream before returning
 *
 * Notes:
 *   - Entries are returned in filesystem order; no sorting is performed
 *   - Symbolic links are not followed (lstat is used)
 *   - Memory ownership remains local; no dynamic allocation is performed
 */
file_list_t read_directory(const char* path, bool show_hidden){
    file_list_t flist;
    flist.count = 0;

    DIR* dir = opendir(path);
    if(!dir){
        // we can't open dir
        fprintf(stderr, "myls: cannot access %s\n", path);
        return flist;
    }

    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        // check show_hidden, skip '.'
        if(!show_hidden && entry->d_name[0] == '.')
            continue;

        // build full path for lstat
        char full_path[PATH_MAX];
        snprintf(full_path,PATH_MAX,"%s/%s",path,entry->d_name);

        struct stat st;
        if(!lstat(full_path,&st)){
            // fill the file_info_t
            file_info_t info;
            strncpy(info.name,entry->d_name,PATH_MAX - 1);
            info.name[PATH_MAX - 1] = '\0';

            info.sec = st.st_mtimespec.tv_sec;
            info.nsec = st.st_mtimespec.tv_nsec;
            info.is_dir = S_ISDIR(st.st_mode);

            // add to flist
            if(flist.count < MAX_FILES){
                flist.files[flist.count] = info;
                flist.count++;
            }
            else{
                // MAX_FILES exceeded
                printf("Warning: too many files in '%s' (max %d). Some entries skipped\n",path,MAX_FILES);
            }
        }
    }
    closedir(dir);
    return flist;
}

/*
 * sort_file_list
 * --------------
 * Sort the contents of a file_list_t according to the selected ordering mode.
 *
 * Parameters:
 *   flist     - pointer to a file_list_t containing directory entries
 *   sort_time - when true, sort entries by modification time (-t);
 *               otherwise, sort entries lexicographically by name
 *
 * Behavior:
 *   - Sorts the entries in-place using qsort()
 *   - When sort_time is enabled:
 *       • Orders entries by modification time (newest first)
 *       • Uses nanosecond precision to break ties
 *       • Falls back to lexicographical name comparison for stability
 *   - When sort_time is disabled:
 *       • Orders entries alphabetically by name
 *
 * Notes:
 *   - Sorting operates only on the valid portion of the array
 *     as indicated by flist->count
 *   - No memory allocation or copying is performed
 */
void sort_file_list(file_list_t *flist, bool sort_time)
{
    if (sort_time)
        qsort(flist->files, flist->count, sizeof(file_info_t), cmp_file_time);
    else
        qsort(flist->files, flist->count, sizeof(file_info_t), cmp_file_lex);
}

/*
 * cmp_file_time
 * -------------
 * Comparator function for time-based ordering of filesystem entries.
 *
 * Parameters:
 *   a, b - pointers to file_info_t elements being compared
 *
 * Returns:
 *   < 0 if entry b should appear before entry a
 *   > 0 if entry a should appear before entry b
 *   = 0 if both entries are considered equal
 *
 * Ordering Rules:
 *   - Entries are ordered by modification time in descending order
 *     (most recently modified first)
 *   - Seconds are compared before nanoseconds
 *   - If modification times are identical, entries are ordered
 *     lexicographically by name to ensure stable output
 *
 * Notes:
 *   - Intended for use with qsort()
 *   - Declared static as it is internal to this translation unit
 */
static int cmp_file_time(const void *a, const void *b)
{
    const file_info_t *fa = a;
    const file_info_t *fb = b;

    if (fa->sec < fb->sec) return 1;
    if (fa->sec > fb->sec) return -1;

    if (fa->nsec < fb->nsec) return 1;
    if (fa->nsec > fb->nsec) return -1;

    return strcmp(fa->name, fb->name); // tie-breaker
}


/*
 * cmp_file_lex
 * ------------
 * Comparator function for lexicographical (alphabetical) ordering of
 * filesystem entries by name.
 *
 * Parameters:
 *   a, b - pointers to file_info_t elements being compared
 *
 * Returns:
 *   < 0 if the name of a comes before the name of b
 *   > 0 if the name of a comes after the name of b
 *   = 0 if both names are identical
 *
 * Notes:
 *   - Uses strcmp() for string comparison
 *   - Intended for use with qsort()
 *   - Declared static as it is internal to this translation unit
 */
static int cmp_file_lex(const void *a, const void *b)
{
    const file_info_t *fa = a;
    const file_info_t *fb = b;
    return strcmp(fa->name, fb->name);
}
