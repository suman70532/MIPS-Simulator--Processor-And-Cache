#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processor.h"
#include <pthread.h>
#include <math.h>

int az = 0;
int idle_cycle;
int pc_final = 0;
float time_idle = 0.0;
int no_branch = 0;
int cycles = 0;
int instructions_executed = 0;
double address_start = 268500992;
int offset = 0;
int bt = 0;
double pc_read = 0;
double pc_write = 0;
int fwdA = 0;
int pfwdA = 0;
int fwdB = 0;
int pfwdB = 0;
int fwdC = 0;
int pfwdC = 0;
int fwdD = 0;
int pfwdD = 0;
int data_memory[8388608][8];
int register_file[32][32];
int HI[32];
int LU[32];
struct ins_mem IM[100];
int ins_number[5];
int stalls = 0;
int ins_access = 0;
int data_access = 0;
struct IF_ID_write ifidwrite;
struct ID_EX_write idexwrite;
struct EX_MEM_write exmemwrite;
struct MEM_WB_write memwbwrite;
struct IF_ID_read ifidread;
struct ID_EX_read idexread;
struct EX_MEM_read exmemread;
struct MEM_WB_read memwbread;
char to_print[11][100];
char array[100][33];
int max;
int row = 0;
FILE* out;
FILE* svgout;
char* temp;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double binary_to_int(int a[])
{

	double b = 0,c = a[31];
	int i;
//	printf("agvhejbdkfffffxdcfgvhbjklcvbhnbcvnbnjmkcnvbn\n");
//	for(i = 0;i < 32 ;i++)
//		printf("%d", a[i]);
//	printf("\n");
	for(i = 30;i >= 0 ;i--)
	{
		b = b*2 + a[i];
		c = 2*c;
	}

	return (b-c);
}
double binary_to_int2(int a[])
{
    double b = 0,c = a[7];
    int i;
    for(i = 6;i >= 0 ;i--)
    {
        b = b*2 + a[i];
        c = 2*c;
    }

    return (b-c);
}

