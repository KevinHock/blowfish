// error check syscalls and malloc
// ?how to test same file?
// autoupdate variables $Revision$ or $ld$

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>    //For getpass(3)
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "blowfish.h"

void err_quit(char *errmsg){
  fputs(errmsg, stderr);
  fflush(NULL);
  exit(1);
}

int main(int argc, char **argv)
{
  printf("Hello World\n");
  int ch, dflag=0, eflag=0, pflag=0; 
  char* password;

  while ((ch = getopt(argc, argv, "p:devh")) != -1) {
     switch (ch) {
     case 'd':
              dflag = 1;
              break;

     case 'e':
              eflag = 1;
              break;
     
     case 'v':
            //    If the -v option is given, the program should print the version string of the program (which you can take from RCS auto-update variables such as $Revision$ or $Id$).

             break;


     case 'h':
              fprintf(stderr, "The \"cipher\" program has the following usage:\n\n\tcipher [-devh] [-p PASSWD] infile outfile\n\nThe program will prompt the user to enter a password (or pass-phrase), then\nread infile, and then produce outfile.  If the -e option is given, the\nprogram should encrypt infile onto outfile, using the supplied user\npassword.  If the -d option is given, the reverse should happen: decrypt\ninfile onto outfile, using the supplied password.  Either the -d or -e\noptions (but not both) must be supplied (an exactly once).  Of course, if\nyou use the same password to encrypt and then decrypt a file, you should get\nback the same exact file data you started with.\n");
              exit(1);
              break;
     case 'p':
              password = malloc(strlen(optarg+1));
              if(!password)
                err_quit("malloc failed");
              strcpy(password, optarg);
              printf("password is %s\n", password);
              pflag=1;
              break;
     case '?':
     default:
              printf("blag\n");
     }
  }

  if (dflag && eflag){
    printf("No encrypt and decrypt fool.\n");
    exit(1);
  }
  
  if (pflag==0){
    password = malloc(_PASSWORD_LEN+1);
    strcpy(password,getpass("Enter Password:"));
    printf("Password is %s\n", password);   
  }

  if (!strcmp(*(argv+argc-2),"-")){
        printf("fdin is stdin\n");
  }else{
    //lstat 2 FOR SYMBOLIC I BELIEVE
    //stat 2
    struct stat inputfile;
    struct stat* ptif = &inputfile;
    stat((*(argv+argc-2)), ptif);
    printf("%lld\n", inputfile.st_size);

    struct stat buf;
    lstat(*(argv+argc-2),&buf);
    if(S_ISREG(buf.st_mode)){
      printf("REG FIEL\n");
    }
    // Check if readable
    // Permission to read to
    int accessrv = access(*(argv+argc-2), R_OK);
    if (accessrv==-1){
      fprintf(stderr, "Oh dear, something went wrong with a()! %s\n", strerror(errno));
    }
    // Make sure it's a file and not anything special
    // What could go wrong with open and read
    printf("ALL GOOD I GUESS\n");
    open(*(argv+argc-2),O_RDONLY,0);
  }

  if (!strcmp(*(argv+argc-1),"-")){
        printf("fdout is stdout\n");
  }else{
    // Check if readable
    // Permission to write to
    // Make sure it's a file and not anything special
    // What could go wrong with open and write
    //lstat 2 FOR SYMBOLIC I BELIEVE
    //stat 2
    struct stat outputfile;
    struct stat* ptof = &outputfile;
    stat((*(argv+argc-2)), ptof);
    printf("ALL GOOD I GUESS\n");
    printf("%lld\n", outputfile.st_size);
    open(*(argv+argc-1),O_WRONLY,0);
  }

  int pgsize = getpagesize();
  // calloc the last page and make it the mode of page size into file size


  char from[128]; 
  char to[128];
  int len = 128;
  strcpy(from, "AAAAAAAAA");
  /* define a structure to hold the key */
  BF_KEY key;

  /* a temp buffer to read user input (the user's password) */
  char temp_buf[16] = "password";

  /* don't worry about these two: just define/use them */
  int n = 0;			/* internal blowfish variables */
  unsigned char iv[8];		/* Initialization Vector */

  /* fill the IV with zeros (or any other fixed data) */
  memset(iv, 0, 8);

  //  call this function once to setup the cipher key 
  BF_set_key(&key, 16, temp_buf);
  

  //  * This is how you encrypt an input char* buffer "from", of length "len"
  //  * onto output buffer "to", using key "key".  Jyst pass "iv" and "&n" as
  //  * shown, and don't forget to actually tell the function to BF_ENCRYPT.
  
  BF_cfb64_encrypt(from, to, 128, &key, iv, &n, BF_ENCRYPT);
  printf("from is %s\n", from);
  printf("TO IS %s\n", to);
  /* Decrypting is the same: just pass BF_DECRYPT instead */
  BF_cfb64_encrypt(to, from, 128, &key, iv, &n, BF_DECRYPT);

  printf("from is %s\n", from);
  printf("TO IS %s\n", to);

}
