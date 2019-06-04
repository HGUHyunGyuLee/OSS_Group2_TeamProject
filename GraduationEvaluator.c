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
		else if (checkblank(saved[filelen[1]])) {
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
		printf("error at string!at[%c][%d]", saved[filelen[1]], saved[filelen[1]]);
	}

	return filelen[0];//end!
}

int primitivetoken(char* saved, tok_t* t, int* filelen) {
	int ntoken = filelen[2];
	filelen[2] += 1;
	int size = 0;

	while ( (!checkblank(saved[filelen[1]+1])) && (saved[filelen[1]+1] != ',')
		&& (saved[filelen[1]+1] != ']') && (saved[filelen[1]+1] != '}')) {
		filelen[1] += 1;
	}

	t[ntoken].end = filelen[1];
	t[ntoken].size = 0;
	filelen[1] += 1;
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

		while (checkblank(saved[filelen[1]])) {
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
			filelen[1] += 1;
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
		printf("error at array!5at[%c][%d]", saved[filelen[1]], saved[filelen[1]]);
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
			filelen[1] = primitivetoken(saved, t, filelen);
		}

		//get :

		while (checkblank(saved[filelen[1]])) {
			filelen[1] = filelen[1] + 1;
		}

		if (saved[filelen[1]] == ':') {
			filelen[1] = filelen[1] + 1;
		}
		else {
			printf("error at object!2at[%c][%d]", saved[filelen[1]], saved[filelen[1]]);
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
		else {
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
	else if (saved[filelen[1]] != ',') {
		printf("error at object!4 :%d@@", saved[filelen[1]]);
	}

	return filelen[1];//end!
}


/*Graduate Evaluator*/
int studentID, semester, toeicScore = 0;
char course, grade1, grade2;
int general1cnt, general2cnt, majorcnt = 0;
int takenMajor, takenGeneral, totalTakencourse = 0;
double generalSum, majorSum,finalGrade;
int Itp, DS, CA, OS, EPP, PL, Algo, CN, DB, CAP = 0;
int HCB, Chp, CLT, SS =0;
int EF, EC, ERC, EAP = 0;
int BSM = 0;
bool toeicFlag, BSMFlag, GenEngFlag, RmajorFlag, RgeneralFlag, TmajorFlag, 
TgeneralFlag, TtakenFlag, gradeFlag = true;


void graduateEvaluator();
void personalInfo();
void evaluate();
void gradeCalculate();
void courseGradeCheck(char c1, char g1, char g2 );
double gradeMatch(char g1, char g2);
void requiredmajorcheck(char f1, char f2, char e3);
void requiredgeneralCheck(char f1, char f2, char f6);
void generalEnglishCheck(char f1, char f9);
void BSMcheck(char f1, char f2, char f3,char f6, char e2, char e3);
void missedFactors();

void graduateEvaluator()
{
   printf("\n\n");
   printf("[Graduate Evaluator]\n");
   printf("\n");
   personalInfo();
   printf("\n");
   evaluate();

   printf("\n");

   if(toeicFlag == false || BSMFlag == false || GenEngFlag == false || RmajorFlag == false || RgeneralFlag == false ||TmajorFlag
== false ||TgeneralFlag == false ||TtakenFlag == false || gradeFlag == false)
{
    printf(" >>> Graduation: Impossible\n");

    printf("\n");

    printf("[Missed Factors]\n");

     printf("\n");
    
}
else{
   printf(" >>> Graduation: Possible\n");
   if(semester == 7 && finalGrade >=4.0) 
   {
      printf(" ~~~~~~ !! early graduator !! ~~~~~~\n");
   }
    printf("\n");
   }
      missedFactors();
}


void personalInfo()
{
   printf("\n");
   printf("- Student ID: %d\n", studentID);
   printf("- Major: Computer Science\n");
   printf("- Semester: %d\n", semester);
}

void missedFactors()
{

   if(toeicFlag == false)
   {
      printf("-------------- Toeic Score -------------\n");
      printf("You need to submit toeic score again\n\n");

   }
    if(BSMFlag == false)
   {
      printf("-------------- BSM course -------------\n");
      printf("You should take %d more BSM course\n\n", 6-BSM);
   }
    if(GenEngFlag == false)
   {
      printf("-------------- General English course -------------\n");
      printf("You should take %d more General English course\n\n", 4-(EF+EC+ERC+EAP));
   }

     if(TmajorFlag == false)
   {
      printf("-------------- Total taken Major course -------------\n");
      printf("You should take %d more Major course\n\n", 66-(takenMajor));
   }
    if(TgeneralFlag == false)
   {
      printf("-------------- Total taken General course -------------\n");
      printf("You should take %d more General course\n\n", 64-(takenGeneral));
   }

    if(gradeFlag == false)
   {
      printf("-------------- Final grade -------------\n");
      printf("You should have more than 2.0 grade\n\n");
   }

    if(RgeneralFlag == false)
   {
      printf("-------------- Required General course -------------\n");
              if(HCB < 1)  printf("You should take Handong Character Building\n");

        if(Chp < 6) {
           for(int i = Chp+1; i<7; i++){
           printf("You should take Chapel %d\n", i);
           }
        }

         if(CLT < 6) {
           for(int i = CLT+1; i<7; i++){
           printf("You should take Community & Leadership Training %d\n", i);
           }
        }

        if(SS < 2)
        {
           for(int i = SS+1; i<3; i++){
           printf("You should take Social Service %d\n", i);
           }

        }
        printf("\n");
   }

    if(RmajorFlag == false)
   {
      printf("-------------- Required Major course -------------\n");
      printf("< Essential >\n");
        if(Itp < 1)  printf("You should take Introduction to Programming\n");
        if(DS < 1) printf("You should take Data Structure\n");
        if (OS < 1) printf("You should take Operating Systems\n");
        if (CA < 1) printf("You should take Computer Architecture\n");
        if (EPP < 1) printf("You should take Enginrreing Project Planning\n");
        if (CAP < 1) printf("You should take Capstone Design\n");

        printf("\n");

        if(PL+Algo+Itp+CN+DB < 2)
        {
         printf(" OR \n\n");
         printf("< %d more courses from below >\n", 2-(PL+Algo+Itp+CN+DB));
         if(Itp < 1)  printf("Introduction to Programming\n");
         if (PL < 1) printf("Programming Language\n");
         if (Algo < 1) printf("Algorithm\n");
         if (CN < 1) printf("Computer Networks\n");
         if (DB < 1) printf("Database\n");
        }

        printf("\n");
   }
}

void evaluate()
{
   //toeic check
    printf("*  Toeic score - %d", toeicScore);
   if(toeicScore < 700) {
      toeicFlag = false;
       printf(" | Unsatisfied\n");
   }else{
      toeicFlag = true;
      printf(" | Satisfied\n");
   } 

   //BSM check
   printf("*  BSM course - %d/6", BSM);
      if(BSM < 6) {
      BSMFlag = false;
       printf(" | Unsatisfied\n");
   }else{
      BSMFlag = true;
      printf(" | Satisfied\n");
   } 

   //General English course check
   int GenEng = EF+EC+ERC+EAP;
   printf("*  Genearl English course - %d/4", GenEng);
      if(GenEng <4) {
      GenEngFlag = false;
       printf(" | Unsatisfied\n");
   }else{
      GenEngFlag = true;
      printf(" | Satisfied\n");
   } 

   //required major course check
   int Rmajor = Itp+ DS+ CA+ OS+ EPP+PL+ Algo+ CN+ DB+ CAP;
   printf("*  Required Major course - %d/6", Rmajor);
   if(Rmajor <6) {
      RmajorFlag = false;
       printf(" | Unsatisfied\n");
   }else{
      RmajorFlag = true;
      printf(" | Satisfied\n");
   } 

   //required feneral course check
   int Rgeneral = HCB+Chp+CLT+SS;
   if(Rgeneral <15) RgeneralFlag = false;

   printf("*  Required General course");
   if(Rgeneral <15) 
   { 
      RgeneralFlag = false;
      printf(" | Unsatisfied\n");
   }else{
      RgeneralFlag = true;
      printf(" | Satisfied\n");
   }
   printf("     # Handong chracture building - %d/1\n", HCB);
   printf("     # Chapel - %d/6\n", Chp);
   printf("     # Commnuity & Leadership - %d/6\n", CLT);
   printf("     # Social Service - %d/2\n", SS);

   //total takne Major course check
   printf("*  Total taken Major course - %d/66", takenMajor);
    if(takenMajor <66) {
      TmajorFlag = false;
       printf(" | Unsatisfied\n");
   }else{
      TmajorFlag = true;
      printf(" | Satisfied\n");
   } 

   //total taken general course check
    printf("*  Total taken General course - %d/64", takenGeneral);
     if(takenMajor <64) {
      TgeneralFlag = false;
       printf(" | Unsatisfied\n");
   }else{
      TgeneralFlag = true;
      printf(" | Satisfied\n");
   } 


   //totak taken course cehck
   printf("*  Total taken course - %d/130", totalTakencourse);
      if(totalTakencourse <130) {
      TtakenFlag = false;
       printf(" | Unsatisfied\n");
   }else{
      TtakenFlag = true;
      printf(" | Satisfied\n");
   } 

   //final grade check 
   printf("*  clFinal grade - %.2f", finalGrade);
      if(finalGrade <2.0) {
      gradeFlag = false;
       printf(" | Unsatisfied\n");
   }else{
      gradeFlag = true;
      printf(" | Satisfied\n");
   } 

}

void courseGradeCheck(char c1, char g1, char g2 ) 
{
    double generalGrade, majorGrade;
    if(c1 == '1')
    {
        general1cnt++;
    }
    if(c1 == '2')
    { 
        general2cnt++;
        generalGrade = gradeMatch(g1,g2);
        generalSum += (generalGrade*2);

    }else if(c1 == '3')
    {     
         majorcnt++;
         majorGrade = gradeMatch(g1, g2);
         majorSum += (majorGrade*3);
    }
   
   gradeCalculate();
}

double gradeMatch(char g1, char g2)
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

void gradeCalculate()
{
   takenMajor = majorcnt*3;
   takenGeneral = (general1cnt*1) + (general2cnt*2);
   totalTakencourse = takenMajor + takenGeneral;
   finalGrade = (generalSum+majorSum)/((general2cnt*2)+(takenMajor));
}

void requiredmajorcheck(char f1, char f2, char e3)
{
   //Introduction to Programming - ItP
   if(f1 == 'I' && f2 == 'n' && e3 == 'i') Itp++; 
   
   //Data Structure - DS
   else if(f1 == 'D'&& f2 == 'a' && e3 == 'u') DS++;

   //Computer Architecture - CA
   else if(f1 == 'C'&& f2 == 'o' && e3 == 'u') CA++;

   //Operating System - OS
   else if(f1 == 'O'&& f2 == 'p' && e3 == 't') OS++;

   //Engineering Project Planning - EPP
   else if(f1 == 'E'&& f2 == 'n' && e3 == 'i') EPP++;

   //Capstron Design
   else if(f1 == 'C'&& f2 == 'a' && e3 == 'i') CAP++;

   //Programming Language - PL
   else if(f1 == 'P'&& f2 == 'r' && e3 == 'a') PL++;

   //Algorithm Analysis - Algo
   else if(f1 == 'A'&& f2 == 'l' && e3 == 's') Algo++;

   //Computer Network - CN
   else if(f1 == 'C'&& f2 == 'o' && e3 == 'o') CN++;

   //Database - DB
   else if(f1 == 'D'&& f2 == 'a' && e3 == 'a') DB++;

}

void requiredgeneralCheck(char f1, char f2, char f6)
{
   //Handong Character-Building
    if(f1 == 'H' && f6 == 'n') HCB++; 
   
   //Chapel
   else if(f1 == 'C'&& f2 == 'h'&& f6 == 'l') Chp++;

   //Community & Leadership Training 
   else if(f1 == 'C'&& f6 == 'n') CLT++;

   //Social Service 
   else if(f1 == 'S'&& f6 == 'l') SS++;

}

void generalEnglishCheck(char f1, char f9)
{
       //English Foundation
   if(f1 == 'E' && f9 == 'F') EF++; 
   
   //English Communication
   else if(f1 == 'E'&& f9 == 'C') EC++;

   //English Reading & Composition
   else if(f1 == 'E'&& f9 == 'R') ERC++;

   //EAP-Engineering
   else if(f1 == 'E'&& f9 == 'n') EAP++;

}

void BSMcheck(char f1, char f2, char f3,char f6, char e2, char e3)
{
   //Calculus 1,2,3 
   if(f1 == 'C' && f2 == 'a' && f3 == 'l' && f6 == 'l'&& e3 == 's')  BSM++;
  
   //Diffrential Equations and Application - Dif
   else if(f1 == 'D'&& f2 == 'i' && f3 == 'f' && f6 == 'e'&& e3 == 'i') BSM++;

   //Engineering Mathematics - EngMath
   else if(f1 == 'E'&& f2 == 'n' && f3 == 'g' && f6 == 'e'&&  e2 == 'c' &&e3 =='i') BSM++;

   //Linear Algebra - Lin
   else if(f1 == 'L'&& f2 == 'i' && f3 == 'n'&& f6 == 'r'&& e3 =='b') BSM++;

   //Statistics - Sta
   else if(f1 == 'S'&& f2 == 't' && f3 == 'a'&& f6 == 's'&& e3 =='i') BSM++;

   //Discrete Mathematics - DisMath
   else if(f1 == 'D'&& f2 == 'i' && f3 == 's'&& f6 =='e'&& e3 =='i') BSM++;

   //Number Theory - NumT
   else if(f1 == 'N'&& f2 == 'u' && f3 == 'm'&& f6 == 'r'&& e3 =='o') BSM++;

    //Mathematical Analysis - MathAnal
   else if(f1 == 'M'&& f2 == 'a' && f3 == 't'&& f6 =='m'&& e3 =='s') BSM++;

   //Fundemental Physics - FunPhysc
   else if(f1 == 'F'&& f2 == 'u' && f3 == 'n'&& f6 =='m'&& e3 =='i') BSM++;

   //Physics 1,2 - Physc
   else if(f1 == 'P'&& f2 == 'h' && f3 == 'y'&& f6 == 'c'&& e3 =='s') BSM++;

   //Physics Lab 1,2 - PhyscLab
   else if(f1 == 'P'&& f2 == 'h' && f3 == 'y' && f6 == 'c' && e3 == 'b') BSM++;

   //General Biology - Bio
   else if(f1 == 'G'&& f2 == 'e' && f3 == 'n'&& f6 == 'a' && e3 == 'o')BSM++;

   //General Chemistry - Chemi
   else if(f1 == 'G'&& f2 == 'e' && f3 == 'n'&& f6 =='a'&& e3 =='t') BSM++;

   //General Chemistry Lab - ChemiLab
   else if(f1 == 'G'&& f2 == 'e' && f3 == 'n' && f6 =='r'&& e3 == 'L') BSM++;
   
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
	  //filename = "cannotGraduate.json";
   }
   
   char* saved = saving(filename, &(filelen[0]));
   if (saved == 0) {
      printf("Please enter correct file name!\n");
      exit(1);
   }
   for (int i = 0; i < filelen[0]; i++) {
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
      printf("[%3d] ", i);
      if (tokens[i].type == PRIMITIVE) {
         for (int j = tokens[i].start; j <= tokens[i].end; j++) {
            printf("%c", saved[j]);
         }
		 printf(" (size=%d, %d~%d, ", tokens[i].size, tokens[i].start, tokens[i].end+1);
		 printf("JSMN_PRIMITIVE");

      }
      else if (tokens[i].type == STRING) {
         for (int j = tokens[i].start + 1; j < tokens[i].end; j++) {
            printf("%c", saved[j]);

         }
		 for (int j = tokens[i].start + 1; j < tokens[i].end; j++) {
			 /**********************For Graduate Evaluator**************************/
			 if (isdigit(saved[j]))
			 {
				 if (tokens[i].size == 0)
				 {
					 if (tokens[i].end - tokens[i].start == 9)
					 {
						 //student ID
						 int num = atoi(&saved[j]);
						 studentID = num;
						 break;
					 }

					 if (tokens[i].end - tokens[i].start == 2)
					 {
						 int num1 = atoi(&saved[j]);
						 if (num1 > 3)
						 {
							 //semester
							 semester = num1;
						 }
						 else {
							 //course
						 }
					 }

					 if (atoi(&saved[j]) > 13)
					 {
						 //toeic score
						 int num2 = atoi(&saved[j]);
						 toeicScore = num2;
						 break;
					 }
				 }
			 }

			 if (isalpha(saved[j]))
			 {
				 char n;
				 if (tokens[i].size == 0 && tokens[i].end - tokens[i].start > 3)
				 {
				 }
			 }
			 /************************************************************/
		 }

		 printf(" (size=%d, %d~%d, ", tokens[i].size, tokens[i].start+1, tokens[i].end);
		 printf("JSMN_STRING");

         if(tokens[i].size == 1){

           /**********************For Graduate Evaluator**************************/
            char f1, f2, f3, f6, f9, e2, e3;
            f1 = saved[(tokens[i].start)+1];
            f2 = saved[(tokens[i].start)+2];
            f3 = saved[(tokens[i].start)+3];
            f6 = saved[(tokens[i].start)+6];
            f9 = saved[(tokens[i].start)+9]; 
            e2 = saved[(tokens[i].end)-2];
            e3 = saved[(tokens[i].end)-3];
            
            requiredmajorcheck(f1, f2, e3);
            requiredgeneralCheck(f1, f2,f6);
            generalEnglishCheck(f1, f9);
            BSMcheck(f1, f2, f3, f6, e2, e3);
            /************************************************************/
         }
      }
      else if (tokens[i].type == ARRAY) {
         for (int j = tokens[i].start; j <= tokens[i].end; j++) {
            printf("%c", saved[j]);        
         }
		 printf(" (size=%d, %d~%d, ", tokens[i].size, tokens[i].start, tokens[i].end+1);
		 printf("JSMN_ARRAY");
            /**********************For Graduate Evaluator**************************/
            char course, grade1, grade2;

            course = saved[(tokens[i].start)+2];
            grade1 = saved[(tokens[i].start)+7];
            grade2 = saved[(tokens[i].start)+8];

            courseGradeCheck(course, grade1, grade2);
             /************************************************************/

      }
      else if (tokens[i].type == OBJECT) {
         for (int j = tokens[i].start; j <= tokens[i].end; j++) {
            printf("%c", saved[j]);
         }
		 printf(" (size=%d, %d~%d, ", tokens[i].size, tokens[i].start, tokens[i].end+1);
		 printf("JSMN_OBJECT");
      }
      printf(")\n");
   }

   graduateEvaluator();

   free(saved);
   free(tokens);
   return 0;
}
