#include "myhead.h"
const int MAXLINE=1024;
const int BUFFSIZE=1024;
const int MAXPARA=1024;
const int MAXPID=512;
char*internal_list[]={"cd","pwd","quit"};
void signal_handler(int sig)
{
    pid_t pid;
    for(int i=0;i<MAXPID;i++)
    {
        if(PIDTABLE[i]!=0)
        {
            pid=waitpid(PIDTABLE[i],NULL,WNOHANG);
            if(pid>0)
            {
                printf("Process %d exited.\n",pid);
                PIDTABLE[i]=0;
            }
            else{
                if(errno!=ECHILD)
                {
                    printf("Error pid: %d.\n",pid);
                }
            }
        }
    }
    return;
}
int init()
{
    
    if(( buf = ( char* ) malloc( sizeof( char ) * BUFFSIZE ) ) == NULL )
    {
        return 0;
    }
    if(( parameters = (char**) malloc( sizeof( char* ) * MAXPARA ) ) == NULL )
    {
        return 0;
    }
    if((PIDTABLE=(pid_t*)malloc(sizeof(pid_t)*MAXPID))==NULL)
    {
        return 0;
    }
    else{
        memset(PIDTABLE,0,sizeof(pid_t)*MAXPID);
    }
    if( (infolist=(struct parse_info*)malloc(sizeof(struct parse_info))) == NULL )
    {
        return 0;
    }
    
    return 1;
}
void init_parse_info(struct parse_info**p)
{
    (*p)->background=0;
    (*p)->info=NULL;
}

void init_info_node(struct info_node**p)
{
    if((*p)!=NULL) return;
    *p=(struct info_node*)malloc(sizeof(struct info_node));
    (*p)->cnt=0;
    (*p)->command=NULL;
    (*p)->paramaters=(char**)malloc(sizeof(char*)*MAXPARA);
    memset((*p)->paramaters,0,sizeof(char*)*MAXPARA);
    (*p)->inputfile=NULL;
    (*p)->outputfile=NULL;
    (*p)->input_direct_noappend=0;
    (*p)->input_direct_append=0;
    (*p)->output_direct_noappend=0;
    (*p)->output_direct_append=0;
    (*p)->piped=0;
    (*p)->next=NULL;
    printf("************%d\n",(*p)->cnt);
}

void destroy_parse_info(struct parse_info*p)
{
    if(p==NULL)
    {
        return;
    }
    struct info_node*pn=p->info,*pnn;
    while(pn!=NULL)
    {
        pnn=pn->next;
        free(pn->paramaters);
        free(pn);
        pn=pnn;
    }
}