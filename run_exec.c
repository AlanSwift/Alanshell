#include "myhead.h"


short run_exec(struct info_node*p)
{
    int status=0;//0 is ok, 1 is slight error, 2 is serious error
    int temp_fd=0;
    struct info_node*ptr=NULL;
    int laststatus=0;
    for(ptr=p;ptr!=NULL;ptr=ptr->next)
    {
        if(ptr->next)
        {
            int fd[2];
            pipe(fd);
            int re=run_single(ptr,temp_fd,fd[1]);
            laststatus=re;
            temp_fd=fd[0];
            if(re==DO_EXIT)
            {
                return DO_EXIT;
            }
            else if(re>status)
            {
                status=re;
            }
        }
        else{
            int re=run_single(ptr,temp_fd,1);
            laststatus=re;
            if(re==DO_EXIT)
            {
                return DO_EXIT;
            }
            else if(re>status)
            {
                status=re;
            }
        }

        fflush(stdout);
    }
    char buf[65];
    sprintf(buf,"%d",laststatus);
    setenv("?",buf,1);
    return status;
}

short is_internal_cmd(char*command)
{
    for(int i=0;internal_list[i]!=NULL;i++)
    {
        if(strcmp(internal_list[i],command)==0)
        {
            return i;
        }
    }
    return -1;
}

int run_single(struct info_node*p,int in_fd,int out_fd)
{
    int status=1;
    pid_t childpid;
    if((childpid=fork())==0)
    {
        int pid=getpid();
        signal(SIGINT,SIG_DFL);
        signal(SIGTSTP,SIG_IGN);
        signal(SIGCONT,SIG_DFL);
        dup2(in_fd,0);
        dup2(out_fd,1);
        if(in_fd!=0)
        {
            close(in_fd);
        }
        if(out_fd!=1)
        {
            close(out_fd);
        }
        int re=run_command(p->command,p->paramaters);
        exit(re);
    }
    else{
        signal(SIGTSTP,ctrl_z);
        signal(SIGCONT,SIG_DFL);
        if(infolist->background==1)
        {
            addpid(childpid,p,RUNNING);
        }
        else{
            fgpid=childpid;
            waitpid(childpid,&status,WUNTRACED);
            fgpid=-1;
            if(WEXITSTATUS(status)==DO_FG)
            {
                if(p->paramaters[1]==NULL)
                {
                    exec_fg(-1);
                    return 0;
                }
                for(int i=1;p->paramaters[i]!=NULL;i++)
                {
                    if(test_num(p->paramaters[i])!=0)
                    {
                        perror("fg: Can't recognize the parameters.");
                        return 2;
                    }
                    else{
                        int mypid=atoi(p->paramaters[i]);
                        if(mypid<0)
                        {
                            perror("fg: Pid error.");
                        }
                        exec_fg(mypid);
                    }
                }
                return 0;
            }
            else if(WEXITSTATUS(status)==DO_BG)
            {
                if(p->paramaters[1]==NULL)
                {
                    exec_bg(-1);
                    return 0;
                }
                for(int i=1;p->paramaters[i]!=NULL;i++)
                {
                    if(test_num(p->paramaters[i])!=0)
                    {
                        perror("bg: Can't recognize the parameters.");
                        return 2;
                    }
                    else{
                        int mypid=atoi(p->paramaters[i]);
                        if(mypid<0)
                        {
                            perror("bg: Pid error.");
                        }
                        exec_bg(mypid);
                    }
                }
                return 0;
            }
            else if(WEXITSTATUS(status)==DO_SET)
            {
                return exec_set((p->paramaters)+1);
            }
            else if(WEXITSTATUS(status)==DO_UNSET)
            {
                return exec_unset((p->paramaters)+1);
            }
            else if(WEXITSTATUS(status)==DO_SHIFT)
            {
                return exec_shift((p->paramaters)+1);
            }
            else if(WEXITSTATUS(status)==DO_JOBS)
            {
                list_jobs((p->paramaters)+1);
                cleanprocess();
                return 0;
            }
            else if(WEXITSTATUS(status)==DO_EXIT)
            {
                return DO_EXIT;
            }
            else if(WEXITSTATUS(status)==DO_CONTINUE)
            {
                return DO_CONTINUE;
            }
            else if(WEXITSTATUS(status)==DO_DECLARE)
            {
                return exec_declare((p->paramaters)+1);
            }
            else if(WEXITSTATUS(status)==DO_LET)
            {
                return exec_let((p->paramaters)+1);
            }
            return WEXITSTATUS(status);
        }

        if(in_fd!=0)
        {
            close(in_fd);
        }
        if(out_fd!=1)
        {
            close(out_fd);
        }
    }
    return 0;
}



