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

void do_ls(const char *dir, int long_format);

int main(int argc, char *argv[])
{
    int long_format = 0;
    int start = 1;

    if (argc > 1 && strcmp(argv[1], "-l") == 0) {
        long_format = 1;
        start = 2;
    }

    if (start == argc) {
        do_ls(".", long_format);
    } else {
        for (int i = start; i < argc; i++) {
            printf("Directory listing of %s:\n", argv[i]);
            do_ls(argv[i], long_format);
            printf("\n");
        }
    }

    return 0;
}

void do_ls(const char *dir, int long_format)
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

    if (!long_format) {
        int rows = (count + 2) / 3;

        for (int row = 0; row < rows; row++) {
            for (int col = 0; col < 3; col++) {
                int index = col * rows + row;

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
