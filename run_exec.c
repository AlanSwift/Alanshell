#include "myhead.h"


short run_exec(struct info_node*p)
{
    int pip_fd[2];
    int status;
    int out_fd,in_fd;
    if(p->piped)
    {
        pipe(pip_fd);
    }

    pid_t child1,child2;
    if( (child1=fork()) != 0 )
    {
        //father
        if(p->piped)
        {
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
                waitpid(child2,&status,0);
            }
        }
        waitpid(child1,&status,0);
    }
    else{//command 1
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
        execvp(p->command,p->paramaters);
    }
    return 1;
}

short is_internal_cmd(char*command)
{
    
}

short run_command(char* command,char**para)
{

}