short run_command(char* command,char**para)
{
    
    int re=is_internal_cmd(command);
    if(re==-1)
    {
        //out
        return execvp(command,para);
    }
    else if(re==0)//cd
    {
        char s[2]="~";
        char*path_cd=NULL;
        if(para[1]==NULL)
        {
            para[1]=s;
            para[2]=NULL;
        }
        return set_path(para[1]);

    }
    else if(re==1)//pwd
    {
        return exec_pwd();
        
    }
    else if(re==2)//exit
    {
        return DO_EXIT;
    }
    else if(re==3)//dir
    {
        return exec_dir(para+1);
    }
    else if(re==4)//clear
    {
        clear();
        return 0;//0 is success
    }
    else if(re==5)//jobs
    {
        return DO_JOBS;
    }
    else if(re==6)//fg
    {
        exit(DO_FG);
    }
    else if(re==7)//bg
    {
        return DO_BG;
    }
    else if(re==8)//time
    {
        return exec_time();
    }
    else if(re==9)//umask
    {
        return exec_umask(para+1);
    }
    else if(re==10)//environ
    {
        return exec_environ();
    }
    else if(re==11)//set
    {
        return DO_SET;
    }
    else if(re==12)//unset
    {
        return DO_UNSET;
    }
    else if(re==13)//exec
    {
        return execvp(para[1],para+1);
    }
    else if(re==14)//test
    {
        return exec_test(para+1);
    }
    else if(re==15)//shift
    {
        return DO_SHIFT;
    }
    else if(re==16)//echo
    {
        return exec_echo(para+1);
    }
    else if(re==17)
    {
        return exec_help();
    }
    else if(re==18)
    {
        return exec_continue();
    }
    else if(re==19)
    {
        return DO_DECLARE;
    }
    else if(re==20)
    {
        return DO_LET;
    }
    
    return 1;
}
short exec_declare(char**para)
{
    for(int i=0;para[i]!=NULL;i++)
    {
        if(Find(para[i],valueables)==NULL)
        {
            valueables=Insert(para[i],"",valueables);
        }
    }
    return 0;
}
short exec_let(char**para)//only support assignment...for the sake of time. I will add the expression later.
{
    if(strcmp(para[1],"=")!=0||para[0]==NULL||para[2]==NULL)
    {
        perror("let: This expression is not supported.");
        return 1;
    }
    position p=Find(para[0],valueables);
    if(p==NULL)
    {
        valueables=Insert(para[0],para[2],valueables);
        return 0;
    }
    else{
        return UpdateAVL(para[0],para[2],valueables);
    }
}

short exec_continue()
{
    return DO_CONTINUE;
}

short exec_help()
{
    char*cmd[]={"more","readme"};
    return execvp(cmd[0],cmd);
}

