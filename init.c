#include "myhead.h"
const int MAXLINE=1024;
const int BUFFSIZE=1024;
const int MAXPARA=1024;
const int MAXPID=512;
const int MAXDIRLIST=100;
char*internal_list[]={"cd","pwd","exit","dir","clear","jobs","fg","bg","time","umask","environ","set","unset","exec","test","shift","echo","help","continue","declare","let"};
char *fgname;
int row_left;
pid_t fgpid=-1;
AVLTree valueables;
short iszombis(pid_t pid)
{
    DIR*dir=NULL;
    FILE*fp=NULL;
    char buf[100];
    sprintf(buf,"/proc/%d/stat",pid);
    if( (fp=fopen(buf,"r"))==NULL )
    {
        return false;
    }
    else{
        int pid,ppid;char c;
        fscanf(fp,"%d %s %c %d",&pid,buf,&c,&ppid);
        if(c=='Z')
        {
            return true;
        }
        else{
            return false;
        }
    }
}
void signal_handler(int sig)
{
    int status;  
    pid_t pid;  
  
    while( (pid = waitpid(-1,&status,WNOHANG)) > 0)  
    {  
        if ( WIFEXITED(status) )  
        {  
            printf("child process revoked. pid[%6d], exit code[%d]\n",pid,WEXITSTATUS(status));
            changestate(pid);
        }  
        else  
            printf("child process revoked.but ...\n");  
    } 
}

void ctrl_z(int sig)
{
    printf("press %d\n",fgpid);
    if(fgpid==-1)
    {
        return;
    }
    else{
        if(kill(fgpid,SIGSTOP)==0)
        {
            addpid(fgpid,NULL,STOP);
            int i;
            for(i=0;i<MAXPID;i++)
            {
                if(PIDTABLE[i]==0)
                {
                    PIDTABLE[i]=fgpid;
                    break;
                }
            }
            if(i==MAXPID)
            {
                perror("System error: Process amount limit exceeded.");
            }
            fgpid=-1;
        }
    }
}
void changestate(pid_t pid)
{
    pro_node*p=processlist;
    for(;p->next!=NULL;p=p->next)
    {
        if(p->next->pid==pid)
        {
            p->next->status=FINISH;
            break;
        }
    }
}
void addbypid(pid_t pid,int mode)
{
    int f=0;
    for(f=0;f<MAXPID;f++)
    {
        if(PIDTABLE[f]==0)
        {
            PIDTABLE[f]=pid;
            break;
        }
    }
    if(f==MAXPID)
    {
        perror("Warning: Process amount limit exist. There are zombis.");
    }

    



    pro_node*pp=processlist;
    while(pp->next!=NULL)
    {
        pp=pp->next;
    }
    pp->next=(pro_node*)malloc(sizeof(pro_node));
    pp->next->next=NULL;
    pp->next->pid=pid;
    pp=pp->next;
    pp->command=(char*)malloc(sizeof(char)*(1000));
    strcpy(pp->command,fgname);
    pp->status=mode;

    DIR*dir=NULL;
    FILE*fp=NULL;
    struct dirent*dirinfo=NULL;
    struct stat buf;
    char*path=(char*)malloc(sizeof(char)*100);
    sprintf(path,"/proc/%d/",pid);
    if( (dir=opendir(path))==NULL )
    {
        pp->status=FINISH;
    }
    else{
        sprintf(path,"/proc/%d/stat",pid);
        if( (fp=fopen(path,"r"))!=NULL )
        {
            int fpid,ppid;
            char bbuf[100];
            char state;
            fscanf(fp,"%d %s %c %d",&fpid,pp->command,&state,&ppid);
        }
        else{
            pp->status=FINISH;
        }
        fclose(fp);
        
    }
    free(path);
    return ;
}
void getname(struct info_node*ptr)
{
    int cnt=0;
    for(int i=0;i<ptr->cnt;i++)
    {
        cnt+=(strlen(ptr->paramaters[i])+1);
    }
    cnt=0;
    for(int i=0;i<ptr->cnt;i++)
    {
        strcpy(fgname+cnt,ptr->paramaters[i]);
        cnt+=strlen(ptr->paramaters[i])+1;
        fgname[cnt-1]=' ';
        fgname[cnt]='\0';
    }
}
void addpid(pid_t pid,struct info_node*ptr,int mode)
{
    //pro_node*p=processlist->next;
    //for(;p->next!=NULL;p=p->next);
    //process_node_init(&p->next);
    if(ptr==NULL)
    {
        return addbypid(pid,mode);
    }

    int f=0;
    for(f=0;f<MAXPID;f++)
    {
        if(PIDTABLE[f]==0)
        {
            PIDTABLE[f]=pid;
            break;
        }
    }
    if(f==MAXPID)
    {
        perror("Warning: Process amount limit exist. There are zombis.");
    }
    pro_node*pp=processlist;
    while(pp->next!=NULL)
    {
        pp=pp->next;
    }
    pp->next=(pro_node*)malloc(sizeof(pro_node));
    pp->next->next=NULL;
    pp->next->pid=pid;
    int cnt=0;
    for(int i=0;i<ptr->cnt;i++)
    {
        cnt+=(strlen(ptr->paramaters[i])+1);
    }
    pp=pp->next;
    pp->command=(char*)malloc(sizeof(char)*(cnt+5));
    cnt=0;
    for(int i=0;i<ptr->cnt;i++)
    {
        strcpy(pp->command+cnt,ptr->paramaters[i]);
        cnt+=strlen(ptr->paramaters[i])+1;
        pp->command[cnt-1]=' ';
        pp->command[cnt]='\0';
    }
    pp->status=mode;
}

