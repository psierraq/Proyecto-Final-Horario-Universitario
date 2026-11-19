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

int leerEntero() {
    int n;                                        // aquí se guarda el número leído
    while (!(cin >> n)) {                         // si cin falla (escribieron letras)...
        cin.clear();                              // ...se borra el estado de error de cin
        cin.ignore(10000, '\n');                  // ...se descarta lo que se escribió mal
        cout << "Entrada invalida. Escriba un numero: ";  // ...y se pide de nuevo
    }
    return n;                                     // se devuelve el número válido
}

// Devuelve la posición del día en el arreglo DIAS (Lunes = 0 ... Sabado = 5), o -1 si no existe.
int indiceDia(string dia) {
    for (int i = 0; i < NUM_DIAS; i++) {          // recorre los 6 días
        if (DIAS[i] == dia) {                     // si el nombre coincide...
            return i;                             // ...devuelve su posición
        }
    }
    return -1;                                    // no se encontró: el día está mal escrito
}

// Revisa que un texto tenga solo dígitos (sirve para validar el CSV antes de usar stoi).
bool esNumero(string texto) {
    if (texto.empty()) {                          // un texto vacío no es un número
        return false;
    }
    for (int i = 0; i < texto.length(); i++) {    // recorre carácter por carácter
        if (!isdigit(texto[i])) {                 // si alguno no es dígito...
            return false;                         // ...no es un número
        }
    }
    return true;                                  // todos eran dígitos
}

void cargarOferta(string ruta) {
    ifstream archivo(ruta);                       // abre el archivo para leerlo
    if (!archivo.is_open()) {                     // si no se pudo abrir...
        cout << "No se pudo abrir el archivo " << ruta << endl;
        return;                                   // ...se sale de la función
    }

    oferta.clear();                               // se borra lo que hubiera de una carga anterior
    elegidas.clear();
    gruposPorMateria.clear();
    resultados.clear();

    string linea;                                 // aquí se guarda cada línea leída
    getline(archivo, linea);                      // la primera línea son los encabezados: se salta
    int numLinea = 1;                             // contador para decir en qué línea hay errores
    int errores = 0;                              // cuántas líneas se descartaron

    while (getline(archivo, linea)) {             // lee línea por línea hasta el final
        numLinea++;                               // ya vamos en la siguiente línea

        // Los archivos hechos en Windows terminan cada línea con '\r': se quita
        if (!linea.empty() && linea[linea.length() - 1] == '\r') {
            linea.erase(linea.length() - 1);
        }
        if (linea.empty()) {                      // las líneas vacías se ignoran
            continue;
        }
        // Excel en español guarda el CSV con ';' en vez de ',': se cambian
        for (int i = 0; i < linea.length(); i++) {
            if (linea[i] == ';') {
                linea[i] = ',';
            }
    	}
		
		stringstream ss(linea);                   // convierte la línea en un "flujo" para partirla
        string materia, sNumero, sCreditos, dia, sInicio, sFin;  // los 6 campos como texto
        getline(ss, materia, ',');                // lee hasta la primera coma
        getline(ss, sNumero, ',');                // lee hasta la segunda coma
        getline(ss, sCreditos, ',');              // y así sucesivamente
        getline(ss, dia, ',');
        getline(ss, sInicio, ',');
        getline(ss, sFin, ',');
        
	    if (materia.empty() || !esNumero(sNumero) || !esNumero(sCreditos) ||
            !esNumero(sInicio) || !esNumero(sFin)) {
            cout << "  Linea " << numLinea << ": faltan datos o hay letras donde van numeros. Se ignora.\n";
            errores++;
            continue;                             // pasa a la siguiente línea
        }
        
        if (indiceDia(dia) == -1) {
            cout << "  Linea " << numLinea << ": el dia \"" << dia << "\" no es valido. Se ignora.\n";
            errores++;
            continue;
        }

        int numero = stoi(sNumero);               // stoi convierte texto a entero
        int creditos = stoi(sCreditos);
        int inicio = stoi(sInicio);
        int fin = stoi(sFin);

        
        if (inicio >= fin || inicio < HORA_INICIO_DIA || fin > HORA_FIN_DIA) {
            cout << "  Linea " << numLinea << ": horas invalidas (" << inicio << " a " << fin << "). Se ignora.\n";
            errores++;
            continue;
        }
        
        if (numero <= 0 || creditos <= 0) {
            cout << "  Linea " << numLinea << ": grupo o creditos en cero. Se ignora.\n";
            errores++;
            continue;
        }

        Sesion s;                                 // se arma la sesion con los datos de la linea
        s.dia = dia;
        s.inicio = inicio;
        s.fin = fin;  
        int pos = -1;                             // -1 significa "no existe todavía"
        for (int i = 0; i < oferta.size(); i++) {
            if (oferta[i].materia == materia && oferta[i].numero == numero) {
                pos = i;                          // se encontró en la posición i
                break;                            // no hace falta seguir buscando
            }
        }

        if (pos != -1) {                          // si ya existía...
            oferta[pos].sesiones.push_back(s);    // ...solo se le agrega esta sesión
        } else {                                  // si no existía...
            Grupo g;                              // ...se crea el grupo nuevo
            g.materia = materia;
            g.numero = numero;
            g.creditos = creditos;
            g.sesiones.push_back(s);              // con su primera sesión
            oferta.push_back(g);                  // y se agrega a la oferta
        }
    }

    archivo.close();                              // siempre se cierra el archivo al terminar
    cout << "Se cargaron " << oferta.size() << " grupos. Lineas con error: " << errores << endl;
}
void mostrarOferta() {
    cout << "\n===== OFERTA DE MATERIAS =====\n";
    for (int i = 0; i < oferta.size(); i++) {     // recorre todos los grupos
        cout << oferta[i].materia << " - Grupo " << oferta[i].numero
             << " (" << oferta[i].creditos << " creditos)\n";
        for (int j = 0; j < oferta[i].sesiones.size(); j++) {  // recorre las sesiones del grupo
            Sesion s = oferta[i].sesiones[j];
            cout << "     " << s.dia << " " << s.inicio << ":00 - " << s.fin << ":00\n";
        }
    }
}
void seleccionarMaterias() {
   
    vector<string> nombres;
    for (int i = 0; i < oferta.size(); i++) {
        bool yaEsta = false;                      
        for (int j = 0; j < nombres.size(); j++) {
            if (nombres[j] == oferta[i].materia) {
                yaEsta = true;
            }
        }
        if (!yaEsta) {                            
            nombres.push_back(oferta[i].materia);
        }
    }

    
    cout << "\nMaterias disponibles:\n";
    for (int i = 0; i < nombres.size(); i++) {
        cout << "  " << i + 1 << ". " << nombres[i] << endl;  
    }

    elegidas.clear();                            
    gruposPorMateria.clear();
    resultados.clear();