char* replace_string(char*p)
{
    int cntall=0;
    int size=100;
    char*res=(char*)malloc(sizeof(char)*size);
    memset(res,0,sizeof(char)*size);
    int indouble=0;
    int insingle=0;
    int indollar=0;
    int hasquote=0;
    char bufsize=100;
    char*buf=(char*)malloc(sizeof(char)*bufsize);
    int cnt=0;
    //printf("^^^^^^^^^^^^ %s ^^^^^^^^^^^\n",p);
    int len=strlen(p);
    for(int i=0;i<len;i++)
    {
        if(indollar)
        {
            if(p[i]=='{'&&!hasquote)
            {
                hasquote=1;
            }
            else if(isalnum(p[i])||p[i]=='?'||p[i]=='#')
            {
                if(cnt+1>=bufsize)
                {
                    bufsize<<=1;
                    char*old=buf;
                    buf=(char*)malloc(sizeof(char)*bufsize);
                    strcpy(buf,old);
                    free(old);
                }
                buf[cnt++]=p[i];
            }
            else if(p[i]=='}')
            {
                if(hasquote)
                {
                    indollar=0;
                    hasquote=0;
                    buf[cnt]='\0';
                    char*value=getenv(buf);
                    if(value!=NULL)
                    {
                        int cnt_1=cntall+strlen(value);
                        if(cnt_1>=size)
                        {
                            size=cnt_1*2;
                            char*old=res;
                            res=(char*)malloc(sizeof(char)*size);
                            strcpy(res,old);
                            free(old);
                        }
                        strcpy(res+cntall,value);
                        cntall=strlen(res);
                    }
                    cnt=0;
                }
                else{
                    free(buf);
                    return NULL;
                }
            }
            else if(hasquote)
            {
                if(cnt+1>=bufsize)
                {
                    bufsize<<=1;
                    char*old=buf;
                    buf=(char*)malloc(sizeof(char)*bufsize);
                    strcpy(buf,old);
                    free(old);
                }
                buf[cnt++]=p[i];
            }
            else{
                indollar=0;
                hasquote=0;
                buf[cnt]='\0';
                char*value=getenv(buf);
                if(value!=NULL)
                {
                    int cnt_1=cntall+strlen(value);
                    if(cnt_1>=size)
                    {
                        size=cnt_1*2;
                        char*old=res;
                        res=(char*)malloc(sizeof(char)*size);
                        strcpy(res,old);
                        free(old);
                    }
                    strcpy(res+cntall,value);
                    cntall=strlen(res);
                }
                else{
                    position p=Find(buf,valueables);
                    if(p!=NULL)
                    {
                        int cnt_1=cntall+strlen(p->value);
                        if(cnt_1>=size)
                        {
                            size=cnt_1*2;
                            char*old=res;
                            res=(char*)malloc(sizeof(char)*size);
                            strcpy(res,old);
                            free(old);
                        }
                        strcpy(res+cntall,p->value);
                        cntall=strlen(res);
                    }
                }
                cnt=0;
            }
        }
        else if(p[i]=='\'')
        {
            insingle=!insingle;
        }
        else if(insingle)
        {
            int cnt_1=cntall+1;
            if(cnt_1>=size)
            {
                size=cnt_1*2;
                char*old=res;
                res=(char*)malloc(sizeof(char)*size);
                strcpy(res,old);
                free(old);
            }
            res[cntall++]=p[i];
        }
        else if(p[i]=='\"')
        {
            indouble=!indouble;
        }
        else if(p[i]=='$')
        {
            indollar=1;
        }
        else if(p[i]=='{'&&i>=1&&p[i-1]=='$')
        {
            hasquote=1;
        }
        else{
            int cnt_1=cntall+1;
            if(cnt_1>=size)
            {
                size=cnt_1*2;
                char*old=res;
                res=(char*)malloc(sizeof(char)*size);
                strcpy(res,old);
                free(old);
            }
            res[cntall++]=p[i];
        }
    }
    if(indollar)
    {
        buf[cnt]='\0';
        char*value=getenv(buf);
        if(value!=NULL)
        {
            int cnt_1=cntall+strlen(value);
            if(cnt_1>=size)
            {
                size=cnt_1*2;
                char*old=res;
                res=(char*)malloc(sizeof(char)*size);
                strcpy(res,old);
                free(old);
            }
            strcpy(res+cntall,value);
            cntall=strlen(res);
        }
        else{
            position p=Find(buf,valueables);
            if(p!=NULL)
            {
                int cnt_1=cntall+strlen(p->value);
                if(cnt_1>=size)
                {
                    size=cnt_1*2;
                    char*old=res;
                    res=(char*)malloc(sizeof(char)*size);
                    strcpy(res,old);
                    free(old);
                }
                strcpy(res+cntall,p->value);
                cntall=strlen(res);
            }
        }
    }
    //printf("echo:: %s\n",res);
    return res;
}

short exec_echo(char**para)
{
    int ok=1;
    for(int i=0;para[i]!=NULL;i++)
    {
        char *p=replace_string(para[i]);
        if(p==NULL)
        {
            ok=0;
            continue;
        }
        else{
            printf("%s",p);
            free(p);
            if(para[i+1]!=NULL)
            {
                printf(" ");
            }
            else{
                printf("\n");
            }
        }
    }
    if(ok)
    {
        return 0;
    }
    else{
        perror("echo: Input error.");
        return 1;
    }
}

short exec_shift(char**para)
{
    int n=0;
    if(para[0]==NULL)
    {
        n=1;
    }
    else{
        n=atoi(para[0]);
        if(n<0)
        {
            perror("shift: index out of bound.");
            return 1;
        }
    }
    if(n==0)
    {
        return 0;
    }
    char * old=getenv("#");
    int cntold=atoi(old);
    char buf[100];
    printf("%d %d\n",cntold,n);
    for(int i=1;i<=cntold-n;i++)
    {
        sprintf(buf,"%d",i+n);
        char*oldvalue=getenv(buf);
        sprintf(buf,"%d",i);
        setenv(buf,oldvalue,1);
    }
    for(int i=max(1,cntold-n+1);i<=cntold;i++)
    {
        sprintf(buf,"%d",i);
        setenv(buf,"NULL",1);
    }
    sprintf(buf,"%d",max(0,cntold-n));
    setenv("#",buf,1);
    return 0;

}

