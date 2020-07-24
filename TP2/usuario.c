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
#define T "\x1b[30m" //Letra Negra
#define M "\x1b[31m" // Letra Roja
#define V "\x1b[37m" //Letra Blanca
#define INFORMACION "pokedex.txt"
#define AVISTAMIENTOS "avistamientos.txt"
#define EVOLUCION "evoluciones.txt"

#define OPCIONES 10
#define ERROR -1
#define EXITO 0

#define SALIDA 'S'
#define INICIAR 'I'




typedef struct comandos{
  char letra;
  char descpricion[MAX_DESCRIPCION];
  char opcion[MAX_DESCRIPCION];
  void (*funcion)(pokedex_t**);
  bool es_valido;
}comandos_t;


void titulo(){
  printf("\t    "B V"***********"Q"            "N T"**********"Q"          "B V"***"Q"          "B V"***"Q"         "B V"**************"Q"      "B V"************"Q"                 "B V"**************"Q"      "B V"****"Q"                 "B V"****"Q"\n");
  printf("\t    "B V"*************"Q"        "N T"**"P M"**********"N T"**"Q"        "B V"***"Q"        "B V"***"Q"          "B V"**************"Q"       "B V"***************"Q"             "B V"**************"Q"         "B V"****"Q"             "B V"****"Q"\n");
  printf("\t    "B V"***"Q"       "B V"****"Q"     "N T"**"P M"**************"N T"**"Q"      "B V"***"Q"      "B V"***"Q"          "B V"***"Q"                    "B V"***"Q"         "B V"******"Q"        "B V"***"Q"                        "B V"****"Q"         "B V"****"Q"\n");
  printf("\t    "B V"***"Q"       "B V"****"Q"   "N T"**"P M"******************"N T"**"Q"    "B V"***"Q"    "B V"***"Q"          "B V"***"Q"                      "B V"***"Q"            "B V"***"Q"      "B V"***"Q"                            "B V"****"Q"     "B V"****"Q"\n");
  printf("\t    "B V"*************"Q"    "N T"**"P M"******"N T"******"P M"******"N T"**"Q"    "B V"***"Q"  "B V"***"Q"            "B V"***"Q"                      "B V"***"Q"            "B V"***"Q"      "B V"***"Q"                              "B V"****"Q" "B V"****"Q"\n");
  printf("\t    "B V"***********"Q"      "N T"**********"B V"**"N T"**********"Q"    "B V"******"Q"              "B V"*************"Q"            "B V"***"Q"            "B V"***"Q"      "B V"*************"Q"                      "B V"*****"Q"\n");
  printf("\t    "B V"***"Q"              "N T"**"B V"******"N T"******"B V"******"N T"**"Q"    "B V"***"Q"  "B V"***"Q"            "B V"***"Q"                      "B V"***"Q"            "B V"***"Q"      "B V"***"Q"                              "B V"****"Q" "B V"****"Q"\n");
  printf("\t    "B V"***"Q"              "N T"**"B V"******************"N T"**"Q"    "B V"***"Q"    "B V"***"Q"          "B V"***"Q"                      "B V"***"Q"            "B V"***"Q"      "B V"***"Q"                            "B V"****"Q"     "B V"****"Q"\n");
  printf("\t    "B V"***"Q"                "N T"**"B V"**************"N T"**"Q"      "B V"***"Q"      "B V"***"Q"          "B V"***"Q"                    "B V"***"Q"         "B V"******"Q"        "B V"***"Q"                        "B V"****"Q"         "B V"****"Q"\n");
  printf("\t    "B V"***"Q"                  "N T"**"B V"**********"N T"**"Q"        "B V"***"Q"        "B V"***"Q"         "B V"***************"Q"       "B V"***************"Q"             "B V"**************"Q"         "B V"****"Q"             "B V"****"Q"\n");
  printf("\t    "B V"***"Q"                    "N T"**********"Q"          "B V"***"Q"           "B V"***"Q"       "B V"***************"Q"      "B V"************"Q"                 "B V"**************"Q"      "B V"****"Q"                 "B V"****"Q"\n\n\n\n\n");

}


void inicializar_opciones();

void iniciar(pokedex_t** pokedex){
  printf("Prendiendo pokedex...\n");
  *pokedex = pokedex_prender();
  if(!pokedex){
    printf("ERROR: No se pudo prender la pokedex\n");
    return;
  }
  inicializar_opciones();
  titulo();
}


void mostrar_ayuda(pokedex_t** pokedex);


void salir(pokedex_t** pokedex){
  printf("Adios %s!\n", (*pokedex)->nombre_entrenador);
}


