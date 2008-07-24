#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dlx.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

typedef struct Point
{
    int x, y, z;
} Point;

typedef struct Bounds
{
    Point min, max;
} Bounds;

typedef struct Piece
{
    int   id;       /* an arbitrary identier - used to display solutions */
    int   pts;      /* number of points in this piece - 4 or 5 */
    Point pt[5];    /* the points */
} Piece;

const char *piece_shapes[13] = { /* rotations   placings */
    "###......#..#.....",        /*     24          432  */
    "####......#.......",        /*     24          432  */
    "###.#........#....",        /*     24          432  */
    "##.#......##......",        /*     24          432  */
    "###..#.....#......",        /*     24          432  */
    ".####........#....",        /*     24          432  */
    ".#.####...........",        /*     24          384  */
    "##........#..#....",        /*     12          324  */
    ".#.##.....##......",        /*     12          216  */
    "####.......#......",        /*     12          216  */
    "###.#.....#.......",        /*     12          216  */
    ".####.#...........",        /*     12          192  */
    ".#.###.#.........." };      /*      3           48  */


int rotate[3][9] = {
    { +1,  0,  0,
       0,  0, -1,
       0, +1,  0 },
    {  0,  0, +1,
       0, +1,  0,
      -1,  0,  0 },
    {  0, -1,  0,
      +1,  0,  0,
       0,  0, +1 } };


static Piece row_info[9999];
static char problem[9999][77];


Bounds bounds(Piece *piece)
{
    Bounds b = { { +999999, +999999, +999999 }, { -9999999, -9999999, -999999 } };
    int n;

    for(n = 0; n < piece->pts; ++n)
    {
        b.min.x = MIN(b.min.x, piece->pt[n].x);
        b.max.x = MAX(b.max.x, piece->pt[n].x);
        b.min.y = MIN(b.min.y, piece->pt[n].y);
        b.max.y = MAX(b.max.y, piece->pt[n].y);
        b.min.z = MIN(b.min.z, piece->pt[n].z);
        b.max.z = MAX(b.max.z, piece->pt[n].z);
    }

    return b;
}

void transform(Piece *piece, int *matrix)
{
    int n, x, y, z;

    for(n = 0; n < piece->pts; ++n)
    {
        x = matrix[0]*piece->pt[n].x + matrix[1]*piece->pt[n].y + matrix[2]*piece->pt[n].z;
        y = matrix[3]*piece->pt[n].x + matrix[4]*piece->pt[n].y + matrix[5]*piece->pt[n].z;
        z = matrix[6]*piece->pt[n].x + matrix[7]*piece->pt[n].y + matrix[8]*piece->pt[n].z;
        piece->pt[n].x = x;
        piece->pt[n].y = y;
        piece->pt[n].z = z;
    }
}

int cmp_points(const void *a, const void *b)
{
    Point *p = (Point *)a, 
          *q = (Point *)b;
    return (p->x == q->x) ? (p->y - q->y) : (p->x - q->x);
}

int count_solutions(int rows_size, int *rows, void *data)
{
    ++*(int*)data;
    return 0;
}

int print_solutions(int rows_size, int *rows, void *data)
{
    int r, n;
    char template[] = ".... .... .... ....\n"
                      ".... .... .... ....\n"
                      ".... .... .... ....\n"
                      ".... .... .... ....\n";

    for(r = 0; r < rows_size; ++r)
        for(n = 0; n < row_info[rows[r]].pts; ++n)
        {
            template[    row_info[rows[r]].pt[n].x +
                      20*row_info[rows[r]].pt[n].y +
                       5*row_info[rows[r]].pt[n].z ] = 'A' + row_info[rows[r]].id;
        }

    puts(template);

    ++*(int*)data;

    return 0;
}

int main()
{
    int r, s, n, a, b, c, x, y, z, rots, rows, solutions;
    Piece pc, rot[64];
    Bounds bnds;
    struct dlx_Node *header;

    rows = 0;
    for(s = 0; s < 13; ++s)
    {
        memset(&pc, 0, sizeof(pc));

        /* Parse piece */
        for(z = 0; z < 2; ++z)
            for(y = 0; y < 3; ++y)
                for(x = 0; x < 3; ++x)
                    if(piece_shapes[s][9*z + 3*y + x] == '#')
                    {
                        pc.pt[pc.pts].x = x;
                        pc.pt[pc.pts].y = y;
                        pc.pt[pc.pts].z = z;
                        ++pc.pts;
                    }

        /* Generate all rotations of piece */
        rots = 0;
        
        for(a = 0; a < (s ? 4 : 1); ++a)
        {
            transform(&pc, rotate[0]);
            for(b = 0; b < (s ? 4 : 1); ++b)
            {
                transform(&pc, rotate[1]);
                for(c = 0; c < (s ? 4 : 1); ++c)
                {
                    transform(&pc, rotate[2]);
                    
                    /* Calculate dimensions, then translate to origin */
                    bnds = bounds(&pc);
                    for(n = 0; n < pc.pts; ++n)
                    {
                        pc.pt[n].x -= bnds.min.x;
                        pc.pt[n].y -= bnds.min.y;
                        pc.pt[n].z -= bnds.min.z;
                    }

                    /* Order points for comparison */
                    qsort(pc.pt, pc.pts, sizeof(Point), cmp_points);

                    /* Compare with previous1ly generated pieces */
                    for(n = 0; n < rots; ++n)
                        if(memcmp(&rot[n], &pc, sizeof(pc)) == 0)
                            break;
                    if(n < rots)
                        continue; /* duplicate piece found! */
                    rot[rots++] = pc;

                    /* Generate rows */
                    for(x = 0; x < 4 - (bnds.max.x - bnds.min.x); ++x)
                    {
                        for(y = 0; y < 4 - (bnds.max.y - bnds.min.y); ++y)
                        {
                            for(z = 0; z < 4 - (bnds.max.z - bnds.min.z); ++z)
                            {
                                row_info[rows].id  = s;
                                row_info[rows].pts = pc.pts;
                                for(n = 0; n < pc.pts; ++n)
                                {
                                    row_info[rows].pt[n].x = pc.pt[n].x + x;
                                    row_info[rows].pt[n].y = pc.pt[n].y + y;
                                    row_info[rows].pt[n].z = pc.pt[n].z + z;
                                }
                                ++rows;
                            }
                        }
                    }
                }
            }
        }
    }

    /* Generate DLX matrix */
    memset(problem, 0, sizeof(problem));
    for(r = 0; r < rows; ++r)
    {
        for(n = 0; n < row_info[r].pts; ++n)
            problem[r][16*row_info[r].pt[n].x + 4*row_info[r].pt[n].y + row_info[r].pt[n].z] = 1;
        problem[r][64 + row_info[r].id] = 1;
    }

    /* Solve problem */
    header = dlx_alloc((char*)problem, rows, 77);
    solutions = 0;
    dlx_solve(header, print_solutions, &solutions);
    dlx_free(header);
    
    printf("Total number of solutions found: %d\n", solutions);

    return 0;
}
