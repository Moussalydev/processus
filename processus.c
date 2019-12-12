#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include<string.h>

#define TUBE_LECTURE 0
#define TUBE_ECRITURE 1

/*
Sujet: Projet de systéme réseau
Classe: Master1 glsi
Ecole: ESP
Année: 2016-2017
Auteurs: Moussa Ly & Mouhamed Diallo
*/

int nb;
int bloquer;
int indication;
FILE* fichier = NULL;
int n[5], p[5], q[5], e[5], x[5];

int cpt=0;
void handler(int signum) {
  if(signum == SIGUSR1) {
    printf("Signal 1 recu\n");
    cpt = cpt | 1;
  }
  if(signum == SIGUSR2) {
    printf("Signal 2 recu\n");
    cpt = cpt | 2;
  }
}
//On cherche la clé publique et celle privée, la clé publique est enrégistré dans un fichier
int calculcle(int val){
     
        fichier = fopen("test.txt", "w");

	printf("------------------------------Saisir le premier grand nombre--------------------------------------\n");
	scanf("%d",&p[val]);
	printf("Saisir le second nombre grand nombre\n");
	scanf("%d",&q[val]);
	n[val]=p[val]*q[val];
        x[val]=(p[val]-1)*(q[val]-1);
       
        printf("----------------------------Donnez un nombre qui n'a aucun facteur commun----------------------------\n");
        scanf("%d",&e[val]);
        
        if(facteurcommun(e[val],x[val])==1){
           fprintf(fichier,"Le couple %d et %d constitue la clé publique du processus numéro %d\n",e[val],n[val],(val+1));
           printf("Clés publique sauvegardée est %d, %d\n",e[val],n[val]);
            
           return e[val];
         }
        else{
         
          printf("Les nombres ne sont pas premiers entre eux\n");
          return 0;
          }
}
//cle privee
int private(int val){
  indication=1;
  int d[nb];
  if(val>0){
  printf("Saisir ta cle privee\n");
  scanf("%d",&d[val]);
  
  if((e[val-1]*d[val])%x[val-1]==1)
       bloquer=1;
  }
   return bloquer;


}
int facteurcommun(int a, int b){
   int r;
   do{
        r=a%b;
        a=b;
        b=r;
    }
    while(r!=0);
    return a;
   
}
//fin du chiffrage RSA et début de la communication
void anneau(int tube[2]){
    int i,nbtube,position,v;
    pid_t fils[nb];
    char* mess[nb];  

    printf("Quel est le nombre de processus\n");
    scanf("%d",&nb);
     //la création des  processus fils au nombre de nb
     nbtube= 2 * nb;
     //La création des tubes 2 fois le nombre de processus
      for(i=0; i< nbtube; i++){   
	   pipe(tube);
      }
      position=0;

      
      for(i=0; i<nb; i++){
        if(calculcle(i)!=0){
           char chaine[10];
           printf("quel est ton message\n");
           scanf ("%9s", chaine);
           private(i);
           getchar ();
           mess[i] = malloc (strlen (chaine) + 1);
 
	      if (mess[i] != NULL)
	      {
		 strcpy (mess[i], chaine);
	      }
           
	      
       } 
        fils[i]=fork();
       
             if(fils[i]==0){
                           while(position< nb){
                                      
		                       if(position==i){
					//fermetture en lecture pour écrire
						    close(tube[TUBE_LECTURE]);
						    write(tube[TUBE_ECRITURE], fils, sizeof(fils));
                                           
                                   if(i==0)
				         printf("le processus %d a écrit\n",getpid());
                                   else
                                   printf("le processus %d va écrire\n",getpid());                                                              
						    close(tube[TUBE_ECRITURE]);
				          //fin de l'écriture           
		                          }
		                          else if(i==position+1){
		                           //lecture à l'élément suivant
						    close(tube[TUBE_ECRITURE]);
						    read(tube[TUBE_LECTURE], fils, sizeof(fils)); 
                                                    //le lancement des signaux
                                                    if(signal(SIGUSR1, handler) == SIG_ERR) {
                                                           perror("Erreur lors du positionnement ");
                                                           exit(EXIT_FAILURE);
                                                     }
                                                     if(signal(SIGUSR2, handler) == SIG_ERR) {
                                                           perror("Erreur lors du positionnement ");
                                                           exit(EXIT_FAILURE);
                                                     }
					             
						    printf("le processus %d lit\n",getpid());                                                   
		               
						    close(tube[TUBE_ECRITURE]);
                                                    //le message doit etre confidentiel et lu seulement si on donne le vrai d
                                                    if(bloquer==0 && indication==1){
                                                      printf("Desole: le message est confidentiel\n");
                                                    }
                                                    else
                                                      printf("Message reçu est: %s\n",mess[i-1]);
                                                     
												   
		                            }
		                           
                                           position++;
                                           
                                     }
                                     
                              
                              break;

                          }        

                     }
          
             }
int main(){
    //La création de l'anneau 
    printf("\n");
    printf("############################################# COMMUNICATION SECURISEE  #######################################\n");
    int tube[2];
    pipe(tube);
    anneau(tube);       
}
