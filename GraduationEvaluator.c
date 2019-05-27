#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h> 
#include <ctype.h>

typedef enum {
   UNDEFINED = 0,
   OBJECT = 1,
   ARRAY = 2,
   STRING = 3,
   PRIMITIVE = 4
} type_t;

typedef struct {
   type_t type; // Token type
   int start; // Token start position
   int end; // Token end position
   int size; // Number of child (nested) tokens
} tok_t;


char* saving(const char* filename, int* filelen);
int scanning(char* save, tok_t** tokens, int* filelen);
int stringtoken(char* save, tok_t* t, int* filelen);
int arraytoken(char* save, tok_t* t, int* filelen);
int objecttoken(char* save, tok_t* t, int* filelen);
int primitivetoken(char* save, tok_t* t, int* filelen);
bool checkblank(char ch);

bool checkblank(char ch) {
   if ((ch >= 9) && (ch <= 13)) {
      return true;
   }
   else if (ch == 32) {
      return true;
   }
   return false;
}
char* saving(const char *filename, int* filelen)
{
   FILE *fp = fopen(filename, "rb"); // it should be rb!!!
   if (fp == 0) {
      return 0;
   }

   char* save = (char*)malloc(100 * (sizeof(char)));//first initialize 100
   int i = 0;
   int cap = 100;
   while (!feof(fp)) {
      if (i >= cap) {
         save = (char*)realloc(save, cap * 2 * (sizeof(char)));
         cap *= 2;
      }
      save[i++] = fgetc(fp);
   }
   save = (char*)realloc(save, i*(sizeof(char)));
   fclose(fp);
   *filelen = i - 1;

   return save;
}

int scanning(char* saved, tok_t** token, int* filelen) {

   tok_t* t = (tok_t*)malloc(sizeof(tok_t) * 1024);
   *token = t;
   int start = 0;
   int end = 0;
   int tindex = 0;
   //filelen[0] is numbers of characters from json
   filelen[1] = 0;//index
   filelen[2] = 0;//tindex

   while (filelen[1] < filelen[0]) {
      if (saved[filelen[1]] == '{') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = OBJECT;
         filelen[1] += 1;
         filelen[1] = objecttoken(saved, t, filelen);
      }
      else if (saved[filelen[1]] == '"') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = STRING;
         filelen[1] += 1;
         filelen[1] = stringtoken(saved, t, filelen);
      }
      else if (saved[filelen[1]] == '[') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = ARRAY;
         filelen[1] += 1;
         filelen[1] = arraytoken(saved, t, filelen);
      }
      else if (checkblank(saved[filelen[1]])){
         filelen[1] = filelen[1] + 1;//blank
      }
      else {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = PRIMITIVE;
         filelen[1] += 1;
         filelen[1] = primitivetoken(saved, t, filelen);
         //prmitive!!
      }
   }

   return filelen[2];
}

int stringtoken(char* saved, tok_t* t, int* filelen) {
   int ntoken = filelen[2];
   filelen[2] += 1;
   int size = 0;
   //should not remove blank!!

   while ((saved[filelen[1]] != '"') && (filelen[1] < filelen[0])) {//string contents
      filelen[1] += 1;
   }

   if (saved[filelen[1]] == '"') {
      if (t[ntoken].size != 1) {//it meeans key
         t[ntoken].size = 0;
      }
      t[ntoken].end = filelen[1];
      filelen[1] += 1;
      
      return filelen[1];
   }
   else {
      printf("error at string!4");
   }

   return filelen[0];//end!
}

