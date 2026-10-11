# Operating Systems Assignment 02
## Re-engineering the Linux ls Utility

### Feature 1: Project Setup
The project is organized into src, bin, obj, and man directories.
The source files are stored in src, compiled programs in bin,
and obj is reserved for object files. The Makefile automates
compilation, and Git branches and tags track feature development.

### Feature 2: Long Listing Format (-l)

**Q1. What is the difference between stat() and lstat()?**

stat() follows a symbolic link and returns information about
the file it points to. lstat() returns information about the
symbolic link itself. lstat() is useful in ls when we need to
identify and display symbolic links correctly.

**Q2. How can st_mode be used to find file types and permissions?**

The st_mode field contains file-type and permission bits.
Macros such as S_ISDIR(st_mode) identify a directory, while
S_ISREG(st_mode) identifies a regular file. Permission bits
can be checked using expressions such as st_mode & S_IRUSR
to check whether the owner has read permission. Similar macros
exist for write and execute permissions. Special permission
bits can also be checked with their predefined macros.

### Feature 3: Column Display (- default)

**Q1. How does down-then-across printing work?**

First, all filenames are stored in an array. The program
calculates the number of rows and columns. It prints the first
entry of each column in a row, then uses the row count to
select entries from the other columns. A simple sequential
loop would print filenames in order rather than arranging
them in the required column layout.

**Q2. What is the purpose of ioctl() here?**

ioctl() with TIOCGWINSZ can retrieve the terminal width.
The program uses this width to decide how many columns fit
on the screen. A fixed width such as 80 characters may cause
poor alignment or wrapping when the actual terminal is
narrower or wider.

### Feature 4: Horizontal Display (-x)

**Q1. Compare vertical and horizontal display logic.**

Down-then-across display needs the total number of entries
and the number of rows to calculate each column's position.
Horizontal display prints entries from left to right and
starts a new line when the next entry does not fit. Therefore,
horizontal display generally needs less index pre-calculation.

**Q2. How are display modes managed?**

Command-line arguments are parsed to identify options such
as -l and -x. The program records the selected mode and calls
the corresponding display function. With no display option,
it uses the default column layout. The implementation should
also define how conflicting options are handled.

### Feature 5: Alphabetical Sorting

**Q1. Why must filenames be collected before sorting?**

Sorting requires the program to compare entries with one
another. Therefore, filenames are collected into an array
before qsort() is called. For directories with millions of
entries, storing filenames uses substantial memory and
sorting requires additional processing time.

**Q2. What is the qsort() comparison function?**

The comparison function supplied to qsort() accepts two
const void pointers. It converts them to the appropriate
string-pointer type and compares the strings, commonly
using strcmp(). It returns a negative value if the first
item comes before the second, zero if they are equal, and
a positive value if the first comes after the second.
const void * allows qsort() to work with different data types.

### Feature 6: Colorized Output

**Q1. How do ANSI escape codes print colors?**

ANSI escape sequences tell a compatible terminal to change
text formatting. For example, printf("\033[0;32m") selects
green text, and printf("\033[0m") resets the formatting.
The reset sequence prevents the color from affecting later
output.

**Q2. Which permission bits identify an executable file?**

The execute permission bits are S_IXUSR for the owner,
S_IXGRP for the group, and S_IXOTH for others. They can be
checked using bitwise AND with st_mode. If any relevant
execute bit is set, the file has execute permission for
that category. The program should also consider the file
type when deciding how to color its name.

### Feature 7: Recursive Listing (-R)

**Q1. What is the base case in recursive ls?**

A base case is the condition that stops further recursive
calls. In recursive ls, the program must not recurse into
the special directory entries "." and "..". It only makes
recursive calls for valid subdirectories. Correctly skipping
these entries prevents endless traversal between parents
and the current directory.

**Q2. Why must a full path be constructed?**

A full path combines the current directory and the child
directory name, such as parent_dir/subdir. This ensures that
the program opens and checks the intended directory.
Calling do_ls("subdir") alone may search relative to the
process's current working directory rather than parent_dir,
so it may access the wrong location or fail.

### Conclusion

This assignment demonstrates file metadata access, command-line
argument parsing, output formatting, sorting, ANSI colors,
recursive directory traversal, Makefile usage, and Git-based
version control. Each feature should be tested independently
and explained during the viva.
