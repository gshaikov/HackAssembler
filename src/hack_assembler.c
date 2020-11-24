#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "translator.h"

/**
 * Config contains all configuration information
 * known by the app
 */
typedef struct Config
{
    char *asm_file_name;
} Config;

bool validate_arguments(int argc, char *argv[]);
Config process_arguments(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if (!validate_arguments(argc, argv))
    {
        fprintf(stderr, "Usage: hack_assembler file\n");
        return EXIT_FAILURE;
    }
    Config config = process_arguments(argc, argv);

    FILE *asmfile = fopen(config.asm_file_name, "r");
    if (asmfile == NULL)
    {
        fprintf(stderr, "Failed to open %s\n", config.asm_file_name);
        return EXIT_FAILURE;
    }

    char binary_file_name[] = "out.hack";
    FILE *binaryfile = fopen(binary_file_name, "w");
    if (binaryfile == NULL)
    {
        fprintf(stderr, "Failed to create %s\n", binary_file_name);
        return EXIT_FAILURE;
    }
    int err = translate(binaryfile, asmfile);
    fclose(binaryfile);
    fclose(asmfile);
    if (err)
    {
        fprintf(stderr, "Failed to translate %s to assembly\n", config.asm_file_name);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool validate_arguments(int argc, char *argv[])
{
    return (argc == 2);
}

Config process_arguments(int argc, char *argv[])
{
    Config c = {.asm_file_name = argv[1]};
    return c;
}
