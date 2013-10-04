// tip: get cvs working with export CVS_RSH="$HOME/myssh"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "blowfish.h"
#include "cipher.h"
#ifdef __linux__
   #include <sys/vfs.h> //statfs
   #include <sys/statvfs.h>
#else
  #include <sys/param.h>//statfs
  #include <sys/mount.h>
  #include <sys/statvfs.h>
#endif

struct statvfs vfsp_ipf;
struct statvfs vfsp_opf;
struct statvfs *vs_ipf = &vfsp_ipf;
struct statvfs *vs_opf = &vfsp_opf;



// Version String
char *version_string = "$Id$";

// User Input Variables
char password_buf[16];
int cmdopt;
int d_opt;
int e_opt;
int p_opt; 
int s_opt;
int i_opt;

// File Copying Variables
char *from;
char *to;
int input_file;
int output_file;
int n;
int pgsize;

// Blowfish Variables
BF_KEY key;
int BF_ENorDE;
int schneider;
unsigned char iv[8];
char input_iv[8];

// Let's see it be global for no reason
struct stat stat_output_file;
struct stat* pointer_output_file = &stat_output_file;
struct stat lstatfillerforOPF;

int ret_val_of_stat;

// statfs for file system id
struct statfs fs_opf;
struct statfs fs_ipf;
struct statfs* fsp_opf = &fs_opf;
struct statfs* fsp_ipf = &fs_ipf;

// File system IDs
fsid_t ipt_fsid;
fsid_t opt_fsid;

// They're int's oh no..
int opt_inode_nbr;
int ipt_inode_nbr;

struct stat stat_input_file;
struct stat* pointer_input_file = &stat_input_file;
struct stat lstatfillerforINPUTFILE;