int primitivetoken(char* saved, tok_t* t, int* filelen) {
   int ntoken = filelen[2];
   filelen[2] += 1;
   int size = 0;

   while (isdigit(saved[filelen[1]]) || isalpha(saved[filelen[1]])) {
      filelen[1] += 1;
   }

   t[ntoken].end = filelen[1] - 1;
   t[ntoken].size = 0;

   return filelen[1];
}
int arraytoken(char* saved, tok_t* t, int* filelen) {
   
   int ntoken = filelen[2];
   filelen[2] += 1;
   int size = 0;

   while (checkblank(saved[filelen[1]])) {
      filelen[1] = filelen[1] + 1;
   }

   while ((saved[filelen[1]] != ']') && (filelen[1] < filelen[0])) {//elements
      size = size + 1;

      while (checkblank(saved[filelen[1]])) {
         filelen[1] = filelen[1] + 1;
      }

      if (size >= 2) {
         if (saved[filelen[1]] == ',') {
            filelen[1] = filelen[1] + 1;
         }
      }
      
      while (saved[filelen[1]] != '{'
         && saved[filelen[1]] != '['
         && saved[filelen[1]] != '"') {//remove blank!
         filelen[1] = filelen[1] + 1;
      }
      

      //get element
      if (saved[filelen[1]] == '{') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = OBJECT;
         filelen[1] += 1;
         filelen[1] = objecttoken(saved, t, filelen);
      }
      else if (saved[filelen[1]] == '"') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = STRING;
         filelen[1] += 1;
         filelen[1] = stringtoken(saved, t, filelen);
      }
      else if (saved[filelen[1]] == '[') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = ARRAY;
         filelen[1] += 1;
         filelen[1] = arraytoken(saved, t, filelen);
      }
      else { 
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = PRIMITIVE;//or undefined??
         filelen[2] += 1;
         filelen[1] = primitivetoken(saved, t, filelen);
      }
      
      while (checkblank(saved[filelen[1]])) {
         filelen[1] = filelen[1] + 1;
      }
   }
   //get ]

   if (saved[filelen[1]] == ']') {//end array
      t[ntoken].end = filelen[1];
      t[ntoken].size = size;
      filelen[1] += 1;

      return filelen[1];
   }
   else {
      printf("error at array!5");
   }

   return filelen[0];//end!
}
int objecttoken(char* saved, tok_t* t, int* filelen) {

   int ntoken = filelen[2];
   

   filelen[2] += 1;//number of token is increased
   int size = 0;

   while (checkblank(saved[filelen[1]])) {
      filelen[1] = filelen[1] + 1;
   }

   while ((saved[filelen[1]] != '}') && (filelen[1] < filelen[0])) {//key and value
      size += 1;

      while (checkblank(saved[filelen[1]])) {
         filelen[1] = filelen[1] + 1;
      }
      //get key size of key is 1 
      //should array, object be key??
      if (size >= 2) {
         if (saved[filelen[1]] == ',') {
            filelen[1] = filelen[1] + 1;
         }
      }

      while (checkblank(saved[filelen[1]])) {
         filelen[1] = filelen[1] + 1;
      }
      //printf("%c", saved[filelen[1]]);

      if (saved[filelen[1]] == '{') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = OBJECT;
         t[filelen[2]].size = 1;//key
         filelen[1] += 1;
         filelen[1] = objecttoken(saved, t, filelen);
      }
      else if (saved[filelen[1]] == '"') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = STRING;
         t[filelen[2]].size = 1;//key
         filelen[1] += 1;
         filelen[1] = stringtoken(saved, t, filelen);
      }
      else if (saved[filelen[1]] == '[') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = ARRAY;
         t[filelen[2]].size = 1;//key
         filelen[1] += 1;
         filelen[1] = arraytoken(saved, t, filelen);
      }
      else {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = PRIMITIVE;
         t[filelen[2]].size = 1;//key
         filelen[1] += 1;
         filelen[1] = arraytoken(saved, t, filelen);
      }

      //get :

      while (checkblank(saved[filelen[1]])) {
         filelen[1] = filelen[1] + 1;
      }

      if (saved[filelen[1]] == ':') {
         filelen[1] = filelen[1] + 1;
      }
      else {
         printf("error at object!2");
      }

      //get value:

      while (checkblank(saved[filelen[1]])) {
         filelen[1] = filelen[1] + 1;
      }

      if (saved[filelen[1]] == '{') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = OBJECT;
         filelen[1] += 1;
         filelen[1] = objecttoken(saved, t, filelen);
      }
      else if (saved[filelen[1]] == '"') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = STRING;
         filelen[1] += 1;
         filelen[1] = stringtoken(saved, t, filelen);
      }
      else if (saved[filelen[1]] == '[') {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = ARRAY;
         filelen[1] += 1;
         filelen[1] = arraytoken(saved, t, filelen);
      }
      else if (isdigit(saved[filelen[1]]) || isalpha(saved[filelen[1]])) {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = PRIMITIVE;
         filelen[1] += 1;
         filelen[1] = primitivetoken(saved, t, filelen);
      }
      
      while (checkblank(saved[filelen[1]])) {
         filelen[1] = filelen[1] + 1;
      }
   }

   //get }


   if (saved[filelen[1]] == '}') {//end object
      t[ntoken].end = filelen[1];
      t[ntoken].size = size;
      filelen[1] += 1;

      return filelen[1];
   }
   else {
      printf("error at object!4 :%d@@", saved[filelen[1]]);
   }

   return filelen[0];//end!
}

  /*For graduation check 1*/