short test_dir(char*filename)
{
    DIR*dir=NULL;
    struct stat dir_info;
    struct dirent*dirp=NULL;
    char*path_more=(char*)malloc(sizeof(char)*1000);
    if(filename==NULL)
    {
        return false;
    }
    if(filename[0]!='/'&&filename[0]!='~')
    {
        getcwd(path_more,1000);
        int len=strlen(path_more);
        path_more[len]='/';
        path_more[len+1]='\0';
        len++;
        strcpy(path_more+len,filename);
        printf("%s",path_more);
    }
    else if(filename[0]=='~')
    {
        strcpy(path_more,pwd->pw_dir);
        strcpy(path_more+strlen(pwd->pw_dir),filename+1);
    }
    else{
        strcpy(path_more,filename);
    }
    if(stat(path_more,&dir_info)==-1)
    {
        return false;
    }
    else{
        if(S_ISDIR(dir_info.st_mode))
        {
            return true;
        }
        else{
            return false;
        }
    }
}
short test_file(char*filename,int flag)
{
    DIR*dir=NULL;
    struct stat dir_info;
    struct dirent*dirp=NULL;
    char*path_more=(char*)malloc(sizeof(char)*1000);
    if(filename==NULL)
    {
        return 1;
    }
    if(filename[0]!='/'&&filename[0]!='~')
    {
        getcwd(path_more,1000);
        int len=strlen(path_more);
        path_more[len]='/';
        path_more[len+1]='\0';
        len++;
        strcpy(path_more+len,filename);
    }
    else if(filename[0]=='~')
    {
        strcpy(path_more,pwd->pw_dir);
        strcpy(path_more+strlen(pwd->pw_dir),filename+1);
    }
    else{
        strcpy(path_more,filename);
    }
    if(lstat(path_more,&dir_info)==-1)
    {
        return 1;
    }
    else{
        switch(flag)
        {
            case TESTf:
            {
                if(S_ISREG(dir_info.st_mode))
                {
                    return 0;
                }
                else{
                    return 1;
                }
                break;
            }
            case TESTr:
            {
                if(access(path_more,R_OK)==0)
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
            case TESTw: 
            {
                if(access(path_more,W_OK)==0)
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
            case TESTx: 
            {
                if(access(path_more,X_OK)==0)
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
            case TESTe: 
            {
                return 0;
            }
            case TESTh: case TESTL:
            {
                if(S_ISLNK(dir_info.st_mode))
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
            case TESTc: 
            {
                if(S_ISCHR(dir_info.st_mode))
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
            case TESTb: 
            {
                if(S_ISBLK(dir_info.st_mode))
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
            case TESTp: 
            {
                if(S_ISFIFO(dir_info.st_mode))
                {
                    return 0;
                }
                else{
                    return 1;
                }

            }
            default: 
            {
                perror("Test: Parameters are not supported.");
                return 1;
            }
        }
    }
}

short exec_test(char**para)
{
    if(para[0][0]=='-')
    {
        //printf("in")
        switch(para[0][1])
        {
            case 'd':
            {
                return test_dir(para[1]);
                break;
            }
            case 'r':
            {
                return test_file(para[1],TESTr);
            }
            case 'w': 
            {
                return test_file(para[1],TESTw);
            }
            case 'x': 
            {
                return test_file(para[1],TESTx);
            }
            case 'e': 
            {
                return test_file(para[1],TESTe);
            }
            case 'L':case 'h':
            {
                return test_file(para[1],TESTL);
            }
            case 'p': 
            {
                return test_file(para[1],TESTp);
            }
            case 'f': 
            {
                return test_file(para[1],TESTf);
            }
            case 'c': 
            {
                return test_file(para[1],TESTc);
            }
            case 'b': 
            {
                return test_file(para[1],TESTb);
            }
            default: 
            {
                perror("Test: Parameters are not supported.");
                return 1;
            }
        }
    }
    else{
        if(para[1][0]=='-')
        {
            if(strcmp(para[1],"-le")==0)
            {
                return test_logic(para[0],para[2],TESTle);
            }
            else if(strcmp(para[1],"-lt")==0)
            {
                return test_logic(para[0],para[2],TESTlt);
            }
            else if(strcmp(para[1],"-ge")==0)
            {
                return test_logic(para[0],para[2],TESTge);
            }
            else if(strcmp(para[1],"-gt")==0)
            {
                return test_logic(para[0],para[2],TESTgt);
            }
            else if(strcmp(para[1],"-eq")==0)
            {
                return test_logic(para[0],para[2],TESTeq);
            }
            else if(strcmp(para[1],"-ne")==0)
            {
                return test_logic(para[0],para[2],TESTne);
            }
            else{
                perror("test: Can't recognize the parameter.");
                return 2;
            }
        }
        else if(strcmp(para[1],"<")==0)
        {
            if(para[0]==NULL||para[2]==NULL)
            {
                perror("test: Parameters error.");
                return 2;
            }
            else{
                if(strcmp(para[0],para[2])<0)
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
        }
        else if(strcmp(para[1],"<=")==0)
        {
            if(para[0]==NULL||para[2]==NULL)
            {
                perror("test: Parameters error.");
                return 2;
            }
            else{
                if(strcmp(para[0],para[2])<=0)
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
        }
        else if(strcmp(para[1],">")==0)
        {
            if(para[0]==NULL||para[2]==NULL)
            {
                perror("test: Parameters error.");
                return 2;
            }
            else{
                if(strcmp(para[0],para[2])>0)
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
        }
        else if(strcmp(para[1],">=")==0)
        {
            if(para[0]==NULL||para[2]==NULL)
            {
                perror("test: Parameters error.");
                return 2;
            }
            else{
                if(strcmp(para[0],para[2])>=0)
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
        }
        else if(strcmp(para[1],"==")==0||strcmp(para[1],"=")==0)
        {
            if(para[0]==NULL||para[2]==NULL)
            {
                perror("test: Parameters error.");
                return 2;
            }
            else{
                if(strcmp(para[0],para[2])==0)
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
        }
        else if(strcmp(para[1],"!=")==0)
        {
            if(para[0]==NULL||para[2]==NULL)
            {
                perror("test: Parameters error.");
                return 2;
            }
            else{
                if(strcmp(para[0],para[2])!=0)
                {
                    return 0;
                }
                else{
                    return 1;
                }
            }
        }
        else{
            perror("test: Can't recognize the parameters.");
            return 2;
        }
    }
}
short test_num(char*a)
{
    if(a==NULL)
    {
        return 1;
    }
    int i=0;
    if(a[0]=='-'||a[0]=='+')
    {
        i++;
    }
    int len=strlen(a);
    for(;i<len;i++)
    {
        if(a[i]>='0'&&a[i]<='9')
        {
            continue;
        }
        else{
            return 1;
        }
    }
    return 0;
}

short test_logic(char*a,char*b,int flag)
{
    if(a==NULL||b==NULL)
    {
        return 2;//error
    }
    else{
        if(test_num(a)||test_num(b))
        {
            return 2;//error
        }
        int num1=atoi(a);
        int num2=atoi(b);
        switch(flag)
        {
            case TESTle: 
            {
                return num1<=num2?0:1;
            }
            case TESTlt: 
            {
                return num1<num2?0:1;
            }
            case TESTge: 
            {
                return num1>=num2?0:1;
            }
            case TESTgt: 
            {
                return num1>num2?0:1;
            }
            case TESTeq: 
            {
                return num1==num2?0:1;
            }
            case TESTne: 
            {
                return num1!=num2?0:1;
            }
            default: 
            {
                return 2;
            }
        }
    }
}

short exec_environ()
{
    for(int i=0;environ[i]!=NULL;i++)
    {
        printf("%s\n",environ[i]);
    }
    return 0;
}

short exec_set(char**parameters)
{
    if(parameters[0]==NULL)
    {
        return exec_environ();
    }
    else if(parameters[1]==NULL)
    {
        return setenv(parameters[0],"NULL",1);
    }
    else
    {
        return setenv(parameters[0],parameters[1],1);
    }
}

short exec_unset(char**parameters)
{
    if(parameters[0]!=NULL)
    {
        return unsetenv(parameters[0]);
    }
    return 1;
}

short exec_umask(char**parameters)
{
    mode_t new_umask=0666,old_umask;
    old_umask=umask(new_umask);
    if(parameters[0]==NULL)
    {
        printf("%04o\n",old_umask);
        umask(old_umask);
    }
    else{
        new_umask=strtoul(parameters[0],0,8);
        printf("%04o\n",new_umask);
        umask(new_umask);
    }
    return 0;
}

short exec_time()
{
    time_t now;
    struct tm*time_now;
    time(&now);
    time_now=localtime(&now);
    printf("%s",asctime(time_now));
    return 0;
}
short exec_bg(pid_t bpid)
{
    pro_node*p=processlist;
    short listall=1;
    if(bpid==-1)
    {
        listall=1;
    }
    else{
        listall=0;
    }
    while(p->next!=NULL)
    {
        if( p->next->status==STOP )
        {
            if(listall==0&&p->next->pid!=bpid)
            {
                continue;
            }
            pid_t pid=p->next->pid;
            kill(pid,SIGCONT);
            p->next->status=RUNNING;
            if(listall==0)
            {
                return 0;
            }
        }
        if(p->next!=NULL)
            p=p->next;
    }
    return 0;
}

short exec_fg(pid_t fpid)
{
    DIR*dir;
    pro_node*p=processlist;
    short listall=1;
    if(fpid==-1)
    {
        listall=1;
    }
    else{
        listall=0;
    }
    while(p->next!=NULL)
    {
        if(p->next->status==RUNNING)
        {
            if(listall==0&&p->next->pid!=fpid)
            {
                continue;
            }
            for(int i=0;i<MAXPID;i++)
            {
                if(PIDTABLE[i]==p->next->pid)
                {
                    PIDTABLE[i]=0;
                    break;
                }
            }
            printf("pid :%d command: %s\n",fpid,p->next->command);
            pid_t pid=p->next->pid;

            pro_node*pt=p->next;
            p->next=p->next->next;

            free(pt->command);
            free(pt);

            fgpid=pid;
            waitpid(pid,NULL,WUNTRACED);
            fgpid=-1;
            if(listall==0)
            {
                return 0;
            }
        }
        if(p->next!=NULL)
            p=p->next;
    }
    return 0;
}

void cleanprocess()
{
    pro_node*p=processlist,*pn,*pt;
    while(p->next!=NULL)
    {
        if(p->next->status==NONSENSE)
        {
            pt=p->next;
            p->next=p->next->next;
            free(pt->command);
            free(pt);
        }
        else if(p->next->status==ZOMBIS)
        {
            waitpid(p->next->pid,0,WNOHANG);
            pt=p->next;
            p->next=p->next->next;
            free(pt->command);
            free(pt);
        }
        if(p->next!=NULL)
        {
            p=p->next;
        }
    }

}

void list_jobs(char**parameters)
{
    int flag=0;
    for(int i=0;parameters[i]!=NULL;i++)
    {
        if(parameters[i][0]=='-')
        {
            for(int j=1;j<strlen(parameters[i]);j++)
            {
                if(parameters[i][j]=='l')
                {
                    flag|=PARA_l;
                }
                else{
                    perror("jobs: Unknow parameters.");
                }
            }
        }
        else{
            perror("jobs: Unknown paramaters.");
        }
    }
    pro_node*p=processlist;
    int cnt=0;
    while(p->next!=NULL)
    {
        readinfo(p->next);
        //waitpid(p->next->pid,NULL,0);
        char tmp[100];
        if(p->next->status==NONSENSE)
        {
            strcpy(tmp,"Finished");
        }
        else if(p->next->status==RUNNING)
        {
            strcpy(tmp,"Running");
        }
        else if(p->next->status==STOP)
        {
            strcpy(tmp,"Stop");
        }
        else if(p->next->status==ZOMBIS)
        {
            strcpy(tmp,"ZOMBIS");
        }
        if(flag&PARA_l)
        {
            
            printf("[%d] process pid: %d, command:%s, status: %s\n",cnt++,p->next->pid,p->next->command,tmp);
        }
        else{
            printf("[%d] command: %s status: %s\n",cnt++,p->next->command,tmp);
        }
        
        p=p->next;
    }
}
void readinfo(struct process_info*p)
{
    DIR*dir=NULL;
    FILE*fp=NULL;
    struct dirent*dirinfo=NULL;
    struct stat buf;
    char*path=(char*)malloc(sizeof(char)*100);
    sprintf(path,"/proc/%d/",p->pid);
    if( (dir=opendir(path))==NULL )
    {
        p->status=NONSENSE;
        free(path);
        return;
    }
    else{
        sprintf(path,"/proc/%d/stat",p->pid);
        if( (fp=fopen(path,"r"))!=NULL )
        {
            int pid,ppid;
            char bbuf[100];
            char state;
            fscanf(fp,"%d %s %c %d",&pid,bbuf,&state,&ppid);
            if(state=='S')
            {
                p->status=RUNNING;
            }
            else if(state=='T'){
                p->status=STOP;
            }
            else if(state=='Z')
            {
                p->status=ZOMBIS;
            }
        }
        else{
            p->status=NONSENSE;
        }
        fclose(fp);
        free(path);
    }
}

short clear()
{
    printf("\033[1H\033[2J");
    return 0;
}

short exec_dir(char**parameters)
{
    char**dir=NULL;
    
    int cntpara=0;
    int flag=0;

    char*path_dir=NULL;
    DIR *dp=NULL;
    struct dirent * dirp=NULL;
    struct stat dir_info;

    dir=(char**)malloc(sizeof(char*)*MAXDIRLIST);
    memset(dir,0,sizeof(char*)*MAXDIRLIST);
    int cntdir=0;
    for(int i=0;parameters[i]!=NULL;i++)
    {
        if(parameters[i][0]!='-')
        {
            if(cntdir==MAXDIRLIST)
            {
                perror("dir: Dirs are too many.");
                return 2;//level 2 error unable to use parameters
            }
            else{
                dir[cntdir++]=(char*)malloc(sizeof(char)*(strlen(parameters[i])+5));
                strcpy(dir[cntdir-1],parameters[i]);
            }
        }
        else{
            for(int j=1;j<strlen(parameters[i]);j++)
            {
                if(parameters[i][j]=='a')
                {
                    flag|=PARA_a;

                }
                else if(parameters[i][j]=='l')
                {
                    flag|=PARA_l;
                }
                else{
                    perror("dir:Unknow parameters");
                    return 2;
                }
            }
        }
    }
    
    if(cntdir==0)
    {
        dir[cntdir++]=(char*)malloc(sizeof(char)*1000);
        getcwd(dir[cntdir-1],1000);
    }
    
    for(int i=0;i<cntdir;i++)
    {
        path_dir=NULL;
        dp=NULL;
        dirp=NULL;
        if(strlen(dir[i])<=0)
        {
            return 2;
        }
        if(dir[i][0]=='~')
        {
            path_dir=(char*)malloc(sizeof(char)*( strlen(pwd->pw_dir) + strlen(dir[i] + 2 ) + 5));
            strcpy(path_dir,pwd->pw_dir);
            strcpy(path_dir+strlen(pwd->pw_dir),dir[i]+1);
        }
        else{
            path_dir=(char*)malloc(sizeof(char)*( strlen(dir[i]) + 5));
            strcpy(path_dir,dir[i]);
        }
        if(stat(path_dir,&dir_info)==-1)
        {
            perror("dir:file or dir can't be found.");
        }
        else{
            if(S_ISDIR(dir_info.st_mode))
            {
                //is dir
                int cnt =strlen(path_dir);
                if(path_dir[cnt-1]!='/')
                {
                    path_dir[cnt++]='/';
                    path_dir[cnt]='\0';
                }
                exec_display(path_dir,flag);
            }
            else{
                exec_displayfile(path_dir,flag);
            }
        }
        free(path_dir);
    }
    for(int i=0;i<cntdir;i++)
    {
        free(dir[i]);
    }
    free(dir);
    return 0;
    
}
int cmp(const void*a,const void*b)
{
    return strcmp((char*)a,(char*)b);
}

short exec_display(char*dir,int flag)
{
    
    DIR* dopen=NULL;
    struct dirent* dinfo=NULL;
    int cntfile=0;
    char **filenames;
    char*buf=NULL;
    int filesize=MAXDIRLIST;
    int maxsize=0;
    if( (filenames=(char**)malloc(sizeof(char*)*filesize)) ==NULL )
    {
        perror("dir: No memory.");
        return 2;
    }
    dopen=opendir(dir);
    if(dopen==NULL)
    {
        perror("dir: Dir can't be open.");
        return 2;
    }
    while( ( dinfo = readdir(dopen) ) != NULL )
    {
        if(cntfile==filesize)
        {
            char**tmp=filenames;
            filenames=(char**)malloc(sizeof(char*)*filesize*2);
            memcpy(filenames,tmp,sizeof(char*)*filesize);
            free(tmp);
            filesize*=2;
        }
        filenames[cntfile]=(char*)malloc(sizeof(char)*(strlen(dinfo->d_name)+5));
        strcpy(filenames[cntfile],dinfo->d_name);
        maxsize=max(maxsize,strlen(dinfo->d_name));
        cntfile++;
    }
    char* temp;
    for (int i = 0; i < cntfile - 1; i++)
        for (int j = 0; j < cntfile - 1 - i; j++) {
            if ( strcmp(filenames[j], filenames[j + 1]) > 0 ) {
                temp=filenames[j+1];
                filenames[j+1]=filenames[j];
                filenames[j]=temp;
            }
        }
    buf=(char*)malloc(sizeof(char)*(strlen(dir) + maxsize + 5));
    strcpy(buf,dir);
    const int base_size=strlen(dir);
    unsigned long long sizeall=0;
    for(int i=0;i<cntfile;i++)
    {
        strcpy(buf+base_size,filenames[i]);
        if(filenames[i][0]=='.'&&!(flag&PARA_a))
        {
            continue;
        }
        else{
            
            int re=exec_displayfile(buf,flag);
            if(re==-1)
            {
                continue;
            }
            sizeall+=((re/4096) + (((re%4096)?1:0) ))*4;
        }
    }
    int r=flag&0x3;
    if(r==0||r==1)
    {
        printf("\n");
    }
    else{
        printf("All used: %llu\n",sizeall);
    }
    free(buf);
    return 0;


}

int exec_displayfile(char*file,int flag)
{
    struct stat buf;
    char*name=NULL;
    name=(char*)malloc(sizeof(char)*(strlen(file)+5));
    for(int i=strlen(file)-1;i>=0;i--)
    {
        if(file[i]=='/')
        {
            strcpy(name,file+i+1);break;
        }
    }
    if(lstat(file,&buf)==-1)
    {
        perr("dir: File %s can't be found.",file);

        return -1;//level 1
    }
    switch(flag & 3)
    {
        
        case PARA_NONE:case PARA_a:
        {
            if(row_left>=strlen(name))
            {
                row_left-=strlen(name);
                printf("%s\t",name);
            }
            else{
                row_left=getmaxrow();
                printf("\n%s\t",name);
                row_left-=strlen(name);
            }
            break;
        }
        case PARA_l: case PARA_a|PARA_l:
        {
            
            if (S_ISLNK(buf.st_mode))
                printf("l");
            else if (S_ISREG(buf.st_mode))
                printf("-");
            else if (S_ISDIR(buf.st_mode))
                printf("d");
            else if (S_ISCHR(buf.st_mode))
                printf("c");
            else if (S_ISBLK(buf.st_mode))
                printf("b");
            else if (S_ISFIFO(buf.st_mode))
                printf("f");
            else if (S_ISSOCK(buf.st_mode))
                printf("s");
            
            /*获取并打印文件所有者的权限*/
            printf("%c", (buf.st_mode & S_IRUSR) ? 'r' : '-');
            printf("%c", (buf.st_mode & S_IWUSR) ? 'w' : '-');
            printf("%c", (buf.st_mode & S_IXUSR) ? 'x' : '-');
            /*获取并打印与组用户对该文件的操作权限*/
            printf("%c", (buf.st_mode & S_IRGRP) ? 'r' : '-');
            printf("%c", (buf.st_mode & S_IWGRP) ? 'w' : '-');
            printf("%c", (buf.st_mode & S_IXGRP) ? 'x' : '-');
            /*获取并打印其它用户的对该文件的操作权限*/
            printf("%c", (buf.st_mode & S_IROTH) ? 'r' : '-');
            printf("%c", (buf.st_mode & S_IWOTH) ? 'w' : '-');
            printf("%c", (buf.st_mode & S_IXOTH) ? 'x' : '-');
            printf(" ");

            struct passwd*p1=getpwuid(buf.st_uid);
            struct group*g1=getgrgid(buf.st_gid);
            printf("%4lu ",buf.st_nlink);
            printf("%8s ",p1->pw_name);
            printf("%8s ",g1->gr_name);
            printf("%6lu ",buf.st_size);
            char*tbuf=(char*)malloc(sizeof(char)*(strlen(ctime(&buf.st_mtime))));
            strcpy(tbuf,ctime(&buf.st_mtime));
            tbuf[strlen(tbuf)-1]='\0';
            printf("%s ",tbuf);
            printf("%s\n",name);
            break;
        }
        default:
        {
            
            break;
        }

    }
    return buf.st_size;
}

short exec_pwd()
{
    char *buf=(char*)malloc(sizeof(char)*1000);
    getcwd(buf,1000);
    printf("%s\n",buf);
    free(buf);
    return 0;
}

short set_path(char*dir)
{
    char*path_cd=NULL;
    if(strlen(dir)<=0)
    {
        return 1;
    }
    if(dir[0]=='~')
    {
        path_cd=(char*)malloc(sizeof(char)*( strlen(pwd->pw_dir) + strlen(dir + 2 )));
        strcpy(path_cd,pwd->pw_dir);
        strcpy(path_cd+strlen(pwd->pw_dir),dir+1);
    }
    else{
        path_cd=(char*)malloc(sizeof(char)*( strlen(dir) +2));
        strcpy(path_cd,dir);
    }
    int re;
    re=chdir(path_cd);
    free(path_cd);
    return re;
}