#include "myhead.h"

static char* get_path()
{

}

int main()
{
    /***********define parameter*******/

    /***********define parameter*******/


    if(!init())
    {
        printf("Shell init error\n");
        return -1;
    }

    printf("-------Myshell---------\n");
    while(1)
    {
        input_prompt();
        int re=read_command();
        printf("*****\n");
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
        }
        return 0;
    }
}
