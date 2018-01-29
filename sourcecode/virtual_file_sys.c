#define _CRT_SECURE_NO_WARNINGS
#define MAXINODE 50
#define READ 1
#define WRITE 2
#define MAXFILESIZE 1024
#define REGULAR 1
#define SPECIAL 2
#define START 0
#define CURRENT 1
#define END 2

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>

typedef struct superblock
{
	int TotalInodes;
	int FreeInodes;
}SUPERBLOCK,*PSUPERBLOCK;

typedef struct inode
{
    char FileName[50];
    int InodeNumber;
    int FileSize;
    int FileActualSize;
    int FileType;
    char *Buffer;
    int LinkCount;
    int ReferenceCount;
    int Permission;
    struct inode *next;
}INODE,*PINODE,**PPINODE;

typedef struct filetable
{
    int readoffset;
    int writeoffset;
    int count;
    int mode;
    PINODE ptrinode;
}FILETABLE,*PFILETABLE;

typedef struct ufdt
{
    PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head=NULL;

void man(char *name)
{
    if(name == NULL)
        return;
    if(_stricmp(name,"create")==0)
    {
        printf("Description : Used to create a new regular file\n");
        printf("Usage : create File_name Permission\n");
    }
    else if(_stricmp(name,"read")==0)
    {
        printf("Description : Used to read data from regular file\n");
        printf("Usage : read File_name No_Of_Bytes_To_Read\n");
    }
    else if(_stricmp(name,"write")==0)
    {
        printf("Description : Used to write into regular file\n");
        printf("Usage : write File_name\n After that enter the data that we want to write\n");
    }
    else if(_stricmp(name,"ls")==0)
    {
        printf("Description : Used to list all information of files\n");
        printf("Usage : ls\n");
    }
    else if(_stricmp(name,"stat")==0)
    {
        printf("Description : Used to display information of file\n");
        printf("Usage : stat File_Name\n");
    }
    else if(_stricmp(name,"fstat")==0)
    {
        printf("Description : Used to display information of file\n");
        printf("Usage : fstat File_Name\n");
    }
    else if(_stricmp(name,"truncate")==0)
    {
        printf("Description : Used to remove data from file\n");
        printf("Usage : truncate File_name\n");
    }
    else if(_stricmp(name,"open")==0)
    {
        printf("Description : Used to open existing file\n");
        printf("Usage : open File_name mode\n");
    }
    else if(_stricmp(name,"close")==0)
    {
        printf("Description : Used to close opened files\n");
        printf("Usage : close File_name\n");
    }
    else if(_stricmp(name,"closeall")==0)
    {
        printf("Description : Used to closed all opened files\n");
        printf("Usage : closeall\n");
    }
    else if(_stricmp(name,"lseek")==0)
    {
         printf("Description : Used to change file offset\n");
        printf("Usage : lseek File_name ChangeInOffset StartPoint\n");
    }
    else if(_stricmp(name,"rm")==0)
    {
         printf("Description : Used to delete the file\n");
        printf("Usage : rm File_name\n");
    }
    else
    {
         printf("ERROR : No manual entry availabe.\n");
    }

}

void DisplayHelp()
{
    printf("ls : To list out all files\n");
    printf("clear : To clear console\n");
    printf("open : To open the file\n");
    printf("close : To close the file\n");
    printf("closeall : To close all opened files\n");
    printf("read : To read the contents from file\n");
    printf("write : To write contents into file\n");
    printf("exit : To terminate file system\n");
    printf("stat : To display information of file using name\n");
    printf("fstat : To display information of file using file descriptor\n");
    printf("truncate : To remove all data from file\n");
    printf("rm : To delete the file\n");
}

int GetFDFromName(char *name)
{
    int i=0;
    while(i<MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable !=NULL)
        {
            if(_stricmp((UFDTArr[i].ptrfiletable->ptrinode->FileName),name)==0)
                break;
        }
        i++;
    }
    if(i==MAXINODE)
        return -1;
    else
        return i;
}

PINODE Get_Inode(char *name)
{
    PINODE temp=head;

    if(name==NULL)
        return NULL;
    while(temp!=NULL)
    {
        if(strcmp(name,temp->FileName)==0)
            break;
        temp=temp->next;
    }
    return temp;
}

void CreateDILB()
{
    PINODE newn=NULL;
    PINODE temp=NULL;

    int i=1;
    while(i<=MAXINODE)
    {
        newn=(PINODE)malloc(sizeof(INODE));
        newn->LinkCount=newn->ReferenceCount=0;
        newn->FileType=newn->FileSize=0;
        newn->Buffer=NULL;
        newn->next=NULL;
        newn->InodeNumber=i;
        if(temp==NULL)
        {
            head=newn;
            temp=head;
        }
        else
        {
            temp->next=newn;
            temp=temp->next;
        }
        i++;
    }
}

