//
// Created by stefkoff on 5/29/24.
//

#include <locale.h>
#include <getopt.h>
#include "../include/process.h"

void print_help(void) {
    printf("ptree - Print running processes in a tree-view.\n");
    printf("Usage: ptree [-p N]\n");
    printf("Options:\n");
    printf("\t-p N\troot process ID. Default is 1.\n");
}

int main(int argc, char **argv) {
    int pid = 1;
    int c;

    while((c = getopt(argc, argv, ":p:h")) != -1) {
        switch (c) {
            case 'p':
                pid = atoi(optarg);
                break;
            case 'h':
                print_help();
                exit(0);
            case '?':
                printf("Unknown option %c\n", optopt);
                exit(1);
        }
    }

    setlocale(LC_CTYPE, "");
    struct PROCESS* p = load_tree(pid);
    sort_tree_by_children(p);
    print_process_tree(p, 0, 0, 1, 1);
    free(p);
    return 0;
}
