#include "myhead.h"


short run_exec(struct info_node*p)
{
    int pip_fd[2];
    int status;
    int out_fd,in_fd;
    pipe(pip_fd);

    pid_t child1,child2;
    printf("pid1:%d\n",getpid());
    if( (child1=fork()) != 0 )
    {
        //father
        printf("I AM FATHER\n");
        
        fflush(stdout);
        if(infolist->background==0)
        {
            printf("***********FFF");
            fflush(stdout);
            //fgpid=child1;
            fgpid=child1;
            //signal(SIGCONT,f);
            currentprocess->pid=child1;
            
            //currentprocess
            int cnt=0;
            for(int i=0;i<p->cnt;i++)
            {
                strcpy(currentprocess->command+cnt,p->paramaters[i]);
                cnt+=strlen(p->paramaters[i])+1;
                currentprocess->command[cnt-1]=' ';
                currentprocess->command[cnt]='\0';
            }
            waitpid(child1,&status,WUNTRACED);
            fgpid=-1;
            currentprocess->pid=-1;
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
                    waitpid(child2,&status,WUNTRACED);
                }
                
                
                    //waitpid(child2,&status,0);
            }
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
        //int ff=kill(getppid(),SIGCONT);
        //printf("%d %d iiiii\n",ff,getppid());
        //signal(SIGTSTP,f);
        int fe=run_command(p->command,p->paramaters);
        printf("***********%d&&&&&****",fe);
        exit(fe);
        //execvp(p->command,p->paramaters);
    }


    return 1;
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
        list_jobs(para+1);
        cleanprocess();
        if(processlist->next!=NULL)
        {
            printf("shit\n");
        }
        return 0;
    }
    else if(re==6)//fg
    {
        return DO_FG;
    }
    
    return -1;
}

short exec_fg()
{
    //waitpid(processlist->next->pid,NULL,0);
    //return 0;
    DIR*dir;
    pro_node*p=processlist;
    while(p->next!=NULL)
    {
        readinfo(p->next);
        if(p->next->status==RUNNING)
        {
            for(int i=0;i<MAXPID;i++)
            {
                if(PIDTABLE[i]==p->next->pid)
                {
                    //printf("ooooooo\n");
                    //waitpid(PIDTABLE[i],NULL,0);
                    PIDTABLE[i]=0;
                    break;
                }
            }
            printf("pid3:%d\n",getpid());
            printf("%s\n",p->next->command);
            pid_t pid=p->next->pid;

            pro_node*pt=p->next;
            p->next=p->next->next;
            currentprocess->pid=pt->pid;
            strcpy(currentprocess->command,pt->command);
            free(pt->command);
            free(pt);
            printf("%d\n",pid);
            fflush(stdout);
            fgpid=pid;
            waitpid(pid,NULL,WUNTRACED);
            currentprocess->pid=-1;
            fgpid=-1;



            /*dir=opendir(buf);
            while(dir)
            {
                dir=opendir(buf);
                usleep(100000);
                //for(int i=0;i<10000000;i++);
            }*/

            //printf("%d***\n",fff);
            //return 0;
        }
        if(p->next!=NULL)
            p=p->next;
    }
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

void clear()
{
    printf("\033[1H\033[2J");
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
                perror("dir:Dirs are too many!");
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
                    //printf("iiiiiiiiii\n");
                    return 2;
                }
            }
        }
    }
    
    if(cntdir==0)
    {
        dir[cntdir++]=(char*)malloc(sizeof(char)*1000);
        getcwd(dir[cntdir-1],1000);
        //printf("%s*******",dir[cntdir-1]);
    }
    //printf("%s*******",dir[cntdir-1]);
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
                printf("FFFFUUUUU %s %d \n",path_dir,flag);
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
        //printf("%s\n",filenames[cntfile]);
        cntfile++;
    }
    char* temp;
    //qsort(filenames,cntfile,sizeof(char*),cmp);
    for (int i = 0; i < cntfile - 1; i++)
        for (int j = 0; j < cntfile - 1 - i; j++) {
            if ( strcmp(filenames[j], filenames[j + 1]) > 0 ) {
                temp=filenames[j+1];
                filenames[j+1]=filenames[j];
                filenames[j]=temp;
                //strcpy(temp, filenames[j + 1]);
                /*temp[strlen(filenames[j + 1])] = '\0';
                strcpy(filenames[j + 1], filenames[j]);
                filenames[j + 1][strlen(filenames[j])] = '\0';
                strcpy(filenames[j], temp);
                filenames[j][strlen(temp)] = '\0';*/
            }
        }
    //free(temp);
    buf=(char*)malloc(sizeof(char)*(strlen(dir) + maxsize + 5));
    strcpy(buf,dir);
    const int base_size=strlen(dir);
    //printf("&&&&&&&&%d\n",cntfile);
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
            //printf("%d\n",re);
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
    //printf("!!!!!!!!!%s %d\n",file,flag);
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
    //printf("name:%s\n",name);
    if(lstat(file,&buf)==-1)
    {
        printf("OOOOOOOO\n");
        perr("dir: File %s can't be found.",file);

        return 1;//level 1
    }
    switch(flag & 3)
    {
        
        case PARA_NONE:case PARA_a:
        {
            //printf("NONE %d\n",flag);
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
            //printf("L\n");
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
            printf("DEFAULT %d\n",flag);
            break;
        }

    }
    //printf("%lu %lu\n",buf.st_blksize,buf.st_blocks);
    return buf.st_size;
}

short exec_pwd()
{
    char *buf=(char*)malloc(sizeof(char)*1000);
    getcwd(buf,1000);
    printf("%s\n",buf);
    free(buf);
    return 1;
}

short set_path(char*dir)
{
    char*path_cd=NULL;
    if(strlen(dir)<=0)
    {
        return 0;
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