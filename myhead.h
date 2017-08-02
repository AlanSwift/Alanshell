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

#define _debug


/********parameter declare*****/
char *buf;
char**parameters;
extern const int MAXLINE;
extern const int BUFFSIZE;
extern const int MAXPARA;
extern const int MAXPID;
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

/*********function declare*****/
void input_prompt();
int read_command();
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

void clear();
void process_node_init(struct process_info**);
void list_jobs(char**);
void readinfo(struct process_info*);
short iszombis(pid_t pid);
void cleanprocess();
short exec_fg();
void ctrl_z(int sig);
void addpid(pid_t,struct info_node*p,int state);
void changestate(pid_t);
void getname(struct info_node*p);
short exec_bg();
short exec_time();
short exec_umask(char**);
short exec_environ();
short exec_set(char**);
short exec_unset(char**);
short exec_test(char**);
short test_dir(char*filename);
short test_file(char*filename,int flag);
void init_environment(int argc,char**argv);
short exec_shift(char **);
short exec_echo(char**);
char* replace_string(char*);


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

/*******hong*************/
/*******for dir**********/
#define PARA_NONE 0
#define PARA_a 1
#define PARA_l 2


//#ifndef __cplusplus
#define true 1
#define false 0
#define RUNNING 1
#define STOP 2
#define FINISH 4
#define NONSENSE 0
#define ZOMBIS 3
#define DO_JOBS 97
#define DO_UNSET 98
#define DO_SET 99
#define DO_FG 100
#define DO_BG 101
#define DO_SHIFT 102
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
#define FEXIST 0
#define FNOTEXIST 1
//#endif

struct info_node{
    char*command;
    char**paramaters;
    char*inputfile;
    char*outputfile;
    int cnt;
    short input_direct_noappend;
    short input_direct_append;
    short output_direct_noappend;
    short output_direct_append;
    short piped;
    struct info_node*next;
};

struct parse_info{
    struct info_node*info;
    int background;
};
typedef struct process_info{
    pid_t pid;
    int status;
    char*command;
    struct process_info*next;
}pro_node;

#endif