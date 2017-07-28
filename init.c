#include "myhead.h"
const int MAXLINE=1024;
const int BUFFSIZE=1024;
const int MAXPARA=1024;
int init()
{
    
    if(( buf = ( char* ) malloc( sizeof( char ) * BUFFSIZE ) ) == NULL )
    {
        return 0;
    }
    if(( parameters = (char**) malloc( sizeof( char* ) * MAXPARA ) ) == NULL )
    {
        return 0;
    }
    return 1;
}