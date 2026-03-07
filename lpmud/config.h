/*
 * Does the system have a getrusage call?
 */
#define RUSAGE


/*
 * Maximum number of bits in a bit field. They are stored in printable
 * strings, 6 bits per byte.
 */
#define MAX_BITS		1200	/* 200 bytes */

/*
 * There is a hash table for living objects, used by find_living().
 */
#define LIVING_HASH_SIZE	100

/*
 * Define what port number the game is to use.
 */
#define PORTNO			2000

/*
 * Set MUD_LIB to the directory which contains the mud data.
 */
#define MUD_LIB			"/home/lpmud/mudlib"

/*
 * This is the file which contains names of objects to be
 * loaded initially. Do not normally change this.
 */
#define INIT_FILE		"room/init_file"

/*
 * This is the subdirectory where all wizards objects are defined.
 */
#define PLAYER_DIR		"players"

/*
 * This is the castle that a wiz gets a copy of.
 */
#define DEFAULT_CASTLE		"room/def_castle.c"

/*
 * A command to run the C preprocessor on a C file (or really, an
 * object definition file).
 */
#define PRE_COMPILE(FILE)	execl("/usr/bin/cpp", "cpp",\
					      "-I./room",\
					      FILE, 0)

/*
 * Max number of local variables in a function.
 */
#define MAX_LOCAL	20	

/*
 * Define what ioctl to use against tty's.
 */

#define USE_TIOCGETP		/* BSD */
/* #define USE_TCGETA */ 	/* SYSV */

/* Maximum number of evaluated nodes/loop.
 * If this is exceeded, current function is halted.
 * The worst case yet encountered is 3600
 */
#define MAX_COST	(5 * 3600)

/* Use fork or vfork to exec a program.
 * Default is fork().
 */

/* #define USE_VFORK */

/*
 * The following define is used when debuging the memory
 * allocation.
 */

/* #define free(x) xfree(x) */

/*
 * Objects are swapped out if they are not used.
 * Specify how many resets the object can stay non-referenced until
 * it is swapped out.
 * A value if '1' will make swap out the object at every reset.
 * A value of '0' will never swap out the object.
 * The count will be cleared by either call_other() or move_object().
 */

#define NUM_RESET_TO_SWAP	2

/*
 * Where to swap out objects. This file is not used if NUM_RESET_TO_SWAP
 * is 0.
 */
#define SWAP_FILE		"/home/lpmud/LP_SWAP"

/*
 * To speed the start of the game up, i-files are saved. This
 * requires more disk space (a lot more). If the i-file is not
 * wanted, define this next symbol.
 */
/* #define REMOVE_I_FILES */

/*
 * This is the maximum array size allowed for one single array.
 */
#define MAX_ARRAY_SIZE 1000

/*
 * Some LPmuds on sun4 and sparcstations have had problems with the
 * call of inet_ntoa() in comm1.c.
 * If the game crash in query_ip_number() wen using inet_ntoa(),
 * then undefine the following symbol.
 * The query_ip_number() is called when doing the 'peolpe' command
 * for example.
 */
#define INET_NTOA_OK

/*
 * Define LOG_SHOUT if you want all shouts to be logged in
 * mudlib/log/SHOUTS.
 */
#define LOG_SHOUT

/*
 * Define LOG_FORCE if you want all forcing to be logged in
 * mudlib/log/FORCE.
 */
#define LOG_FORCE

/*
 * Maximum number of players in the game.
 */
#define MAX_PLAYERS	40

/*
 * When uploading files, we want fast response; however, normal players
 * shouldn't be able to hog the system in this way.  Define ALLOWED_ED_CMDS
 * to be the ratio of the no of ed cmds executed per player cmd, and
 * MAX_CMDS_PER_BEAT to be the bax no of buffered player commands the
 * system will accept in each heartbeat interval.
 */

#define	ALLOWED_ED_CMDS		20
#define	MAX_CMDS_PER_BEAT	5 /* not implemented yet :-( */

/*
 * Reserve an extra memory area from malloc(), to free when we run out
 * of memory to get some warning and start Armageddon.
 * If this value is 0, no area will be reserved.
 */
#define RESERVED_SIZE		400000

/* Define the size of the shared string hash table.  This number should
 * a prime, probably between 1000 and 30000; if you set it to about 1/5
 * of the number of distinct strings you have, you will get a hit ratio
 * (number of comparisons to find a string) very close to 1, as found strings
 * are automatically moved to the head of a hash chain.  You will never
 * need more, and you will still get good results with a smaller table.
 * THIS IS NOT IMPLEMENTED YET.
 */

#define	HTABLE_SIZE	2203	/* there is a table of some primes too */

/*
 * Object hash table size.
 * Define this like you did with the strings; probably set to about 1/4 of
 * the number of objects in a game, as the distribution of accesses to
 * objects is somewhat more uniform than that of strings.
 */

#define OTABLE_SIZE	1009	/* we have several thousand obs usually */
   
/************************************************************************/
/*	END OF CONFIG -- DO NOT ALTER ANYTHING BELOW THIS LINE		*/
/************************************************************************/

/*
 * some generic large primes used by various hash functions in different files
 * You can alter these if you know of a better set of numbers!  Be sure
 * they are primes...
 */

#define	P1		701	/* 3 large, different primes */
#define	P2		14009	/* There's a file of them here somewhere :-) */
#define	P3		54001
