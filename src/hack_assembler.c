#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: hack_assembler file\n");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "%s\n", argv[1]);
    return EXIT_SUCCESS;
}
