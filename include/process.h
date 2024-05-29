//
// Created by stefkoff on 5/29/24.
//

#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <dirent.h>

#ifndef C_PROCESS_TREE_VIEW_PROCESS_H
#define C_PROCESS_TREE_VIEW_PROCESS_H

struct PROCESS {
    int pid;
    char *name;
    char state;
    int ppid;
    struct PROCESS **children;
    int childrenCount;
};

int* get_proc_dirs(void);

struct PROCESS* get_process(int);

struct PROCESS* load_tree(int);

void print_process_tree(struct PROCESS*, int, int, int, int);

void sort_tree_by_children(struct PROCESS*);

#endif //C_PROCESS_TREE_VIEW_PROCESS_H
