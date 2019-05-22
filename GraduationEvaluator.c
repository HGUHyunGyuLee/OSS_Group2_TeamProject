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

char* saving(const char *filename, int* filelen)
{
   FILE *fp = fopen(filename, "rb"); // it should be rb!!!
   if (fp == 0) {
      printf("Your file doesn't exist");
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
      }/*
      else if (isdigit(saved[i])) {
         t[filelen[2]].start = i;
         t[filelen[2]].type = PRIMITIVE;//or undefined??
         //filelen[2] += 1;
         filelen[1] = numbertoken(saved, token, filelen);
      }*/
      else if (isblank(saved[filelen[1]])){
         filelen[1] = filelen[1] + 1;//blank or etc...
      }
      else {
         filelen[1] = filelen[1] + 1;
         //printf("error at parsing");
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

int arraytoken(char* saved, tok_t* t, int* filelen) {
   
   int ntoken = filelen[2];
   filelen[2] += 1;
   int size = 0;

   while (saved[filelen[1]] != '{'
      && saved[filelen[1]] != '['
      && saved[filelen[1]] != '"') {//remove blank!
      filelen[1] = filelen[1] + 1;
   }

   while ((saved[filelen[1]] != ']') && (filelen[1] < filelen[0])) {//elements
      size = size + 1;

      while (saved[filelen[1]] != ','
         && saved[filelen[1]] != '{'
         && saved[filelen[1]] != '['
         && saved[filelen[1]] != '"') {//remove blank!
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
      }/*
      else if (isdigit(saved[filelen[1]])) {
         t[filelen[2]].start = filelen[1];
         t[filelen[2]].type = PRIMITIVE;//or undefined??
         filelen[2] += 1;
         filelen[1] = numbertoken(saved, token, filelen);
      }*/


      while (saved[filelen[1]] != ','
         && saved[filelen[1]] != '{'
         && saved[filelen[1]] != '['
         && saved[filelen[1]] != '"'
         && saved[filelen[1]] != ']') {//remove blank!
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

   while (saved[filelen[1]] != '{'
      && saved[filelen[1]] != '['
      && saved[filelen[1]] != '"') {//remove blank!
      filelen[1] = filelen[1] + 1;
   }
   

   while ((saved[filelen[1]] != '}') && (filelen[1] < filelen[0])) {//key and value
      size += 1;
      
      while (saved[filelen[1]] != ',' 
            && saved[filelen[1]] != '{'
            && saved[filelen[1]] != '['
            && saved[filelen[1]] != '"') {//remove blank!
         filelen[1] = filelen[1] + 1;
      }

      //get key size of key is 1 
      //should array, object be key??
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

      //get :

      while (saved[filelen[1]] != ':') {//remove blank!
         filelen[1] = filelen[1] + 1;
      }


      if (saved[filelen[1]] == ':') {
         filelen[1] = filelen[1] + 1;
      }
      else {
         printf("error at object!2");
      }

      //get value:

      while (saved[filelen[1]] != '{'
         && saved[filelen[1]] != '['
         && saved[filelen[1]] != '"') {//remove blank!
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
      else if (saved[filelen[1]] == ',') {
         filelen[1] = filelen[1] + 1;
      }
      else {
         filelen[1] = filelen[1] + 1;
         //printf("error at object!3");
      }

      while (saved[filelen[1]] != ','
         && saved[filelen[1]] != '{'
         && saved[filelen[1]] != '['
         && saved[filelen[1]] != '"'
         && saved[filelen[1]] != '}') {//remove blank!
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
      printf("error at object!4");
   }

   return filelen[0];//end!
}


int main()
{

   int* filelen = (int*)malloc(sizeof(int) * 3);//filelen[0] for filelength filelen[1] for index filelen[2] for tindex

   char* saved = saving("StudentInfo.json", &(filelen[0]));

   if (saved == NULL)
      return -1;
   tok_t *tokens = NULL;

   int ntokens = scanning(saved, &tokens, filelen);
   printf("scanning is ended\n");

   for (int i = 0; i < ntokens; i++) {
      printf("[%3d] ", i + 1);
      for (int j = tokens[i].start; j <= tokens[i].end; j++) {
         printf("%c", saved[j]);
      }
      printf(" (size=%d, %d~%d ", tokens[i].size, tokens[i].start, tokens[i].end);
      if (tokens[i].type == PRIMITIVE)
         printf("JSMN_PRIMITIVE");
      else if (tokens[i].type == STRING)
         printf("JSMN_STRING");
      else if (tokens[i].type == ARRAY)
         printf("JSMN_ARRAY");
      else if (tokens[i].type == OBJECT)
         printf("JSMN_OBJECT");


      printf("]\n");
   }

   free(saved);
   free(tokens);
   system("PAUSE");
   return 0;
}