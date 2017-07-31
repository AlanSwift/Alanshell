#include "myhead.h"
const int max_len_prompt=100;

void replacehome(char*path,char*home)
{
    char buf[100];
    if(strlen(path)<strlen(home))
    {
        return;
    }
    if(strncmp(path,home,strlen(home))==0)
    {
        memset(buf,0,sizeof(buf));
        buf[0]='~';
        strcpy(buf+1,path+strlen(home));
        strcpy(path,buf);
        return;
    }
    else{
        return;
    }
}

void input_prompt()
{
    char path[max_len_prompt];
    char username[max_len_prompt];
    int uid=1;

    

    getcwd(path,max_len_prompt);
    gethostname(username,max_len_prompt);
    uid=geteuid();
    replacehome(path,pwd->pw_dir);
    printf(BROWN"[Alan's shell]"LIGHT_GREEN"%s@%s"WHITE":"LIGHT_BLUE"%s"WHITE"%c"NONE" ",pwd->pw_name,username,path,!uid?'#':'$');


}