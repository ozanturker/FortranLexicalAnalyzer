#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <unistd.h>
#include <dirent.h>
#include <conio.h>

void SelectMyFile(char*,int,int,byte*);

#define KEYDOWN(vkey) (GetAsyncKeyState(vkey) & 0x8000)
#define KEYUP(vkey) !KEYDOWN(vkey)
#define ConsoleClr 7
#define BUF_SIZE 1024
#define DEFTitle "05140002021_05140002022_05150000710   "


void SetString(char* Data,char* Value)
{
    int Index = 0;
    while(Value[Index]!=0x0) Data[Index] = Value[Index++];
    Value[Index] = 0x0;
}

void CopyString(char* Data,char* Value)
{
    int Index = 0,Index2 = 0;

    while(Data[Index] !=0x0)Index++;
    while(Value[Index2] != 0x0) Data[Index + Index2] = Value[Index2++];
    Data[Index + Index2] = 0x0;
}

char* TrimString(char * s) {
    int l = strlen(s);

    while(isspace(s[l - 1])) --l;
    while(*s && isspace(*s)) ++s, --l;

    return s;
}

HANDLE OpenConsole(char* FileName,HANDLE mainhSrc)
{
    HANDLE hScr = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(hScr);
    SetConsoleTitle(FileName);

	FILE* TextFile = fopen(FileName,"r");

    if (!TextFile)
    {
        CloseConsole(hScr,mainhSrc);
        SetConsoleTextAttribute(mainhSrc, 15* 12);
        printf("\n\n\n'%s' Acilamadi.",FileName);
        SetConsoleTitle(DEFTitle);
        return NULL;
    }

    fseek(TextFile, 0, SEEK_END);
    int FileLen = ftell(TextFile);
    char* chr = malloc(FileLen +1);
    fseek(TextFile, 0, SEEK_SET);
    fread(chr,FileLen,1,TextFile);

    fclose(TextFile);

    chr[FileLen] = 0x0;
    DWORD written;

    WriteFile(hScr, chr, strlen(chr), &written, NULL);


    SetConsoleTextAttribute(hScr, 16* 15);
    WriteFile(hScr, "\n\n\n\nHTX Ekranina Donmek Icin ESC ye Basiniz.", 44, &written, NULL);

	return hScr;
}

void CloseConsole(HANDLE hScr,HANDLE mainhSrc)
{
    SetConsoleActiveScreenBuffer(mainhSrc);
	CloseHandle(hScr);
	SetConsoleTitle(DEFTitle);
}

struct myStackList
{
    char* Value;
    struct myStackList *nextValue;
};

void AddItem(struct myStackList* Stack,char* Value,int* Count)
{
    struct myStackList* Gecici = Stack;

    while(Gecici->nextValue != NULL)
        Gecici = Gecici->nextValue;

    Gecici->nextValue = malloc(sizeof(struct myStackList));
    Gecici->nextValue->Value = Value;
    Gecici->nextValue->nextValue = NULL;
    (*Count)++;
}

char* GetItem(struct myStackList* Stack,int* Count)
{
    struct myStackList* Gecici = Stack;
    struct myStackList* Onceki = NULL;

    while(Gecici->nextValue != NULL)
    {
        Onceki = Gecici;
        Gecici = Gecici->nextValue;
    }

    Onceki->nextValue = NULL;
    (*Count)--;
    return Gecici->Value;
}

char* ReadItem(struct myStackList* Stack)
{
    struct myStackList* Gecici = Stack;

    while(Gecici->nextValue != NULL)
        Gecici = Gecici->nextValue;

    return Gecici->Value;
}

char* ReadItem2(struct myStackList* Stack,int Where)
{
    struct myStackList* Gecici = Stack;
    int WhereIam = 0;

    while(Gecici->nextValue != NULL)
    {
        if(WhereIam == Where) return Gecici->Value;
        Gecici = Gecici->nextValue;
        WhereIam++;
    }

    return Gecici->Value;
}

void SetCenter(int X,int Y)
{
	HWND ConsoleWindow = GetForegroundWindow();
	RECT rS;

	GetWindowRect(GetDesktopWindow(),&rS);
	SetWindowPos(ConsoleWindow,NULL,(rS.right  - rS.left - X) / 2,(rS.bottom - rS.top  - Y) / 2, X, Y, SWP_SHOWWINDOW );
	SetConsoleTitle(DEFTitle);
}

int main(int argc, char *argv[])
{
    SetCenter(1000,500);

    struct myStackList* myList;
    myList = malloc(sizeof(struct myStackList));
    myList->nextValue = NULL;
    myList->Value = NULL;
    int ListCount = 0;

    byte* KState = malloc(3);
    KState[0] = 0;
    KState[1] = 0;
    KState[2] = 0;

    char* MainDir = malloc(255);
    GetCurrentDirectory(255,MainDir);

    SelectMyFile(MainDir,0,2,KState);

    KState[0] = 2;
    KState[1] = 2;
    KState[2] = 2;

    PrintHTXFile(MainDir,myList,&ListCount,KState,0);

    while(1==1);

    return 0;
}

