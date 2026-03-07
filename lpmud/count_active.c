#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>

/*
 * Count the number of active players.
 * A players is supposed to be active, if he has logged in the last
 * 30 days.
 */

int main(int argc, char **argv)
{
    struct stat st;
    int i, num;
    time_t now;

    now = time(0L);
    for (i=1, num=0; i<argc; i++) {
	if (stat(argv[i], &st) == -1)
	    continue;
	if (now - st.st_mtime < 30 * 24 * 60 * 60)
	    num++;
    }
    printf("%d\n", num);
    return 0;
}
