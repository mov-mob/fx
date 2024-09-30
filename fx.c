//fx.c version 1.0
// Calculadora que admite fórmulas matemáticas:
//__next={coments, debug, no pointer, idiom=es}
//1 parsec  3.086 × 10^16 metros
#include <stdio.h>

//#include <conio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libintl.h>
#include <locale.h>

/* Tipos definidos: */
typedef char MaxCalcCad[256];
typedef char str80[81];
typedef long double Ldoble;
typedef int bool;

int i,p,ruptura;
MaxCalcCad formula="";
Ldoble resultado=0L;
bool error;
bool _fact=0;

/* Prototipos de las funciones: */
char *ch2ptr(char ch);
char *copy(char *cadena, int index, int count);
Ldoble fact(Ldoble f);
Ldoble calcular_formula(int *p, MaxCalcCad cad, bool *error);
bool eval(MaxCalcCad formula, Ldoble *valor, int *ruptura);
char sigp(MaxCalcCad formula,int *p);
Ldoble expr(MaxCalcCad formula,int *p,char *c);
Ldoble exprsimp(MaxCalcCad formula,char *c,int *p);
Ldoble termino(MaxCalcCad formula,int *p,char *c);
Ldoble fact_s(MaxCalcCad formula,char *c,int *p);
Ldoble fct(MaxCalcCad formula,char *c,int *p);
Ldoble proc_as_num(MaxCalcCad formula,char *c,int *p);
Ldoble proc_as_new_exp(MaxCalcCad formula,char *c, int *p);
Ldoble proc_like_stdfunc(MaxCalcCad formula,char *c,int *p);


int main(void){
	/*
	idioma esEs
	*/
	setlocale(LC_ALL, "es_Es");
	int cont,opcion, SALIR=0;
	Ldoble a,b,c,d,x;
	resultado=0L;
	//textcolor(1);
	//textbackground(0);
	//clrscr();
	printf("\n");
	error=0;
	cont=0;SALIR=0;
	fgets(formula,80,stdin);
	formula[strcspn(formula, "\r\n")] = 0;
	
	//printf("El resultado de la formula=%s es:\n",formula);
	resultado=calcular_formula(&p,formula,&error);
	if(error){
		//gotoxy(10,5);
		printf("\n");
		//textcolor(3);
		printf("\nError!\n");
		printf("%s\n",formula);
		for(i=1;i<=p;++i)
			printf(" ");
			printf("^\n\r");
			//textcolor(1);
		}
		else
			printf("\n%10.19Lf\n",resultado);

	return 0;
}

char *ch2ptr(char ch){
	char *cp=" ";
	cp[0]=ch;
	return (cp);
}

char *copy(char *cadena, int index, int count)
{
	char *cp;
	index;
	if((cp=strdup(cadena+index))==NULL){
		//gotoxy(10,25);
		printf("\n\n");
		printf("Error en asignación de memoria. _copy()");
		exit(1);
	}
	cp[count]='\0';
	return (cp);
}

Ldoble calcular_formula(int *p, MaxCalcCad cad, bool *error){
	static Ldoble r;
	ruptura=0;
	//static int i,ruptura;
	//static char c;
	*error=eval(cad, &r, p);
	return (r);
}

bool eval(MaxCalcCad formula, Ldoble *valor, int *ruptura){
	static int p,i;
	register int l;
	static char c;
	MaxCalcCad aux="";
	l=strlen(formula);
	for(i=0; i<l;i++)
		formula[i]=toupper(formula[i]);
	if(formula[0]=='.'){
		strcat(aux,ch2ptr('0'));
		strcat(aux,formula);
		strcpy(formula,aux);
	}
	l=strlen(formula);
	if(formula[0]=='+'){
		for(i=0;i<l;i++)
			formula[i]=formula[i+1];
		formula[l]='\0';
	}
	p=-1;
	c=sigp(formula,&p);
	*valor = expr(formula,&p,&c);
	if(c=='\r')
		error=0;
	else
		error=1;
	*ruptura=p;
	return (error);
}
char sigp(MaxCalcCad formula,int *p){
	char c;
	do{
		++*p;
		if(*p<strlen(formula))
			c=formula[*p];
		else
			c='\r';
	}while (c==' ');
	return c;
}
Ldoble expr(MaxCalcCad formula,int *p,char *c){
	Ldoble e;
	char operador;

	e=exprsimp(formula,c,p);
	while ((*c=='+')||(*c=='-')){
		operador=*c;
		*c=sigp(formula,p);
		switch(operador){
			case '+':e=e+exprsimp(formula,c,p); break;
			case '-':e=e-exprsimp(formula,c,p); break;
		}
	}
	return (e);
}
Ldoble exprsimp(MaxCalcCad formula,char *c,int *p){
	Ldoble s;
	char operador;
	s=termino(formula,p,c);
	while((*c=='*')||(*c=='/')){
		operador=*c;
		*c=sigp(formula,p);
		switch(operador){
			case '*': s=s*termino(formula,p,c);break;
			case '/': s=s/termino(formula,p,c);break;
		}
	}
	return (s);
}
Ldoble termino(MaxCalcCad formula,int *p,char *c){
	Ldoble t;
	unsigned long int nt;
	t=fact_s(formula,c,p);
	while(*c=='^'){
		*c=sigp(formula,p);
		t=powl(t,fact_s(formula,c,p));
	}
	return (t);
}
Ldoble fact_s(MaxCalcCad formula,char *c,int *p){
	Ldoble f;
	if(*c=='-'){
		*c=sigp(formula,p);
		if(_fact){
			--*p;
			ruptura=*p;
			return((Ldoble)-1);
		}
		//*c=sigp(formula,p);
		return(-fct(formula,c,p));
	}
	return(fct(formula,c,p));
}