void guardar(pokedex_t** pokedex){
  int retorno;
  printf("Guardando informacion...\n");
  retorno = pokedex_apagar(*pokedex);
  if(retorno == EXITO)
    printf("Guardado exitoso!\n");
  else
    printf("Error: No se pudo guardar\n");
}


void avistar(pokedex_t** pokedex){
  int retorno;
  printf("Avistando Pokemones...\n");
  retorno = pokedex_avistar(*pokedex, AVISTAMIENTOS);
  if(retorno == EXITO)
    printf("Se ha actualizado la pokedex!\n");
  else
    printf("ERROR: No se pudo actualizar la pokedex\n");
}


void evolucionar(pokedex_t** pokedex){
  int retorno;
  printf("Evolucionando pokemones...\n");
  retorno = pokedex_evolucionar(*pokedex, EVOLUCION);
  if(retorno == EXITO)
    printf("Se han evolucionado tus pokemones!\n");
  else
    printf("¡ERROR! No se pudieron evolucionar todos los pokemones\n");
}


void recien_capturados(pokedex_t** pokedex){
  if(lista_vacia((*pokedex)->ultimos_capturados)){
    printf("No se capturaron pokemones recientemente\n");
    return;
  }
  pokedex_ultimos_capturados(*pokedex);
}


void recien_avistados(pokedex_t** pokedex){
  if(lista_vacia((*pokedex)->ultimos_vistos)){
    printf("No se avistaron pokemones recientemente\n");
    return;
  }
  pokedex_ultimos_vistos(*pokedex);
}


void info_especie(pokedex_t** pokedex){
  int especie;
  printf("Ingrese el numero de las especie\n>>");
  scanf("%i", &especie);
  pokedex_informacion(*pokedex, especie, NULL);
}


void info_pokemon(pokedex_t** pokedex){
  int especie;
  char nombre[MAX_NOMBRE];
  printf("Ingrese el numero de las especie\n>>");
  scanf("%i", &especie);
  printf("Ingrese el nombre del pokemon\n>>");
  scanf("%s", nombre);
  pokedex_informacion(*pokedex, especie, nombre);
}



comandos_t opciones[OPCIONES] = {
  {'I', "Presione 'I' para iniciar la pokedex\n", "_Inicilizar\n", iniciar, true},
  {'H', "Presione 'H' si necesita ayuda\n", "_Ayuda\n", mostrar_ayuda, true},
  {'S', "Presione 'S' para Salir del programa\n", "_Salir\n", salir, true},
  {'G', "Presione 'G' para guardar la informacion de la pokedex\n", "_Guardar\n" ,guardar, false},
  {'A', "Presione 'A' para avistar_pokemones\n", "_Avistar\n", avistar, false},
  {'E', "Presione 'E' para evolucionar pokemones\n", "_Evolucionar\n", evolucionar, false},
  {'C', "Presione 'C' para ver las capturas recientes\n", "_Capturas Recientes\n", recien_capturados, false},
  {'V', "Presione 'V' para ver los pokemones recien avistados\n", "_Avistamientos Recientes\n", recien_avistados, false},
  {'M', "Presione 'M' para ver la informacion de una especie\n", "_Informacion de una Especie\n", info_especie, false},
  {'P', "Presione 'P' para ver la informacion de un pokemon\n", "_Informacion de un Pokemon\n", info_pokemon, false},
};


void inicializar_opciones(){
  for(int i=0; i < OPCIONES; i++){
    if(opciones[i].letra == INICIAR)
      opciones[i].es_valido = false;
    else
      opciones[i].es_valido = true;
  }
}

void mostrar_ayuda(pokedex_t** pokedex){
  for(int i=0; i < OPCIONES; i++){
    if(opciones[i].es_valido == true)
      printf("%s", opciones[i].descpricion);
  }
}


void mostrar_opciones(){
  for(int i=0; i < OPCIONES; i++){
    if(opciones[i].es_valido == true)
      printf("%s", opciones[i].opcion);
  }
}

void ejecutar_opcion(char opcion, pokedex_t** pokedex){
  bool encontrado = false;
  int i = 0;
  while(!encontrado && i < OPCIONES){
    if(opciones[i].letra == opcion && opciones[i].es_valido){
      encontrado = true;
      opciones[i].funcion(pokedex);
    }
    i++;
  }
  if(!encontrado)
    printf("Comando invalido. Si necesita ayuda presione 'H'\n");
}

int main(){
  char letra = ' ';
  pokedex_t* pokedex = NULL;
  while(letra != SALIDA){
    printf("Elija una opcion\n");
    mostrar_opciones();
    printf("\n>>");
    scanf(" %c", &letra);
    ejecutar_opcion(letra, &pokedex);
  }
  pokedex_destruir(pokedex);
  return 0;
}
