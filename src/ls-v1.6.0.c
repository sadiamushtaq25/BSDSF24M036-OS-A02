#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

#define MAX_FILES 1000
#define NAME_LEN 256

int compare_names(const void *a, const void *b)
{
    return strcmp((const char *)a, (const char *)b);
}

void do_ls(const char *dir, int long_format, int horizontal, int recursive);

const char *color_name(const char *path, const char *name)
{
    static char result[1024];
    struct stat st;
    char fullpath[PATH_MAX];

    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

    if (lstat(fullpath, &st) == -1)
        return name;

    const char *color = "\033[0m";

    if (S_ISDIR(st.st_mode))
        color = "\033[1;34m";
    else if (S_ISLNK(st.st_mode))
        color = "\033[1;36m";
    else if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        color = "\033[1;32m";

    snprintf(result, sizeof(result), "%s%s\033[0m", color, name);
    return result;
}

int main(int argc, char *argv[])
{
    int long_format = 0;
    int horizontal = 0;
    int recursive = 0;
    char *dirs[argc];
    int dir_count = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            int valid_options = 1;

            for (int j = 1; argv[i][j] != '\0'; j++) {
                if (argv[i][j] == 'l')
                    long_format = 1;
                else if (argv[i][j] == 'x')
                    horizontal = 1;
                else if (argv[i][j] == 'R')
                    recursive = 1;
                else
                    valid_options = 0;
            }

            if (!valid_options) {
                fprintf(stderr, "Unknown option: %s\\n", argv[i]);
                return 1;
            }
        } else {
            dirs[dir_count++] = argv[i];
        }
    }

    if (dir_count == 0) {
        do_ls(".", long_format, horizontal, recursive);
    } else {
        for (int i = 0; i < dir_count; i++) {
            if (dir_count > 1 || recursive)
                printf("%s:\n", dirs[i]);

            do_ls(dirs[i], long_format, horizontal, recursive);

            if (i < dir_count - 1)
                printf("\n");
        }
    }

    return 0;
}

void do_ls(const char *dir, int long_format, int horizontal, int recursive)
{
    DIR *dp = opendir(dir);
    struct dirent *entry;
    char names[MAX_FILES][NAME_LEN];
    int count = 0;

    if (dp == NULL) {
        perror(dir);
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        if (count < MAX_FILES) {
            snprintf(names[count], NAME_LEN, "%s", entry->d_name);
            count++;
        }
    }

    closedir(dp);
    qsort(names, count, NAME_LEN, compare_names);

    if (!long_format) {
        int columns = 3;
        int rows = (count + columns - 1) / columns;

        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < columns; col++) {
                int index = horizontal
                    ? row * columns + col
                    : col * rows + row;

                if (index < count)
                    printf("%-25s", color_name(dir, names[index]));
            }
            printf("\n");
        }
    } else {
        for (int i = 0; i < count; i++) {
            char path[PATH_MAX];
            char perms[11];
            char timebuf[80];
            struct stat info;

            snprintf(path, sizeof(path), "%s/%s", dir, names[i]);

            if (lstat(path, &info) == -1) {
                perror(path);
                continue;
            }

            perms[0] = S_ISDIR(info.st_mode) ? 'd' :
                       S_ISLNK(info.st_mode) ? 'l' : '-';

            perms[1] = (info.st_mode & S_IRUSR) ? 'r' : '-';
            perms[2] = (info.st_mode & S_IWUSR) ? 'w' : '-';
            perms[3] = (info.st_mode & S_IXUSR) ? 'x' : '-';
            perms[4] = (info.st_mode & S_IRGRP) ? 'r' : '-';
            perms[5] = (info.st_mode & S_IWGRP) ? 'w' : '-';
            perms[6] = (info.st_mode & S_IXGRP) ? 'x' : '-';
            perms[7] = (info.st_mode & S_IROTH) ? 'r' : '-';
            perms[8] = (info.st_mode & S_IWOTH) ? 'w' : '-';
            perms[9] = (info.st_mode & S_IXOTH) ? 'x' : '-';
            perms[10] = '\0';

            struct passwd *pw = getpwuid(info.st_uid);
            struct group *gr = getgrgid(info.st_gid);
            struct tm *tm_info = localtime(&info.st_mtime);

            if (tm_info)
                strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
            else
                strcpy(timebuf, "unknown");

            printf("%s %2lu %-8s %-8s %8ld %s %s\n",
                   perms, (unsigned long)info.st_nlink,
                   pw ? pw->pw_name : "unknown",
                   gr ? gr->gr_name : "unknown",
                   (long)info.st_size, timebuf,
                   color_name(dir, names[i]));
        }
    }

    if (recursive) {
        for (int i = 0; i < count; i++) {
            char path[PATH_MAX];
            struct stat info;

            snprintf(path, sizeof(path), "%s/%s", dir, names[i]);

            if (lstat(path, &info) == -1)
                continue;

            /* Recurse into real directories, not symlinks. */
            if (S_ISDIR(info.st_mode)) {
                printf("\n%s:\n", path);
                do_ls(path, long_format, horizontal, recursive);
            }
        }
    }
}
