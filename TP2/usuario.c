#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "abb.h"
#include "lista.h"
#include "pokedex.h"

#define Q "\x1b[0m" //Reset
#define N "\x1b[40m" //Resaltado Negro
#define P "\x1b[41m" //Resaltado Rojo
#define B  "\x1b[47m" //Resaltado Blanco
#define INFORMACION "pokedex.txt"
#define AVISTAMIENTOS "avistamientos.txt"
#define EVOLUCION "evolucion.txt"


void inicio(){
  printf("\t    "B"***********"Q"            "N"**********"Q"          "B"***"Q"          "B"***"Q"         "B"**************"Q"      "B"************"Q"                 "B"**************"Q"      "B"****"Q"                 "B"****"Q"\n");
  printf("\t    "B"*************"Q"        "N"**"P"**********"N"**"Q"        "B"***"Q"        "B"***"Q"          "B"**************"Q"       "B"***************"Q"             "B"**************"Q"         "B"****"Q"             "B"****"Q"\n");
  printf("\t    "B"***"Q"       "B"****"Q"     "N"**"P"**************"N"**"Q"      "B"***"Q"      "B"***"Q"          "B"***"Q"                    "B"***"Q"         "B"******"Q"        "B"***"Q"                        "B"****"Q"         "B"****"Q"\n");
  printf("\t    "B"***"Q"       "B"****"Q"   "N"**"P"******************"N"**"Q"    "B"***"Q"    "B"***"Q"          "B"***"Q"                      "B"***"Q"            "B"***"Q"      "B"***"Q"                            "B"****"Q"     "B"****"Q"\n");
  printf("\t    "B"*************"Q"    "N"**"P"******"N"******"P"******"N"**"Q"    "B"***"Q"  "B"***"Q"            "B"***"Q"                      "B"***"Q"            "B"***"Q"      "B"***"Q"                              "B"****"Q" "B"****"Q"\n");
  printf("\t    "B"***********"Q"      "N"**********"B"**"N"**********"Q"    "B"******"Q"              "B"*************"Q"            "B"***"Q"            "B"***"Q"      "B"*************"Q"                      "B"*****"Q"\n");
  printf("\t    "B"***"Q"              "N"**"B"******"N"******"B"******"N"**"Q"    "B"***"Q"  "B"***"Q"            "B"***"Q"                      "B"***"Q"            "B"***"Q"      "B"***"Q"                              "B"****"Q" "B"****"Q"\n");
  printf("\t    "B"***"Q"              "N"**"B"******************"N"**"Q"    "B"***"Q"    "B"***"Q"          "B"***"Q"                      "B"***"Q"            "B"***"Q"      "B"***"Q"                            "B"****"Q"     "B"****"Q"\n");
  printf("\t    "B"***"Q"                "N"**"B"**************"N"**"Q"      "B"***"Q"      "B"***"Q"          "B"***"Q"                    "B"***"Q"         "B"******"Q"        "B"***"Q"                        "B"****"Q"         "B"****"Q"\n");
  printf("\t    "B"***"Q"                  "N"**"B"**********"N"**"Q"        "B"***"Q"        "B"***"Q"         "B"***************"Q"       "B"***************"Q"             "B"**************"Q"         "B"****"Q"             "B"****"Q"\n");
  printf("\t    "B"***"Q"                    "N"**********"Q"          "B"***"Q"           "B"***"Q"       "B"***************"Q"      "B"************"Q"                 "B"**************"Q"      "B"****"Q"                 "B"****"Q"\n\n\n\n\n");

  printf("Presiona I para iniciar la pokedex\n");
  printf("Presiona G para guardar la informacion\n");
  printf("Presiona S para salir de la pokedex\n");
  printf("Presiona H si necesitas ayuda\n");
  printf("Presiona A para actualizar los avistamientos\n");
  printf("Presiona E para actulizar la informacion de pokemones evolucionados\n");
  printf("Presiona C para mostrar las capturas recientes\n");
  printf("Presiona V para ver los ultimos avistamientos\n");
  printf("Presiona M para ver la informmacion de la especie\n");
  printf("Presiona P para ver la informacion del pokemon\n\a");
}

int main(){
  inicio();

  char letra;
  scanf("%c", &letra);
  if(letra == 'q'){
    printf("Adios!\n");
      return 0;
}







  return 0;
}
