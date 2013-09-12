// do file stuff without blowfish
// check at least 2 args first
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
#include "cipher.h"

int ipf=0;

int main(int argc, char **argv){
  int ch, dflag=0, eflag=0, pflag=0; 
  char* password;

  jessie();
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
              if(!(password = malloc(strlen(optarg+1))))
                malloc_fail(__LINE__);  
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
    if(!(password = malloc(_PASSWORD_LEN+1)))
      malloc_fail(__LINE__);
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
    ipf = open(*(argv+argc-2),O_RDONLY,0);
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
  char *hehe;
  hehe = malloc(128);
  int amt_read = read(ipf,from,128);
  if(amt_read != 128){
    // Partial Read
  }
  printf("from is %s.\nfrom length is %lu", from, strlen(from));

  // while(n=read(STDIN_FILENO, buf, BUFFSIZE) == 128){
    // if(write(STDOUT_FILENO, buf, n) != n)
      // fuck;
  // }

  // if(n>0)
    // readfails;
  

  char to[128];
  int len = 128;
  int i;
  
  strcpy(from, "A");
  for(i=0; i<126;i++)
    strcat(from, "A");



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
  BF_cfb64_encrypt(from, to, 128, &key, iv, &n, BF_DECRYPT);

  printf("from is %s\n", from);
  printf("TO IS %s\nto.length is %lu", to, strlen(to));


  /*
   * This is how you encrypt an input char* buffer "from", of length "len"
   * onto output buffer "to", using key "key".  Jyst pass "iv" and "&n" as
   * shown, and don't forget to actually tell the function to BF_ENCRYPT.
   */
  // BF_cfb64_encrypt(from, to, len, &key, iv, &n, BF_ENCRYPT);

  /* Decrypting is the same: just pass BF_DECRYPT instead */
  // BF_cfb64_encrypt(from, to, len, &key, iv, &n, BF_DECRYPT);

   printf("lol %lu\n", strlen("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
  return 0;
}























void err_quit(char *errmsg, int linenumber){
  fputs(errmsg, stderr);
  fflush(NULL);
  exit(1);
}

void jessie(){
  printf("          _______  _______  _______  _______  _______  _______          \n");
  printf("|\\     /|(  ____ \\(  ____ \\(  ____ \\(  ___  )(  ___  )(  ___  )|\\     /|\n");
  printf("( \\   / )| (    \\/| (    \\/| (    \\/| (   ) || (   ) || (   ) || )   ( |\n");
  printf(" \\ (_) / | (__    | (__    | (__    | (___) || (___) || (___) || (___) |\n");
  printf("  \\   /  |  __)   |  __)   |  __)   |  ___  ||  ___  ||  ___  ||  ___  |\n");
  printf("   ) (   | (      | (      | (      | (   ) || (   ) || (   ) || (   ) |\n");
  printf("   | |   | (____/\\| (____/\\| (____/\\| )   ( || )   ( || )   ( || )   ( |\n");
  printf("   \\_/   (_______/(_______/(_______/|/     \\||/     \\||/     \\||/     \\|\n");
  printf("                                                                        \n");
  printf(" ______   _        _______          \n");
  printf("(  ___ \\ ( \\      (  ___  )|\\     /|\n");
  printf("| (   ) )| (      | (   ) || )   ( |\n");
  printf("| (__/ / | |      | |   | || | _ | |\n");
  printf("|  __ (  | |      | |   | || |( )| |\n");
  printf("| (  \\ \\ | |      | |   | || || || |\n");
  printf("| )___) )| (____/\\| (___) || () () |\n");
  printf("|/ \\___/ (_______/(_______)(_______)\n");
  printf("                                    \n");
  printf(" _______ _________ _______          _________ _       \n");
  printf("(  ____ \\\\__   __/(  ____ \\|\\     /|\\__   __/( (    /|\n");
  printf("| (    \\/   ) (   | (    \\/| )   ( |   ) (   |  \\  ( |\n");
  printf("| (__       | |   | (_____ | (___) |   | |   |   \\ | |\n");
  printf("|  __)      | |   (_____  )|  ___  |   | |   | (\\ \\) |\n");
  printf("| (         | |         ) || (   ) |   | |   | | \\   |\n");
  printf("| )      ___) (___/\\____) || )   ( |___) (___| )  \\  |\n");
  printf("|/       \\_______/\\_______)|/     \\|\\_______/|/    )_)\n");
  printf("                                                      \n");
  printf("_________         _________ _______             _______  _ \n");
  printf("\\__   __/|\\     /|\\__   __/(  ____ \\  |\\     /|(  ____ )( )\n");
  printf("   ) (   | )   ( |   ) (   | (    \\/  | )   ( || (    )|| |\n");
  printf("   | |   | (___) |   | |   | (_____   | |   | || (____)|| |\n");
  printf("   | |   |  ___  |   | |   (_____  )  | |   | ||  _____)| |\n");
  printf("   | |   | (   ) |   | |         ) |  | |   | || (      (_)\n");
  printf("   | |   | )   ( |___) (___/\\____) |  | (___) || )       _ \n");
  printf("   )_(   |/     \\|\\_______/\\_______)  (_______)|/       (_)\n");
  printf("                                                           \n");
}

void malloc_fail(int linenumber){
  err_quit("malloc fail @ line %d \n",linenumber);
}


