#ifndef MYAVL_H
#define MYAVL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct avlnode;
typedef struct avlnode *position;
typedef struct avlnode *AVLTree;
struct avlnode{
    char*key;
    char*value;
    AVLTree left;
    AVLTree right;
    int height;
};
int height(position p);
AVLTree Insert(char*key,char*value,AVLTree T);
AVLTree Delete(char*key,AVLTree T);
position Findmin(AVLTree);
position Findmax(AVLTree);
position Find(char*key,AVLTree);
short UpdateAVL(char*key,char*value,AVLTree);
void tracetree(AVLTree);
int max(int a,int b);
int min(int a,int b);

AVLTree single_rotate_with_left(AVLTree);
AVLTree single_rotate_with_right(AVLTree);
AVLTree double_rotate_with_left(AVLTree);
AVLTree double_rotate_with_right(AVLTree);

#endif