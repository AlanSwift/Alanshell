#include "myavl.h"
int min(int a,int b)
{
    return a>b?b:a;
}
int max(int a,int b)
{
    return a>b?a:b;
}
int height(position p)
{
    if(p==NULL)
    {
        return -1;
    }
    else{
        return p->height;
    }
}
void tracetree(AVLTree T)
{
    if(!T)
    {
        return;
    }
    else{
        printf("key: %s value: %s\n",T->key,T->value);
        tracetree(T->left);
        tracetree(T->right);
    }
}

AVLTree Insert(char*key,char*value,AVLTree T)
{
    if(key==NULL)
    {
        return T;
    }
    //printf("key: %s value: %s\n",key,value);
    if(T==NULL)
    {
        //printf("FUCK\n");
        T=(AVLTree)malloc(sizeof(struct avlnode));
        if(T==NULL)
        {
            perror("System: No memory.");
            //return T;
        }
        else{
            T->key=(char*)malloc(sizeof(char)*(strlen(key)+5));
            T->value=(char*)malloc(sizeof(char)*(strlen(value)+5));
            strcpy(T->key,key);
            strcpy(T->value,value);
            T->left=NULL;
            T->right=NULL;
            T->height=0;
        }
    }
    else if(strcmp(key,T->key)<0)
    {
        T->left=Insert(key,value,T->left);
        if(height(T->left)-height(T->right)==2)
        {
            if(strcmp(key,T->left->key)<0)
            {
                T=single_rotate_with_left(T);
            }
            else{
                T=double_rotate_with_left(T);
            }
        }
    }
    else if(strcmp(key,T->key)>0)
    {
        T->right=Insert(key,value,T->right);
        if(height(T->right)-height(T->left)==2)
        {
            if(strcmp(key,T->right->key)>0)
            {
                T=single_rotate_with_right(T);
            }
            else{
                T=double_rotate_with_right(T);
            }
        }
    }
    T->height=max(height(T->left),height(T->right))+1;
    return T;
}
short UpdateAVL(char*key,char*value,AVLTree T)
{
    if(!T)
    {
        return 0;
    }
    else if(strcmp(key,T->key)<0)
    {
        return UpdateAVL(key,value,T->left);
    }
    else if(strcmp(key,T->key)>0)
    {
        return UpdateAVL(key,value,T->right);
    }
    else{
        free(T->value);
        T->value=(char*)malloc(sizeof(char)*(strlen(value)+5));
        strcpy(T->value,value);
        return 1;
    }

}

AVLTree Delete(char*key,AVLTree T)
{
    if(T==NULL)
    {
        return T;
    }
    if(strcmp(key,T->key)<0)
    {
        T->left=Delete(key,T->left);
        if(height(T->right)-height(T->left)==2)
        {
            AVLTree p=T->right;
            if(height(p->left)<=height(p->right))
            {
                T=single_rotate_with_right(T);
            }
            else{
                T=double_rotate_with_right(T);
            }
        }
    }
    else if(strcmp(key,T->key)>0)
    {
        T->right=Delete(key,T->right);
        if(height(T->left)-height(T->right)==2)
        {
            AVLTree p=T->left;
            if(height(p->left)<=height(p->right))
            {
                T=single_rotate_with_left(T);
            }
            else{
                T=double_rotate_with_left(T);
            }
        }
    }
    else{
        if((T->left!=NULL)&&(T->right!=NULL))
        {
            if(height(T->left)>height(T->right))
            {
                position p=Findmax(T->left);
                free(T->key);free(T->value);
                T->key=(char*)malloc(sizeof(char)*(strlen(p->key)+5));
                T->value=(char*)malloc(sizeof(char)*(strlen(p->value)+5));
                strcpy(T->key,p->key);
                strcpy(T->value,p->value);
                T->left=Delete(p->key,T->left);
            }
            else{
                position p=Findmin(T->right);
                free(T->key);free(T->value);
                T->key=(char*)malloc(sizeof(char)*(strlen(p->key)+5));
                T->value=(char*)malloc(sizeof(char)*(strlen(p->value)+5));
                strcpy(T->key,p->key);
                strcpy(T->value,p->value);
                T->right=Delete(p->key,T->right);
            }
        }
        else{
            position tmp=T;
            T=(T->left!=NULL)?(T->left):(T->right);
            free(tmp->key);
            free(tmp->value);
            free(tmp);
        }
    }
    if(T!=NULL)
    {
        T->height=max(height(T->left),height(T->right))+1;
    }
    return T;
}
position Findmin(AVLTree T)
{
    while(T)
    {
        if(T->left!=NULL)
        {
            T=T->left;
        }
        else{
            return T;
        }
    }
    return T;
}
position Findmax(AVLTree T)
{
    while(T)
    {
        if(T->right!=NULL)
        {
            T=T->right;
        }
        else{
            return T;
        }
    }
    return T;
}
position Find(char*key,AVLTree T)
{
    if(!T)
    {
        return T;
    }
    if(strcmp(key,T->key)<0)
    {
        return Find(key,T->left);
    }
    else if(strcmp(key,T->key)>0)
    {
        return Find(key,T->right);
    }
    else{
        return T;
    }

}

AVLTree single_rotate_with_left(AVLTree k2)
{
    AVLTree k1;
    k1=k2->left;
    k2->left=k1->right;
    k1->right=k2;
    k2->height=max(height(k2->left),height(k2->right))+1;
    k1->height=max(height(k1->left),k2->height)+1;
    return k1;
}
AVLTree single_rotate_with_right(AVLTree k1)
{
    AVLTree k2;
    k2=k1->right;
    k1->right=k2->left;
    k2->left=k1;
    k1->height=max(height(k1->left),height(k1->right))+1;
    k2->height=max(k1->height,height(k2->right))+1;
    return k2;
}
AVLTree double_rotate_with_left(AVLTree k3)
{
    k3->left=single_rotate_with_right(k3->left);
    return single_rotate_with_left(k3);
}
AVLTree double_rotate_with_right(AVLTree k1)
{
    k1->right=single_rotate_with_left(k1->right);
    printf("&&&&&&:%s\n",k1->right->key);
    return single_rotate_with_right(k1);
}