int generalcnt, majorcnt = 0;
int C, D, O = 0;  
double generalSum, majorSum, finalGrade;
bool earlyG, semester = false;
bool courseFlag, gradeFlag, RcourseFlag = true;

void arrayCheck(char c1, char g1, char g2 );
double gradeCheck(char g1, char g2);
void digitcheck(char c1, int start, int end);
void lettercheck(char c1, char c2, char c3);
void graduationTest();
void finalGraduationCheck();
/*************************/

void arrayCheck(char c1, char g1, char g2 ) 
{
    double generalGrade, majorGrade;
    if(c1 == '2')
    { 
        generalcnt++;
        generalGrade = gradeCheck(g1,g2);
        generalSum += (generalGrade*2);

    }else if(c1 == '3')
    {     
         majorcnt++;
         majorGrade = gradeCheck(g1, g2);
         majorSum += (majorGrade*3);
    }
}

double gradeCheck(char g1, char g2)
{
    if(g1 == 'A')
    {
        if(g2 == '+') return 4.5;
        else return 4.0;

    }else if(g1 == 'B')
    {
        if(g2 == '+') return 3.5;
        else return 3.0;

    }
    else if(g1 == 'C')
    {
         if(g2 == '+') return 2.5;
        else return 2.0;
        
    }
    else if(g1 == 'D')
    {
        if(g2 == '+') return 1.5;
        else return 1.0;
        
    }
    else
    {
        return 0;
    }
}


void digitcheck(char c1, int start, int end)
{
    char l[10];
    //학기수 
    if(c1 == '7') earlyG = true;
    else if(c1 == '8' || c1 == '9') semester = true;
}

void lettercheck(char c1, char c2, char c3)
{
   if(c1 == 'C' && c2 == ' ' && c3 == 'n') C++;
   else if(c1 == 'D'&& c2 == 'a' && c3 == 'r') D++;
   else if(c1 == 'O'&& c2 == 'p' && c3 == 'm') O++;
}

void graduationTest()
{
    finalGrade = (generalSum+majorSum)/((generalcnt*2)+(majorcnt*3));
    //전공 이수 학점 부족 또는 교양 이수 학점 부족
    if(majorcnt <19 || generalcnt <19)courseFlag = false;
    if(C < 1 || D < 1 || O < 1) RcourseFlag = false;
    if(finalGrade <= 2.0) gradeFlag = false;

    finalGraduationCheck();
}


