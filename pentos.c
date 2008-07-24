#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dlx.h"

#define MINS(v,x) do { if(x<v) v=x; } while(0)
#define MAXS(v,x) do { if(x>v) v=x; } while(0)

typedef struct Point
{
    int x, y;
} Point;

typedef struct Pentomino
{
    char  id;
    Point points[5];
} Pentonimo;

Pentonimo pentominos[12] = {
    { 'F', { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 2, 2 } } },
    { 'I', { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 4, 0 } } },
    { 'L', { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 3, 1 } } },
    { 'P', { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 1, 1 }, { 2, 1 } } },
    { 'N', { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 2, 1 }, { 3, 1 } } },
    { 'T', { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 1, 1 }, { 1, 2 } } },
    { 'U', { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 0, 1 }, { 2, 1 } } },
    { 'V', { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 2, 1 }, { 2, 2 } } },
    { 'W', { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 2, 1 }, { 2, 2 } } },
    { 'X', { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 1, 2 } } },
    { 'Y', { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 2, 1 } } },
    { 'Z', { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 2, 2 } } } };

int xforms[8][4] = {
    { +1,  0,
       0, +1 },
    {  0, +1,
      +1,  0 },
    { -1,  0,
       0, +1 },
    {  0, -1,
      +1,  0 },
    { +1,  0,
       0, -1 },
    {  0, +1,
      -1,  0 },
    { -1,  0,
       0, -1 },
    {  0, -1,
      -1,  0 } };

int cmp_points(const void *a, const void *b)
{
    Point *p = (Point *)a, 
          *q = (Point *)b;
    return (p->x == q->x) ? (p->y - q->y) : (p->x - q->x);
}

static Pentonimo row_info[4096];

int callback(int rows_size, int *rows, void *data)
{
    int n, p;
    *((int*)data) += 1;
    char layout[6][11];
    for(n = 0; n < rows_size; ++n)
        for(p = 0; p < 5; ++p)
        {
            Pentonimo *shape = &row_info[rows[n]];
            layout[shape->points[p].y][shape->points[p].x] = shape->id;
        }
    for(n = 0; n < 6; ++n)
    {
        layout[n][10] = '\0';
        printf("%s\n", layout[n]);
    }
    printf("\n");
	
/*
    char layout[8][9];
    layout[3][3] = layout[3][4] = layout[4][3] = layout[4][4] = '.';
    for(n = 0; n < rows_size; ++n)
        for(p = 0; p < 5; ++p)
        {
            Pentonimo *shape = &row_info[rows[n]];
            layout[shape->points[p].y][shape->points[p].x] = shape->id;
        }

    for(n = 0; n < 8; ++n)
    {
        layout[n][8] = '\0';
        printf("%s\n", layout[n]);
    }
    printf("\n");
*/

    return 0;
}

int main()
{
    int count = 0;
    int s, t, n, width, height, tx, ty;
    Point points[8][5];
    char  problem[4096][72];

    for(s = 0; s < 12; ++s)
    {
        for(t = 0; t < 8; ++t)
        {
            /* Transform pentomino */
            for(n = 0; n < 5; ++n)
            {
                points[t][n].x = xforms[t][0]*pentominos[s].points[n].x
                               + xforms[t][1]*pentominos[s].points[n].y;
                points[t][n].y = xforms[t][2]*pentominos[s].points[n].x
                               + xforms[t][3]*pentominos[s].points[n].y;
            }

            /* Normalize coordinates */
            qsort(points[t], 5, sizeof(Point), cmp_points);

            /* Translate and calculate dimensions */
            {
                int minx=+999999, miny=+999999,
                    maxx=-999999, maxy=-999999;

                for(n = 0; n < 5; ++n)
                {
                    MINS(minx, points[t][n].x);
                    MAXS(maxx, points[t][n].x);
                    MINS(miny, points[t][n].y);
                    MAXS(maxy, points[t][n].y);
                }

                if(minx || miny)
                {
                    for(n = 0; n < 5; ++n)
                    {
                        points[t][n].x -= minx;
                        points[t][n].y -= miny;
                    }
                }

                width  = 1 + maxx - minx;
                height = 1 + maxy - miny;
            }

            /* Check if this transform yields a shape previously found... */
            for(n = 0; n < t; ++n)
                if(memcmp(points[t], points[n], sizeof(*points)) == 0)
                    break;
            if(n < t)
                continue; /* duplicate shape found! */

            /* Place in grid */
            for(tx = 0; tx <= 10 - width;  ++tx)
            for(ty = 0; ty <=  6 - height; ++ty)
            {
/*
                for(n = 0; n < 5; ++n)
                    if( (points[t][n].x + tx == 3 || points[t][n].x + tx == 4) &&
                        (points[t][n].y + ty == 3 || points[t][n].y + ty == 4) )
                    {
                        break;
                    }
                if(n < 5)
                    continue;
*/

                memset(problem[count], 0, sizeof(problem[count]));
                problem[count][s] = 1;
                row_info[count].id = pentominos[s].id;
                for(n = 0; n < 5; ++n)
                {
                    int x = points[t][n].x + tx,
                        y = points[t][n].y + ty,
                        p = 10*y + x;
/*
                    if(p >= 37) p -= 2;
                    if(p >= 29) p -= 2;
*/
                    problem[count][p + 12] = 1;

                    row_info[count].points[n].x = x;
                    row_info[count].points[n].y = y;
                }
                ++count;
            }
        }
    }

    {
        int solutions = 0;
        struct dlx_Node *header = dlx_alloc((char*)problem, count, 72);
        printf("dlx_search(): %d\n", dlx_solve(header, callback, &solutions));
        dlx_free(header);
        printf("Total number of solutions found: %d\n", solutions);
    }

    /*
    printf("ASSERT(%d == %d)\n", count, (int)(sizeof(row_info)/sizeof(*row_info)));
    */

    return 0;
}