void InitializeSuperBlock()
{
    int i=0;
    while(i<MAXINODE)
    {
        UFDTArr[i].ptrfiletable=NULL;
        i++;
    }
    SUPERBLOCKobj.TotalInodes=MAXINODE;
    SUPERBLOCKobj.FreeInodes=MAXINODE;
}

int CreateFile(char *name,int Permission)
{
    int i=0;
    PINODE temp=head;

    if((name==NULL) || (Permission==0) || (Permission>3))
        return -1;
    if(SUPERBLOCKobj.FreeInodes==0)
        return -2;
    if(Get_Inode(name)!=NULL)
        return -3;
    (SUPERBLOCKobj.FreeInodes)--;

    while(temp!=NULL)
    {
        if(temp->FileType==0)
            break;
        temp=temp->next;
    }

    while(i<MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable==NULL)
            break;
        i++;
    }

    UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable==NULL)
        return -4;

    UFDTArr[i].ptrfiletable->count=1;
    UFDTArr[i].ptrfiletable->mode=Permission;
    UFDTArr[i].ptrfiletable->readoffset=0;
    UFDTArr[i].ptrfiletable->writeoffset=0;

    UFDTArr[i].ptrfiletable->ptrinode=temp;
    strcpy_s(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
    UFDTArr[i].ptrfiletable->FileType=REGULAR;
    UFDTArr[i].ptrfiletable->ReferenceCount=1;
    UFDTArr[i].ptrfiletable->LinkCount=1;
    UFDTArr[i].ptrfiletable->FileSize=MAXFILESIZE;
    UFDTArr[i].ptrfiletable->FileActualSize=0;
    UFDTArr[i].ptrfiletable->Permission=Permission;
    UFDTArr[i].ptrfiletable->Buffer=(char *)malloc(MAXFILESIZE);
    memset(UFDTArr[i].ptrfiletable->ptrinode->Buffer,0,1024);

    return i;
}

int rm_file(char * name)
{
    int fd=0;
    fd=GetFDFromName(name);
    if(fd==-1)
        return -1;
    (UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;

    if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount==0)
    {
        UFDTArr[fd].ptrfiletable->ptrinode->FileType=0;
        free(UFDTArr[fd].ptrfiletable);
    }

    UFDTArr[fd].ptrfiletable=NULL;
    (SUPERBLOCKobj.FreeInodes)++;
}

int ReadFile(int fd,char *arr,int isize)
{
    int read_size=0;
    if(UFDTArr[fd].ptrfiletable==NULL)
        return -1;
    if(UFDTArr[fd].ptrfiletable->mode!=READ && UFDTArr[fd].ptrfiletable->mode!=READ+WRITE)
        return -2;
    if((UFDTArr[fd].ptrfiletable->ptrinode->Permission!=READ) && (UFDTArr[fd].ptrfiletable->ptrinode->Permission!=READ+WRITE))
        return -2;
    if(UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
        return -3;
    
    if(UFDTArr[fd].ptrfiletable->ptrinode->FileType!=REGULAR)
        return -4;
    read_size=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);
    if(read_size < isize)
    {
        strncpy(arr,
                (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),
                read_size);

        UFDTArr[fd].ptrfiletable->readoffset = (UFDTArr[fd].ptrfiletable->readoffset)+read_size;
    }
    else
    {
         strncpy(arr,
                 (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),
                 isize);

        UFDTArr[fd].ptrfiletable->readoffset = (UFDTArr[fd].ptrfiletable->readoffset)+isize;

    }
    return isize;
}

int WriteFile(int fd,char *arr,int isize)
{
    if(((UFDTArr[fd].ptrfiletable->mode)!=WRITE) && ((UFDTArr[fd].ptrfiletable->mode)!=READ+WRITE))
        return -1;
    
    if(((UFDTArr[fd].ptrfiletable->ptrinode->Permission)!=WRITE) && ((UFDTArr[fd].ptrfiletable->ptrinode->Permission)!=READ+WRITE))
        return -1;
    
    if((UFDTArr[fd].ptrfiletable->writeoffset) == MAXFILESIZE)
        return -2;
    
    if((UFDTArr[fd].ptrfiletable->ptrinode->FileType)!=REGULAR)
        return -3;

    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);
    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = (UFDTArr[fd].ptrfiletable->readoffset)+read_size;
    }
    else
    {
         strncpy(arr,
                 (UFDTArr[fd].ptrfiletable->ptrinode->Buffer) + (UFDTArr[fd].ptrfiletable->readoffset),
                 isize);

        UFDTArr[fd].ptrfiletable->readoffset = (UFDTArr[fd].ptrfiletable->readoffset)+isize;

    }
    return isize;
}


