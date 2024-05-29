//
// Created by stefkoff on 5/29/24.
//
#include "../include/process.h"

// list processes that are in /proc filter. Find only the number one
int* get_proc_dirs(void) {
    // initialie regex
    regex_t regex;
    int reti;

    // search only for numbers
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
            // if we have a match for a given folder, re-allocate memory for the int array and store it
            processes = realloc(processes, (i + 1) * sizeof(int));
            processes[i++] = atoi(ep->d_name);
        }
    }

    // free regex
    regfree(&regex);
    (void) closedir(dp);

    return processes;
}

// get process information
struct PROCESS* get_process(int pid) {
    struct PROCESS *p = malloc(sizeof(*p));
    // TODO: check the process max length, because we can blow here
    p->name = malloc(100);

    // TODO: same for the filename length
    char *filename = malloc(100);
    if(filename == NULL) {
        printf("Enable to allocate memory for filename variable");
        return NULL;
    }
    sprintf(filename, "/proc/%d/stat", pid);

    // open the proc file
    FILE *f = fopen(filename, "r");

    if(f == NULL) {
        free(filename);
        return NULL;
    }

    fscanf(f, "%d %s %c %d", &p->pid, p->name, &p->state, &p->ppid);
    free(filename);
    fclose(f);

    // initialize the default values: no children for now
    p->childrenCount = 0;
    p->children = NULL;

    return p;
}

// build the entire tree recursivly
struct PROCESS* load_tree(int startPid) {
    struct PROCESS* main = get_process(startPid);

    if(main == NULL) {
        // nothing to do more
        free(main);
        return NULL;
    }

    // load available processes
    // TODO: make static?
    int* availableProcesses = get_proc_dirs();
    int i = 0;
    int pid = availableProcesses[i];
    while(pid) {
        if(pid == startPid) {
            // we are on the same process. Go next
            pid = availableProcesses[++i];
            continue;
        }
        // try to load the given process and see if the ppid is the same as the root one (requested for the tree)
        struct PROCESS* childProcess = get_process(pid);


        if(childProcess == NULL) {
            // no - next
            free(childProcess);
            pid = availableProcesses[++i];
            continue;
        }

        if(childProcess->ppid == main->pid) {
            // found it
            if(!main->childrenCount) {
                // re-initialize in case something is not OK
                main->childrenCount = 1;
                main->children = malloc(sizeof(childProcess));
            } else {
                main->childrenCount++;
                // re-allocate memory to store the next one
                main->children = realloc(main->children, main->childrenCount * sizeof(*childProcess));
            }

            // recursivlly load the given child children
            main->children[main->childrenCount - 1] = load_tree(pid);
        }
        pid = availableProcesses[++i];
    }

    return main;
}

// recursiblly print the process three
// TODO: remove unused arguments
void print_process_tree(struct PROCESS* p, int dept, int hasMore, int isFirst, int isLast) {
    if(p == NULL || !p->pid) {
        return;
    }

    for(int i = 0; i < dept; i++) {
        if(i == dept - 1) {
            // print some fancy stuff
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

    if(dept != 0) {
        printf(" ");
    }

    // TODO: find better way to print
    printf("Process ID: %d\tProcess name: %s\tState: %c\tParent PID: %d\n",p->pid, p->name, p->state, p->ppid);

    if(p->childrenCount) {
        int i = 0;
        while(i < p->childrenCount) {
            if(p->children[i] != NULL) {
                // recursilly print the child process three
                print_process_tree(p->children[i], dept + 1, i < p->childrenCount - 1,i == 0 ? 1 : 0, i == p->childrenCount - 1);
            }
            i++;
        }
    }
}

// sort the tree by no-children-first
void sort_tree_by_children(struct PROCESS* p) {
    if(p->children == NULL) {
        // no children - no issues
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

                // sort both current and next
                sort_tree_by_children(p->children[i]);
                sort_tree_by_children(p->children[j]);
            }
        }
    }
}
