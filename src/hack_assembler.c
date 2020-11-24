#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

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
    // code ...
    fclose(asmfile);

    return EXIT_SUCCESS;
}

bool validate_arguments(int argc, char *argv[])
{
    return (argc == 2);
}

Config process_arguments(int argc, char *argv[])
{
    Config c = {.asm_file_name=argv[1]};
    return c;
}
