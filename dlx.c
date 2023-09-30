#include "dlx.h"

#include <stdlib.h>

typedef struct dlx_Node {
    struct dlx_Node *left, *right, *up, *down;
    struct dlx_Node *column;
    union data {
        int row;
        int size;
        int max_depth;
    } data;
} dlx_Node;

typedef int (*callback_t)(int rows_size, int *rows, void *);

typedef struct ResultHandler {
    int         *cur_row, *rows;
    callback_t  callback;
    void        *data;
} ResultHandler;

static void cover_column(dlx_Node *col)
{
    dlx_Node *i, *j;

    col->left->right = col->right;
    col->right->left = col->left;
    for(i = col->down; i != col; i = i->down)
        for(j = i->right; j != i; j = j->right)
        {
            j->down->up = j->up;
            j->up->down = j->down;
            --(j->column->data.size);
        }
}

static void uncover_column(dlx_Node *col)
{
    dlx_Node *i, *j;
    for(i = col->up; i != col; i = i->up)
        for(j = i->left; j != i; j = j->left)
        {
            j->down->up = j->up->down = j;
            ++(j->column->data.size);
        }
    col->left->right = col->right->left = col;
}

int dlx_search(dlx_Node *header, ResultHandler *rh)
{
    int result = 0;
    dlx_Node *c, *r, *j;

    ++rh->cur_row;
    if(header->right == header)
    {
        result = rh->callback(rh->cur_row - rh->rows, rh->rows, rh->data);
    }
    else
    {
        /* Pick a column - using heuristic */
        c = header->right;
        if(c->data.size > 1)
        {
            for(j = c->right; j != header; j = j->right)
                if(j->data.size < c->data.size)
                {
                    c = j;
                    if(j->data.size <= 1)
                        break;
                }
        }

        /* Narrow solution on this column */
        cover_column(c);
        for(r = c->down; r != c; r = r->down)
        {
            *rh->cur_row = r->data.row;

            /* Cover row 'r' */
            for(j = r->right; j != r; j = j->right)
                cover_column(j->column);

            /* Recurse.. */
            result = dlx_search(header, rh);

            /* Uncover row 'r' */
            for(j = r->left; j != r; j = j->left)
                uncover_column(j->column);

            if(result != 0)
                break;

        }
        uncover_column(c);
    }
    --rh->cur_row;

    return result;
}

int dlx_solve(dlx_Node *header, callback_t callback, void *data)
{
    int result;
    int rows[header->data.max_depth];
    /* NB. rows - 1 is technically undefined behavior */
    ResultHandler rh = { rows - 1, rows, callback, data };

    result = dlx_search(header, &rh);

    return result;
}

void dlx_free(dlx_Node *header)
{
    free(header);
}

dlx_Node *dlx_alloc(char *data, int rows, int cols)
{
    dlx_Node *header, *row, *cur;
    int r, c, count;

    /* Allocate memory for header, columns and ones */
    count = 0;
    for(r = 0; r < rows; ++r)
        for(c = 0; c < cols; ++c)
            ++count;
    header = malloc((count + cols + 1)*sizeof(dlx_Node));
    if(header == NULL)
        return NULL;

    /* Create cyclic linked list of column headers */
    for(c = 0; c < cols + 1; ++c)
    {
        header[c].left      = header + c - 1;
        header[c].right     = header + c + 1;
        header[c].up        = header[c].down = header + c;
        header[c].data.size = 0;
    }
    header[   0].left  = header + cols;
    header[cols].right = header +    0;

    /* Add data */
    row = header + cols + 1;
    for(r = 0; r < rows; ++r, data += cols)
    {
        /* Count number of elements in this row */
        count = 0;
        for(c = 0; c < cols; ++c)
            if(data[c])
                ++count;
        if(count == 0)
            continue;

        /* Create cyclinked linked list of data in this row */
        for(c = 0; c < count; ++c)
        {
            row[c].left     = row + c - 1;
            row[c].right    = row + c + 1;
            row[c].data.row = r;
        }
        row[0].left  = row + count - 1;
        row[count - 1].right = row;

        /* Insert nodes into columns */
        cur = row;
        for(c = 0; c < cols; ++c)
            if(data[c])
            {
                cur->data.row = r;
                cur->column   = header + c + 1;

                /* Insert into column */
                cur->up       = cur->column->up;
                cur->down     = cur->column;
                cur->up->down = cur;
                cur->down->up = cur;
                cur->column->data.size += 1;

                ++cur;
            }

        row += count;
    }

    header->data.max_depth = cols;

    return header;
}

