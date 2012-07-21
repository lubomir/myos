/*
 * string.h -- define interface to basic string and memory functions
 */

/*
 * Fill the first len bytes of the memory starting at dest with byte val.
 */
void * memset(void *dest, int val, int len);

/*
 * Copy len bytes from memory area src to memory area dest.
 * The memory must not overlap.
 */
void * memcpy(void *dest, const void *src, int len);

/*
 * Compare strings s1 and s2. This function returns integer less than,
 * equal to, or greater than zero if s1 is found, respectively, to be less
 * than, to match, or be greater than s2.
 */
int strcmp(const char *s1, const char *s2);

/*
 * Copy the string src including the terminating null byte ('\0') to the
 * buffer pointed to by dest. The strings may not overlap, and the destination
 * string dest must be large enough.
 */
char * strcpy(char *dest, const char *src);

/*
 * Append the src string to the dest string, overwriting the terminating null
 * byte ('\0') at the end of dest, and then adds a terminating null byte.
 * The strings may not overlap, and the dest string must be large enough.
 */
char * strcat(char *dest, const char *src);

/*
 * Count byte length of of the string s excluding the terminating null byte.
 */
unsigned int strlen(const char s);