int ReturnColorID(char* ColorName)
{
    if(!strcmp(ColorName,"red"))
        return 12;
    else if(!strcmp(ColorName,"blue"))
        return 9;
}

void PrintHTXFile(char* FileName,struct myStackList* myList,int* ListCount,byte* KState,int SelectedLinkedID)
{
    system("cls");

    FILE* HTXFile = fopen(FileName,"r");

    if (!HTXFile)
    {
        printf("'%s' Acilamadi.",FileName);
        return;
    }

    fseek(HTXFile, 0, SEEK_END);
    int FileLen = ftell(HTXFile);
    char* chr = malloc(FileLen +1),c;
    fseek(HTXFile, 0, SEEK_SET);
    fread(chr,FileLen,1,HTXFile);
    chr[FileLen-7] = 0x0;

    fclose(HTXFile);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    struct myStackList* myStack;
    myStack = malloc(sizeof(struct myStackList));
    myStack->nextValue = NULL;
    myStack->Value = NULL;
    int StackCount = 0;


    int StartedP = -1,EndP = -1,StartedCom = -1,Index = 0,strLen = 0;
    char* sColor;
    char* eColor;

    int LinkedCount = 0;

    while(chr[Index] != 0x0)
    {
        if(chr[Index] == '/' && FileLen >= Index+1 && chr[Index+1] == '*') //aciklama Satýrý mý ?
        {
            Index +=2;
            while(chr[Index] != '*' || (FileLen >= Index +1 && chr[Index + 1] != '/'))
                Index++;
            Index +=2;
        }
        else if(chr[Index] == '[')
        {
            StartedP = Index + 1;
            while(chr[Index] != ']')
                Index++;

            strLen = Index - StartedP;
            sColor = malloc(strLen+ 1);
            while(StartedP != Index)
                sColor[strLen - (Index - StartedP)] = chr[StartedP++];

            sColor[strLen] = 0x0;

            char* LinkedFileName = strstr(sColor,"|");
            if(LinkedFileName) // Bu Bir Linked
            {
                char* LinkedTag = malloc(strlen(sColor));
                strcpy(LinkedTag,sColor);
                LinkedTag[LinkedFileName - sColor] = 0x0;

                LinkedFileName++;

                AddItem(myList,LinkedFileName,ListCount);

                GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
                SetConsoleTextAttribute(hConsole, 10);

                if(SelectedLinkedID == LinkedCount)
                    SetConsoleTextAttribute(hConsole, 16 * 10);

                printf("%s",LinkedTag);
                SetConsoleTextAttribute(hConsole, consoleInfo.wAttributes);

                LinkedCount++;
            }
            else if(sColor[0] == 'e' && sColor[1] == 'n' && sColor[2] == 'd' && sColor[3] == '_')
            {
                char* eColor = GetItem(myStack,&StackCount);

                if(!strcmp(sColor+4,eColor))
                {
                    if(StackCount != 0)
                        SetConsoleTextAttribute(hConsole, ReturnColorID(ReadItem(myStack)));
                    else
                        SetConsoleTextAttribute(hConsole, ConsoleClr);
                }
                else
                {
                    printf("\n\n\n");
                    SetConsoleTextAttribute(hConsole, (16 * 15) + ReturnColorID(eColor));
                    printf("%s   ",eColor);
                    SetConsoleTextAttribute(hConsole, 16* 15);
                    printf("Renginin Kapatilmasi Ile Ilgili Bir Sorun Olustu.",eColor);
                    return;
                }
            }
            else
            {


                SetConsoleTextAttribute(hConsole, ReturnColorID(sColor));

                AddItem(myStack,sColor,&StackCount);
            }
        }
        else
            printf("%c",chr[Index]);


        Index++;
    }

    int mChar = 0;

    WHILEKP:
    while(1==1)
    {
        if(KState[0] == 0 && KEYDOWN(VK_UP)){
            mChar = -1;
            KState[0] = 1;
            break;
        }
        else if(KState[1] == 0 && KEYDOWN(VK_DOWN)){
            mChar = 1;
            KState[1] = 1;
            break;
        }
        else if(KState[2] == 0 && KEYDOWN(VK_RETURN)){
            mChar = 0;
            KState[2] = 1;
            break;
        }

        if(KEYUP(VK_UP))
            KState[0] = 0;
        if(KEYUP(VK_DOWN))
            KState[1] = 0;
        if(KEYUP(VK_RETURN))
            KState[2] = 0;
    }

    if(KState[2] == 1)
    {
        HANDLE hScr, mainhSrc;
        mainhSrc = GetStdHandle(STD_OUTPUT_HANDLE);

        char* FName = ReadItem2(myList,SelectedLinkedID+1);
        FName = TrimString(FName);
        hScr = OpenConsole(FName,mainhSrc);

        for(c=getch(); c != 0x1b && c != 0x03;c=getch());

        CloseConsole(hScr,mainhSrc);
        goto WHILEKP;
    }

    if(KState[0] == 1 || KState[1] == 1 )
    {
        SelectedLinkedID += mChar;

        if(SelectedLinkedID < 0) SelectedLinkedID = LinkedCount - 1;
        if(SelectedLinkedID > LinkedCount - 1) SelectedLinkedID = 0;

        PrintHTXFile(FileName,myList,ListCount,KState,SelectedLinkedID);

        KState[0] = 2;
        KState[1] = 2;
    }
}

