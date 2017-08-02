#include "myhead.h"
static char* get_path()
{

}

int main(int argc,char**argv)
{
    /***********define parameter*******/

    /***********define parameter*******/
    printf("%d %s\n",argc,argv[0]);
    init_environment(argc,argv);
    int in_fd,outfd;

    if(!init())
    {
        printf("Shell init error\n");
        return -1;
    }
    if(signal(SIGCHLD,signal_handler)==SIG_ERR)
    {
        printf("signal error.\n");
        return -1;
    }
    /*
    if(signal(SIGTSTP,ctrl_z)==SIG_ERR)
    {
        printf("signal error....\n");
        return -1;
    }*/

    printf("-------Myshell---------\n");

    while(1)
    {
        /*in_fd=open("/dev/tty", O_CREAT |O_RDONLY, 0666);
        close(fileno(stdin));
        dup2(in_fd,fileno(stdin));
        close(in_fd);*/
        freopen("/dev/tty","r",stdin);
        freopen( "/dev/tty", "w", stdout );

        input_prompt();
        int re=read_command();
        //printf("*****ininin\n");
        if(re==-1)
        {
            printf("Input error!\n");
            continue;
        }
        if(re==0)
        {
            continue;
        }
        else{
            for(int i=0;i<re;i++)
            {
                printf("%d:%s\n",i,parameters[i]);
            }
            //printf("&&&&&%d\n",re);
            re=parsing(re);

            //printf("&&&&&&&&&&&&&^^^$$$\n");
            if(re==-1)
            {
                printf("The command input is not valid.\n");
                continue;
            }
            else{
                //return 0;
                #ifdef _debug
                printf("background: %d\n",infolist->background);
                struct info_node*p=infolist->info;
                while(p!=NULL)
                {
                    printf("***********\n");
                    printf("command:%s\n",p->command);
                    printf("parameters:\n");
                    for(int i=0;i<p->cnt;i++)
                    {
                        printf("%s ",p->paramaters[i]);
                    }
                    printf("\n");
                    printf("inputfile: %s\n",p->inputfile);
                    printf("outputfile: %s\n",p->outputfile);
                    printf("piped: %d\n",p->piped);
                    printf("input dire noappend: %d, append: %d\n",p->input_direct_noappend,p->input_direct_append);
                    printf("output dire noappend: %d, append: %d\n",p->output_direct_noappend,p->output_direct_append);
                    p=p->next;
                }

                #endif
                printf("#########\n");
                run_exec(infolist->info);
            }
        }
        //return 0;
    }
}
