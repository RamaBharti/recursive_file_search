#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#ifndef S_ISLNK
#define S_ISLNK(mode) (0)
#endif


#define MAX_PATH_LEN 4096

// Function to recursively search for a target in the current directory
void search_directory(const char *target, const char *current_path) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char path[MAX_PATH_LEN];

    // Open the current directory
    if ((dir = opendir(current_path)) == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Ignore "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Construct the full path of the entry
        snprintf(path, MAX_PATH_LEN, "%s/%s", current_path, entry->d_name);

        // Get information about the entry
        if (stat(path, &statbuf) == -1) {   //******
            perror("Error getting file information");
            exit(EXIT_FAILURE);
        }

        // Check if the entry is a directory and if so, recurse into it
        if (S_ISDIR(statbuf.st_mode)) {
            search_directory(target, path);
        }

        // Check if the entry matches the target
        if (strcmp(entry->d_name, target) == 0) {
            // Print the absolute path and type of the target
            if (S_ISDIR(statbuf.st_mode)) {
                printf("Directory found: %s\n", path);
            } else if (S_ISLNK(statbuf.st_mode)) {
                printf("Link found: %s\n", path);
            } else {
                printf("File found: %s\n", path);
            }
            closedir(dir);
            exit(EXIT_SUCCESS);
        }
    }

    // Target not found in the current directory
    closedir(dir);
}

int main(int argc, char *argv[]) {
    // Check for the correct number of arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <target>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Get the current working directory
    char current_path[MAX_PATH_LEN];
    if (getcwd(current_path, MAX_PATH_LEN) == NULL) {
        perror("Error getting current working directory");
        exit(EXIT_FAILURE);
    }

    // Call the search function with the target and current directory
    search_directory(argv[1], current_path);

    // If the target is not found, print a message
    printf("The target '%s' was not found in the current directory.\n", argv[1]);

    return EXIT_SUCCESS;
}
