# myls — Unix `ls` command implementation in C

A from-scratch implementation of the Unix `ls` command in C, built to understand filesystem traversal, metadata handling, and POSIX system calls.  
The project mirrors core `ls` behavior while focusing on clean design, correctness, and extensibility.

---

## 🔧 Technologies & Concepts

**Language**
- C (C99)

**System APIs**
- `opendir`
- `readdir`
- `lstat`
- `stat`

**Concepts**
- POSIX filesystem traversal  
- Directory streams and inode metadata  
- Time-based and lexicographical sorting  
- Command-line option parsing  
- Stable sorting with custom comparators  
- Error handling and edge-case correctness  

---

## ✨ Features Implemented

- Default `ls` behavior (lists current directory when no operands are provided)
- Supports multiple file and directory operands
- Command-line option parsing:
  - `-a` — include hidden files
  - `-t` — sort by modification time (newest first)
- Accurate time-based sorting using:
  - seconds + nanoseconds (tie-safe)
- Clean separation of concerns:
  - argument parsing
  - path classification
  - directory reading
  - sorting
  - output formatting
- Uses `lstat()` to avoid following symlinks (matches `ls` semantics)

---

## 📂 Usage

```bash
./myls [OPTIONS] [FILES | DIRECTORIES]
```
## 📂 Example
```bash
./myls
./myls test
./myls -a
./myls -t
./myls -t -a src include
```

## 🧠 Architecture Overview

### Execution Flow

1. **Parse command-line options**
   - Extract `-a` and `-t` flags

2. **Classify operands**
   - Separate files from directories
   - Default to `.` if no paths are provided

3. **Sort operands**
   - Lexicographical ordering (matches `ls`)

4. **Read directory contents**
   - Traverse using `opendir()` / `readdir()`
   - Collect metadata via `lstat()`

5. **Sort directory entries**
   - Alphabetical or modification-time based

6. **Print output**
   - Files first, then directories
   - Correct spacing between directory outputs

---

## 🧩 Core Data Structures

### `file_info_t`

Stores metadata required for sorting and display:
- Entry name
- Modification timestamp (seconds + nanoseconds)
- Directory flag

### `file_list_t`

Fixed-size container for directory contents:
- Avoids dynamic allocation
- Enables predictable memory usage
- Simplifies sorting and iteration

---

## 🔍 Sorting Logic

### Alphabetical Sort
- Uses `strcmp()`

### Time-Based Sort (`-t`)
- Newest first
- Nanosecond precision
- Name used as a deterministic tie-breaker

Custom comparator functions are implemented and passed to `qsort()`.

---

## ⚠️ Known Limitations

- No support for:
  - `-l`, `-R`, `-h`, colorized output, or permissions formatting
- Output format is simplified (one entry per line)
- Fixed upper bound on directory size (`MAX_FILES`)

These are intentional trade-offs to prioritize correctness and clarity.

---

## 📈 Learning Outcomes

- Gained hands-on experience with Unix filesystem internals
- Learned to safely traverse directories using POSIX APIs
- Implemented stable, multi-key sorting logic
- Designed modular C code suitable for incremental extension
- Practiced clean commit structuring and documentation

---

## 🚀 Future Work

- Long listing format (`-l`)
- Recursive traversal (`-R`)
- Permission and ownership display
- Dynamic memory allocation for large directories
- Colorized output

