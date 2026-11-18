#include <iostream>   
#include <fstream>   
#include <sstream>    
#include <string>    
#include <vector>     
#include <iomanip>    // setw y left: alinear las columnas d la tabla
#include <cctype>     // isdigit: saber si un carácter es un dígito
using namespace std;  // para no escribir std:: antes de cout, string, etc.



const int NUM_DIAS = 6;                                  // las clases de lunes a sábado
const string DIAS[NUM_DIAS] = {"Lunes", "Martes", "Miercoles",
                               "Jueves", "Viernes", "Sabado"};  // sin tildes
const int HORA_INICIO_DIA = 6;                           // la clase más temprano empieza a las 6
const int HORA_FIN_DIA = 22;                             // la ultima clas eprogramada es maximo hasta las 22
const int NUM_HORAS = HORA_FIN_DIA - HORA_INICIO_DIA;    // 16 franjas de una hora (las filas de la tabla)




struct Sesion {        // una clase en un día específico
    string dia;      
    int inicio;        // hora de inicio en formato militar (7 = 7:00 a. m.)
    int fin;           // hora de fin (9 = 9:00 a. m.)
};

struct Grupo {                 // un grupo de una materia
    string materia;            // nombre de la materia
    int numero;                // número del grupo 
    int creditos;              // créditos de la materia
    vector<Sesion> sesiones;   // todas sus clases en la semana
};

vector<Grupo> oferta;                  // todos los grupos leídos del archivo
vector<string> elegidas;               // nombres de las materias que escogió el usuario
vector<vector<int>> gruposPorMateria;  // gruposPorMateria[k] = posiciones en "oferta" de los grupos de la materia k
vector<vector<int>> resultados;        // cada horario válido es una lista de posiciones en "oferta"
int maxCreditos = 25;                  // filtro: máximo de créditos permitidos
int horaMinima = 6;                    // filtro: no tener clase antes de esta hora
string diaLibre = "";                  // filtro: día que se quiere libre ("" = ninguno)