void draw_svg(){
    int i;
    //int a = remove(temp);
    FILE *f1 = fopen("out.svg","w");
    FILE *f2 = fopen("start.txt","r");
    char string[1000];
    while(!feof(f2))
    {
        fscanf(f2,"%s",string);
        /*if(strcmp(string,"DATA0") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[0]);
        else if(strcmp(string,"DATA1") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[1]);
        else if(strcmp(string,"DATA2") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[2]);
        else if(strcmp(string,"DATA3") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[3]);
        else if(strcmp(string,"DATA4") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[4]);
        else*/
        if(strcmp(string,"#e5e5e1") == 0)//if match found increment num
        {
            if(to_print[0][0] == '1')
            {
             //   printf("1+++++++++++++++++++\n");
                fprintf(f1, "%s ", "#000000");
            }
            else
                fprintf(f1, "%s ", string);
        }
        else if(strcmp(string,"#e5e5e2") == 0)//if match found increment num
        {
            if(to_print[0][1] == '1')
            {
               // printf("2+++++++++++++++++++\n");
                fprintf(f1, "%s ", "#000000");
            }
            else
                fprintf(f1, "%s ", string);
        }
        else if(strcmp(string,"#e5e5e3") == 0)//if match found increment num
        {
            if(to_print[0][2] == '1')
            {
                //printf("3+++++++++++++++++++\n");
                fprintf(f1, "%s ", "#000000");
            }
            else
                fprintf(f1, "%s ", string);
        }
        else if(strcmp(string,"#e5e5e4") == 0)//if match found increment num
        {
            if(to_print[0][3] == '1')
            {
                //printf("4+++++++++++++++++++\n");
                fprintf(f1, "%s ", "#000000");
            }
            else
                fprintf(f1, "%s ", string);
        }
        else if(strcmp(string,"#e5e5e5") == 0)//if match found increment num
        {
            if(to_print[0][4] == '1')
            {
                //printf("5+++++++++++++++++++\n");
                fprintf(f1, "%s ", "#000000");
            }
            else
                fprintf(f1, "%s ", string);
        }
        else
            fprintf(f1, "%s ", string);   

        for(i = 0;i < 1000;i++)
            string[i] = '\0';
    }

    FILE *f3 = fopen("regwr.txt","r");
        while(!feof(f3))
        {
            fscanf(f3,"%s",string);
            if(strcmp(to_print[1], "00") == 0)
            {
                //printf("6+++++++++++++++++++\n");
                fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[1], "10") == 0)
            {
                //printf("7+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e0") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[1], "01") == 0)
            {
                //printf("8+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e1") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[1], "11") == 0)
            {
                //printf("9+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e0") == 0)
                    fprintf(f1, "%s ", "#000000");
                else if(strcmp(string, "#e5e5e1") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
    FILE *f4 = fopen("memwr.txt","r");
    while(!feof(f4))
        {
            fscanf(f4,"%s",string);
            if(strcmp(to_print[2], "00") == 0)
                fprintf(f1, "%s ", string);
            else if(strcmp(to_print[2], "10") == 0)
            {
                //printf("10+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e0") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[2], "01") == 0)
            {
                //printf("11+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e1") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[2], "11") == 0)
            {
                //printf("12+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e0") == 0)
                    fprintf(f1, "%s ", "#000000");
                else if(strcmp(string, "#e5e5e1") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }

    FILE *f5 = fopen("branch.txt","r");
    while(!feof(f5))
        {
            fscanf(f5,"%s",string);
            if(strcmp(to_print[3], "0") == 0)
            {
                //printf("13+++++++++++++++++++\n");
                fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[3], "1") == 0)
            {
                //printf("14+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e5") == 0)
                    fprintf(f1, "%s ", "#000000");
                else if(strcmp(string, "#f4b170") == 0)
                    fprintf(f1, "%s ", "#f57900");
                else
                    fprintf(f1, "%s ", string);
            }
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
    FILE *f6 = fopen("forwardingr1.txt","r");
    while(!feof(f6))
        {
            fscanf(f6,"%s",string);
            if(strcmp(to_print[4], "3") == 0)
            {
                //printf("15+++++++++++++++++++\n");
                fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[4], "0") == 0)
            {
                //printf("16+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e0") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[4], "1") == 0)
            {
                //printf("17+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e1") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[4], "2") == 0)
            {
              //  printf("18+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e2") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
            FILE *f7 = fopen("forwardingr2.txt","r");
    while(!feof(f7))
        {
            fscanf(f7,"%s",string);
            if(strcmp(to_print[5], "3") == 0)
            {
                //printf("19+++++++++++++++++++\n");
                fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[5], "0") == 0)
            {
                //printf("20+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e0") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[5], "1") == 0)
            {
                //printf("21+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e1") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            else if(strcmp(to_print[5], "2") == 0)
            {
                //printf("22+++++++++++++++++++\n");
                if(strcmp(string, "#e5e5e2") == 0)
                    fprintf(f1, "%s ", "#000000");
                else
                    fprintf(f1, "%s ", string);
            }
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
    FILE *f8 = fopen("cycle1.txt","r");
    //printf("23a+++++++++++++++++++\n");
    if(strcmp(to_print[6], " ") == 0)
    {
      //  printf("23b+++++++++++++++++++\n");
        while(!feof(f8))
        {
            fscanf(f8,"%s",string);
            if(strcmp(string,"DATA0") == 0)//if match found increment num
                fprintf(f1, "%s ", to_print[6]);
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
    }
    else
    {
       // if(f8 == NULL) 
         //   printf("mada=========\n");
        //printf("%dlllllllllllllllL\n", feof(f2));
        //printf("%s==================\n", to_print[6]);
        //printf("24+++++++++++++++++++\n");
        while(!feof(f8))
        {
        //    printf("25su+++++++++++++++++++\n");
            fscanf(f8,"%s",string);
            if(strcmp(string,"DATA0") == 0)//if match found increment num
            {
              //  printf("25sub+++++++++++++++++++\n");
                fprintf(f1, "%s ", to_print[6]);
              //  printf("25sum+++++++++++++++++++\n");
            }
            else if(strcmp(string , "#e5e5e5") == 0 )
                fprintf(f1, "%s ", "#000000");
            else if(strcmp(string , "#c18989") == 0 )
                fprintf(f1, "%s ", "#bc0000");
            else if(strcmp(string , "#b1d88a") == 0 )
                fprintf(f1, "%s ", "#8ae234");
            else if(strcmp(string , "#a5b7cc") == 0 )
                fprintf(f1, "%s ", "#729fcf");
            else if(strcmp(string , "#f4b170") == 0 )
                fprintf(f1, "%s ", "#f57900");
            else if(strcmp(string , "#eddf6d") == 0 )
                fprintf(f1, "%s ", "#edd400");
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
        //printf("bol chutiye *****\n");   
    }
   // printf("250+++++++++++++++++++\n");
    FILE *f9 = fopen("cycle2.txt","r");
   // printf("25a+++++++++++++++++++\n");
    if(strcmp(to_print[7], " ") == 0)
    {
      //  printf("25b+++++++++++++++++++\n");
        while(!feof(f9))
        {
            fscanf(f9,"%s",string);
            if(strcmp(string,"DATA1") == 0)//if match found increment num
                fprintf(f1, "%s ", to_print[7]);
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
    }
    else
    {
       // printf("25c+++++++++++++++++++\n");
        while(!feof(f9))
        {
            //printf("25+++++++++++++++++++\n");
            fscanf(f9,"%s",string);
            if(strcmp(string,"DATA1") == 0)//if match found increment num
                fprintf(f1, "%s ", to_print[7]);
            else if(strcmp(string , "#e5e5e5") == 0 )
                fprintf(f1, "%s ", "#000000");
            else if(strcmp(string , "#c18989") == 0 )
                fprintf(f1, "%s ", "#bc0000");
            else if(strcmp(string , "#b1d88a") == 0 )
                fprintf(f1, "%s ", "#8ae234");
            else if(strcmp(string , "#a5b7cc") == 0 )
                fprintf(f1, "%s ", "#729fcf");
            else if(strcmp(string , "#f4b170") == 0 )
                fprintf(f1, "%s ", "#f57900");
            else if(strcmp(string , "#eddf6d") == 0 )
                fprintf(f1, "%s ", "#edd400");
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }   
    }

    FILE *f10 = fopen("cycle3.txt","r");
    if(strcmp(to_print[8], " ") == 0)
    {
        while(!feof(f10))
        {
            fscanf(f10,"%s",string);
            if(strcmp(string,"DATA2") == 0)//if match found increment num
                fprintf(f1, "%s ", to_print[8]);
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
    }
    else
    {
        while(!feof(f10))
        {
            fscanf(f10,"%s",string);
            if(strcmp(string,"DATA2") == 0)//if match found increment num
                fprintf(f1, "%s ", to_print[8]);
            else if(strcmp(string , "#e5e5e5") == 0 )
                fprintf(f1, "%s ", "#000000");
            else if(strcmp(string , "#c18989") == 0 )
                fprintf(f1, "%s ", "#bc0000");
            else if(strcmp(string , "#b1d88a") == 0 )
                fprintf(f1, "%s ", "#8ae234");
            else if(strcmp(string , "#a5b7cc") == 0 )
                fprintf(f1, "%s ", "#729fcf");
            else if(strcmp(string , "#f4b170") == 0 )
                fprintf(f1, "%s ", "#f57900");
            else if(strcmp(string , "#eddf6d") == 0 )
                fprintf(f1, "%s ", "#edd400");
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }   
    }

    FILE *f11 = fopen("cycle4.txt","r");
    if(strcmp(to_print[9], " ") == 0)
    {
        while(!feof(f11))
        {
            fscanf(f11,"%s",string);
            if(strcmp(string,"DATA3") == 0)//if match found increment num
                fprintf(f1, "%s ", to_print[9]);
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
    }
    else
    {
        while(!feof(f11))
        {
            fscanf(f11,"%s",string);
            if(strcmp(string,"DATA3") == 0)//if match found increment num
                fprintf(f1, "%s ", to_print[9]);
            else if(strcmp(string , "#e5e5e5") == 0 )
                fprintf(f1, "%s ", "#000000");
            else if(strcmp(string , "#c18989") == 0 )
                fprintf(f1, "%s ", "#bc0000");
            else if(strcmp(string , "#b1d88a") == 0 )
                fprintf(f1, "%s ", "#8ae234");
            else if(strcmp(string , "#a5b7cc") == 0 )
                fprintf(f1, "%s ", "#729fcf");
            else if(strcmp(string , "#f4b170") == 0 )
                fprintf(f1, "%s ", "#f57900");
            else if(strcmp(string , "#eddf6d") == 0 )
                fprintf(f1, "%s ", "#edd400");
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }   
    }

    FILE *f12 = fopen("cycle5.txt","r");
    if(strcmp(to_print[10], " ") == 0)
    {
        while(!feof(f12))
        {
            fscanf(f12,"%s",string);
            if(strcmp(string,"DATA4") == 0)//if match found increment num
                fprintf(f1, "%s ", to_print[10]);
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }
    }
    else
    {
        while(!feof(f12))
        {
            fscanf(f12,"%s",string);
            if(strcmp(string,"DATA4") == 0)//if match found increment num
                fprintf(f1, "%s ", to_print[10]);
            else if(strcmp(string , "#e5e5e5") == 0 )
                fprintf(f1, "%s ", "#000000");
            else if(strcmp(string , "#c18989") == 0 )
                fprintf(f1, "%s ", "#bc0000");
            else if(strcmp(string , "#b1d88a") == 0 )
                fprintf(f1, "%s ", "#8ae234");
            else if(strcmp(string , "#a5b7cc") == 0 )
                fprintf(f1, "%s ", "#729fcf");
            else if(strcmp(string , "#f4b170") == 0 )
                fprintf(f1, "%s ", "#f57900");
            else if(strcmp(string , "#eddf6d") == 0 )
                fprintf(f1, "%s ", "#edd400");
            else
                fprintf(f1, "%s ", string);
            for(i = 0;i < 1000;i++)
                string[i] = '\0';
        }   
    }
    FILE *f13 = fopen("end.txt","r");
    while(!feof(f13))
    {
        fscanf(f13,"%s",string);
        /*if(strcmp(string,"DATA0") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[0]);
        else if(strcmp(string,"DATA1") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[1]);
        else if(strcmp(string,"DATA2") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[2]);
        else if(strcmp(string,"DATA3") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[3]);
        else if(strcmp(string,"DATA4") == 0)//if match found increment num
            fprintf(f1, "%s ", to_print[4]);
        else*/
        fprintf(f1, "%s ", string);
        for(i = 0;i < 1000;i++)
            string[i] = '\0';
    }
    fclose(f1);
    fclose(f2);
    fclose(f3);
    fclose(f4);
    fclose(f5);
    fclose(f6);
    fclose(f7);
    fclose(f8);
    fclose(f9);
    fclose(f10);
    fclose(f11);
    fclose(f12);
    fclose(f13);
}

double take_value(char c[]){
	int i = 1;
	char index[3];
	index[0] = '\0';
	index[1] = '\0';
	index[2] = '\0';
	while(c[i] != '\0')
	{
		index[i-1] = c[i];
		i++;
	}
	return(binary_to_int(register_file[atoi(index)]));
//	printf("kuch to bta de bhai\n");
}
void write_value(double a , char b[]){
	int i = 1;
	char index[3];
	char no[33];
	while(b[i] != '\0')
	{
		index[i-1] = b[i];
		i++;
	}
	if(a >= 0){
		for(i = 0;i < 32;i++)
		{
			register_file[atoi(index)][i] = (int) a%2 ;
			a = a/2;
		}
	}
	else{
		a = -1*a - 1;
		for(i = 0; i < 32;i++){
			register_file[atoi(index)][i] = (int) a%2;
			a = a/2;
		}
		for(i = 0;i<32;i++){
			register_file[atoi(index)][i] = register_file[atoi(index)][i] ^ 1;
		}
	}
}

void set_LH(double a, int b){
	int result[64],i;
	double c = 0;
	double d = HI[31];
	for(i = 30;i>=0;i--){
		c = 2*c + HI[i];
		d = 2*d;
	}
	for(i = 31;i >= 0;i--){
		c = 2*c + LU[i];
		d = 2*d;
	}
	if(b = 1)
		a += c-d;
	if(a >= 0){
		for(i = 0;i < 64;i++)
		{
			result[i] = (int) a%2 ;
			a = a/2;
		}
	}
	else{
		a = -1*a - 1;
		for(i = 0; i < 64;i++){
			result[i] = (int) a%2;
			a = a/2;
		}
		for(i = 0;i<64;i++){
			result[i] = result[i] ^ 1;
		}
	}
	for(i = 0;i<32;i++){
		LU[i] = result[i];
	}
	for(i = 32;i < 64;i++){
		HI[i-32] = result[i];
	}
//	printf("result\n");
		for(i = 31;i >=0;i--){
//		printf("%d",HI[i]);
	}
	for(i = 31;i>=0;i--){
//		printf("%d",LU[i]);
	}	
}

void add_function(int index, char a[], char b[], char c[], char d[]){
	int y[32];
	int z[32];
	int i;
    if(index == 0){
    	ifidwrite.pc = pc_read;
        strncpy(ifidwrite.Rn,c,10);
        strncpy(ifidwrite.Rm,d,10);
        strncpy(ifidwrite.Rd,b,10);
    }
    if(index == 1){
    	idexwrite.pc = ifidread.pc;
        strncpy(idexwrite.Rn,ifidread.Rn,10);
//        printf(" ****** Here (%s)\n", ifidread.Rm);
        strncpy(idexwrite.Rm,ifidread.Rm,10);
        strncpy(idexwrite.Rd,ifidread.Rd,10);
        idexwrite.RW = 1;
        idexwrite.MR = 0;
        //if(fwdA == 0 ){
        	idexwrite.rn = take_value(c);
//        	printf(" ******** rn = %lf \n",idexwrite.rn);
    	//}	
        //if(fwdB == 0){
			idexwrite.rm = take_value(d);
//			printf(" ******** rm = %lf \n",idexwrite.rm);
		//}
    }
    else if(index == 2){
    	exmemwrite.pc = idexread.pc;
        strncpy(exmemwrite.Rd, idexread.Rd, 10);
        exmemwrite.RW = 1;
        exmemwrite.MW = 0;
        //alu
        if(strcmp(a, "add") == 0){
         	exmemwrite.rd = idexread.rn + idexread.rm;
 //        	printf("========= %lf %lf %lf ==========\n", idexread.rn, idexread.rm, exmemwrite.rd);
         	}        	
        else if(strcmp(a, "sub") == 0)
        	exmemwrite.rd = idexread.rn - idexread.rm;
        else if(strcmp(a, "and") == 0)
        	exmemwrite.rd = (int) idexread.rm & (int) idexread.rn; 
        else if(strcmp(a, "or") == 0)
        	exmemwrite.rd = (int) idexread.rm | (int) idexread.rn;
        else if(strcmp(a, "nor") == 0){
        	exmemwrite.rd = ~((int) idexread.rm | (int) idexread.rn);
        }
        else if(strcmp(a, "sltu") == 0)
        {	
        	if(idexread.rn < idexread.rm)
        		exmemwrite.rd = 1;
        	else
        		exmemwrite.rd = 0;		
        }
        else if(strcmp(a, "sllv") == 0)
        {
        	int l = idexread.rn;
        	for(i = 0;i < 32; i++)
        	{
        		y[i] = l%2;
        		l = l/2;
        	}

        	for(i = 0;i < 32 - idexread.rm; i++)
        		z[(int) idexread.rm + i] = y[i];
        	for(i = 0;i < idexread.rm; i++)
        		z[i] = 0; 
        	exmemwrite.rd = binary_to_int(z);        	
        }
       /* if(fwdA == 1){
    		pthread_mutex_lock(&mutex);
    		idexwrite.rn = exmemwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}
    	else if(fwdB == 1){
    		pthread_mutex_lock(&mutex);
    		idexwrite.rm = exmemwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}*/
    }
    else if(index == 3){
		if(bt != 4){
			pc_write = exmemread.pc + 4;
		}    	
		else{
			pc_write = exmemread.pc + (4 * offset);
		}

        strncpy(memwbwrite.Rd, exmemread.Rd, 10);
        memwbwrite.RW = 1;
    	memwbwrite.rd = exmemread.rd;

    	/*if(fwdA == 2){
    		pthread_mutex_lock(&mutex);
    		idexwrite.rn = memwbwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}
    	if(fwdB == 2){
    		pthread_mutex_lock(&mutex);
    		idexwrite.rm = memwbwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}
    	if(fwdC == 1){
    		pthread_mutex_lock(&mutex);
    		exmemwrite.rd = memwbwrite.rd;
    		pthread_mutex_unlock(&mutex);	
    	}*/
    }
    else if(index == 4){

		write_value(memwbread.rd, b);
	}
}
void immediate_add_function(int index, char a[], char b[], char c[], char d[]){
	int y[32];
	int z[32];	
    if(index == 0)
    {
    	ifidwrite.pc = pc_read;
        strncpy(ifidwrite.Rn,c,10);
        strncpy(ifidwrite.Rm," ",10);
        strncpy(ifidwrite.Rd,b,10);
    }
    else if(index == 1)
    {
    	idexwrite.pc = ifidread.pc;
        strncpy(idexwrite.Rn,ifidread.Rn,10);
        strncpy(idexwrite.Rm,ifidread.Rm,10);
        strncpy(idexwrite.Rd,ifidread.Rd,10);
       // if(fwdA == 0){
        	idexwrite.rn = take_value(c);
        //}
        idexwrite.RW = 1;
        idexwrite.MR = 0;
    }
    else if(index == 2)
    {
    	exmemwrite.pc = idexread.pc;
    	//printf("****** YAHAN PAR HAI*****\n");
        strncpy(exmemwrite.Rd, idexread.Rd, 10);
        exmemwrite.RW = 1;
        exmemwrite.MW = 0;

        //alu
    	if(strcmp(a, "addi") == 0)
         	exmemwrite.rd = idexread.rn + atoi(d);        //findimm gives no in integer
        else if(strcmp(a, "ori") == 0)
        {
//        	printf("%d-------------------<\n", atoi(d));
         	exmemwrite.rd = (int) idexread.rn | atoi(d);	
//         	printf("%lf-------------------<-%lf\n", exmemwrite.rd, idexread.rn);
        }
        else if(strcmp(a,"slti") == 0)
        {
        	if(idexread.rn < atoi(d))				//doubt in rn and rd
        		exmemwrite.rd = 1;
        	else
        		exmemwrite.rd = 0;
        }
        else if(strcmp(a,"sll") == 0)
        {
        	int i = 0;
        	int l = idexread.rn;
        	for(i = 0;i < 32; i++)
        	{
        		y[i] = l%10;
        		l = l/10;
        	}
        	for(i = 0;i < 32-atoi(d); i++)
        		z[atoi(d)+i] = y[i];
        	for(i = 0;i < atoi(d); i++)
        		z[i] = 0; 
        	exmemwrite.rd = binary_to_int(z);
        }
        //printf("******YAHAN PAR BHI HAI ********\n");
        //printf("******** fwdB = %d\n",fwdB);
       /* if(fwdA == 1){
    		pthread_mutex_lock(&mutex);
    		idexwrite.rn = exmemwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}
    	if(fwdB == 1){
    		pthread_mutex_lock(&mutex);
    		//printf(" ###### ab toh aa hi jayega ######\n");
    		idexwrite.rm = exmemwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}*/
    }
    else if(index == 3)
    {
    	if(bt == 0){
			pc_write = exmemread.pc + 4;
		}    	
		else{
			pc_write = exmemread.pc + (4 * offset);
		}
        strncpy(memwbwrite.Rd, exmemread.Rd, 10);
        memwbwrite.RW = 1;

        memwbwrite.rd = exmemread.rd;

       /* if(fwdA == 2){
    		pthread_mutex_lock(&mutex);
    		idexwrite.rn = memwbwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}
    	else if(fwdB == 2){
    		pthread_mutex_lock(&mutex);
    		idexwrite.rm = memwbwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}
    	if(fwdC == 1){
    		pthread_mutex_lock(&mutex);
    		exmemwrite.rd = memwbwrite.rd;
    		pthread_mutex_unlock(&mutex);	
    	}*/
    }
    else if(index == 4)
    {
		write_value(memwbread.rd, b);
	}
}
void load_function(int index, char a[], char b[], char c[], char d[]){
    if(index == 0){
    	ifidwrite.pc = pc_read;
        strncpy(ifidwrite.Rn,c,10);
        strncpy(ifidwrite.Rm," ",10);
        strncpy(ifidwrite.Rd,b,10);

    }
    if(index == 1){
    	idexwrite.pc = ifidread.pc;
        strncpy(idexwrite.Rn,ifidread.Rn,10);
        strncpy(idexwrite.Rm,ifidread.Rm,10);
        strncpy(idexwrite.Rd,ifidread.Rd,10);
        idexwrite.RW = 1;
        idexwrite.MR = 1;
        data_access++;

        idexwrite.rn = take_value(c);
    }
    else if(index == 2){
    	exmemwrite.pc = idexread.pc;
        strncpy(exmemwrite.Rd, " ", 10);
        exmemwrite.RW = 1;
        exmemwrite.MW = 0;

        exmemwrite.rn = idexread.rn;
  //      printf("################# %lf",idexread.rn);
    }
    else if(index == 3){
    	int i;
    	int array[32];
        if(bt == 0){
			pc_write = exmemread.pc + 4;
		}    	
		else{
			pc_write = exmemread.pc + (4 * offset);
		}
        strncpy(memwbwrite.Rd, b, 10);
        memwbwrite.RW = 1;
        memwbwrite.MR = 1;
        memwbwrite.rn = exmemread.rn;

        if(strcmp(a, "load") == 0)
        {
        	double m = memwbwrite.rn + atof(d) - address_start;
            for(i = 0;i < 32 ;i++)
            {
                array[i] = data_memory[(int)m + (i/8)][i%8];
 //               printf("%d", array[i]);
            }
   //         printf("======================================\n");
        	memwbwrite.rd = binary_to_int(array);
     //   	printf("======================================%lf\n",memwbwrite.rd);
       // 	printf("################ %lf\n", memwbwrite.rd);			//make an ofset funtion and find_from_mem funtion
        }
    	else if(strcmp(a, "lb") == 0)
    	{
        	double m = memwbwrite.rn + atof(d) - address_start;
        	memwbwrite.rd = binary_to_int2(data_memory[(int)m]);
        }
        //if(fwdC == 1){
        //	pthread_mutex_lock(&mutex);
        //	exmemwrite.rd = memwbwrite.rd;
        //	pthread_mutex_unlock(&mutex);
        //}
    }
    else if(index == 4){
		write_value(memwbread.rd, b);
	}
}
void immediate_load_function(int index, char a[], char b[], char c[], char d[]){
    int y[32];
    int z[32];
    if(index == 0){
    	ifidwrite.pc = pc_read;
        strncpy(ifidwrite.Rn, " ",10);
        strncpy(ifidwrite.Rm," ",10);
        strncpy(ifidwrite.Rd,b,10);

    }
    if(index == 1){
    	idexwrite.pc = ifidread.pc;
        strncpy(idexwrite.Rn,ifidread.Rn,10);
        strncpy(idexwrite.Rm,ifidread.Rm,10);
        strncpy(idexwrite.Rd,ifidread.Rd,10);
        idexwrite.RW = 1;
        idexwrite.MR = 0;
    }
    else if(index == 2){
    	exmemwrite.pc = idexread.pc;
        int i = 0;
        strncpy(exmemwrite.Rd, idexread.Rd, 10);
        exmemwrite.RW = 1;
        exmemwrite.MW = 0;

        int l = atoi(c);
//        printf("-----------------------%d-----------------\n", l);
  //      printf("Yes\n");
        for(i = 0;i <= 31; i++)
        {
        	y[i] = l%2;
       		l = l/2;
       	}
    //   	printf("Yes\n");
        for(i = 0;i < 16; i++)
       		z[i] = 0;
       	for(i = 16;i <= 31; i++)
       		z[i] = y[ i - 16]; 
      // 	printf("aa rha hai\n");
       	exmemwrite.rd = binary_to_int(z);
       	//printf("-----------------------%lf-----------------\n", binary_to_int(z));
       //	printf("Yes\n");
       	/*if(fwdA == 1){
    		pthread_mutex_lock(&mutex);
    		idexwrite.rn = exmemwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}
    	else if(fwdB == 1){
    		pthread_mutex_lock(&mutex);
    		idexwrite.rm = exmemwrite.rd;
    		pthread_mutex_unlock(&mutex);
    	}*/
    }
    else if(index == 3){
        if(bt == 0){
			pc_write = exmemread.pc + 4;
		}    	
		else{
			pc_write = exmemread.pc + (4 * offset);
		}
        strncpy(memwbwrite.Rd, b, 10);
        memwbwrite.RW = 1;
        memwbwrite.MR = 0;

        memwbwrite.rd = exmemread.rd;
       // if(fwdC == 1){
        //	pthread_mutex_lock(&mutex);
        	//exmemwrite.rd = memwbwrite.rd;
        //	pthread_mutex_unlock(&mutex);
        //}
    }
    else if(index == 4)
    {
		write_value(memwbread.rd, b);
	}
}
void store_function(int index, char a[], char b[], char c[], char d[]){
    if(index == 0){
    	ifidwrite.pc = pc_read;
   // 	printf("yahan par hai\n");
        strncpy(ifidwrite.Rn,c,10);
        strncpy(ifidwrite.Rm," ",10);
        strncpy(ifidwrite.Rd,b,10);
     //   printf("yahan khtam\n");
    }
    if(index == 1){
    	idexwrite.pc = ifidread.pc;
    	//printf("aa gya\n");
        strncpy(idexwrite.Rn,ifidread.Rn,10);
        strncpy(idexwrite.Rm,ifidread.Rm,10);
        strncpy(idexwrite.Rd,ifidread.Rd,10);
        idexwrite.MR = 1;
        data_access++;
        idexwrite.RW = 0;
        //printf("phasne wala hai\n");
       //pthread_mutex_lock(&mutex);
        idexwrite.rn = take_value(c);
        //int z = 9;
       	//printf("kaunsa galata hai\n");
		idexwrite.rd = take_value(b);

	//	printf("=========================%lf  %lf =======================\n", idexwrite.rn,idexwrite.rd);
		//pthread_mutex_unlock(&mutex);
		//printf("nhi phasa\n");
    }
    else if(index == 2){
    	exmemwrite.pc = idexread.pc;
        strncpy(exmemwrite.Rd, b, 10);
        exmemwrite.RW = 0;
        exmemwrite.MW = 0;

        exmemwrite.rn = idexread.rn;
        

       // if(fwdC == 0){
        	exmemwrite.rd = idexread.rd;	
        //}
 //       printf("$$$$$$$$$$$$$$$$$$$$$$$$ Rd = %lf\n",idexread.rd);
   //     printf("$$$$$$$$$$$$$$$$$$$$$$$$ Rn = %lf\n",idexread.rn);

    }
    else if(index == 3){
        if(bt == 0){
			pc_write = exmemread.pc + 4;
		}    	
		else{
			pc_write = exmemread.pc + (4 * offset);
		}
        strncpy(memwbwrite.Rd, " ", 10);
        memwbwrite.RW = 0;

         if(strcmp(a, "store") == 0)
        {
     //   	printf("$$$$$$$$ AA GYA $$$$$$$\n");
        	int j;
    		double k = exmemread.rd;
        	double m = exmemread.rn + atof(d);
       // 	printf("$$$$$$$$$$$$$$$$$$$$$$$$ Rd = %lf\n",exmemread.rd);
        //	printf("$$$$$$$$$$$$$$$$$$$$$$$$ Rn = %lf\n",exmemread.rn);
        //	printf("$$$$$$$$$$$$$$$$$$$$$$$$ Rn = %lf\n",atof(d));
        	for(j = 0;j < 32;j++)
        	{
        		double no = m - address_start;
        		data_memory[(int)no + (j/8)][j%8] = (int)fmod(k,2);                //change code
        //		printf("%d", (int)fmod(k,2));
        		k = k/2;  
        	}
        //	printf("\n");
        //	printf("%lf\n", k);
        }
    	else if(strcmp(a, "sb") == 0)
    	{
    		int j;
    		double k = exmemread.rd;
        	double m = exmemread.rn;
        	for(j = 0;j < 8;j++)
        	{
        		double no = m - address_start;
        		data_memory[(int)m][j] = (int)fmod(k,2);                      //change code
        		k = k/2;  
        	}
        //	printf("333333*********************************%lf\n", m);			//make an offset funtion and write_to_mem funtion
        }
    }
    else if(index == 4){
    	//does nothing
    	int c = 2;
    }    
}

void mult_function(int index, char a[], char b[], char c[], char d[]){
	if(index == 0){
		ifidwrite.pc = pc_read;
        strncpy(ifidwrite.Rn,b,10);
        strncpy(ifidwrite.Rm,c,10);
        strncpy(ifidwrite.Rd," ",10);
	}
	else if(index == 1){
		idexwrite.pc = ifidread.pc;
        strncpy(idexwrite.Rn,ifidread.Rn,10);
        strncpy(idexwrite.Rm,ifidread.Rm,10);
        strncpy(idexwrite.Rd,ifidread.Rd,10);
        idexwrite.rn = take_value(b);
        idexwrite.rm = take_value(c);
        idexwrite.RW = 1;
        idexwrite.MR = 0;
       // printf("=========================%lf  %lf =======================\n", idexwrite.rn,idexwrite.rm);
	}
	else if(index == 2){
		exmemwrite.pc = idexread.pc;
        exmemwrite.RW = 1;
        exmemwrite.MW = 0;
        exmemwrite.hilo = idexread.rn*idexread.rm;
     //   printf("||||||||||||||||||||  rn  ||||%lf||||||||||||||||||||\n", idexread.rn);
      //  printf("|||||||||||||||||||||  rm    |||%lf||||||||||||||||||||\n", idexread.rm);
       // printf("|||||||||||||||||||||   rn*rm   |||%lf||||||||||||||||||||\n", exmemwrite.rd);
	}
	else if(index == 3){
		if(bt == 0){
			pc_write = exmemread.pc + 4;
		}    	
		else{
			pc_write = exmemread.pc + (4 * offset);
		}
        memwbwrite.RW = 1;
        memwbwrite.hilo = exmemread.hilo;
     //   printf("||||||||||||||  rn*rm    ||||||||||%lf||||||||||||||||||||\n", memwbwrite.rd);
	}
	else if(index == 4){
		if(strcmp(a,"mult") == 0)
			set_LH(memwbread.hilo,0);
		else
			set_LH(memwbread.hilo,1);
	}
}
void branch_equal_function(int index, char a[], char b[], char c[], char d[]){
	int y[32];
	int z[32];
	int i;
    if(index == 0){
    	ifidwrite.pc = pc_read;
        strncpy(ifidwrite.Rn,b,10);
        strncpy(ifidwrite.Rm,c,10);
        strncpy(ifidwrite.Rd," ",10);
    }
    if(index == 1){
    	idexwrite.pc = ifidread.pc;
        strncpy(idexwrite.Rn,ifidread.Rn,10);
        //printf(" ****** Here (%s)\n", ifidread.Rm);
        strncpy(idexwrite.Rm,ifidread.Rm,10);
        strncpy(idexwrite.Rd,ifidread.Rd,10);
        idexwrite.rn = take_value(b);
        idexwrite.rm = take_value(c);
        idexwrite.RW = 0;
        idexwrite.MR = 0;
    }
    else if(index == 2){
    	exmemwrite.pc = idexread.pc;
        strncpy(exmemwrite.Rd, idexread.Rd, 10);
        exmemwrite.RW = 0;
        exmemwrite.MW = 0;
        //alu
        if(idexread.rn == idexread.rm || strcmp(b,c) == 0){
         	exmemwrite.rd = 0;
         	bt = 4;
         	no_branch++;
         	offset = atoi(d);
        }
    }
    else if(index == 3){
    	if(bt == 0){
			pc_write = exmemread.pc + 4;
		}    	
		else{
			pc_write = exmemread.pc + (4 * offset);
		}
        strncpy(memwbwrite.Rd, exmemread.Rd, 10);
        memwbwrite.RW = 0;
    	memwbwrite.rd = 0;
    }
    else if(index == 4){
    	int z = 2;
	}
}
void branch_function(int index, char a[], char b[], char c[], char d[]){
	int y[32];
	int z[32];
	int i;
    if(index == 0){
    	ifidwrite.pc = pc_read;
        strncpy(ifidwrite.Rn,b,10);
        strncpy(ifidwrite.Rm," ",10);
        strncpy(ifidwrite.Rd," ",10);
    }
    if(index == 1){
    	idexwrite.pc = ifidread.pc;
        strncpy(idexwrite.Rn,ifidread.Rn,10);
        //printf(" ****** Here (%s)\n", ifidread.Rm);
        strncpy(idexwrite.Rm,ifidread.Rm,10);
        strncpy(idexwrite.Rd,ifidread.Rd,10);
        idexwrite.rn = take_value(b);
        idexwrite.RW = 0;
        idexwrite.MR = 0;
    }
    else if(index == 2){
    	exmemwrite.pc = idexread.pc;
        strncpy(exmemwrite.Rd, idexread.Rd, 10);
        exmemwrite.RW = 0;
        exmemwrite.MW = 0;
        //alu
        if(strcmp(a, "bgez") == 0){
         	if(idexread.rn >= 0.0){
         		exmemwrite.rd = 0;
         		bt = 4;
         		no_branch++;
         		offset = atoi(c);
         	}
        }        	
        else if(strcmp(a, "bgtz") == 0){
        	if(idexread.rn > 0.0){
         		exmemwrite.rd = 0;
         		bt = 4;
         		no_branch++;
         		offset = atoi(c);
         	}
        }
        else if(strcmp(a, "blez") == 0){
        	if(idexread.rn <= 0.0){
         		exmemwrite.rd = 0;
         		bt = 4;
         		no_branch++;
         		offset = atoi(c);
         	} 
        }
        else if(strcmp(a, "bltz") == 0){
        	if(idexread.rn < 0.0){
         		exmemwrite.rd = 0;
         		bt = 4;
         		no_branch++;
         		offset = atoi(c);
         	}
        }

    }
    else if(index == 3){
    	if(bt == 0){
			pc_write = exmemread.pc + 4;
		}    	
		else{
			pc_write = exmemread.pc + (4 * offset);
		}
        strncpy(memwbwrite.Rd, exmemread.Rd, 10);
        memwbwrite.RW = 0;
    	memwbwrite.rd = 0;
    }
    else if(index == 4){
    	int z = 2;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
void jump_function(int index, char a[], char b[], char c[], char d[]){
    int y[32];
    int z[32];
    int i;
    if(index == 0){
        ifidwrite.pc = pc_read;
        strncpy(ifidwrite.Rn," ",10);
        strncpy(ifidwrite.Rm," ",10);
        strncpy(ifidwrite.Rd," ",10);
    }
    if(index == 1){
        idexwrite.pc = ifidread.pc;
        strncpy(idexwrite.Rn,ifidread.Rn,10);
        //printf(" ****** Here (%s)\n", ifidread.Rm);
        strncpy(idexwrite.Rm,ifidread.Rm,10);
        strncpy(idexwrite.Rd,ifidread.Rd,10);
        idexwrite.RW = 0;
        idexwrite.MR = 0;
        jt = 4;
        offset = atoi(c) 
    }
    else if(index == 2){
        exmemwrite.pc = offset*4;
        strncpy(exmemwrite.Rd, idexread.Rd, 10);
        exmemwrite.RW = 0;
        exmemwrite.MW = 0;
        //alu
        if(strcmp(a, "j") == 0){

        }           
        else if(strcmp(a, "jal") == 0){

        }

    }
    else if(index == 3){
        if(jt == 0){
            pc_write = exmemread.pc + 4;
        }       
        else{
            pc_write = exmemread.pc + (4 * offset);
        }
        strncpy(memwbwrite.Rd, exmemread.Rd, 10);
        memwbwrite.RW = 0;
        memwbwrite.rd = 0;
    }
    else if(index == 4){
        int z = 2;
    }
}

void do_data_forwading(){
	if(fwdA == 1){
    		idexwrite.rn = exmemwrite.rd;
    }
    if(fwdB == 1){
    	//	printf("##### AB HOYEGA NA #####\n");
    		idexwrite.rm = exmemwrite.rd;
    }
	if(fwdA == 2){
    		idexwrite.rn = memwbwrite.rd;
    }
    if(fwdB == 2){
    		idexwrite.rm = memwbwrite.rd;
    }
    if(fwdC == 1){
    		exmemwrite.rd = memwbwrite.rd;	
    }
    if(fwdD == 1){
    	//	printf("############ AA GYA NA YAHAN PAR #########\n");
    		idexwrite.rd = memwbwrite.rd;
    }

}
void* evaluate(void * args){
    thread_info* infor = (struct thread_info* )args;
    
    //code for printing the instruction above
    pthread_mutex_lock(&mutex);
    char ans[100];
    sprintf(ans, "%s %s %s %s",infor->ins, infor->first,infor->second, infor->third);
    strncpy(to_print[infor->index + 6],ans,100);
    pthread_mutex_unlock(&mutex);

    //code for setting the value of registers
    if(infor->index == 0){
        strncpy(ifidwrite.instruction.ins,infor->ins,10);
        strncpy(ifidwrite.instruction.first ,infor->first,10);
        strncpy(ifidwrite.instruction.second, infor->second,10);
        strncpy(ifidwrite.instruction.third ,infor->third,10);
    }
    //add type instructions
    if(strcmp(infor->ins,"and") == 0 |strcmp(infor->ins,"or") == 0 |strcmp(infor->ins,"add") == 0 |strcmp(infor->ins,"sub") == 0 |strcmp(infor->ins, "nor") == 0 | strcmp(infor->ins, "sltu") == 0 | strcmp(infor->ins, "sllv") == 0){
        add_function(infor->index, infor->ins, infor->first, infor->second, infor->third);
    }
    //immediate add type instructions
    else if(strcmp(infor->ins, "addi") == 0 | strcmp(infor->ins, "ori") == 0 | strcmp(infor->ins, "slti") == 0 | strcmp(infor->ins, "sll") == 0){
        immediate_add_function(infor->index, infor->ins, infor->first, infor->second, infor->third);
    }
    //load type instructions
    else if(strcmp(infor->ins, "load") == 0 | strcmp(infor->ins, "lb") == 0){
        load_function(infor->index, infor->ins, infor->first, infor->second, infor->third);
    }
    //immediate load type instruction
    else if(strcmp(infor->ins, "lui") == 0 ){
        immediate_load_function(infor->index, infor->ins, infor->first, infor->second, infor->third);
    }
    //store type instructions
    else if(strcmp(infor->ins, "store") == 0 | strcmp(infor->ins, "sb") == 0){
        store_function(infor->index, infor->ins, infor->first, infor->second, infor->third);
    }

    else if(strcmp(infor->ins, "mult") == 0 | strcmp(infor->ins, "madd") == 0){
    	mult_function(infor->index, infor->ins, infor->first, infor->second, infor->third);
    }
    else if(strcmp(infor->ins, "bltz") == 0 | strcmp(infor->ins, "blez") == 0 | strcmp(infor->ins, "bgtz") == 0 | strcmp(infor->ins, "bgez") == 0 ){
    	branch_function(infor->index, infor->ins, infor->first, infor->second, infor->third);
    }

    else if(strcmp(infor->ins, "j") == 0 | strcmp(infor->ins, "jal") == 0){
        jump_function(infor->index, infor->ins, infor->first, infor->second, infor->third);   
    }

    else if(strcmp(infor->ins, "beq") == 0)
    {
        branch_equal_function(infor->index, infor->ins, infor->first, infor->second, infor->third);   
    }


}

void* set_thread_status (void* args){
    thread_info2* infor2 = (struct thread_info2* )args;
    char a[6];
    a[0] = infor2->thr[0] + '0';
    a[1] = infor2->thr[1] + '0';
    a[2] = infor2->thr[2] + '0';
    a[3] = infor2->thr[3] + '0';
    a[4] = infor2->thr[4] + '0';
//    printf("%s=====================\n", a);
    strncpy(to_print[0],a,100);
  //  printf("%s=====================\n", to_print[0]);
}


int convert_to_decimal(char num[]){
    int i = 0;
    double val = 0;
    while(num[i] != '\0'){
        val = 2*val + (num[i] - '0');
        i++;
    }
    return val;
}

int convert_to_decimal1(char num[]){
    double c = num[0] - '0';
    int i = 1;
    double val = 0;
    while(num[i] != '\0'){
        c = 2*c;
        val = 2*val + (num[i] - '0');
        i++;
    }
    return (val-c);
}

void add_to_array(char* str){
    
    int i = 0;
    char c;
    while(i < 8){
        c = *(str + i); 
        int j = i*4;
        switch(c){
             case '0': array[row][j] = '0';array[row][j+1] = '0';array[row][j+2] ='0';array[row][j+3] = '0'; break;
             case '1': array[row][j] = '0';array[row][j+1] = '0';array[row][j+2] ='0';array[row][j+3] = '1'; break;
             case '2': array[row][j] = '0';array[row][j+1] = '0';array[row][j+2] ='1';array[row][j+3] = '0'; break;
             case '3': array[row][j] = '0';array[row][j+1] = '0';array[row][j+2] ='1';array[row][j+3] = '1'; break;
             case '4': array[row][j] = '0';array[row][j+1] = '1';array[row][j+2] ='0';array[row][j+3] = '0'; break;
             case '5': array[row][j] = '0';array[row][j+1] = '1';array[row][j+2] ='0';array[row][j+3] = '1'; break;
             case '6': array[row][j] = '0';array[row][j+1] = '1';array[row][j+2] ='1';array[row][j+3] = '0'; break;
             case '7': array[row][j] = '0';array[row][j+1] = '1';array[row][j+2] ='1';array[row][j+3] = '1'; break;
             case '8': array[row][j] = '1';array[row][j+1] = '0';array[row][j+2] ='0';array[row][j+3] = '0'; break;
             case '9': array[row][j] = '1';array[row][j+1] = '0';array[row][j+2] ='0';array[row][j+3] = '1'; break;
             case 'A': array[row][j] = '1';array[row][j+1] = '0';array[row][j+2] ='1';array[row][j+3] = '0'; break;
             case 'B': array[row][j] = '1';array[row][j+1] = '0';array[row][j+2] ='1';array[row][j+3] = '1'; break;
             case 'C': array[row][j] = '1';array[row][j+1] = '1';array[row][j+2] ='0';array[row][j+3] = '0'; break;
             case 'D': array[row][j] = '1';array[row][j+1] = '1';array[row][j+2] ='0';array[row][j+3] = '1'; break;
             case 'E': array[row][j] = '1';array[row][j+1] = '1';array[row][j+2] ='1';array[row][j+3] = '0'; break;
             case 'F': array[row][j] = '1';array[row][j+1] = '1';array[row][j+2] ='1';array[row][j+3] = '1'; break;
             case 'a': array[row][j] = '1';array[row][j+1] = '0';array[row][j+2] ='1';array[row][j+3] = '0'; break;
             case 'b': array[row][j] = '1';array[row][j+1] = '0';array[row][j+2] ='1';array[row][j+3] = '1'; break;
             case 'c': array[row][j] = '1';array[row][j+1] = '1';array[row][j+2] ='0';array[row][j+3] = '0'; break;
             case 'd': array[row][j] = '1';array[row][j+1] = '1';array[row][j+2] ='0';array[row][j+3] = '1'; break;
             case 'e': array[row][j] = '1';array[row][j+1] = '1';array[row][j+2] ='1';array[row][j+3] = '0'; break;
             case 'f': array[row][j] = '1';array[row][j+1] = '1';array[row][j+2] ='1';array[row][j+3] = '1'; break;
             default:  printf("\n %c ---- Invalid hexadecimal digit \n",c); return ;
         }
         i++;
    }
    row++;
}
void arithmetic_logic(int row_num, char code[]){
    char part[6];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 10; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
    int ans = convert_to_decimal(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    char dollar[255] ;
    strcpy(dollar, "$");
    char* sec = buffer;
    strcat(dollar, sec);
    strncpy(IM[row_num].second,dollar,10);
    char part1[6];
    j = 0;
    for(i = 11; i <= 15; i++){
        part1[j] = array[row_num][i];
        j++;
    }
    part1[5] = '\0';
    char dollar1[255];
    char buffer1[20];
    ans = convert_to_decimal(part1);
    snprintf(buffer1,10,"%d", ans);
    strcpy(dollar1, "$");
    sec = buffer1;
    strcat(dollar1, sec);
    strncpy(IM[row_num].third,dollar1,10);
    char part2[6];
    j = 0;
    for(i = 16; i <= 20; i++){
        part2[j] = array[row_num][i];
        j++;
    }
    part2[5] = '\0';
    char dollar2[255];
    char buffer2[20];
    ans = convert_to_decimal(part2);
    snprintf(buffer2,10,"%d", ans);
    strcpy(dollar2, "$");
    sec = buffer2;
    strcat(dollar2, sec);
    strncpy(IM[row_num].first,dollar2,10); 
}
void arithmetic_logic_1(int row_num, char code[]){
    char part[6];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 10; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
    int ans = convert_to_decimal(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    char dollar[255] ;
    strcpy(dollar, "$");
    char* sec = buffer;
    strcat(dollar, sec);
    strncpy(IM[row_num].third,dollar,10);
    char part1[6];
    j = 0;
    for(i = 11; i <= 15; i++){
        part1[j] = array[row_num][i];
        j++;
    }
    part1[5] = '\0';
    char dollar1[255];
    char buffer1[20];
    ans = convert_to_decimal(part1);
    snprintf(buffer1,10,"%d", ans);
    strcpy(dollar1, "$");
    sec = buffer1;
    strcat(dollar1, sec);
    strncpy(IM[row_num].second,dollar1,10);
    char part2[6];
    j = 0;
    for(i = 16; i <= 20; i++){
        part2[j] = array[row_num][i];
        j++;
    }
    part2[5] = '\0';
    char dollar2[255];
    char buffer2[20];
    ans = convert_to_decimal(part2);
    snprintf(buffer2,10,"%d", ans);
    strcpy(dollar2, "$");
    sec = buffer2;
    strcat(dollar2, sec);
    strncpy(IM[row_num].first,dollar2,10); 
}
void arithmetic_logic_2(int row_num, char code[]){
    char part[6];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 10; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
    int ans = convert_to_decimal(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    char dollar[255] ;
    strcpy(dollar, "$");
    char* sec = buffer;
    strcat(dollar, sec);
    strncpy(IM[row_num].first,dollar,10);
    char part1[6];
    j = 0;
    for(i = 11; i <= 15; i++){
        part1[j] = array[row_num][i];
        j++;
    }
    part1[5] = '\0';
    char dollar1[255];
    char buffer1[20];
    ans = convert_to_decimal(part1);
    snprintf(buffer1,10,"%d", ans);
    strcpy(dollar1, "$");
    sec = buffer1;
    strcat(dollar1, sec);
    strncpy(IM[row_num].second,dollar1,10);
    strncpy(IM[row_num].third," ",10); 
}
void arithmetic_logic_3(int row_num, char code[]){
    
    int i = 25;
    int j = 0;
    char part1[6];
    j = 0;
    for(i = 11; i <= 15; i++){
        part1[j] = array[row_num][i];
        j++;
    }
    part1[5] = '\0';
    char dollar1[255];
    char buffer1[20];
    int ans = convert_to_decimal(part1);
    snprintf(buffer1,10,"%d", ans);
    strcpy(dollar1, "$");
    char* sec = buffer1;
    strcat(dollar1, sec);
    strncpy(IM[row_num].second,dollar1,10);
    char part2[6];
    j = 0;
    for(i = 16; i <= 20; i++){
        part2[j] = array[row_num][i];
        j++;
    }
    part2[5] = '\0';
    char dollar2[255];
    char buffer2[20];
    ans = convert_to_decimal(part2);
    snprintf(buffer2,10,"%d", ans);
    strcpy(dollar2, "$");
    sec = buffer2;
    strcat(dollar2, sec);
    strncpy(IM[row_num].first,dollar2,10);
    char part[6];
    j = 0;
 //   printf("##### %s\n",array[row_num]);
    for(i = 21; i <= 25; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
   // printf("**** %s\n",part);
    ans = convert_to_decimal(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    sec = buffer;
    strncpy(IM[row_num].third,sec,10);
}
void arithmetic_logic_4(int row_num, char code[]){
    char part[6];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 10; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
    int ans = convert_to_decimal(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    char dollar[255] ;
    strcpy(dollar, "$");
    char* sec = buffer;
    strcat(dollar, sec);
    strncpy(IM[row_num].second,dollar,10);
    char part1[6];
    j = 0;
    for(i = 11; i <= 15; i++){
        part1[j] = array[row_num][i];
        j++;
    }
    part1[5] = '\0';
    char dollar1[255];
    char buffer1[20];
    ans = convert_to_decimal(part1);
    snprintf(buffer1,10,"%d", ans);
    strcpy(dollar1, "$");
    sec = buffer1;
    strcat(dollar1, sec);
    strncpy(IM[row_num].first,dollar1,10);
    char part2[17];
    j = 0;
    for(i = 16; i <= 32; i++){
        part2[j] = array[row_num][i];
        j++;
    }
    part2[16] = '\0';
    char dollar2[255];
    char buffer2[20];
    ans = convert_to_decimal(part2);
    snprintf(buffer2,10,"%d", ans);
    strncpy(IM[row_num].third,buffer2,10);
}

void memory_reference(int row_num, char code[]){
    char part[6];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 10; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
    int ans = convert_to_decimal(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    char dollar[255] ;
    strcpy(dollar, "$");
    char* sec = buffer;
    strcat(dollar, sec);
    strncpy(IM[row_num].second,dollar,10);
    char part1[6];
    j = 0;
    for(i = 11; i <= 15; i++){
        part1[j] = array[row_num][i];
        j++;
    }
    part1[5] = '\0';
    char dollar1[255];
    char buffer1[20];
    ans = convert_to_decimal(part1);
    snprintf(buffer1,10,"%d", ans);
    strcpy(dollar1, "$");
    sec = buffer1;
    strcat(dollar1, sec);
    strncpy(IM[row_num].first,dollar1,10);
    char part2[17];
    j = 0;
    for(i = 16; i <= 31; i++){
        part2[j] = array[row_num][i];
        j++;
    }
    part2[16] = '\0';
    char buffer2[20];
    ans = convert_to_decimal(part2);
    snprintf(buffer2,10,"%d", ans);
    strncpy(IM[row_num].third,buffer2,10);
}
void memory_reference_2(int row_num, char code[]){
    int i = 25;
    int j = 0;
    char part1[6];
    j = 0;
    for(i = 11; i <= 15; i++){
        part1[j] = array[row_num][i];
        j++;
    }
    part1[5] = '\0';
    char dollar1[255];
    char buffer1[20];
    int ans = convert_to_decimal(part1);
    snprintf(buffer1,10,"%d", ans);
    strcpy(dollar1, "$");
    char* sec = buffer1;
    strcat(dollar1, sec);
    strncpy(IM[row_num].first,dollar1,10);
    char part2[17];
    j = 0;
    for(i = 16; i <= 31; i++){
        part2[j] = array[row_num][i];
        j++;
    }
    part2[16] = '\0';
    char buffer2[20];
    ans = convert_to_decimal(part2);
    snprintf(buffer2,10,"%d", ans);
    strncpy(IM[row_num].second,buffer2,10);
    strncpy(IM[row_num].third," ",10);
}
void jump_condition(int row_num, char code[]){
    char part[27];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 31; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[26] = '\0';
    int ans = convert_to_decimal1(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    strncpy(IM[row_num].first,buffer,10); 
}

void jalr_condition(int row_num, char code[]){
    char part[6];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 10; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
    int ans = convert_to_decimal(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    char dollar[255] ;
    strcpy(dollar, "$");
    char* sec = buffer;
    strcat(dollar, sec);
    strncpy(IM[row_num].first,dollar,10);

    char part2[6];
    j = 0;
    for(i = 16; i <= 20; i++){
        part2[j] = array[row_num][i];
        j++;
    }
    part2[5] = '\0';
    char dollar2[255];
    char buffer2[20];
    ans = convert_to_decimal(part2);
    snprintf(buffer2,10,"%d", ans);
    strcpy(dollar2, "$");
    sec = buffer2;
    strcat(dollar2, sec);
    strncpy(IM[row_num].second,dollar2,10);

}
void jr_condition(int row_num, char code[]){
    char part[6];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 10; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
    int ans = convert_to_decimal(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    char dollar[255] ;
    strcpy(dollar, "$");
    char* sec = buffer;
    strcat(dollar, sec);
    strncpy(IM[row_num].first,dollar,10);
}

void branch_equal_condition(int row_num, char code[]){
    char part[6];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 10; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
    int ans = convert_to_decimal1(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    char dollar[255] ;
    strcpy(dollar, "$");
    char* sec = buffer;
    strcat(dollar, sec);
    strncpy(IM[row_num].first,dollar,10);
    char part1[6];
    j = 0;
    for(i = 11; i <= 15; i++){
        part1[j] = array[row_num][i];
        j++;
    }
    part1[5] = '\0';
    char dollar1[255];
    char buffer1[20];
    ans = convert_to_decimal1(part1);
    snprintf(buffer1,10,"%d", ans);
    strcpy(dollar1, "$");
    sec = buffer1;
    strcat(dollar1, sec);
    strncpy(IM[row_num].second,dollar1,10);
    char part2[17];
    j = 0;
    for(i = 16; i <= 31; i++){
        part2[j] = array[row_num][i];
        j++;
    }
    part2[16] = '\0';
    char buffer2[20];
    ans = convert_to_decimal1(part2);
    snprintf(buffer2,10,"%d", ans);
    strncpy(IM[row_num].third,buffer2,10); 
}
void branch_condition(int row_num, char code[]){
    char part[6];
    int i = 25;
    int j = 0;
    for(i = 6; i <= 10; i++){
        part[j] = array[row_num][i];
        j++;
    }
    part[5] = '\0';
    int ans = convert_to_decimal1(part);
    char buffer[20];
    snprintf(buffer,10,"%d", ans);
    char dollar[255] ;
    strcpy(dollar, "$");
    char* sec = buffer;
    strcat(dollar, sec);
    strncpy(IM[row_num].first,dollar,10);
    char part1[17];
    j = 0;
    for(i = 16; i <= 31; i++){
        part1[j] = array[row_num][i];
        j++;
    }
    part1[16] = '\0';
    char buffer1[20];
    ans = convert_to_decimal1(part1);
    snprintf(buffer1,10,"%d", ans);
    strncpy(IM[row_num].second,buffer1,10);
}
//arithmetic logic functions
void add_code(int row_num, char code[]){
    arithmetic_logic(row_num, code);
    strcpy(IM[row_num].ins,"add");
}
void sub_code(int row_num, char code[]){
    arithmetic_logic(row_num, code);
    strcpy(IM[row_num].ins,"sub");
}
void and_code(int row_num, char code[]){
    arithmetic_logic(row_num, code);
    strcpy(IM[row_num].ins,"and");   
}
void or_code(int row_num, char code[]){
    arithmetic_logic(row_num, code);
    strcpy(IM[row_num].ins ,"or"); 
}
void nor_code(int row_num, char code[]){
    arithmetic_logic(row_num, code);
    strcpy(IM[row_num].ins ,"nor"); 
}
void sltu_code(int row_num, char code[]){
    arithmetic_logic(row_num, code);
    strcpy(IM[row_num].ins ,"sltu"); 
}
void sllv_code(int row_num, char code[]){
    arithmetic_logic_1(row_num, code);
    strcpy(IM[row_num].ins ,"sllv"); 
}
void mult_code(int row_num, char code[]){
    arithmetic_logic_2(row_num, code);			//mult rs, rt
    strcpy(IM[row_num].ins ,"mult"); 
}
void madd_code(int row_num, char code[]){
    arithmetic_logic_2(row_num, code);			//madd rs, rt
    strcpy(IM[row_num].ins ,"madd"); 
}
void addi_code(int row_num, char code[]){
    arithmetic_logic_4(row_num, code);			//addi rt, rs. imm(0-15)
    strcpy(IM[row_num].ins ,"addi"); 
}
void ori_code(int row_num, char code[]){
    arithmetic_logic_4(row_num, code);
    strcpy(IM[row_num].ins ,"ori"); 
}
void slti_code(int row_num, char code[]){
    arithmetic_logic_4(row_num, code);			//slti rt, rs. imm(0-15)
    strcpy(IM[row_num].ins ,"slti"); 			
}
void sll_code(int row_num, char code[]){
    arithmetic_logic_3(row_num, code);			//sll rd, rt, shamet(6-10)
    strcpy(IM[row_num].ins ,"sll"); 
}
void lui_code(int row_num, char code[]){
    memory_reference_2(row_num, code);			//lui  rt, imm 
    strcpy(IM[row_num].ins ,"lui"); 
}
//memory reference realted commands
void load_code(int row_num, char code[]){
    memory_reference(row_num, code);
    strcpy(IM[row_num].ins,"load");
}
void store_code(int row_num, char code[]){
    memory_reference(row_num, code);
    strcpy(IM[row_num].ins,"store");    
}
void lb_code(int row_num, char code[]){
    memory_reference(row_num, code);
    strcpy(IM[row_num].ins,"lb");
}
void sb_code(int row_num, char code[]){
    memory_reference(row_num, code);
    strcpy(IM[row_num].ins,"sb");    
}


//branch related commands
void jump_code(int row_num, char code[]){
	jump_condition(row_num,code);
	strcpy(IM[row_num].ins,"j");
}
void jal_code(int row_num, char code[]){
    jump_condition(row_num,code);
    strcpy(IM[row_num].ins,"jal");
}
void jalr_code(int row_num, char code[]){
    jalr_condition(row_num,code);
    strcpy(IM[row_num].ins,"jalr");
}
void jr_code(int row_num, char code[]){
    jr_condition(row_num,code);
    strcpy(IM[row_num].ins,"jr");
}
void brancheq_code(int row_num, char code[]){
	branch_equal_condition(row_num,code);
	strcpy(IM[row_num].ins,"beq");
}
void bgez_code(int row_num, char code[]){
	branch_condition(row_num,code);
	strcpy(IM[row_num].ins,"bgez");
}
void bgtz_code(int row_num, char code[]){
	branch_condition(row_num,code);
	strcpy(IM[row_num].ins,"bgtz");
}
void blez_code(int row_num, char code[]){
	branch_condition(row_num,code);
	strcpy(IM[row_num].ins,"blez");
}
void bltz_code(int row_num, char code[]){
	branch_condition(row_num,code);
	strcpy(IM[row_num].ins,"bltz");
}

void add_to_IM(int row_num){
 //   printf("@@@@@@@ %s\n", array[row_num]);
    char opcode[7];
    char func[7];
    char branch[6];
    int i = 31;
    int j = 0;
    for(i = 0; i <= 5; i++){
        opcode[j] =  array[row_num][i];
        j++;
    }
    j = 0;
    for(i = 26; i <= 31; i++){
        func[j] =  array[row_num][i];
        j++;
    }
    j = 0;
    for(i = 11; i <= 15; i++){
        branch[j] =  array[row_num][i];
        j++;
    }
    branch[5] = '\0';
    int ansa = convert_to_decimal(opcode);
    int ansb = convert_to_decimal(func);
    int ansc = convert_to_decimal(branch);
    if(ansa == 0){
        if(ansb == 32){
            char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
            add_code(row_num, code);
        }
        else if(ansb == 34){
            char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }    
            sub_code(row_num, array[row_num]);
        }
        else if(ansb == 36){
            char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
            and_code(row_num, array[row_num]);
        }
        else if(ansb == 37){
        char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
            or_code(row_num, array[row_num]);
        }


        else if(ansb == 24){
        	char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
        	mult_code(row_num, code);	
        }
        else if(ansb == 39)
        {
        	char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
        	nor_code(row_num, code);	
        }
        else if(ansb == 0)
        {
        	char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
        	sll_code(row_num, code);	
        }
        else if(ansb == 4)
        {
        	char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
        	sllv_code(row_num, code);	
        }
        else if(ansb == 43)
        {
        	char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
        	sltu_code(row_num, code);	
        }


        else if(ansb == 8)
        {
            char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
            jr_code(row_num, code);   
        }
        else if(ansb == 9)
        {
            char code[33];
            for(i = 0; i < 32; i++){
                code[i] = array[row_num][i];
            }
            jalr_code(row_num, code);   
        }

    }
    else if(ansa == 35){
        char code[33];
        for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        load_code(row_num, code);
    }
    else if(ansa == 43){
        char code[33];
        for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        store_code(row_num, code);
    }
    else if(ansa == 2){
        char code[33];
        for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }    
        jump_code(row_num, code);
    }



    else if(ansa == 3){
        char code[33];
        for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }    
        jal_code(row_num, code);
    }



    else if(ansa == 4){
        char code[33];
        for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        brancheq_code(row_num, code);
    }


    
    else if(ansa == 8)
    {
    	char code[33];
    	for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        addi_code(row_num, code);
    }
    else if(ansa == 10)
    {
    	char code[33];
    	for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        slti_code(row_num, code);
    }
    else if(ansa == 13)
    {
    	char code[33];
    	for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        ori_code(row_num, code);
    }
    else if(ansa == 15)
    {
    	char code[33];
    	for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        lui_code(row_num, code);
    }
    else if(ansa == 28 && ansb == 0)
    {
    	char code[33];
    	for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        madd_code(row_num, code);
    }
    else if(ansa == 1)
    {
    	if(ansc == 0){
    		char code[33];
    		for(i = 0; i < 32; i++){
            	code[i] = array[row_num][i];
        	}
        	bltz_code(row_num, code);
    	}
    	else if(ansc == 1){
 //   		printf("YES\n");
    		char code[33];
    		for(i = 0; i < 32; i++){
            	code[i] = array[row_num][i];
        	}
        	bgez_code(row_num, code);
    	}
    }
    else if(ansa == 7){
    	if(ansc == 0){
    		char code[33];
    		for(i = 0; i < 32; i++){
            	code[i] = array[row_num][i];
        	}
        	bgtz_code(row_num, code);
    	}
    }
    else if(ansa == 6){
    	if(ansc == 0){
    		char code[33];
    		for(i = 0; i < 32; i++){
            	code[i] = array[row_num][i];
        	}
        	blez_code(row_num, code);
    	}
    }
    else if(ansa == 20){
    	char code[33];
    	for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        lb_code(row_num, code);
    }
    else if(ansa == 28){
    	char code[33];
    	for(i = 0; i < 32; i++){
            code[i] = array[row_num][i];
        }
        sb_code(row_num, code);
    }
}

void create_IM(){
    int i = 0;
    for(i = 0; i < row; i++){
        add_to_IM(i);
    }
    //for (i = 0; i < row; i++)
    //{
        /* code */
 //       printf("%s " , IM[i].ins);
   //     printf("%s " , IM[i].first);
     //   printf("%s " , IM[i].second);
       // printf("%s\n\n", IM[i].third);
    //}
}
int check_data_forwading(){
    int a = 0;
    //path1
    if(exmemwrite.RW == 1 && strcmp(exmemwrite.Rd,idexwrite.Rn) == 0 && strcmp(exmemwrite.Rd , " ") != 0){
        fwdA = 1;
//        printf("DATA FORWADING fwdA = %d fwdB = %d fwdC = %d\n", fwdA, fwdB, fwdC);
    }
    if(exmemwrite.RW == 1 && strcmp(exmemwrite.Rd,idexwrite.Rm) == 0 && strcmp(exmemwrite.Rd , " ") != 0){
        fwdB = 1;
//        printf("DATA FORWADING fwdA = %d fwdB = %d fwdC = %d\n", fwdA, fwdB, fwdC);
    }
    //path2
    if(memwbwrite.RW == 1 && strcmp(exmemwrite.Rd,idexwrite.Rn) != 0 && strcmp(memwbwrite.Rd,idexwrite.Rn) == 0 && strcmp(idexwrite.Rn , " ") != 0){
        fwdA = 2;
//        printf("DATA FORWADING fwdA = %d fwdB = %d fwdC = %d\n", fwdA, fwdB, fwdC);
    }
    if(memwbwrite.RW == 1 && strcmp(exmemwrite.Rd,idexwrite.Rm) != 0 && strcmp(memwbwrite.Rd,idexwrite.Rm) == 0 && strcmp(idexwrite.Rm , " ") != 0){
        fwdB = 2;
 //       printf("DATA FORWADING fwdA = %d fwdB = %d fwdC = %d\n", fwdA, fwdB, fwdC);
    }
    if(memwbwrite.RW == 1 && strcmp(memwbwrite.Rd,idexwrite.Rd) == 0 && strcmp(idexwrite.Rd , " ") != 0){
        fwdD = 1;
 //       printf("DATA FORWADING fwdA = %d fwdB = %d fwdC = %d fwdD = %d\n", fwdA, fwdB, fwdC,fwdD);
    }
    //path3
    if(memwbwrite.RW == 1 && memwbwrite.MR == 1 && strcmp(memwbwrite.Rd,exmemwrite.Rd) == 0 && strcmp(exmemwrite.Rd , " ") != 0){
        fwdC = 1;
//        printf("DATA FORWADING fwdA = %d fwdB = %d fwdC = %d\n", fwdA, fwdB, fwdC);
    }
    //stalling with data forwading
    if(idexwrite.MR == 1 && strcmp(ifidwrite.Rn,idexwrite.Rd) == 0 && strcmp(idexwrite.Rd , " ") != 0){
//        printf("STALLING WITH DATA FORWADING\n");
        a = 1;
    }
    if(idexwrite.MR == 1 && strcmp(ifidwrite.Rm,idexwrite.Rd) == 0 && strcmp(idexwrite.Rd , " ") != 0){
//        printf("STALLING WITH DATA FORWADING\n");
        a = 1;
    }
    return a;
}
void transfer_registers(){
 //   printf(" IF/ID Rn = %s IF/ID Rm = %s IF/ID Rd = %s ID/EX Rn = %s ID/EX Rm = %s ID/EX Rd = %s EX/MEM Rd = %s MEM/WB Rd = %s ID/EX MR = %d EX/MEM RW = %d MEM/WB RW = %d, \n", ifidwrite.Rn, ifidwrite.Rm, ifidwrite.Rd, idexwrite.Rn, idexwrite.Rm, idexwrite.Rd, exmemwrite.Rd, memwbwrite.Rd, idexwrite.MR, exmemwrite.RW, memwbwrite.RW);
    strncpy(ifidread.Rn, ifidwrite.Rn,10);
    strncpy(ifidread.Rm, ifidwrite.Rm,10);
    strncpy(ifidread.Rd, ifidwrite.Rd,10);
 //   printf(" ******* (%s)\n", ifidread.Rm);
    strncpy(idexread.Rn, idexwrite.Rn,10);
    strncpy(idexread.Rm, idexwrite.Rm,10);
    strncpy(idexread.Rd, idexwrite.Rd,10);
    idexread.RW = idexwrite.RW;
    idexread.MR = idexwrite.MR;
    strncpy(exmemread.Rd, exmemwrite.Rd,10);
    exmemread.RW = exmemwrite.RW;
    exmemread.MW = exmemwrite.MW;
    strncpy(memwbread.Rd, memwbwrite.Rd,10);
    memwbread.RW = memwbwrite.RW;
    ifidread.rn = ifidwrite.rn;
    ifidread.rm = ifidwrite.rm;
    ifidread.rd = ifidwrite.rd;
    idexread.rn = idexwrite.rn;
    idexread.rm = idexwrite.rm;
    idexread.rd = idexwrite.rd;
    exmemread.rd = exmemwrite.rd;
    exmemread.hilo = exmemwrite.hilo;
    exmemread.rn = exmemwrite.rn;
    memwbread.rd = memwbwrite.rd;
    memwbread.hilo = memwbwrite.hilo;
    memwbread.rn = memwbwrite.rn;
    memwbread.MR = memwbwrite.MR;
    pc_read = pc_write;
    ifidread.pc = ifidwrite.pc;
    idexread.pc = idexwrite.pc;
    exmemread.pc = exmemwrite.pc;

    strncpy(ifidwrite.Rn, " ",10);
    strncpy(ifidwrite.Rm, " ",10);
    strncpy(ifidwrite.Rd, " ",10);
    strncpy(idexwrite.Rn, " ",10);
    strncpy(idexwrite.Rm, " ",10);
    strncpy(idexwrite.Rd, " ",10);
   /* pc_write = 0;
    ifidwrite.pc = 0;
    idexwrite.pc = 0;
    exmemwrite.pc = 0;
    idexwrite.RW = 0;
    ifidwrite.rn = 0;
    ifidwrite.rm = 0;
    ifidwrite.rd = 0;
    idexwrite.rn = 0;
    idexwrite.rm = 0;
    idexwrite.rd = 0;
    exmemwrite.rd = 0;
    exmemwrite.hilo = 0;
    exmemwrite.rn = 0;
    memwbwrite.rd = 0;
    memwbwrite.hilo = 0;
    memwbwrite.rn = 0;*/
    idexwrite.MR = 0;
    strncpy(exmemwrite.Rd, " ",10);
    exmemwrite.RW = 0;
    exmemwrite.MW = 0;
    strncpy(memwbwrite.Rd, " ", 10);
    memwbwrite.RW = 0;  
    memwbwrite.MR = 0;  
}
void generate_output(){
	int i = 0;
	int ans = instructions_executed - (2*no_branch) - stalls;
	fprintf(out,"Instructions,%d\n",ans);
	fprintf(out,"Cycles,%d\n",cycles);
	float ipc = row*1.0/cycles;
	fprintf(out,"IPC,%0.4f\n",ipc);
	float time = cycles/2.0;
    idle_cycle = cycles - ans;
	float idle_time = idle_cycle/2.0;
	fprintf(out,"Time (ns),%0.4f\n",time);
	//float time_idle = (4 + stalls )/2.0;
	fprintf(out,"Idle time (ns),%0.4f\n",idle_time);
	float percent = (idle_time/time)*100.0;
	fprintf(out,"Idle time (%%),%0.4f%%\n",percent);
	fprintf(out,"Cache Summary\n");
	fprintf(out,"Cache L1-I\n");
	int ans1 = ins_access - (2*no_branch);
	ins_access = ans1 - stalls;
	fprintf(out,"num cache accesses,%d\n",ins_access);
	fprintf(out,"Cache L1-D\n");
	fprintf(out,"num cache accesses,%d\n",data_access);
}

void print_memory_file(int start, int offset_value){
	int k = 0;
    int j = 0;
    for(k = start;k<start + offset_value;k++){
        double adr = k + address_start;
    	double quotient = adr;
    	char a[9];
    	char result[5];
    	int temp;
    	int i = 7;
    	result[0] = '0';
    	result[1] = 'x';
    	while(quotient!=0.0){
        	 temp = fmod(quotient, 16);

      //To convert integer into character
      		if( temp < 10)
           		temp =temp + 48;
      		else
         		temp = temp + 55;

      		a[i]= temp;
      		i--;
      		quotient = quotient / 16.0;
      		quotient = floor(quotient);
  		}
        printf("0x%s: ", a);
        int r3 = data_memory[k][0]+data_memory[k][1]*2+data_memory[k][2]*4+data_memory[k][3]*8;
        int r2 = data_memory[k][4]+data_memory[k][5]*2+data_memory[k][6]*4+data_memory[k][7]*8;
        if( r2 < 10)
           		r2 =r2 + 48;
      		else
         		r2 = r2 + 55;
        if( r3 < 10)
           		r3 =r3 + 48;
      		else
         		r3 = r3 + 55;
         result[2] = r2;
         result[3] = r3;
         printf("%s\n", result);
    	printf("\n");
    }
}
void print_register_file(){
	int k = 5;
    int j = 0;
    printf("%d  ----> ", k);
    for(k = 0;k<32;k++){
        printf("$%d: ", k);
        for( j = 31; j >= 0 ; j--){
        	printf("%d", register_file[k][j]);
        }
    	printf("\n");
    }
    printf("hi  ----> ");
    for( j = 31; j >= 0 ; j--){
        printf("%d", HI[j]);
    }
    printf("\n");
    printf("lo  ----> ");
    for( j = 31; j >= 0 ; j--){
        printf("%d", LU[j]);
    }
    printf("\n");
    printf("pc  ----> ");
    printf("%d", pc_final);
    printf("\n");
}
int main_program(){

    int j;
    for(j=0;j<5;j++){
        strncpy(to_print[j + 6], " ", 100);
    }
    int start = 0;
    pthread_t threads[6];
    if(ins_number[0] > row + 3){
    	cycles = cycles - 1;
        printf("End of Program\n");
        //print_register_file();
        //printf("\n\n");
        //print_memory_file();
        generate_output();
        exit(0);
    }
    if(bt == 2 && ins_number[0] > row ){
    	cycles = cycles + 2;
    	printf("End of Program\n");
        //print_register_file();
        //printf("\n\n");
        //print_memory_file();
        generate_output();
        exit(0);
    }
    thread_info my_struct[5];
    thread_info2 thread_status;
    int i = 0;
    for(i = 0; i < 5; i++ ){
        if( ins_number[i] >= 0 && ins_number[i] < row){     
            if(i == 0){
            	ins_access++;
            	instructions_executed++;
            	pc_final = ins_number[0]*4;
            }
            my_struct[i].index = i;  
            strncpy(my_struct[i].ins,IM[ins_number[i]].ins,10);
            strncpy(my_struct[i].first,IM[ins_number[i]].first,10);
            strncpy(my_struct[i].second,IM[ins_number[i]].second,10);
            strncpy(my_struct[i].third,IM[ins_number[i]].third,10);
            pthread_create(&threads[i],NULL,evaluate,&my_struct[i]);
            thread_status.thr[i] = 1;
        }
        else
            thread_status.thr[i] = 0;   
    }
    pthread_create(&threads[5],NULL,set_thread_status,&thread_status);
    for(i = 0;i<5;i++){
        if( ins_number[i] < row && ins_number[i] >= 0){    
            pthread_join(threads[i],NULL);
        }
    }
    pthread_join(threads[5],NULL);
    pfwdA = fwdA;
    pfwdB = fwdB;
    pfwdC = fwdC;
    fwdA = 0;
    fwdB = 0;
    fwdC = 0;
    int a = check_data_forwading();
    do_data_forwading();
    transfer_registers();
    int x = 0;
    for(j=6;j<11;j++){
        printf("%s\n",to_print[j]);
        if(strcmp(to_print[j], " ") == 0 && ins_number[0] <= row)
        	x = 1;
    }
    idle_cycle += x;
    if( strcmp(to_print[7], " ") == 0 ){
        to_print[1][0] = '0';
    }
    else{
        to_print[1][0] = '1';
    }
    if(memwbread.RW == 1){
        to_print[1][1] = '1';
    }
    else{
        to_print[1][1] = '0';
    }
    if(memwbread.MR == 1){
        to_print[2][0] = '1';
    }
    else{
        to_print[2][0] = '0';
    }
    if(exmemread.MW == 1){
        to_print[2][1] = '1';
    }
    else{
        to_print[2][1] = '0';
    }
    if(bt == 4){
        to_print[3][0] = '1';
    }
    else{
        to_print[3][0] = '0';
    }
    if(strcmp(to_print[8], " ") == 0){
        to_print[4][0] = '3';
    }
    else{
        if(pfwdA == 0){
            to_print[4][0] = '0';
        }
        else if(pfwdA == 2){
            to_print[4][0] = '1';
        }
        else if(pfwdA == 1){
            to_print[4][0] = '2';
        }
    }
    if(strcmp(to_print[8], " ") == 0){
        to_print[5][0] = '3';
    }
    else{
        if(pfwdB == 0){
            to_print[5][0] = '0';
        }
        else if(pfwdB == 2){
            to_print[5][0] = '1';
        }
        else if(pfwdB == 1){
            to_print[5][0] = '2';
        }
    }
    draw_svg();
    printf("End \n");
    //print_register_file();
    //print_memory_file();

    return a;

}

void execute_program(){
    draw_svg();
    int check = 0;
    int flag = 0;
    printf("Enter step to go to next time step...\n");
    printf("Enter regdump or memorydump to see register file dump or memory dump...\n");
    printf("%s\n", );
    int count = -1;
    int i = 0;
    int stall = 0;
    for(i = 0;i<5;i++){
        ins_number[i] = -1-i;
    }
    while(1){
        char a[100];
        int temp;
        scanf("%s",a);
 //       printf("^^^^^^^^^^^^ %s\n",a);
        if(strcmp(a,"regdump") == 0){
	        		print_register_file();
	    }
    	else if(strcmp(a, "memdump") == 0){
    		char start[11];
    		scanf("%s",start);
    		int i = 0;
    		char value[9];
    		for( i = 2;i<11;i++){
    			value[i-2] = start[i];
    		}
    		printf("^^^^^^^^^^^^%s\n",value);
    		int k = 0;
    		double ans = 0;
    		for(k = 0;k < 8;k++){
    			if( value[k] == '0') { ans = ans + pow(16, 7 - k) * (0) ; }
    			if( value[k] == '1') { ans = ans + pow(16, 7 - k) * (1) ; }
    			if( value[k] == '2') { ans = ans + pow(16, 7 - k) * (2) ; }
    			if( value[k] == '3') { ans = ans + pow(16, 7 - k) * (3) ; }
    			if( value[k] == '4') { ans = ans + pow(16, 7 - k) * (4) ; }
    			if( value[k] == '5') { ans = ans + pow(16, 7 - k) * (5) ; }
    			if( value[k] == '6') { ans = ans + pow(16, 7 - k) * (6) ; }
    			if( value[k] == '7') { ans = ans + pow(16, 7 - k) * (7) ; }
    			if( value[k] == '8') { ans = ans + pow(16, 7 - k) * (8) ; }
    			if( value[k] == '9') { ans = ans + pow(16, 7 - k) * (9) ; }
    			if( value[k] == 'a' || value[k] == 'A') { ans = ans + pow(16, 7 - k) * (10) ; }
    			if( value[k] == 'b' || value[k] == 'B') { ans = ans + pow(16, 7 - k) * (11) ; }
    			if( value[k] == 'c' || value[k] == 'C') { ans = ans + pow(16, 7 - k) * (12) ; }
    			if( value[k] == 'd' || value[k] == 'D') { ans = ans + pow(16, 7 - k) * (13) ; }
    			if( value[k] == 'e' || value[k] == 'E') { ans = ans + pow(16, 7 - k) * (14) ; }
    			if( value[k] == 'f' || value[k] == 'F') { ans = ans + pow(16, 7 - k) * (15) ; }
    				
    		}
    		printf("######################%lf\n",ans);
    		int offset_value = 0;
    		scanf("%d",&offset_value);
    		printf("######################%d\n",offset_value);
    		ans = ans - address_start;
    		print_memory_file(ans, offset_value);
    	}
        else if(strcmp(a,"run") == 0){
        	while(1){
	        	count++;
	        	stall++;
	        	if(count <= 5){
	            	flag = 1;
	        	}
	        	count = count % 5;
	        	stall = stall % 5;
	        	//branch not taken
		        if(bt == 0 && az == 0){
		        	for(i = 0;i<5;i++){
		            	ins_number[i] = ins_number[i] + 1;
		        	}
		    	}
		    	//branch taken
		    	else if (bt != 0){
                    az = 1;
		    		if(bt == 4){
		    			//printf("################### %d\n", offset);
		    			ins_number[0] = ins_number[2] + offset + 1;
		    			ins_number[1] = -10;
		    			ins_number[2] = -10;
		    			ins_number[3] = -10;
		    			ins_number[4] = ins_number[4] + 1;
                        cycles--;
		    		}
		    		else{
		    			int temp0 = ins_number[0];
		    			int temp1 = ins_number[1];
		    			int temp2 = ins_number[2];
		    			int temp3 = ins_number[3];
		    			int temp4 = ins_number[4];
						ins_number[0] = temp0 + 1;
		    			ins_number[1] = temp0;
		    			ins_number[2] = temp1;
		    			ins_number[3] = temp2;
		    			ins_number[4] = temp3;
		    		}
		    		bt--;
		    	}
                else if(bt == 0 && az == 1)
                {
                        int temp0 = ins_number[0];
                        int temp1 = ins_number[1];
                        int temp2 = ins_number[2];
                        int temp3 = ins_number[3];
                        int temp4 = ins_number[4];
                        ins_number[0] = temp0 + 1;
                        ins_number[1] = temp0;
                        ins_number[2] = temp1;
                        ins_number[3] = temp2;
                        ins_number[4] = temp3;
                        az = 0;
                }
		        if(flag == 1 && check == 1){
		            if(stall == 0){
		                temp = ins_number[4] - 1;
		                ins_number[4] = -10;
		            }
		            else{
		                temp = ins_number[stall];
		                ins_number[stall - 1] = -10;
		            }
		        }       
		        cycles++;
		        int b = main_program();
		        for(i = 0;i<5;i++){
		        	if(ins_number[i] >= 0 && ins_number[i] < row){
		        		int w = 2;
		        	}
		        	else{
		        		time_idle++;
		        		break;
		        	}
		        }
		        if(flag == 1 && check == 1){
		            if(stall == 0){
		                ins_number[4] = temp;
		                flag = 0;
		                check = 0;
		            }
		            else{
		                ins_number[stall - 1] = temp;
		            }
		        }
		        if(b == 1){
		        	stall = 1;
		            stalls = stalls + 1;
		                ins_number[0] = ins_number[0] - 1;
		            ins_number[stall] = -10;
		            check = 1;
		        }
		    }	
        }
        else if (strcmp(a,"step") == 0){
	        count++;
	        stall++;
	        if(count <= 5){
	            flag = 1;
	        }
	        count = count % 5;
	        stall = stall % 5;
	        //branch not taken
	        if(bt == 0 && az == 0){
	        	for(i = 0;i<5;i++){
	            	ins_number[i] = ins_number[i] + 1;
	        	}
	    	}
	    	//branch taken
	    	else if (bt != 0){
	    		az = 1;
	    		if(bt == 4){
	    			//printf("################### %d\n", offset);
	    			ins_number[0] = ins_number[2] + offset + 1;
	    			ins_number[1] = -10;
	    			ins_number[2] = -10;
	    			ins_number[3] = -10;
	    			ins_number[4] = ins_number[4] + 1;
                    cycles--;
	    		}
	    		else{
	    			int temp0 = ins_number[0];
	    			int temp1 = ins_number[1];
	    			int temp2 = ins_number[2];
	    			int temp3 = ins_number[3];
	    			int temp4 = ins_number[4];
					ins_number[0] = temp0 + 1;
	    			ins_number[1] = temp0;
	    			ins_number[2] = temp1;
	    			ins_number[3] = temp2;
	    			ins_number[4] = temp3;
	    		}
	    		bt--;
	    	}
	    	else if(bt == 0 && az == 1){
	    			int temp0 = ins_number[0];
	    			int temp1 = ins_number[1];
	    			int temp2 = ins_number[2];
	    			int temp3 = ins_number[3];
	    			int temp4 = ins_number[4];
					ins_number[0] = temp0 + 1;
	    			ins_number[1] = temp0;
	    			ins_number[2] = temp1;
	    			ins_number[3] = temp2;
	    			ins_number[4] = temp3;
	    			az = 0;
	    	}
	        if(flag == 1 && check == 1){
	            if(stall == 0){
	                temp = ins_number[4] - 1;
	                ins_number[4] = -10;
	            }
	            else{
	                temp = ins_number[stall];
	                ins_number[stall - 1] = -10;
	            }
	        }  
	        cycles++;     
	        int b = main_program();
	        for(i = 0;i<5;i++){
		        	if(ins_number[i] >= 0 && ins_number[i] < row){
		        		int w = 2;
		        	}
		        	else{
		        		time_idle++;
		        		break;
		        	}
		        }
	        if(flag == 1 && check == 1){
	            if(stall == 0){
	                ins_number[4] = temp;
	                flag = 0;
	                check = 0;
	            }
	            else{
	                ins_number[stall - 1] = temp;
	            }
	        }
	        if(b == 1){
	        	stall = 1;
	            stalls = stalls + 1;
	                ins_number[0] = ins_number[0] - 1;
	            ins_number[stall] = -10;
	            check = 1;
	        }
	    }
	    else {
	    	printf("Unrecognized Character\n");
	    }
    }
}
