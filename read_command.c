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
