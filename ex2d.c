//mahmoud dawood
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// method to read "file.txt" file
void  readHistory();
//command line method
void  commandline(char sentence[512], int argc, int sentenceLength);
//get command from history
char *commandFromHistory(char* theNumberOFTheCommand,int* );
//count the number of word in sentence "before in ex1 the code was in the loop I put the code in a method new"
int numberOfWord(char * word,int sentenceLength);
/*I made this method in order to check when the user enter !a that a smaller than  the current size of the file
 * the argument to free it if the file failed */
int theCurrentSizeOfTheFile(char *);
//this number to check if the first word is "cd" it changes  by commandline() function
int cdFlag = 0;
int main() {
    char word [512];
    int commandNumber=0;
    int totalWordNumber=0;
    while(1){
        //count the word number in each iteration
        int wordNumber=0;
        // the word without spaces
        char cutWord[512];

        //the current directory path
        char dirPath [512];
        if (getcwd(dirPath, sizeof(dirPath)) == NULL)
        {
            perror("getcwdu() error");
            exit(EXIT_FAILURE);
        }
        //print the current directory path >
        printf("%s >",dirPath);

        fgets(word,512,stdin);
        word[strlen(word)-1]='\0';
        strcpy(cutWord,word);
        // To keep track of non-space character count
        int count = 0;
        for (int i = 0; cutWord[i]; i++)
            if (cutWord[i] != ' ')
                cutWord[count++] = cutWord[i]; // count++
        cutWord[count] = '\0';
        //if exit was entered stop the loop

          if(strcmp(word,"done")==0)
          {
              commandNumber++;
              totalWordNumber++;
              break;
          }
        //open file to add new information
        FILE *file = fopen("file.txt","a");
        if(file == NULL)
        {
            fprintf(stderr, "%s\n", "cannot open file");
            exit(EXIT_FAILURE);
        }
        //if the user enter ! at the first char
        if(cutWord[0]=='!')
        {
            char* commandNum=(char*)(malloc((strlen(cutWord)-1)*sizeof (char)));
            if(commandNum==NULL)
            {
                fprintf(stderr, "%s\n", "malloc error");
                exit(EXIT_FAILURE);
            }
            //new sentence without "!" in the beginning
            for (int i = 0; i <strlen(cutWord) ; ++i)
            {
                commandNum[i]=cutWord[i+1];
            }
            int theLengthOfTheCommand=0;
            char * sentence=commandFromHistory(commandNum,&theLengthOfTheCommand);
            //this number in order to now if the sentence is == history or not
            int check = 0;
            if(theCurrentSizeOfTheFile(commandNum)<atoi(commandNum))
            {
                printf("NOT IN. HISTORY \n");
                check = 0;
            }
            else{
            //because the sentence end full of spaces i check if sentence==history in this way
            for (int i = 0; i < theLengthOfTheCommand; i++)
            {
                if(sentence[i]!="history"[i])
                {
                    check = 1;
                    break;
                }
                if(i==6)
                {
                    readHistory();
                    fputs("history",file);
                    fputc('\n',file);
                    fclose(file);
                }
            }
            }
            //to avoid sending "history" or empty sentence
             if(check == 1)
            {
                commandline(sentence, numberOfWord(sentence,theLengthOfTheCommand), theLengthOfTheCommand);
                if(cdFlag==0){
                    for (int i = 0; i < theLengthOfTheCommand; ++i)
                    {
                        fputc(sentence[i], file );
                    }
                    fputc('\n',file);
                    fclose(file);
                }
            }
            free(commandNum );
            free(sentence );
            continue;
        }
        //if the user entered empty input continue to the next iteration
        if(strcmp(cutWord,"")==0){
            continue;}

        word[strlen(word)]='\0';
        //if history was entered ,call method to print the history and continue to the next iteration
        if(strcmp(cutWord,"history")==0)
        {
            commandNumber++;
            totalWordNumber++;
            fputs(word,file);
            fputc('\n',file);
            readHistory();
            fclose(file);
            continue;
        }
        //count who many word in the sentence
        wordNumber= numberOfWord(word,strlen(word));
        //increase the command number
        commandNumber++;
        //increase the total word in the command
        totalWordNumber+=wordNumber;
        commandline(word, wordNumber, strlen(word));
        if(cdFlag==0){
            fputs(word,file);
            fputc('\n',file);
        }
        fclose(file);
        }
    printf("%d command \n",commandNumber);
    printf("%d words \n",totalWordNumber);
    return 0;
}