void finalGraduationCheck()
{

    //Graduation Impossible
    if(courseFlag == false)
    {
        printf("[Graduation Impossible]\n");
        if(majorcnt<20) {
            printf("major course - %d/60 uncompleted\n", majorcnt*3);
        }
        if(generalcnt<20) printf("general course - %d/40 uncompleted\n", generalcnt*2);
    }

     if(RcourseFlag == false)
    {
        if(C < 1)  printf("you should take C Programming\n");
        if(D < 1) printf("you should take Data Structure\n");
        if (O < 1) printf("you should take Operating Systems\n");
    }
    
    else if(gradeFlag == false)
    {
        printf("you are final grade is lower than 2.0\n");

    }

    //Graduation Possible
    if(courseFlag == true && gradeFlag == true && RcourseFlag == true)
    {
         printf("[Graduation Possible]\n");
         if(earlyG == true && finalGrade >= 4.0) printf("Early Graduation\n");
        printf("major course - %d/40 completed\n", majorcnt*3);
        printf("general course - %d/40 completed\n", generalcnt*2);
        printf("required major course - %d/3 completed\n", C+D+O);   
        printf("final grade - %.2f\n", finalGrade);

    }

}


int main(int argc, char* argv[])
{
    char temp;
    char *r;

   int* filelen = (int*)malloc(sizeof(int) * 3);//filelen[0] for filelength filelen[1] for index filelen[2] for tindex
   char* filename = 0;
   if (argv[1] != 0) {
      filename = argv[1];
   }
   else {
      printf("Please enter your file for parsing!\n");
      exit(1);
   }

   char* saved = saving(filename, &(filelen[0]));
   if (saved == 0) {
      printf("Please enter correct file name!\n");
      exit(1);
   }
   for (int i = 0; i < filelen[0]; i++) {
      //printf("%c\n", saved[i]);
      //printf("%d\n", saved[i]);
      if (!checkblank(saved[i])) {
         //printf("%c", saved[i]);
      }
   }
   if (saved == NULL)
      return -1;
   tok_t *tokens = NULL;

   int ntokens = scanning(saved, &tokens, filelen);
   printf("scanning is ended\n");

   for (int i = 0; i < ntokens; i++) {
      printf("[%3d] ", i + 1);
      if (tokens[i].type == PRIMITIVE) {
         for (int j = tokens[i].start; j <= tokens[i].end; j++) {
            printf("%c", saved[j]);
         }

        

      }
      else if (tokens[i].type == STRING) {
         for (int j = tokens[i].start + 1; j < tokens[i].end; j++) {
            printf("%c", saved[j]);

         }
             /*For graduation check 2*/
            char chek, chek2, chek3;

            chek = saved[(tokens[i].start)+1];
            chek2 = saved[(tokens[i].start)+2];
            chek3 = saved[(tokens[i].end)-2];

            digitcheck(chek, tokens[i].start, tokens[i].end);
            lettercheck(chek, chek2, chek3);
            /*************************/


      }
      else if (tokens[i].type == ARRAY) {
         for (int j = tokens[i].start; j <= tokens[i].end; j++) {
            printf("%c", saved[j]);         
         }
            /*For graduation check 3*/

            char course, grade1, grade2;

            course = saved[(tokens[i].start)+2];
            grade1 = saved[(tokens[i].start)+7];
            grade2 = saved[(tokens[i].start)+8];

            arrayCheck(course, grade1, grade2);
            /*************************/

      }
      else if (tokens[i].type == OBJECT) {
         for (int j = tokens[i].start; j <= tokens[i].end; j++) {
            printf("%c", saved[j]);
         }

      }
      printf(" (size=%d, %d~%d, ", tokens[i].size, tokens[i].start, tokens[i].end);
     if (tokens[i].type == PRIMITIVE)
           
         printf("JSMN_PRIMITIVE");
      else if (tokens[i].type == STRING)
         printf("JSMN_STRING");
      else if (tokens[i].type == ARRAY)
         printf("JSMN_ARRAY");
      else if (tokens[i].type == OBJECT)
         printf("JSMN_OBJECT");


      printf(")\n");
   }


    /*Graduation Check*/
        printf("\n\n");

        courseFlag = true;
        RcourseFlag = true;
        gradeFlag = true;

        graduationTest();

        printf("\n\n"); 
   

   free(saved);
   free(tokens);
   return 0;
}