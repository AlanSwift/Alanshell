#include <stdio.h>
int main()
{
    printf("Hello wor\n");
    int n=10;
    printf("%d\n",n+3);
}



/////////back
if( (child1=fork()) != 0 )
    {
        //father
        printf("I AM FATHER\n");
        fflush(stdout);
        if(p->piped)
        {
            printf("PIPIPIPIPIPIP\n");
            if((child2=fork())==0)
            {
                close(pip_fd[1]);
                close(fileno(stdin));
                dup2(pip_fd[0],fileno(stdin));
                close(pip_fd[0]);
                run_exec(p->next);
            }
            else{
                close(pip_fd[0]);close(pip_fd[1]);
                if(infolist->background==0)
                {
                    waitpid(child2,&status,0);
                    if(WEXITSTATUS(status)==DO_FG)
                    {
                        exec_fg();
                    }
                }
                else{
                    int f=0;
                    for(f=0;f<MAXPID;f++)
                    {
                        if(PIDTABLE[f]==0)
                        {
                            PIDTABLE[f]=child2;
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
                    pp->next->pid=child2;
                    int cnt=0;
                    for(int i=0;i<p->cnt;i++)
                    {
                        cnt+=(strlen(p->paramaters[i])+1);
                    }
                    pp=pp->next;
                    pp->command=(char*)malloc(sizeof(char)*(cnt+5));
                    cnt=0;
                    for(int i=0;i<p->cnt;i++)
                    {
                        strcpy(pp->command+cnt,p->paramaters[i]);
                        cnt+=strlen(p->paramaters[i])+1;
                        pp->command[cnt-1]=' ';
                        pp->command[cnt]='\0';
                    }

                }
                    //waitpid(child2,&status,0);
            }
        }
        printf("FUCK YOU\n");
        fflush(stdout);
        if(infolist->background==0)
        {
            printf("***********FFF");
            fflush(stdout);
            waitpid(child1,&status,0);
            if(WEXITSTATUS(status)==DO_FG)
            {
                printf("ggggggg\n");
                exec_fg();
            }
        }
        else{
            int f=0;
            for(f=0;f<MAXPID;f++)
            {
                if(PIDTABLE[f]==0)
                {
                    PIDTABLE[f]=child1;
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
            pp->next->pid=child1;
            int cnt=0;
            for(int i=0;i<p->cnt;i++)
            {
                cnt+=(strlen(p->paramaters[i])+1);
            }
            pp=pp->next;
            pp->command=(char*)malloc(sizeof(char)*(cnt+5));
            cnt=0;
            for(int i=0;i<p->cnt;i++)
            {
                strcpy(pp->command+cnt,p->paramaters[i]);
                cnt+=strlen(p->paramaters[i])+1;
                pp->command[cnt-1]=' ';
                pp->command[cnt]='\0';
            }
            printf("pid2:%d\n",getpid());
            //waitpid(child1,NULL,WNOHANG);
        }
        
    }
    else{//command 1
        printf("pid son: %d father: %d\n",getpid(),getppid());
        if(p->input_direct_append||p->input_direct_noappend)
        {
            in_fd=open(p->inputfile, O_CREAT |O_RDONLY, 0666);
            close(fileno(stdin));
            dup2(in_fd,fileno(stdin));
            close(in_fd);
        }
        if(p->output_direct_append||p->output_direct_noappend)
        {
            if(p->output_direct_noappend)
            {
                out_fd = open(p->outputfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
            }
            else{
                out_fd = open(p->outputfile, O_WRONLY|O_CREAT|O_APPEND, 0666);
            }
            
            dup2(out_fd,fileno(stdout));
            close(out_fd);
        }
        else if(p->piped)
        {
            close(pip_fd[0]);
            close(fileno(stdout));
            dup2(pip_fd[1],fileno(stdout));
            close(pip_fd[1]);
            
        }
        int fe=run_command(p->command,p->paramaters);
        printf("***********%d&&&&&****",fe);
        exit(fe);
        //execvp(p->command,p->paramaters);
    }




    if(ptr==p)
            {
                if(p->input_direct_append||p->input_direct_noappend)
                {
                    close(pip_fd[0]);
                    in_fd=open(p->inputfile, O_CREAT |O_RDONLY, 0666);
                    close(fileno(stdin));
                    dup2(in_fd,fileno(stdin));
                    close(in_fd);
                }
                else{
                    freopen("/dev/tty","r",stdin);
                }
                if(p->output_direct_append||p->output_direct_noappend)
                {
                    close(pip_fd[1]);
                    if(p->output_direct_noappend)
                    {
                        out_fd = open(p->outputfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                    }
                    else{
                        out_fd = open(p->outputfile, O_WRONLY|O_CREAT|O_APPEND, 0666);
                    }
                    dup2(out_fd,fileno(stdout));
                    close(out_fd);
                }
                else if(p->piped)
                {
                    //close(pip_fd[0]);
                    close(fileno(stdout));
                    dup2(pip_fd[1],fileno(stdout));
                    close(pip_fd[1]);
                }
                else{
                    close(pip_fd[1]);
                    freopen("/dev/tty","w",stdout);
                }
            }
            else{
                if(ptr->input_direct_append||ptr->input_direct_noappend)
                {
                    close(pip_fd[0]);
                    in_fd=open(p->inputfile, O_CREAT |O_RDONLY, 0666);
                    close(fileno(stdin));
                    dup2(in_fd,fileno(stdin));
                    close(in_fd);
                }
                else{
                    //close(pip_fd[1]);
                    close(fileno(stdin));
                    dup2(pip_fd[0],fileno(stdin));
                    close(pip_fd[0]);
                }
                if(ptr->output_direct_append||ptr->output_direct_noappend)
                {
                    close(pip_fd[1]);
                    if(ptr->output_direct_noappend)
                    {
                        out_fd = open(ptr->outputfile, O_WRONLY|O_CREAT|O_TRUNC, 0666);
                    }
                    else{
                        out_fd = open(ptr->outputfile, O_WRONLY|O_CREAT|O_APPEND, 0666);
                    }
                    dup2(out_fd,fileno(stdout));
                    close(out_fd);
                }
                else if(ptr->piped)
                {
                    close(fileno(stdout));
                    dup2(pip_fd[1],fileno(stdout));
                    close(pip_fd[1]);
                }
                else{
                    close(pip_fd[1]);
                    sleep(2);
                    freopen("/dev/tty","w",stdout);
                }
                
                
                
            }



            for(int i;;)
            {
                fork()
                {
                    //parent
                    wait()

                }
                {
                    //son
                    stdin pip[0]
                    stdout pip[1]
                    return
                }
            }




/////////////// lastest//////////////////
if(infolist->background==0)
                {
                    fgpid=child2;
                    currentprocess->pid=child2;
                    int cnt=0;
                    for(int i=0;i<p->next->cnt;i++)
                    {
                        strcpy(currentprocess->command+cnt,p->next->paramaters[i]);
                        cnt+=strlen(p->next->paramaters[i])+1;
                        currentprocess->command[cnt-1]=' ';
                        currentprocess->command[cnt]='\0';
                    }

                    waitpid(child2,&status,WUNTRACED);
                    fgpid=-1;
                    if(WEXITSTATUS(status)==DO_FG)
                    {
                        exec_fg();
                    }
                }
                else{
                    int f=0;
                    for(f=0;f<MAXPID;f++)
                    {
                        if(PIDTABLE[f]==0)
                        {
                            PIDTABLE[f]=child2;
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
                    pp->next->pid=child2;
                    int cnt=0;
                    for(int i=0;i<p->cnt;i++)
                    {
                        cnt+=(strlen(p->next->paramaters[i])+1);
                    }
                    pp=pp->next;
                    pp->command=(char*)malloc(sizeof(char)*(cnt+5));
                    cnt=0;
                    for(int i=0;i<p->next->cnt;i++)
                    {
                        strcpy(pp->command+cnt,p->next->paramaters[i]);
                        cnt+=strlen(p->next->paramaters[i])+1;
                        pp->command[cnt-1]=' ';
                        pp->command[cnt]='\0';
                    }

