#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "abb.h"
#include "lista.h"
#include "pokedex.h"

#define ABRIR "a+"
#define LEER "r"
#define ESCRIBIR "w"

#define FORMATO_E "%i;%[^;];%i;[^;];[^;];%i\n"
#define FORMATO_A  "%i;%[^;];[^;];[^;];%i;%c\n"
#define FORMATO_P_P "%c;[^;];%i;%c\n"
#define FORMATO_P_E "%c;%i;[^;];[^;]\n"



pokedex_t* pokedex_crear(char entrenador[MAX_NOMBRE]){
  if(!entrenador)
    return NULL;

  pookedex_t* aux = calloc(1, sizeof(pokedex_t));
  if(!aux)
    return NULL;
  aux->nombre_entrenador = entrenador;
  return aux;
}
int pokedex_avistar(pokedex_t* pokedex, char ruta_archivo[MAX_RUTA]){
  FILE * archivo = fopen(ruta_archivo, LEER);
  particular_pokemon_t* poke = calloc(1,sizeof(particular_pokemon_t));
  especie_pokemon_t* especie = calloc(1, sizeof(especie_pokemon_t));
  char capturado;
  int leidos = 0;
  while(leidos != EOF){ //numero_pokemon;nombre_especie; descripción_especie;nombre_pokemon;nivel;capturado(S/N)
    fscanf(archivo, FORMATO_A, especie->numero_pokemon, especie->nombre, especie->descripcion, poke->nombre, &capturado);
  }
}
