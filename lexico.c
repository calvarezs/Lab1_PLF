//Definicion de librerias ANSI utilizadas
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>  
#include <string.h>
#include <math.h>

//Definicion de tipo de dato bool
#define TRUE 1
#define FALSE 0  

//Definicion de codigos de error
#define NINGUNO 0
#define FIN_ARCHIVO 1
#define ERROR_LEXICO 2

//Definicion constantes 
#define LARGO_MAXIMO_CADENA 30  

//Definicion de estructuras
////////ListaPalabras.h////////

//Definicion nodo de lista de palabras
typedef struct Palabra
{
    char* Contenido;
	int LineaActual;
    struct Palabra* Siguiente;
}Palabra;
//Definicion Lista de palabras
typedef struct ListaPalabras
{
    Palabra* Cabeza;
	Palabra* Cola;
}ListaPalabras;
////////////////////////////////////////////

//Definición de Cabeceras
////////ListaPalabras.h////////
ListaPalabras* CrearListaPalabras();
ListaPalabras* InsertarPalabra(ListaPalabras* listaPalabras, char* palabra, int lineaActual);
void AnularListaPalabras(ListaPalabras* listaPalabras);
////////EncontrarErroresParametros.h////////
int EncontrarErroresParametros( int argc, char **argv );
////////ObtenerTextoEntrada.h////////
ListaPalabras* ObtenerTextoEntrada(char* NombreArchivoEntrada);
////////RevisarProgramaSintaxisPL.h//////
void RevisarPrograma(ListaPalabras* listaPalabras,char* nombreArchivoSalida);
int EsIdentificador(char* palabraActual);
int EsNumero(char* palabraActual);
////////////////////////////////////////////
int strposstr(char* s1, char* s2);
//////////////////////////////////////////////////////main.c/////////////////////////////////////////////////////
//Bloque principal del programa
int main ( int argc, char **argv ) {
    
    printf("Iniciando Programa.\n");
    ListaPalabras* ContenidoArchivoEntrada;
	ListaPalabras* ProgramaRevisado;
    int entradaInvalida;

    //Revisar parámetros de entrada
    entradaInvalida = EncontrarErroresParametros(argc,argv); 
    if(entradaInvalida == TRUE)
    {
        return 1;
    }
 
 	printf("Leyendo archivo entrada.\n");
	//Leer archivo de entrada
	ContenidoArchivoEntrada = ObtenerTextoEntrada(argv[1]);

	printf("Generar archivo resultante.\n");
	//Generar resultado
	RevisarPrograma(ContenidoArchivoEntrada,argv[2]);
	
	printf("Cerrando programa.\n");
	//Liberar memoria utilizada
	AnularListaPalabras(ContenidoArchivoEntrada);
	
	//Fin de programa
	printf("Fin de programa.\nPresione cualquier tecla para continuar\n");
	getchar();
	return 0;
}

///////////////////////////////////////////////////////EncontrarErroresParametros.c////////////////////////////////////////////////////////////////////////
//Funcion que revisa los parametros de entrada
//Entrada: argumentos iniciales argc y argv
//Salida: entero-> 1 para resultado verdadero, 0 para resultado falso
int EncontrarErroresParametros( int argc, char **argv )
{
    //Revisando parametros iniciales
	if(argc == 1)
	{ 
		printf("Error: Faltan par%cmetros en la l%cnea de comandos.\nUso: lexico.exe archivo_entrada archivo_salida\n",160,161);
		return 1;
	}
	else if(argc == 2)
	{ 
		printf("Error: Falta par%cmetro en la l%cnea de comandos.\nUso: lexico.exe archivo_entrada archivo_salida\n",160,161);
		return 1;
	}
	else if(argc != 3) 
	{ 
		printf("Error: Demasiados par%cmetros en la l%cnea de comandos.\nUso: lexico.exe archivo_entrada archivo_salida\n",160,161);
		return 1;
	}
	//Revisando existencia archivo entrada
	FILE* archivo_entrada = fopen(argv[1],"r");
	if(!archivo_entrada)
	{
		printf("Error: El archivo de entrada no existe.\n");
		return 1;
	} 
	fclose(archivo_entrada);
	
	//Revisando existencia archivo salida
	FILE* archivo_salida = fopen(argv[2],"r");
	if(archivo_salida)
	{
		printf("Error: El archivo de salida ya existe.\n");
		fclose(archivo_salida);
		return 1;
	}

    return 0;
}

