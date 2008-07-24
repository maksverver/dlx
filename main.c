#include "dlx.h"
#include <stdio.h>
#include <stdlib.h>

int callback(int rows_size, int *rows, void *data)
{
    int n = 0;

    for(n = 0; n < rows_size; ++n)
        printf("%d%c", rows[n], ((n == rows_size - 1) ? '\n' : ' '));

    return 0;
}

int main()
{
    int rows, cols, n;
    struct dlx_Node *header;
    char *data;

    if(scanf("%d %d", &rows, &cols) != 2)
    {
        fprintf(stderr, "Premature end of input\n");
        exit(1);
    }

    data = malloc(rows*cols*sizeof(*data));
    if(data == NULL)
    {
        perror(NULL);
        exit(1);
    }

    for(n = 0; n < rows*cols; ++n)
    {
        int v;
        if(scanf("%d", &v) != 1)
        {
            fprintf(stderr, "Premature end of input\n");
            exit(1);
        }
        data[n] = v;
    }

    header = dlx_alloc(data, rows, cols);
    if(header == NULL)
    {
        perror(NULL);
        exit(1);
    }

    if(dlx_solve(header, callback, NULL) == -1)
    {
        perror(NULL);
        exit(1);
    }

    return 0;
}
