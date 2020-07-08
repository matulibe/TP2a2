#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "pokedex.h"

#define ABRIR "a+"
#define LEER "r"
#define ESCRIBIR "w"
#define CAPTURADO 'S'
#define NO_CAPTURADO 'N'

#define FORMATO_E "%i;%[^;];%i;[^;];[^;];%i\n"
#define FORMATO_A  "%i;%[^;];[^;];[^;];%i;%c\n"
#define FORMATO_P_P "%c;[^;];%i;%c\n"
#define FORMATO_P_E "%c;%i;[^;];[^;]\n"
#define CANT_E 6
#define CANT_A 6
#define CANT_P_P 4
#define CANT_P_E 4

#define MAX 100
#define ERROR -1
#define EXITO 0



pokedex_t* pokedex_crear(char entrenador[MAX_NOMBRE]){
  if(!entrenador)
    return NULL;

  pokedex_t* aux = calloc(1, sizeof(pokedex_t));
  if(!aux)
    return NULL;
  strcpy(aux->nombre_entrenador, entrenador);
  return aux;
}


int pokedex_avistar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
  FILE * archivo = fopen(ruta_archivo, LEER);
  particular_pokemon_t* poke = calloc(1,sizeof(particular_pokemon_t));
  if(!poke)
    return ERROR;
  especie_pokemon_t* especie;
  char especie_actual[MAX];
  char capturado;
  int leidos = 0;
  while(leidos != EOF){
    fscanf(archivo, FORMATO_A, &especie->numero, especie->nombre, especie->descripcion, poke->nombre, poke->nivel, &capturado);
    especie_pokemon_t* buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, especie));
    if(!buscador){
      especie_pokemon_t* especie_aux = calloc(1, sizeof(especie_pokemon_t));
      if(!especie_aux)
        return  ERROR;
      especie_aux = especie;
      arbol_insertar(pokedex->pokemones, (void*)especie_aux);
      especie_aux->pokemones = lista_crear();
      buscador = (especie_pokemon_t*)(arbol_buscar(pokedex->pokemones, especie->numero));
    }
    lista_insertar(buscador->pokemones, (void*)poke);
    if(capturado == CAPTURADO)
      lista_apilar(pokedex->ultimos_capturados, (void*)poke);
    lista_encolar(pokedex->ultimos_vistos, (void*)poke);
  }

  return EXITO;
}


int pokedex_evolucionar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
  
}