////////////////////////////////////////////////////////////////ListaPalabras.c///////////////////////////////////////////////////////
//TDA lista palabras
//Funcion constructor
ListaPalabras* CrearListaPalabras()
{
    ListaPalabras* nuevo = (ListaPalabras*) malloc(sizeof(ListaPalabras));
	nuevo->Cabeza = NULL;
    return nuevo;
}

//Funcion insertar
ListaPalabras* InsertarPalabra(ListaPalabras* listaPalabras, char* palabra, int lineaActual)
{
	//Se crea una nueva estructura palabra
	Palabra* nuevaPalabra = (Palabra*) malloc(sizeof(Palabra));
	//Se le adjunta la palabra generada
	nuevaPalabra->Contenido = malloc(strlen(palabra)*sizeof(char));
	strcpy(nuevaPalabra->Contenido,palabra);
	nuevaPalabra->LineaActual = lineaActual;
	nuevaPalabra->Siguiente = NULL;
	//Se adjunta la nueva palabra a la lista de palabras 
	if(listaPalabras->Cabeza == NULL)
	{
		listaPalabras->Cabeza = nuevaPalabra;
		listaPalabras->Cola = nuevaPalabra;
	}
	else
	{
		listaPalabras->Cola->Siguiente = nuevaPalabra;
		listaPalabras->Cola = nuevaPalabra;
	}
	//Se retorna la lista de palabras modificada
	return listaPalabras;
}

//Funcion eliminar
void AnularListaPalabras(ListaPalabras* listaPalabras)
{
	Palabra* palabraActual = listaPalabras->Cabeza;
	
	while(palabraActual!=NULL)
	{
		free(palabraActual->Contenido); 
		listaPalabras->Cabeza = palabraActual;
		palabraActual = palabraActual->Siguiente;
		free(listaPalabras->Cabeza);
	} 
}
//////////////////////////////////////////////ObtenerTextoEntrada.c////////////////////////////////////////////////////
//Funcion que obtiene lista de palabras del texto de entrada
//Entrada: el nombre del archivo de texto a leer
//Salida: el listado de palabras
ListaPalabras* ObtenerTextoEntrada(char* NombreArchivoEntrada)
{
	ListaPalabras* listaResultado; 
	char letra;
	char* palabra;
	FILE* archivo;
	int posLetra;
	int posLinea;

	//Se inicia lista palabras
	listaResultado = CrearListaPalabras();

	//Se inicia el archivo
	archivo = fopen(NombreArchivoEntrada, "r");

	//Para cada letra leida se revisa que sea valida
	palabra = (char *) malloc(sizeof(char));

	posLetra = -1;
	posLinea = 1;
	while (!feof(archivo))
	{
		//Obtengo una letra del archivo de texto
		letra = (char)fgetc(archivo);
		if(feof(archivo) && posLetra == -1 && posLinea == 1)
		{
		    printf("El archivo se encuentra en blanco.\n");
		} 
		//Si la letra leida es letra o numero añadir a la palabra final
		if( isalnum( letra ) && letra < 127 )
		{ 
			//Si la palabra comienza con un numero y se actualiza una letra separar en dos palabras
			if(posLetra!=-1 && isdigit(palabra[0]) && isalpha(letra))
			{ 
				palabra[posLetra+1] = '\0';
				InsertarPalabra(listaResultado, palabra, posLinea); 
				palabra = (char *) realloc(palabra, 2*sizeof(char) );
				palabra[0] = letra;
				palabra[1] = '\0';
				posLetra = -1;
			}
			//En caso contrario seguir actualizando la nueva palabra
			posLetra++;	
			palabra = (char *) realloc(palabra, (posLetra + 2)*sizeof(char) );  
			palabra[posLetra] = letra;
			palabra[posLetra+1] = '\0'; 
		}
		//En caso contrario si es un simbolo cualquiera añadir como palabra de largo 1
		else if ( isgraph(letra) )
		{ 
		    if(posLetra!=-1)
		    {
		        palabra[posLetra+1] = '\0';
                InsertarPalabra(listaResultado, palabra, posLinea); 
		    }
			palabra = (char *) realloc(palabra, 2*sizeof(char) );
			palabra[0] = letra;
			palabra[1] = '\0';
			InsertarPalabra(listaResultado, palabra, posLinea); 
			palabra = (char *) realloc(palabra, sizeof(char) );
			posLetra = -1;
		}
		//En caso contrario devolver la palabra construida hasta el momento
		else
		{
			if(posLetra >= 0)
			{
				InsertarPalabra(listaResultado, palabra, posLinea); 
				palabra = (char *) realloc(palabra, sizeof(char) );
			}
			if(letra == '\n')
			{ 
				posLinea ++;
			}
			posLetra = -1;
		} 
	} 
	fclose(archivo);
	return listaResultado;
} 

