#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define MAX_FILES 1000
#define NAME_LEN 256

int compare_names(const void *a, const void *b)
{
    const char *name_a = a;
    const char *name_b = b;
    return strcmp(name_a, name_b);
}

void do_ls(const char *dir, int long_format, int horizontal);

int main(int argc, char *argv[])
{
    int long_format = 0;
    int horizontal = 0;
    char *dirs[argc];
    int dir_count = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            long_format = 1;
        else if (strcmp(argv[i], "-x") == 0)
            horizontal = 1;
        else
            dirs[dir_count++] = argv[i];
    }

    if (dir_count == 0) {
        do_ls(".", long_format, horizontal);
    } else {
        for (int i = 0; i < dir_count; i++) {
            printf("Directory listing of %s:\n", dirs[i]);
            do_ls(dirs[i], long_format, horizontal);
            printf("\n");
        }
    }

    return 0;
}

void do_ls(const char *dir, int long_format, int horizontal)
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
                int index;

                if (horizontal)
                    index = row * columns + col;
                else
                    index = col * rows + row;

                if (index < count)
                    printf("%-25s", names[index]);
            }
            printf("\n");
        }
        return;
    }

    for (int i = 0; i < count; i++) {
        char path[4096];
        char perms[11];
        char timebuf[80];
        struct stat info;
        struct passwd *pw;
        struct group *gr;
        struct tm *tm_info;

        snprintf(path, sizeof(path), "%s/%s", dir, names[i]);

        if (lstat(path, &info) == -1) {
            perror(path);
            continue;
        }

        if (S_ISDIR(info.st_mode))
            perms[0] = 'd';
        else if (S_ISLNK(info.st_mode))
            perms[0] = 'l';
        else
            perms[0] = '-';

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

        pw = getpwuid(info.st_uid);
        gr = getgrgid(info.st_gid);
        tm_info = localtime(&info.st_mtime);

        if (tm_info != NULL)
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", tm_info);
        else
            strcpy(timebuf, "unknown");

        printf("%s %2lu %-8s %-8s %8ld %s %s\n",
               perms, (unsigned long)info.st_nlink,
               pw ? pw->pw_name : "unknown",
               gr ? gr->gr_name : "unknown",
               (long)info.st_size, timebuf, names[i]);
    }
}
