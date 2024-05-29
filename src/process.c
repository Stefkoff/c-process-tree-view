//
// Created by stefkoff on 5/29/24.
//
#include "../include/process.h"

int* get_proc_dirs(void) {
    regex_t regex;
    int reti;

    reti = regcomp(&regex, "^[0-9]+", REG_EXTENDED);
    if(reti) {
        printf("Could not initialize regex\n");
        return NULL;
    }

    DIR* dp;
    struct dirent *ep;
    dp = opendir("/proc");
    int *processes = malloc(1 * sizeof(int));
    if(dp == NULL) {
        printf("Could not open dir /proc");
        return NULL;
    }
    int i = 0;
    while((ep = readdir(dp)) != NULL) {
        reti = regexec(&regex, ep->d_name, 0, NULL, 0);

        if(!reti) {
            processes = realloc(processes, (i + 1) * sizeof(int));
            processes[i++] = atoi(ep->d_name);
        }
    }

    regfree(&regex);
    (void) closedir(dp);

    return processes;
}

struct PROCESS* get_process(int pid) {
    struct PROCESS *p = malloc(sizeof(*p));
    p->name = malloc(100);

    char *filename = malloc(100);
    if(filename == NULL) {
        printf("Enable to allocate memory for filename variable");
        return NULL;
    }
    sprintf(filename, "/proc/%d/stat", pid);

    FILE *f = fopen(filename, "r");

    if(f == NULL) {
        free(filename);
        return NULL;
    }

    fscanf(f, "%d %s %c %d", &p->pid, p->name, &p->state, &p->ppid);
    free(filename);
    fclose(f);

    p->childrenCount = 0;
    p->children = NULL;

    return p;
}

struct PROCESS* load_tree(int startPid) {
    struct PROCESS* main = get_process(startPid);

    if(main == NULL) {
        free(main);
        return NULL;
    }

    int* availableProcesses = get_proc_dirs();
    int i = 0;
    int pid = availableProcesses[i];
    while(pid) {
        if(pid == startPid) {
            pid = availableProcesses[++i];
            continue;
        }
        struct PROCESS* childProcess = get_process(pid);


        if(childProcess == NULL) {
            free(childProcess);
            pid = availableProcesses[++i];
            continue;
        }

        if(childProcess->ppid == main->pid) {
            if(!main->childrenCount) {
                main->childrenCount = 1;
                main->children = malloc(sizeof(childProcess));
            } else {
                main->childrenCount++;
                main->children = realloc(main->children, main->childrenCount * sizeof(*childProcess));
            }

            main->children[main->childrenCount - 1] = load_tree(pid);
        }
        pid = availableProcesses[++i];
    }

    return main;
}

void print_process_tree(struct PROCESS* p, int dept, int hasMore, int isFirst, int isLast) {
    if(p == NULL || !p->pid) {
        return;
    }

    for(int i = 0; i < dept; i++) {
        if(i == dept - 1) {
            wchar_t uChar;
            if(isFirst && isLast == 0) {
                uChar = 0x251c; // ├
            } else if(isFirst && isLast == 1) {
                uChar = 0x2570; // ╰
            } else if(isFirst ==0 && isLast == 1) {
                uChar = 0x2570; // ╰
            } else {
                uChar = 0x2502; // │
            }
            printf("%lc", uChar);

        } else {
            printf(" ");
        }
    }

    /**
     * TODO: sort the items based on having children or not for better view - or not
     */

    if(dept != 0) {
        printf(" ");
    }

    printf("Process ID: %d\tProcess name: %s\tState: %c\tParent PID: %d\n",p->pid, p->name, p->state, p->ppid);

    if(p->childrenCount) {
        int i = 0;
        while(i < p->childrenCount) {
            if(p->children[i] != NULL) {
                print_process_tree(p->children[i], dept + 1, i < p->childrenCount - 1,i == 0 ? 1 : 0, i == p->childrenCount - 1);
            }
            i++;
        }
    }
}

void sort_tree_by_children(struct PROCESS* p) {
    if(p->children == NULL) {
        return;
    }
    for(int i = 0; i < p->childrenCount - 1; i++) {
        if(p->children[i] == NULL) {
            continue;
        }
        for(int j = i; j < p->childrenCount; j++) {
            if(p->children[j] == NULL) {
                continue;
            }
            if(p->children[j]->childrenCount < p->children[i]->childrenCount) {
                struct PROCESS* temp = p->children[i];
                p->children[i] = p->children[j];
                p->children[j] = temp;

                sort_tree_by_children(p->children[i]);
                sort_tree_by_children(p->children[j]);
            }
        }
    }
}