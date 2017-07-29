#include "myhead.h"

int read_command()
{
    char* re=fgets(buf,BUFFSIZE,stdin);
    if(re==NULL)
    {
        puts("");
        return 0;
    }
    if(buf[0]=='\0')
    {
        return -1;
    }

    return parse_input();
    return 0;
}
int parse_input()
{
    int count=0;
    char * p_st=buf;
    char * p_end=buf;
    int in_string=0;
    while(1)
    {
        
        if((*p_st)=='\n')
        {
            if(count==0)
            {
                return -1;
            }
            else{
                *p_st='\0';
                return count;
            }
        }
        while(  ((*p_st)==' '&&(*p_end)==' ') || ( (*p_st)=='\t'&&(*p_end)=='\t' ) || (((*p_st)=='\0')&&((*p_end)=='\0')) )
        {
            p_st++;p_end++;
        }
        if(*p_st=='\"')
        {
            in_string=1;
            p_end++;
        }
        if(in_string)
        {
            while(in_string)
            {
                if(*p_end=='\"')
                {
                    in_string=0;p_end++;
                    if(*p_end!=' '||*p_end!='\n')
                    {
                        return -1;//error
                    }
                    else{
                        if(*p_end==' ')
                        {
                            *p_end='\0';
                        }
                    }
                }
                else{
                    p_end++;
                }
            }
        }
        else{
            while(*p_end!=' '&&*p_end!='\n')
            {
                p_end++;
            }
        }
        if(*p_end==' ')
        {
            *p_end='\0';
        }
        
        parameters[count++]=p_st;
        p_st=p_end;
    }
}
short parsing(int n)
{
    short ok=0;
    short in=0;
    int cnt=0;
    init_parse_info(&infolist);
    init_info_node(&(infolist->info));
    //printf("@@@@@@%d\n",infolist->info->cnt);
    struct info_node*p=infolist->info;
    for(int i=0;i<n;i++)
    {
        //printf("^^^^:%d %s\n",i,parameters[i]);
        if(!in)
        {
            p->command=parameters[i];
            p->paramaters[(p->cnt)++]=p->command;
            p->paramaters[p->cnt]=NULL;
            in=1;
        }
        else{
            if(strcmp(parameters[i],"|")==0)
            {
                if(i==0)
                {
                    return -1;
                }
                p->piped=1;
                init_info_node(&(p->next));
                p=p->next;
                in=0;
            }
            else if(strcmp(parameters[i],"&")==0)
            {
                infolist->background=1;
                if(i!=n-1)
                {
                    return -1;
                }
                
            }
            else if(strcmp(parameters[i],"<")==0)
            {
                if(p->input_direct_append==1||p->input_direct_noappend==1)
                {
                    i++;
                    continue;
                }
                p->input_direct_noappend=1;
                if(i>=n-1)
                {
                    return -1;
                }
                else{
                    p->inputfile=parameters[i+1];
                    i+=1;
                }
            }
            else if(strcmp(parameters[i],"<<")==0)
            {
                if(p->input_direct_append==1||p->input_direct_noappend==1)
                {
                    i++;
                    continue;
                }
                p->input_direct_append=1;
                if(i>=n-1)
                {
                    return -1;
                }
                else{
                    p->inputfile=parameters[i+1];
                    i+=1;
                }
            }
            else if(strcmp(parameters[i],">")==0)
            {
                if(p->output_direct_append==1||p->output_direct_noappend==1)
                {
                    i++;
                    continue;
                }
                p->output_direct_noappend=1;
                if(i>=n-1)
                {
                    return -1;
                }
                else{
                    p->outputfile=parameters[i+1];
                    i++;
                }
            }
            else if(strcmp(parameters[i],">>")==0)
            {
                if(p->output_direct_append==1||p->output_direct_noappend==1)
                {
                    i++;
                    continue;
                }
                p->output_direct_append=1;
                if(i>=n-1)
                {
                    return -1;
                }
                else{
                    p->outputfile=parameters[i+1];
                    i++;
                }
            }
            else{
                p->paramaters[(p->cnt)++]=parameters[i];
                p->paramaters[p->cnt]=NULL;
            }
        }
    }
    
    return 1;
}
