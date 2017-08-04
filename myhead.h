#ifndef ALAN_SHELL_H
#define ALAN_SHELL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>
#include <dirent.h> 
#include <linux/limits.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <grp.h>
#include <time.h>
#include "myavl.h"
#include <ctype.h>

//#define _debug


/********variables declare*****/
char *buf;//input buffer
char**parameters;//input parameters for parse
extern const int MAXLINE;//Maxline
extern const int BUFFSIZE;//the max size of input buffer
extern const int MAXPARA;//max number of parameters
extern const int MAXPID;//max number of background processes
extern const int MAXDIRLIST;
extern char*internal_list[100];
extern int row_left;
extern pid_t fgpid;
extern char *fgname;
extern char** environ;
extern AVLTree valueables;
struct passwd* pwd;
pid_t *PIDTABLE;
struct parse_info;
struct info_node;
struct parse_info *infolist;
struct process_info;
struct process_info *processlist;
struct process_info *currentprocess;
/******variables declare end****/

/*********function declare******/
void file_loop(int argc,char**);
void main_loop(char**);
void input_prompt();
int read_command(FILE*stream);
int init();
int parse_input();
void init_parse_info(struct parse_info**p);
void init_info_node(struct info_node**p);
void destroy_parse_info(struct parse_info*p);
void signal_handler(int);
short parsing(int);
short run_exec();
short run_command(char*,char**);
int run_single(struct info_node*,int in_fd,int out_fd);
short is_internal_cmd(char*command);
short set_path(char*path_cd);
short exec_pwd();
short exec_dir(char**);
short exec_display(char*folder,int flag);
int exec_displayfile(char*,int flag);
void perr(char*,char*);
int getmaxrow();

short clear();
void process_node_init(struct process_info**);
void list_jobs(char**);
void readinfo(struct process_info*);
short iszombis(pid_t pid);
void cleanprocess();
short exec_fg(pid_t);
void ctrl_z(int sig);
void addpid(pid_t,struct info_node*p,int state);
void changestate(pid_t);
void getname(struct info_node*p);
short exec_bg(pid_t);
short exec_time();
short exec_umask(char**);
short exec_environ();
short exec_set(char**);
short exec_unset(char**);
short exec_test(char**);
short test_dir(char*filename);
short test_file(char*filename,int flag);
short test_logic(char*a,char*b,int flag);
void init_environment(int argc,char**argv);
short exec_shift(char **);
short exec_echo(char**);
char* replace_string(char*);
short test_num(char*);
short exec_help();
short exec_continue();
short exec_declare(char**);
short exec_let(char**);


/********color defination******/
/*********linux only***********/
#define NONE         "\033[m"  
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"
/*****color defination end******/


/*******define options**********/
#define PARA_NONE 0
#define PARA_a 1 //-a
#define PARA_l 2 //-l


#define true 1
#define false 0

/******define status*****/
#define RUNNING 1
#define STOP 2
#define FINISH 4
#define NONSENSE 0
#define ZOMBIS 3
/**status defination end**/

/****define exit code*****/
#define DO_LET 95
#define DO_DECLARE 96
#define DO_JOBS 97
#define DO_UNSET 98
#define DO_SET 99
#define DO_FG 100
#define DO_BG 101
#define DO_SHIFT 102
#define DO_EXIT 103
#define DO_CONTINUE 104
/**exit code defination end**/

/***********for test*****/
/*********test files*****/
#define TESTr 0
#define TESTw 1
#define TESTx 2
#define TESTs 3
#define TESTe 4
#define TESTf 5
#define TESTh 6
#define TESTL 7
#define TESTc 8
#define TESTb 9
#define TESTp 10
/********test logic******/
#define TESTle 11
#define TESTlt 12
#define TESTge 13
#define TESTgt 14
#define TESTeq 15
#define TESTne 16
/******test end**********/

/******* this struct is used to store command's info*******/
struct info_node{
    char*command;//command
    char**paramaters;//the parameters like -la
    char*inputfile;//input redirection's filename
    char*outputfile;//output redirection's filename
    int cnt;//the number of parameters
    short input_direct_noappend;//flag: <
    short input_direct_append;//flag: <<
    short output_direct_noappend;//flag: >
    short output_direct_append;//flag: >>
    short piped;//whether has pipe
    struct info_node*next;//next command
};
/*******************************end*************************/

/****************one command********************************/
struct parse_info{
    struct info_node*info;//command's info
    int background;//whether it's in background
};
/*****************************end***************************/

/**************this struct is to store the process**********/
typedef struct process_info{
    pid_t pid;//pid
    int status;//status:running ect
    char*command;//command
    struct process_info*next;//next process
}pro_node;
/******************************end**************************/
#endif