int OpenFile(char *name,int mode)
{
    int i=0;
    PINODE temp=NULL;

    if(name==NULL || mode<=0)
        return -1;
    temp = Get_Inode(name);
    if(temp == NULL)
        return -2;

    if(temp->Permission<mode)
        return -3;

    while(i<MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable==NULL)
        {
            break;
        }

        i++;
    }

    UFDTArr[i].ptrfiletable=(PFILETABLE)malloc(sizeof(FILETABLE));
    if(UFDTArr[i].ptrfiletable==NULL)
        return -1;
    UFDTArr[i].ptrfiletable->count =1;
    UFDTArr[i].ptrfiletable->mode=mode;

    if(mode==READ+WRITE)
    {
        UFDTArr[i].ptrfiletable->readoffset=0;
        UFDTArr[i].ptrfiletable->writeoffset=0;
    }
    else if(mode == READ)
    {
        UFDTArr[i].ptrfiletable->readoffset=0;
    }
    else if(mode == WRITE)
    {
        UFDTArr[i].ptrfiletable->writeoffset=0;
    }
    
    UFDTArr[i].ptrfiletable->ptrinode=temp;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;
    return i;
}


void CloseFileByFD(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset=0;
    UFDTArr[fd].ptrfiletable->writeoffset=0;
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
}

int CloseFileByName(char *name)
{
    int i=0;
    i=GetFDFromName(name);
    if(i == -1)
        return -1;


    UFDTArr[i].ptrfiletable->readoffset=0;
    UFDTArr[i].ptrfiletable->writeoffset=0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
    return 0;
}

void CloseAllFile()
{
    int i=0;
    while(i<MAXINODE)
    {
        if(UFDTArr[i].ptrfiletable!=NULL)
        {
            UFDTArr[i].ptrfiletable->readoffset=0;
            UFDTArr[i].ptrfiletable->writeoffset=0;
            (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
        }
        i++;
    }
}

int LseekFile(int fd,int size,int form)
{
    if((fd<0) || (form>2))
        return -1;
    if(UFDTArr[fd].ptrfiletable==NULL)
        return -1;
    if((UFDTArr[fd].ptrfiletable->mode==READ) || (UFDTArr[i].ptrfiletable->mode==READ+WRITE))
    {
        if(form==CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                return -1;
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size)<0)
                return -1;
            (UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->readoffset)+size;
        }
        else if(form == START)
        {
            if(size>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                return -1;
            if(size<0)
                return -1;
            (UFDTArr[fd].ptrfiletable->readoffset)=size;
        }
        else if(form == END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size >MAXFILESIZE)
                return -1;
            if(((UFDTArr[fd].ptrfiletable->readoffset)+size)<0)
                return -1;
            (UFDTArr[fd].ptrfiletable->readoffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
        }
    }
    else if((UFDTArr[fd].ptrfiletable->mode==WRITE) 
    {
        if(form==CURRENT)
        {
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size) > MAXFILESIZE)
                return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)<0)
                return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size) > (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
            {
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=(UFDTArr[fd].ptrfiletable->writeoffset)+size;
            }

            (UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->writeoffset)+size;
        }
        else if(form == START)
        {
            if(size > MAXFILESIZE)
                return -1;
            if(size<0)
                return -1;
            if(size>(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize))
                (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)=size;
                
            (UFDTArr[fd].ptrfiletable->writeoffset)=size;
        }
        else if(form == END)
        {
            if((UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size >MAXFILESIZE)
                return -1;
            if(((UFDTArr[fd].ptrfiletable->writeoffset)+size)<0)
                return -1;
            (UFDTArr[fd].ptrfiletable->writeoffset)=(UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+size;
        }
    }
}   

void ls_file()
{
    PINODE temp=head;

    if(SUPERBLOCKobj.FreeInodes == MAXINODE)
    {
        printf("Error: There is no file\n");
        return;
    }
    printf("\nFile Name\tInode number\tFile size\tLink count\n");
    printf("-----------------------------------------------------------\n");
    while(temp!=NULL)
    {
        if(temp->FileType!=0)
        {
            printf("%s\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
        }
        temp=temp->next;
    }
    printf("---------------------------------------------------------\n");
}

int fsta_file(int fd)
{
        PINODE TEMP=head;
}

    }
}