Ldoble fct(MaxCalcCad formula,char *c,int *p){
	//char fn[20];
	//int l,inicio;
	Ldoble f;
	if ((isdigit(*c))||(*c=='.'))
		f=proc_as_num(formula,c,p);
	else if (*c=='(')
		f=proc_as_new_exp(formula,c,p);
	else if(*c=='-')
		f=fact_s(formula,c,p);
	else f=proc_like_stdfunc(formula,c,p);
	return (f);
}
Ldoble proc_as_num(MaxCalcCad formula,char *c,int *p){
	int l,inicio;
	bool _error=0;
	Ldoble f;
	char ch;

	inicio=*p;
	do{
		if((*c=='.')&&_fact)
			return((Ldoble)-1);
		*c=sigp(formula,p);
	}while((isdigit(*c)));//||(*c=='.'));
	if(*c=='.'){
		if(_fact) _error=1;
		l=*p;
		do{
			*c=sigp(formula,p);
		}while((isdigit(*c)));//||(*c=='.'));
		f=atof(copy(formula,l,*p-l));
		if((f!=(Ldoble)0)&&_error){
			*p=l;
			return(f);
		}
	}
	if(*c=='E'){
		//*c=sigp(formula,p);
		ch=sigp(formula,p);
		l=*p;
		do{
			*c=sigp(formula,p);
		}while(isdigit(*c));//||*c=='.');
		if((ch=='-')&&(++l==*p)&&(*c=='\r')){
			ruptura=--*p;
			*c=ch;
			//return(f);
		}
	}
	if(*c=='.'){
		ruptura=*p;
		return(f);
	}

	f=(atof(copy(formula,inicio,*p-inicio)));
	if(*c=='!'){
		f=fact(f);
		*c=sigp(formula,p);
	}
	else if(*c=='²'){
		f=f*f;
		*c=sigp(formula,p);
	}

	return(f);
}
Ldoble proc_as_new_exp(MaxCalcCad formula,char *c, int *p){
	Ldoble f;
	*c=sigp(formula,p);
	f=expr(formula,p,c);
	if(*c==')'){
		*c=sigp(formula,p);
		if(*c=='²'){
			f*=f;
			*c=sigp(formula,p);
		}
		else if(*c=='!'){
			f=fact(f);
			*c=sigp(formula,p);
		}
	}
	else
		ruptura = *p;
	return (f);
}
Ldoble fact(Ldoble f){
	//int n;
	//n=(int)f;
	if(f>0)
		return(f*fact(f-1));
	return((Ldoble)1);
}
Ldoble proc_like_stdfunc(MaxCalcCad formula,char *c,int *p){
	Ldoble f;
	if(!strcmp(copy(formula,*p,3),"ABS")){
		*p=*p+2;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=fabsl(f);
		}
	else if(!strcmp(copy(formula,*p,4),"SQRT")){
		*p+=3;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=sqrt(f);
	}
	else if(!strcmp(copy(formula,*p,1),"¿")){
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=sqrt(f);
	}
	else if(!strcmp(copy(formula,*p,3),"SQR")){
		*p+=2;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f*=f;
	}
	else if(!strcmp(copy(formula,*p,3),"SIN")){
		*p+=2;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=sin(f);
	}
	else if(!strcmp(copy(formula,*p,3),"SEN")){
		*p+=2;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=sin(f);
	}
	else if(!strcmp(copy(formula,*p,3),"COS")){
		*p+=2;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=cos(f);
	}
	else if(!strcmp(copy(formula,*p,3),"TAN")){
		*p+=2;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=tanl(f);
	}
	else if(!strcmp(copy(formula,*p,2),"TG")){
		*p+=1;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=tanl(f);
	}
	else if(!strcmp(copy(formula,*p,6),"ARCTAN")){
		*p+=5;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=atanl(f);
	}
	else if(!strcmp(copy(formula,*p,2),"LN")){
		++*p;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=logl(f);
	}
	else if(!strcmp(copy(formula,*p,3),"LOG")){
		*p+=2;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
		f=log(f);
	}
	else if(!strcmp(copy(formula,*p,2),"PI")){
		++*p;
		*c=sigp(formula,p);
		//f=fct(formula,c,p);
		f=(Ldoble) 3.1415926535897932384626433832795029;
	}
	else if(!strcmp(copy(formula,*p,2),"UA")){
		++*p;
		*c=sigp(formula,p);
		f=(Ldoble) 149597870700L;
	}
	else if(!strcmp(copy(formula,*p,2),"PC")){
		++*p;
		*c=sigp(formula,p);
		f=(Ldoble) 30860000000000000L;
	}	
	else if(!strcmp(copy(formula,*p,1),"C")){
		//++*p;
		*c=sigp(formula,p);
		f=(Ldoble) 299792458L;
	}
	else if(!strcmp(copy(formula,*p,3),"EXP")){
		*p+=2;
		*c=sigp(formula,p);
		f=fct(formula,c,p);
// verificar que expl(f) no haga overflow: if((f=expl(f))==_LHUGE_VAL))
		f=expl(f);
	}	
	else if(!strcmp(copy(formula,*p,1),"E")){
	   //++*p; no se incrementa porque se sumaría 0 al valor del puntero
	   *c=sigp(formula,p);
	   f=(Ldoble) 2.7182818284590452353602874713526625L;
	}
	else if(!strcmp(copy(formula,*p,4),"FACT")){
		*p+=3;
		*c=sigp(formula,p);
		_fact=1;
		f=fct(formula,c,p);
		_fact=0;
		if(f<0){
			return(f);
		 }
		f=fact(f);
	}
	//else
	ruptura=*p;
return(f);
}
