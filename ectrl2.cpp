#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <iostream>


#define MAX_ELEMENTS 					100


struct period 
{
    time_t t0;
    int    cap;
	time_t t1;
	bool   hole;
	struct period  *prev;
	struct period  *next;
};

struct capacityduration 
{
    time_t T0;   /* duration: T0 ----> TN  */
    time_t TN;
};



/* les couches ont des périodes pré-remplies et triés sans recouvrement */
struct period high[MAX_ELEMENTS]		={	
												{30,50,50,false,NULL,NULL},
												{80,50,100,false,NULL,NULL},
												{130,100,150,false,NULL,NULL}
											 };
											 
struct period medium[MAX_ELEMENTS]	={	
												{20,40,60,false,NULL,NULL},
												{90,30,100,false,NULL,NULL},
												{120,40,130,false,NULL,NULL}
											 };
											 
struct period low[MAX_ELEMENTS]		={	
												{10,5,40,false,NULL,NULL},
												{70,20,110,false,NULL,NULL},
												{140,30,160,false,NULL,NULL}
											};

struct period *   resultats[MAX_ELEMENTS];

struct 	period* process(capacityduration  duration, struct period* pLow, struct period* pMedium,struct period* pHigh);

void 	consolidation(int numero_duration , struct capacityduration   t );
void 	display_results();
void 	init_results();
int 	compare(void const* a, void const* b);
void    sort(void **tab,int ncount,size_t size, int (*compare)(const void *a, const void *b));
 
#define DEBUG  if (0) printf("%s : %d\r\n",__FUNCTION__,__LINE__);
#define PTR(p) if (0)  printf("%s : %d -0x%x\r\n",__FUNCTION__,__LINE__, (unsigned int)p);

int main(void)
{
   DEBUG
   init_results();
   DEBUG
   //définition de la tranche horaire à considérer
   //pour le calcul de la capacité consolidé sur 3 couches    
   struct capacityduration   T; 
   T.T0=0;
   T.TN=60;
   DEBUG
   // calcul la capacité consolidé
   // sur la première tranche horaire de la journée pour l'exemple
   consolidation(0, T); 
   
   T.T0=70;
   T.TN=110;
   DEBUG
   // calcul la capacité consolidé
   // sur la première tranche horaire de la journée pour l'exemple
   consolidation(1, T); 
   
   T.T0=120;
   T.TN=160;
   DEBUG
   // calcul la capacité consolidé
   // sur la première tranche horaire de la journée pour l'exemple
   consolidation(2, T); 
   
  DEBUG
   //affiche les resutats 
   display_results();
   DEBUG
}

void consolidation(int numero_duration, struct capacityduration   T )
{
	/* la journée étant divisé en 24 heures */
	/* on calcul 24 capacité consolidé max par jour */
	/* on simplifie le problème en prenant en considération que les tranche d'une heure max */
	/* les tranches sont numéroté de 0 à 23 */
	DEBUG
	if (numero_duration < 0 || numero_duration >= MAX_ELEMENTS )
		return;
	DEBUG
	if (numero_duration < 0 || numero_duration >= MAX_ELEMENTS )
		return;
	DEBUG
	/* la liste des périodes de chaque couche est limité à 100 éléments */
	
	struct period* pHigh 	= &high[numero_duration];
	PTR(pHigh)
	struct period* pMedium 	= &medium[numero_duration];
	PTR(pMedium)
	struct period* pLow		= &low[numero_duration];
	PTR(pLow)
	DEBUG
	struct period* pp = process(T, pLow, pMedium, pHigh);
	PTR(pp)
	DEBUG
	resultats[numero_duration] = pp;
	
}

void display_results()
{
	DEBUG
	for (int tr = 0; tr < MAX_ELEMENTS; tr++) 
	{
		struct period* pp = resultats[tr];
		while( pp)
		{
			if (pp->hole == false)
				printf("|-%d-", pp->cap);
			
			pp = pp->next;
			if (pp == NULL)
				printf("|\n");
		}
		
	}
}

void init_results()
{
	DEBUG
	for (int tr = 0; tr < MAX_ELEMENTS; tr++) 
	{
		resultats[tr]=NULL;
	}
	DEBUG
}


int compare(void const* a, void const* b)
{
    struct period* arg1 = (struct period*)a;
    struct period* arg2 = (struct period*)b;
 
    if ((arg1)->t0 <  (arg2)->t0)    return -1;
    if ((arg1)->t0 >  (arg2)->t0)    return 1;
	if ((arg1)->t0 == (arg2)->t0)  return 0;
    
	return 0;
}

/* fonction : process 
    input   : capacityduration  duration, struct period* pLow, struct period* pMedium,struct period* pHigh
    sortie  : liste doublement  chaînée des capacités consolodé par période  

*/
struct period* process(capacityduration  duration, struct period* pLow, struct period* pMedium,struct period* pHigh)
{
   struct period* input[3];
   
   input[0] = pLow;
   input[1] = pMedium;
   input[2] = pHigh;
   
   sort((void**)input, 3, sizeof(struct period), compare);
   
   struct period*  pp = (struct period*)malloc(sizeof(struct period));
   struct period* vertical = pp;
   pp->next = pp->prev = NULL;
   
   for(int i = 0; i < 4; i++)
   {
	    if (i < 3)
		{
			pp->t0  = input[i]->t0;
			pp->cap = input[i]->cap;
		
		}else
		{
		  for(int j = 0; j < 3; j++)
		  if (input[j]->t1 == duration.TN)	
		  {
			struct period* prev = pp->prev;
			if ( prev->cap != input[j]->cap)
			{
				pp->t0  = input[2]->t0;
				pp->t1  = input[j]->t1;
				pp->cap = input[j]->cap;
			}else
			{
				prev->next=NULL;
				
			}
		  }	
		}
		
		int min = 0;
	
		switch(i)
		{
			case 0: {
					
					min = input[0]->t1;
					
					if (min > input[1]->t0)
						min=input[1]->t0;
					
					if (min > input[2]->t0)
						min=input[2]->t0;
					break;
					}
					
			case 1: {
					
					min = input[1]->t1;
					
					if (min > input[0]->t1)
						min=input[0]->t1;
					
					if (min > input[2]->t0)
						min=input[2]->t0;
					break;
					}
					
			case 2: {
					
					min = input[2]->t1;
					
					if (min > input[1]->t1)
						min=input[1]->t1;
					
					if (min > input[0]->t1)
						min=input[0]->t1;
					break;
					}
		}
		
		pp->t1   = min; 
		
		pp->hole = false;
		if (pp->t0 < duration.T0 || pp->t1 > duration.TN)
		  pp->hole = true;
		
		//
		struct period* qq = (struct period*)malloc(sizeof(struct period));
		pp->next = qq;
		qq->prev = pp ;
		qq->next = NULL;
		
		pp = qq;
  }
  
  return vertical;
 }
 
 void sort(void **tab,int ncount,size_t size, int (*compare)(const void *a, const void *b))
 {
	for(int i = 0;i < ncount-1 ; i++) 
	{
		if (compare(tab[i],tab[i+1]) > 0 )
		{
			void* p = tab[i];
			tab[i]  = tab[i+1];
			tab[i+1]=p;
		}
	}
 }