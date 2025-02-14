/* { dg-require-effective-target size32plus } */

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#endif

#define NMAX 2000

static int x[NMAX], a[NMAX][NMAX];

static int __attribute__((noinline))
mvt (long N)
{
  int i,j;

  /* These two loops should be interchanged.  */
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      x[i] += a[j][i];

  return x[1];
}

extern void abort ();

int
main (void)
{
  int i, j, res;

  for (i = 0; i < NMAX; i++)
    for (j = 0; j < NMAX; j++)
      a[i][j] = j;

  for (i = 0; i < NMAX; i++)
    x[i] = i;

  res = mvt (NMAX);

#if DEBUG
  fprintf (stderr, "res = %d \n", res);
#endif

  if (res != 2001)
    abort ();

  return 0;
}

/* PRE destroys the perfect nest and we can't cope with that yet.  */
/* { dg-final { scan-tree-dump-times "will be interchanged" 1 "graphite" } } */

