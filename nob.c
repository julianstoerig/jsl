#define NOBDEF static inline
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"


int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Cmd compile_cmd = {0};
    Cmd run_cmd = {0};


    char *bin_path = "tests.elf";
    char *src_path = "test.c";
    char *std_versions[] = {"99", "11", "17", "23"};

    delete_file("base_short.h");
    copy_file("base.h", "base_short.h");
    Cmd edit_base_short_cmd = {0};
    cmd_append(&edit_base_short_cmd, "sed", "-i", "s/js_//g", "base_short.h");
    if (!cmd_run(&edit_base_short_cmd)) {
        printf("[ERROR] Failed to edit \"base_short.h\" to remove prefixes");
        return 1;
    }

    int n_std_versions = sizeof(std_versions) / sizeof(*std_versions);
    int status_codes[16] = {0};
    int n_status_codes = sizeof(status_codes) / sizeof(*status_codes);

    for (int i=0; i<n_std_versions; ++i) {
        nob_cc(&compile_cmd);
        nob_cc_flags(&compile_cmd);
        cmd_append(&compile_cmd, temp_sprintf("-std=c%s", std_versions[i]));
        nob_cc_output(&compile_cmd, bin_path);
        nob_cc_inputs(&compile_cmd, src_path);
        if (!cmd_run(&compile_cmd)) {
            printf("[ERROR] failed to compile under C%s\n", std_versions[i]);
            status_codes[i] = 1;
        }

        cmd_append(&run_cmd, temp_sprintf("./%s", bin_path));
        if (!cmd_run(&run_cmd)) {
            printf("[ERROR] executing tests for C%s\n", std_versions[i]);
            status_codes[i] = 1;
        }
        run_cmd = (Cmd){0};
    }

    for (int i=0; i<n_status_codes; ++i) {
        if (status_codes[i])
            return 1;
    }
}