void perr(char*message,char*file)
{
    char*buf=(char*)malloc(sizeof(char)*(strlen(message)+strlen(file)+10));
    sprintf(buf,message,file);
    perror(buf);
    free(buf);
}
int getmaxrow()
{
    struct winsize size;
    ioctl(STDIN_FILENO,TIOCGWINSZ,&size);
    return size.ws_row;
}

void init_environment(int argc,char**argv)
{
    //********init $0-9*******//
    char buf[100];
    sprintf(buf,"%d",argc-1);
    setenv("#",buf,1);
    for(int i=0;i<argc;i++)
    {
        sprintf(buf,"%d",i);
        setenv(buf,argv[i],1);
    }
    //*******init $?*********//
    setenv("?","0",1);

    //*********set PATH******//
    char*path=getenv("PATH");
    //printf("PATH: %s\n",path);
    int cntbytes=strlen(path);
    char*curr=(char*)malloc(sizeof(char)*1000);
    getcwd(curr,1000);
    cntbytes+=strlen(curr);
    cntbytes+=strlen("/myshell");
    cntbytes+=5;
    char*new_path=(char*)malloc(sizeof(char)*cntbytes);
    cntbytes=0;
    strcpy(new_path,path);
    cntbytes=strlen(new_path);
    strcpy(new_path+cntbytes,":");
    cntbytes=strlen(new_path);
    strcpy(new_path+cntbytes,curr);
    cntbytes=strlen(new_path);
    setenv("PATH",new_path,1);
    path=getenv("PATH");
    //printf("PATH: %s\n",path);
    free(new_path);
    //*********add MYSHELL******//
    strncat(curr,"/Alanshell",strlen("/Alanshell"));
    setenv("ALANSHELL",curr,1);
    free(curr);
}

int init()
{
    row_left=getmaxrow();
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
    pwd=getpwuid(getuid());
    process_node_init(&processlist);
    process_node_init(&currentprocess);
    currentprocess->command=(char*)malloc(sizeof(char)*1000);
    fgname=(char*)malloc(sizeof(char)*1000);
    
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
    //printf("************%d\n",(*p)->cnt);
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

void process_node_init(struct process_info**p)
{
    (*p)=(pro_node*)malloc(sizeof(pro_node));
    (*p)->command=NULL;
    (*p)->pid=0;
    (*p)->status=NONSENSE;
    (*p)->next=NULL;
}
