#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define INITIAL_CAPACITY 16
#define NAME_WIDTH 256

void do_ls(const char *dir, int long_format);
int terminal_width(void);
int compare_names(const void *a, const void *b);

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
            if (i + 1 < argc)
                printf("\n");
        }
    }

    return 0;
}

int terminal_width(void)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 &&
        ws.ws_col > 0)
        return ws.ws_col;

    return 80;
}

int compare_names(const void *a, const void *b)
{
    const char * const *name_a = a;
    const char * const *name_b = b;

    return strcmp(*name_a, *name_b);
}

void do_ls(const char *dir, int long_format)
{
    DIR *dp = opendir(dir);
    struct dirent *entry;
    char **names = NULL;
    size_t count = 0;
    size_t capacity = INITIAL_CAPACITY;

    if (dp == NULL) {
        perror(dir);
        return;
    }

    names = malloc(capacity * sizeof(*names));
    if (names == NULL) {
        perror("malloc");
        closedir(dp);
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;

        if (count == capacity) {
            size_t new_capacity = capacity * 2;
            char **temp = realloc(names,
                                  new_capacity * sizeof(*names));

            if (temp == NULL) {
                perror("realloc");
                break;
            }

            names = temp;
            capacity = new_capacity;
        }

        names[count] = strdup(entry->d_name);
        if (names[count] == NULL) {
            perror("strdup");
            break;
        }

        count++;
    }

    closedir(dp);

    if (!long_format) {
        int max_name_len = 0;

        for (size_t i = 0; i < count; i++) {
            int len = (int)strlen(names[i]);
            if (len > max_name_len)
                max_name_len = len;
        }

        int col_width = max_name_len + 2;
        int width = terminal_width();
        int columns = width / col_width;

        if (columns < 1)
            columns = 1;

        size_t rows = (count + (size_t)columns - 1)
                      / (size_t)columns;

        for (size_t row = 0; row < rows; row++) {
            for (int col = 0; col < columns; col++) {
                size_t index = (size_t)col * rows + row;

                if (index < count) {
                    printf("%-*s", col_width, names[index]);
                }
            }
            printf("\n");
        }
    } else {
        for (size_t i = 0; i < count; i++) {
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
                strftime(timebuf, sizeof(timebuf),
                         "%b %d %H:%M", tm_info);
            else
                strcpy(timebuf, "unknown");

            printf("%s %2lu %-8s %-8s %8ld %s %s\n",
                   perms, (unsigned long)info.st_nlink,
                   pw ? pw->pw_name : "unknown",
                   gr ? gr->gr_name : "unknown",
                   (long)info.st_size, timebuf, names[i]);
        }
    }

    for (size_t i = 0; i < count; i++)
        free(names[i]);

    free(names);
}