int main(int argc, char **argv){
  

  jessie();

  while ((cmdopt = getopt(argc, argv, "p:desivh")) != -1) {
     switch (cmdopt) {
     case 'd':
              d_opt = 1;
              break;

     case 'e':
              e_opt = 1;
              break;
     
     case 'v':
              printf("Version of the program is %s\n", version_string);
              break;

     case 'h':
              err_quit("The \"cipher\" program has the following usage:\n\n\tcipher [-devh] [-p PASSWD] infile outfile\n\nThe program will prompt the user to enter a password (or pass-phrase), then\nread infile, and then produce outfile.  If the -e option is given, the\nprogram should encrypt infile onto outfile, using the supplied user\npassword.  If the -d option is given, the reverse should happen: decrypt\ninfile onto outfile, using the supplied password.  Either the -d or -e\noptions (but not both) must be supplied (an exactly once).  Of course, if\nyou use the same password to encrypt and then decrypt a file, you should get\nback the same exact file data you started with.\n");
              break;
     case 'i':
              i_opt=1;
              break;
     case 'p':
              p_opt=1;
              strncpy(password_buf, optarg, 15);
              
              break;
     case 's':
              s_opt=1;
              break;
     case '?':
      default:
              printf("\n");
     }
  }



  // Need d or e and input file and output file
  if (argc < 4)
    err_quit("Not enough arguments. Please select to decrypt or encrypt and specify both an input and an output.\n");

  if(argc > 4){
    // ./cipher -e -s -p hey sss IS INVALID
    // after -p we need 3 things
    // so argc-1 argc-2 argc-3 argc-4 is -p
    int check_minus;
    if(p_opt){
      if(strcmp(*(argv+argc-4),"-p")!=0)
        err_quit("Invalid arguments. The \"cipher\" program has the following usage:\n\n\tcipher [-devh] [-p PASSWD] infile outfile\n\nThe program will prompt the user to enter a password (or pass-phrase), then\nread infile, and then produce outfile.  If the -e option is given, the\nprogram should encrypt infile onto outfile, using the supplied user\npassword.  If the -d option is given, the reverse should happen: decrypt\ninfile onto outfile, using the supplied password.  Either the -d or -e\noptions (but not both) must be supplied (an exactly once).  Of course, if\nyou use the same password to encrypt and then decrypt a file, you should get\nback the same exact file data you started with.\n");
      check_minus = argc-4;
    }else
      check_minus = argc-3;

    // If it doesnt have - after -2 or after -3 if p=1 then throw a fit
    for(; check_minus>0; check_minus--){
      if(strncmp(*(argv+check_minus),"-",1)==0){
        
      }else{
        printf("Bad, *(argv+%d) doesnt start with a -.\nThe string is %s\n", check_minus, *(argv+check_minus));
        err_quit("Invalid arguments. The \"cipher\" program has the following usage:\n\n\tcipher [-devh] [-p PASSWD] infile outfile\n\nThe program will prompt the user to enter a password (or pass-phrase), then\nread infile, and then produce outfile.  If the -e option is given, the\nprogram should encrypt infile onto outfile, using the supplied user\npassword.  If the -d option is given, the reverse should happen: decrypt\ninfile onto outfile, using the supplied password.  Either the -d or -e\noptions (but not both) must be supplied (an exactly once).  Of course, if\nyou use the same password to encrypt and then decrypt a file, you should get\nback the same exact file data you started with.\n");
      }
    }
  }

  // No decrypt and encrypt
  if ( (d_opt && e_opt) || (!d_opt && !e_opt) )
    err_quit("No encrypt and decrypt fool.\nThe \"cipher\" program has the following usage:\n\n\tcipher [-devh] [-p PASSWD] infile outfile\n\nThe program will prompt the user to enter a password (or pass-phrase), then\nread infile, and then produce outfile.  If the -e option is given, the\nprogram should encrypt infile onto outfile, using the supplied user\npassword.  If the -d option is given, the reverse should happen: decrypt\ninfile onto outfile, using the supplied password.  Either the -d or -e\noptions (but not both) must be supplied (an exactly once).  Of course, if\nyou use the same password to encrypt and then decrypt a file, you should get\nback the same exact file data you started with.\n");
  
  // Prompt for password
  if (!p_opt){
    strncpy(password_buf,getpass("Enter Password:"),15);
       
  }

  // Prompt for password a 2nd time is s_opt
  if(s_opt){
    if(strncmp(password_buf,getpass("Once again enter your password please:"),15)!=0)
      err_quit("Sorry you must type in the same password");
    printf("Thank you.\n");
  } 

  // If -i set iv
  if(i_opt){
    printf("Please enter IV: \n");
    fgets(input_iv, 8, stdin);
    
  }

  


  // Input
  if (!strcmp(*(argv+argc-2),STD))
        printf("Input is STDIN\n");
  else{
    
    // Get size of input file
    ret_val_of_stat = stat((*(argv+argc-2)), pointer_input_file);
    if(ret_val_of_stat==-1){
      fprintf(stderr, "Accessing the input file got messed up, %s\n", strerror(errno));
      exit(1);
    }

    

    // Get fsid
    #ifdef __linux__
      statvfs(*(argv+argc-2), vs_ipf);  
    #else
      statfs(*(argv+argc-2), fsp_ipf);
      ipt_fsid = fs_ipf.f_fsid;
    #endif
    
    
    // Get inode number
    ipt_inode_nbr = stat_input_file.st_ino;
    

    // Is input file a regular file?
    ret_val_of_stat = stat(*(argv+argc-2),&lstatfillerforINPUTFILE);
    if(ret_val_of_stat==-1){
      fprintf(stderr, "Accessing the input file got messed up, %s\n", strerror(errno));
      exit(1);
    }
    if(S_ISREG(lstatfillerforINPUTFILE.st_mode))
      printf("REGULAR INPUT FILE\n");
    else
      err_quit("Input file needs to exist and be a regular file.\n");

    
    // Permission to read input file?
    if ((access(*(argv+argc-2), R_OK))==-1){
      fprintf(stderr, "Accessing the input file got messed up, %s\n", strerror(errno));
      exit(1);
    }

    // Open input file with RDONLY
    if((input_file = open(*(argv+argc-2),O_RDONLY))==-1)
      fprintf(stderr, "Opening input file got messed up, %s \n", strerror(errno));
  }




  // Output
  if (!strcmp(*(argv+argc-1),STD))
        printf("Output is STDOUT\n");
  else{
    
    // Get size of output file
    ret_val_of_stat = stat((*(argv+argc-1)), pointer_output_file);
    // if(ret_val_of_stat==-1){
    //   fprintf(stderr, "Accessing the input file got messed up, %s\n", strerror(errno));
    //   exit(1);
    // }
    // printf("Size of output file is %d\n", stat_output_file.st_size);

    // Get fdin
      
    #ifdef __linux__
      statvfs(*(argv+argc-1), vs_opf);  
    #else
      statfs(*(argv+argc-1), fsp_opf);
      opt_fsid = fs_opf.f_fsid;
    #endif



    // Get inode number
    opt_inode_nbr = stat_output_file.st_ino;
    

    // Check if input and outfile file(s) are the same file
    if(opt_inode_nbr==ipt_inode_nbr){
      

      #ifdef __linux__
        if(vfsp_opf.f_fsid==vfsp_ipf.f_fsid){
          close(input_file);
          err_quit("ERROR:(linux) Input file the same as output file.\n");
        }
      #else
        if(memcmp(opt_fsid.val, ipt_fsid.val, sizeof(opt_fsid.val))==0){
          close(input_file);
          err_quit("ERROR: Input file the same as output file.\n");
        }
      #endif

      

      printf("But the fsid's aren't.\n");
    }

    // Output file a regular file?
    ret_val_of_stat = stat(*(argv+argc-1),&lstatfillerforOPF);
    // if(ret_val_of_stat==-1){
    //   fprintf(stderr, "Accessing the input file got messed up, %s\n", strerror(errno));
    //   exit(1);
    // }
    if(S_ISREG(lstatfillerforOPF.st_mode))
      printf("Regular output file.\n");
    else if(stat_output_file.st_size==0)
      printf("Output file doesn't exist.\n");
    else
      err_quit("Output file not a regular file.\n");

    
    
    // Permission to write to output file?
    int dontexistifitstwo;
    if(access(*(argv+argc-1), W_OK)==-1){
      // printf("errno is %d. If it doesn't exist is should be 2\n", errno);
      dontexistifitstwo = errno;
    }
    if (((access(*(argv+argc-1), W_OK))==-1) && (dontexistifitstwo!=2)){
      fprintf(stderr, "Accessing the output file got messed up, %s\n", strerror(errno));
      exit(1);
    }


    // Open output file with WRONLY or CREAT
    if((output_file = open(*(argv+argc-1), O_RDWR | O_CREAT, 777))==-1)
      fprintf(stderr, "Opening output file got messed up, %s \n", strerror(errno));
  }




  // Do file copying
  if((from = calloc(1,getpagesize()))==NULL)
    err_quit("Allocating memory for from failed.");
  
  if((to = calloc(1,getpagesize()))==NULL){
    free(from);
    err_quit("Allocating memory for to failed.");
  }
  
  pgsize = getpagesize();


  // Set up key
  BF_set_key(&key, 16, (unsigned char*)password_buf);


  // One or the other and "#define BF_DECRYPT  0"
  if(e_opt)
    BF_ENorDE = BF_ENCRYPT;


  // Loop through read and write
  while((n=read(input_file, from, pgsize)) == pgsize){
    
    if(i_opt==0){
      memset(iv, 0, 8);
    }else{
      memcpy(iv, input_iv, 7);
      
      if(memcmp(input_iv, iv, sizeof(iv))==0)
        printf("ignore me!\n");
    }
    BF_cfb64_encrypt((unsigned char*)from, (unsigned char*)to, pgsize, &key, iv, &schneider, BF_ENorDE);
    if(write(output_file, to, n) != n){
      printf("Partial write to output file.\n");
      free(from);
      free(to);
      if(strcmp(*(argv+argc-1),"-")!=0)
        unlink(*(argv+argc-1));
      close(output_file);
      exit(1);
    }
    if(n == -1){
      fprintf(stderr, "Writing output file got messed up, %s\n", strerror(errno));

      free(from);
      free(to);
      if(strcmp(*(argv+argc-1),"-")!=0)
        unlink(*(argv+argc-1));
      close(output_file); 
      exit(1);
    }
  }

  if(n<0){
    fprintf(stderr, "Reading input file got messed up, %s\n", strerror(errno));
    free(from);
    free(to);
    exit(1);
  }else{
    
    if(i_opt==0){
      memset(iv, 0, 8);
    }else{
      memcpy(iv, input_iv, 7);
      
      if(memcmp(input_iv, iv, sizeof(iv))==0)
        printf("ignore!\n");
    }
    BF_cfb64_encrypt((unsigned char*)from, (unsigned char*)to, pgsize, &key, iv, &schneider, BF_ENorDE);
    if(write(output_file,to,n)!=n){
      fprintf(stderr, "Writing output file got messed up, %s\n", strerror(errno));
      free(from);
      free(to);
      if(strcmp(*(argv+argc-1),"-")!=0)
        unlink(*(argv+argc-1));
      close(output_file);
      exit(1);
    }
  }




   // Close input file
  if(close(input_file)==-1){
    fprintf(stderr, "Closing input file got messed up, %s \n", strerror(errno));
    
    if(close(output_file)==-1)
      fprintf(stderr, "Closing output file got messed up too, %s \n", strerror(errno));
    
    
    free(from);
    free(to);
    exit(1);
  }
  
  // Close output file
  if(close(output_file)==-1){
    fprintf(stderr, "Closing output file got messed up, %s \n", strerror(errno));

    free(from);
    free(to);
    exit(1);
  }

  return 0;
}








void err_quit(char *errmsg){
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
