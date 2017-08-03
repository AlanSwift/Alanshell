#include "myhead.h"



int main(int argc,char**argv)
{
    /***********define parameter*******/

    /***********define parameter*******/
    

    init_environment(argc,argv);

    if(!init())
    {
        perror("System: Shell init error.");
        return -1;
    }
    if(signal(SIGCHLD,signal_handler)==SIG_ERR)
    {
        perror("System: Signal error.");
        return -1;
    }
    if(argc==1)
    {
        main_loop(argv);
    }
    else{
        file_loop(argc,argv);
    }
    return 0;   
}
void file_loop(int argc,char**argv)
{
    FILE*fp=NULL;
    for(int i=1;i<argc;i++)
    {
        fp=fopen(argv[i],"r");
        if(fp==NULL)
        {
            perror("System: No such file");
            continue;
        }
        else{
            while(1)
            {
                int re=read_command(fp);
                if(re==-2)
                {
                    goto OUT;
                }
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

                    re=parsing(re);
                    if(re==-1)
                    {
                        printf("The command input is not valid.\n");
                        continue;
                    }
                    else{
                        int re=run_exec(infolist->info);
                        if(re==DO_EXIT)
                        {
                            return ;
                        }
                    }
            
                }
            }
            OUT:
                fclose(fp);
                continue;
                

        }
    }
}

void main_loop(char**argv)
{
    printf("----------------Myshell-------------\n");

    while(1)
    {

        input_prompt();

        int re=read_command(stdin);
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
            #ifdef _debug
            for(int i=0;i<re;i++)
            {
                printf("%d:%s\n",i,parameters[i]);
            }
            #endif

            re=parsing(re);
            if(re==-1)
            {
                printf("The command input is not valid.\n");
                continue;
            }
            else{
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
                int re=run_exec(infolist->info);
                if(re==DO_EXIT)
                {
                    printf("Bye Byte.\n");
                    return ;
                }
            }
        }
    }
}