void commandline(char sentence[512], int argc , int sentenceLength)
{
    char* argv[argc+1];
    argv[argc]=NULL;
    int wordsCounter=0;
    char wordBuffer[100];
    int wordLength;
    for(int i=0;i<sentenceLength;)
    {
        wordLength=0;
        while (sentence[i]!=' '&&sentence[i]!='\0'&&i<sentenceLength){
        wordBuffer[wordLength++]=sentence[i++];}
        while (sentence[i]==' '&&sentence[i]!='\0'&&i<sentenceLength)
        i++;
        wordBuffer[wordLength] = '\0';
        argv[wordsCounter] = (char*)malloc(wordLength*sizeof(char));
        strcpy(argv[wordsCounter++],wordBuffer);
    }
    if(sentence[sentenceLength-1]==' '||sentence[0]==' ')
    {
        cdFlag = 0;
        printf("delete spaces from the end and the beginning and try again  \n");
        return;
    }
    else if(strcmp(argv[0],"cd")==0)
    {
        cdFlag = 1;
        printf("the command not supported (Yet) \n");
        return;
    }
    else
    {
        cdFlag = 0;
        pid_t x=fork();
        if(x==0)
        {
            execvp(argv[0],argv);
            perror("fork error");
            exit(EXIT_FAILURE);
        }
        wait(NULL);
    }
}

void  readHistory()
{
    FILE *read = fopen("file.txt","r");
    if(read == NULL)
    {
        fprintf(stderr, "%s\n", "cannot open file");
        exit(EXIT_FAILURE);
    }
    char line[513];
    int numberLine=0;
    while(fgets(line,513,read)){
        numberLine++;
        printf("%d : %s",numberLine,line);}
    fclose(read);
}
char *commandFromHistory(char* theNumberOFTheCommand,int* length)
{
    FILE *read = fopen("file.txt","r");
    if(read == NULL)
    {
        free(theNumberOFTheCommand);
        fprintf(stderr, "%s\n", "cannot open file");
        exit(EXIT_FAILURE);
    }
    char* line= (char *)malloc(513*sizeof(char));
    if(line == NULL)
    {
        free(theNumberOFTheCommand);
        fprintf(stderr, "%s\n", "cannot allocate");
        exit(EXIT_FAILURE);
    }
    int numberLine=0;
    while(fgets(line,513,read))
    {
        if(atoi(theNumberOFTheCommand)-1==numberLine){
            int lengthF=0;
            for (int i = 0; i <513 ; i++)
            {
                if (line[i]=='\0')
                    break;
                lengthF++;
            }
            //update the length
            *length=lengthF-1;
            fclose(read);
            return line;}
        numberLine++;
    }
    fclose(read);
    return NULL;
}

// this method will count the word in the sentence
int numberOfWord(char * word,int sentenceLength)
{
    int count=0;
    for (int i=0;i<sentenceLength;i++){
        //if in  the first place there are word
        if (i==0&&word[i]!=' '&&(i+1)<sentenceLength)
            count++;
        //if ' ' came before char in sequence this mean there is word
        if(word[i]==' '&&word[i+1]!=' '&&(i+1)<sentenceLength)
            count++;
    }
    return count;
}
int theCurrentSizeOfTheFile(char * s){
    int count = 0;
    FILE *read = fopen("file.txt","r");
    if(read == NULL)
    {
        free(s);
        fprintf(stderr, "%s\n", "cannot open file");
        exit(EXIT_FAILURE);
    }
    char line[513];
    while(fgets(line,513,read))
    {
        count++;
    }
    fclose(read);
    return count;
}




