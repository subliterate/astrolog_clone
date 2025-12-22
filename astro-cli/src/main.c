#include <stdio.h>
#include <string.h>
#include "command.h"

void print_usage() {
    printf("Usage: astro <command> [options]\n\n");
    printf("Astro-CLI: A modern command-line interface for the Astrolog engine.\n\n");
    printf("Commands:\n");
    printf("  chart        Generate astrological charts (e.g., natal, transit).\n");
    printf("  predict      Forecast future astrological events (e.g., transits, returns).\n");
    printf("  data         Extract raw data (e.g., planet positions) in various formats.\n");
    printf("  config       Get or set configuration options.\n");
    printf("  help         Display this help message.\n\n");
    printf("Use 'astro <command> --help' for more information about a specific command.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    char *command = argv[1];

    // Decrement argc and increment argv to pass to handlers
    int sub_argc = argc - 1;
    char **sub_argv = argv + 1;

    if (strcmp(command, "help") == 0) {
        print_usage();
    } else if (strcmp(command, "chart") == 0) {
        return handle_chart_command(sub_argc, sub_argv);
    } else if (strcmp(command, "predict") == 0) {
        return handle_predict_command(sub_argc, sub_argv);
    } else if (strcmp(command, "data") == 0) {
        return handle_data_command(sub_argc, sub_argv);
    } else if (strcmp(command, "config") == 0) {
        return handle_config_command(sub_argc, sub_argv);
    } else {
        printf("Unknown command: %s\n\n", command);
        print_usage();
        return 1;
    }

    return 0;
}