void SelectMyFile(char* Directory, int LineNumber,int ColorNumber,byte* State)
{
    system("cls");

    int mChar = 0;
    char* SelectedFileName;
    DIR *dp;
    struct dirent *ep;
    int NowLinenumber = 0;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, 12);
    printf("Su Anki Konum: %s\n\n",Directory);
    SetConsoleTextAttribute(hConsole, ConsoleClr);

    dp = opendir (Directory);
    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            if(NowLinenumber == LineNumber)
            {
                SetConsoleTextAttribute(hConsole, ColorNumber);

                SelectedFileName = malloc(ep->d_namlen+1);
                SetString(SelectedFileName,ep->d_name);
                SelectedFileName[ep->d_namlen] = 0x0;
            }

            if(ep->d_name[1] == '.')
            {
                printf("  -> %s\n",ep->d_name);
                NowLinenumber++;
            }
            else if(strstr(ep->d_name,".htx"))
            {
                if(NowLinenumber == LineNumber) SetConsoleTextAttribute(hConsole, ColorNumber);
                else  SetConsoleTextAttribute(hConsole, 13);

                printf("   -> [HTX File] %s\n",ep->d_name);
                SetConsoleTextAttribute(hConsole, ConsoleClr);
                NowLinenumber++;
            }
            else if(ep->d_name[0] != '.')
            {
                char* TestDirec = malloc(255);
                TestDirec[0] = 0x0;
                CopyString(TestDirec,Directory);
                CopyString(TestDirec,"\\");
                CopyString(TestDirec,ep->d_name);

                DIR* dir = opendir(TestDirec);
                if (dir)
                {
                    closedir(dir);
                    printf("   -> %s\n",ep->d_name);
                    NowLinenumber++;
                }
                free(TestDirec);
            }

            SetConsoleTextAttribute(hConsole, ConsoleClr);
        }

        closedir (dp);
    }

    while(1==1)
    {
        if(State[0] == 0 && KEYDOWN(VK_UP)){
            mChar = -1;
            State[0] = 1;
            break;
        }
        else if(State[1] == 0 && KEYDOWN(VK_DOWN)){
            mChar = 1;
            State[1] = 1;
            break;
        }
        else if(State[2] == 0 && KEYDOWN(VK_RETURN)){
            mChar = 0;
            State[2] = 1;
            break;
        }

        if(KEYUP(VK_UP))
            State[0] = 0;
        if(KEYUP(VK_DOWN))
            State[1] = 0;
        if(KEYUP(VK_RETURN))
            State[2] = 0;
    }

    if(State[2] == 1)
    {
        if(LineNumber == 0)
        {
            int index = 0;
            while(Directory[index] != 0x0) index++;
            while(Directory[index] != '\\' && Directory[index] != '/') index--;
            Directory[index] = 0x0;
            State[2] = 2;
            SelectMyFile(Directory,LineNumber,ColorNumber,State);
        }
        else if(LineNumber > 0)
        {
            int Bidur = 0;
            CopyString(Directory,"\\");
            CopyString(Directory,SelectedFileName);

            DIR* dir = opendir(Directory);
            if (dir)
            {
                closedir(dir);
                State[2] = 2;
                SelectMyFile(Directory,1,ColorNumber,State);
            }
            else
            {
                State[2] = 2;
                return Directory;
            }
        }
        State[2] = 2;
    }
    if(State[0] == 1 || State[1] == 1 )
    {
        LineNumber += mChar;

        if(LineNumber < 0) LineNumber = NowLinenumber - 1;
        if(LineNumber > NowLinenumber - 1) LineNumber = 0;

        free(SelectedFileName);

        State[0] = 2;
        State[1] = 2;

        SelectMyFile(Directory,LineNumber,ColorNumber,State);


    }
}