//////////////////////////RevisarProgramaSintaxisPL.c///////////////////////////
void RevisarPrograma(ListaPalabras* listaPalabras,char* nombreArchivoSalida)
{ 
	FILE* archivoSalida = fopen(nombreArchivoSalida,"w");
	
	//Se definen todos los terminales del lenguaje a revisar
	char* listaTerminales[] = {"const","var","procedure","call","begin","end","if","then","while","do","odd"};
	int nRestoTerminales = 11;
	char* listaSimbolosSimples[] ={"=",".",",",";","+","-","*","/","(",")","#","<",">"}; 
	int nSimbolosSimples = 13;
	char* listaSimbolosDobles[] ={":","<",">"};
	int nSimbolosDobles = 3;
	
	int palabraEncontrada,i;
	Palabra* palabraActual = listaPalabras->Cabeza;
	char* posibleTerminal;

	while(palabraActual != NULL)
	{
		palabraEncontrada = FALSE;
		posibleTerminal = palabraActual->Contenido;
		//Ver si la palabra es un terminal de la lista terminales
		i=0;
		while(palabraEncontrada == FALSE && i < nRestoTerminales)
		{ 
			if(strcmp(posibleTerminal,listaTerminales[i])==0)
			{ 
				palabraEncontrada = TRUE;
				fprintf(archivoSalida,"%s\n",listaTerminales[i]);	 
			}
			i++;
		}
		//caso contrario (cc) ver si la palabra es terminal doble de la lista de simbolos dobles
		i=0;
		while(palabraEncontrada == FALSE && i < nSimbolosDobles)
		{
			if(strcmp(posibleTerminal,listaSimbolosDobles[i]))
			{
				if(palabraActual->Siguiente != NULL && strcmp(palabraActual->Siguiente->Contenido,"=")==0)
				{ 
					palabraEncontrada = TRUE;
					fprintf(archivoSalida,"%s=\n",listaSimbolosDobles[i]);	 
					palabraActual = palabraActual->Siguiente;	
				}
			}
			i++;
		}
		//cc ver si es simbolo simple 
		i=0;
		while(palabraEncontrada == FALSE && i < nSimbolosSimples)
		{
			if(strcmp(posibleTerminal,listaSimbolosSimples[i])==0)
			{ 
				palabraEncontrada = TRUE;
				fprintf(archivoSalida,"%s\n",listaSimbolosSimples[i]);	 
			}
			i++;
		}
		//cc ver si es numero
		if(palabraEncontrada==FALSE && EsNumero(posibleTerminal)==TRUE)
		{ 
			fprintf(archivoSalida,"NUMERO\n");
		}
		//devolver identificador 
		else if(palabraEncontrada==FALSE && EsIdentificador(posibleTerminal)==TRUE)
		{ 
			fprintf(archivoSalida,"IDENTIFICADOR\n");	
		}
		if(palabraActual != NULL)
		{
			palabraActual = palabraActual->Siguiente;
		}
	}
	fclose(archivoSalida);
	return;
}



//Funcion que verifica que la palabra a revisar es Identificador
//Entrada: el identificador a revisar
//Salida: un entero-> 1 para resultado verdadero, 0 para resultado falso
int EsIdentificador(char* palabraActual)
{
	int i;
	int largo = strlen(palabraActual); 
	//Si la palabra no comienza con letra retornar falso
	if(!isalpha(palabraActual[0]))
	{
		return FALSE;
	}
	//Si el resto de letras no son ni letras ni numeros retornar falso
	for(i=1;i<largo;i++)
	{
		if(!isalnum(palabraActual[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}

//Funcion que verifica que la palabra a revisar es numerica
//Entrada: el numero a revisar
//Salida: un entero-> 1 para resultado verdadero, 0 para resultado falso 
int EsNumero(char* palabraActual)
{
	int i;
	int largo = strlen(palabraActual); 
	//Si todos los valores de la palabra son digitos retornar verdadero
	for(i=0;i<largo;i++)
	{
		if(!isdigit(palabraActual[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}


//Funcion que localiza en que posicion de un string se encuentra 
int strposstr(char* s1, char* s2)
{
	char* pos = strstr(s1,s2);

	int i, largo;
	largo = strlen(s1);

	for (i = 0; i < largo; ++i)
	{
		if(&s1[i]==pos)
		{
			return i;
		}
	}
	return 99999999;
}