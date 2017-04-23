#include<pthread.h>
extern FILE* yyin;
extern FILE* out;
extern FILE* svgout;
extern char* temp;
extern int yylineno; /* from lexer */
extern int max;
extern int row;

typedef struct ins_mem
{
    char ins[10];
    char first[10];
    char second[10];
    char third[10];
}ins_mem;
typedef struct IF_ID_read
{
    double pc;
    struct ins_mem instruction;
    char Rn[10];
    double rn;
    char Rm[10];
    double rm;
    char Rd[10];
    double rd;

}IF_ID_read;
typedef struct ID_EX_read
{
    double pc;
    int RW;
    int MR;
    char Rn[10];
    char Rm[10];
    char Rd[10];
    double rn;
    double rm;
    double rd;
}ID_EX_read;
typedef struct EX_MEM_read
{
    double hilo;
    double pc;
    int RW;
    int MW;
    char Rd[10];
    double rd;
    double rn;
}EX_MEM_read;
typedef struct MEM_WB_read
{
    double hilo;
    int MR;
    int RW;
    char Rd[10];
    double rd;
    double rn;
}MEM_WB_read;

typedef struct IF_ID_write
{
    double pc;
    struct ins_mem instruction;
    char Rn[10];
    char Rm[10];
    char Rd[10];
    double rn;
    double rm;
    double rd;
}IF_ID_write;
typedef struct ID_EX_write
{
    double pc;
    int RW;
    int MR;
    char Rn[10];
    char Rm[10];
    char Rd[10];
    double rn;
    double rm;
    double rd;
}ID_EX_write;
typedef struct EX_MEM_write
{
    double hilo;
    double pc;
    int RW;
    int MW;
    char Rd[10];
    double rd;
    double rn;
}EX_MEM_write;
typedef struct MEM_WB_write
{
    double hilo;
    int RW;
    int MR;
    char Rd[10];
    double rd;
    double rn;
}MEM_WB_write;

typedef struct thread_info{
    int index;
    char ins[10];
    char first[10];
    char second[10];
    char third[10];
}thread_info;
typedef struct thread_info2{
    int thr[5];
}thread_info2;

void draw_svg();
void add_function(int index, char a[], char b[], char c[], char d[]);
void load_function(int index, char a[], char b[], char c[], char d[]);
void store_function(int index, char a[], char b[], char c[], char d[]);
void* evaluate(void * args);
int convert_to_decimal(char num[]);
void add_to_array(char* str);
void arithmetic_logic(int row_num, char code[]);
void memory_reference(int row_num, char code[]);
void add_code(int row_num, char code[]);
void sub_code(int row_num, char code[]);
void and_code(int row_num, char code[]);
void or_code(int row_num, char code[]);
void load_code(int row_num, char code[]);
void store_code(int row_num, char code[]);
void jump_code(int row_num, char code[]);
void brancheq_code(int row_num, char code[]);
void store_to_IM(int row_num);
void create_IM();
int check_data_forwading();
void transfer_registers();
int main_program();
void execute_program();