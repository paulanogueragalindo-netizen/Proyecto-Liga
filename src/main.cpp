#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <iomanip>

// ===================== STRUCTS =====================

struct Partido {
    std::string fecha;
    std::string equipo_local;
    std::string equipo_visitante;
    int goles_local;
    int goles_visitante;
};

struct Equipo {
    std::string nombre;
    int partidos_jugados;
    int partidos_ganados;
    int partidos_empatados;
    int partidos_perdidos;
    int goles_favor;
    int goles_contra;
    int diferencia_goles;
    int puntos;
};

// ===================== UTILIDADES =====================

std::string fecha_actual() {
    time_t ahora = time(0);
    tm* t = localtime(&ahora);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", t);
    return std::string(buffer);
}

std::string separar_texto(std::string texto, char cc) {
    std::string palabra_individual;
    std::stringstream separador(texto);
    getline(separador, palabra_individual, cc);
    getline(separador, palabra_individual, cc);
    return palabra_individual;
}

// ===================== ARCHIVOS =====================

int cargar_configuracion(std::string& titulo, int& puntos_victoria, int& puntos_empate, int& puntos_derrota, std::vector<std::string>& equipos) {
    std::ifstream configuracion("../../data/config.txt");

    if (!configuracion.is_open()) {
        std::cout << "Error: No se pudo abrir config.txt Verifique que el archivo existe." << std::endl;
        return 1;
    }

    std::string linea;

    if (!getline(configuracion, linea)) {
        std::cout << "Error: Formato invalido en config.txt" << std::endl;
        return 1;
    }
    titulo = separar_texto(linea, '=');

    if (!getline(configuracion, linea)) {
        std::cout << "Error: Falta el campo de puntos por victoria." << std::endl;
        return 1;
    }
    puntos_victoria = std::stoi(separar_texto(linea, '='));

    if (!getline(configuracion, linea)) {
        std::cout << "Error: Falta el campo de puntos por empate." << std::endl;
        return 1;
    }
    puntos_empate = std::stoi(separar_texto(linea, '='));

    if (!getline(configuracion, linea)) {
        std::cout << "Error: Falta el campo de puntos por derrota." << std::endl;
        return 1;
    }
    puntos_derrota = std::stoi(separar_texto(linea, '='));

    while (getline(configuracion, linea)) {
        // Ignorar comentarios
        if (!linea.empty() && linea[0] == '#') continue;
        std::string equipo = separar_texto(linea, '=');
        if (!equipo.empty()) {
            equipos.push_back(equipo);
        }
    }

    if (equipos.empty()) {
        std::cout << "Error: No se encontraron equipos en config.txt" << std::endl;
        return 1;
    }

    return 0;
}

// Lee todos los partidos desde partidos.txt y retorna un vector de Partido
std::vector<Partido> leer_partidos() {
    std::vector<Partido> partidos;
    std::ifstream archivo("../../data/partidos.txt");

    if (!archivo.is_open()) {
        return partidos;
    }

    std::string linea;
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        std::stringstream ss(linea);
        std::string dato;
        Partido p;

        getline(ss, p.fecha, ';');
        getline(ss, p.equipo_local, ';');
        getline(ss, p.equipo_visitante, ';');

        std::string goles_l, goles_v;
        getline(ss, goles_l, ';');
        getline(ss, goles_v, ';');

        p.goles_local = std::stoi(goles_l);
        p.goles_visitante = std::stoi(goles_v);

        partidos.push_back(p);
    }

    archivo.close();
    return partidos;
}

void guardar_partido_archivo(const Partido& p) {
    std::ofstream archivo("../../data/partidos.txt", std::ios::app);
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir partidos.txt para escritura." << std::endl;
        return;
    }
    archivo << p.fecha << ";" << p.equipo_local << ";" << p.equipo_visitante << ";"
            << p.goles_local << ";" << p.goles_visitante << "\n";
    archivo.close();
}

void guardar_jornada_archivo(const Partido& p, int& numero_jornadas, int& numero_partidos_jornada_actual, int total_equipos) {
    std::ofstream archivo("../../data/fechas.txt", std::ios::app);
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir fechas.txt para escritura." << std::endl;
        return;
    }
    archivo << p.fecha << ";" << p.equipo_local << ";" << p.equipo_visitante << ";"
            << p.goles_local << ";" << p.goles_visitante << "\n";
    numero_partidos_jornada_actual++;
    if (numero_partidos_jornada_actual == total_equipos / 2) {
        archivo << "FIN_JORNADA\n";
        numero_jornadas++;
        numero_partidos_jornada_actual = 0;
        archivo << "JORNADA=" << numero_jornadas << "\n";
    }
    archivo.close();
}

// ===================== LOGICA DE LA LIGA =====================

// Usa puntero para modificar directamente el equipo (uso obligatorio de punteros)
void actualizar_estadisticas(Equipo* equipo, int goles_favor, int goles_contra, int puntos_victoria, int puntos_empate, int puntos_derrota) {
    equipo->partidos_jugados++;
    equipo->goles_favor += goles_favor;
    equipo->goles_contra += goles_contra;
    equipo->diferencia_goles = equipo->goles_favor - equipo->goles_contra;

    if (goles_favor > goles_contra) {
        equipo->partidos_ganados++;
        equipo->puntos += puntos_victoria;
    } else if (goles_favor == goles_contra) {
        equipo->partidos_empatados++;
        equipo->puntos += puntos_empate;
    } else {
        equipo->partidos_perdidos++;
        equipo->puntos += puntos_derrota;
    }
}

// Construye el vector de Equipos a partir de los partidos y la configuracion
std::vector<Equipo> construir_tabla(const std::vector<std::string>& nombres_equipos,
                                    const std::vector<Partido>& partidos,
                                    int puntos_victoria, int puntos_empate, int puntos_derrota) {
    std::vector<Equipo> tabla;

    for (int i = 0; i < (int)nombres_equipos.size(); i++) {
        Equipo e;
        e.nombre = nombres_equipos[i];
        e.partidos_jugados = 0;
        e.partidos_ganados = 0;
        e.partidos_empatados = 0;
        e.partidos_perdidos = 0;
        e.goles_favor = 0;
        e.goles_contra = 0;
        e.diferencia_goles = 0;
        e.puntos = 0;
        tabla.push_back(e);
    }

    for (int i = 0; i < (int)partidos.size(); i++) {
        for (int j = 0; j < (int)tabla.size(); j++) {
            if (tabla[j].nombre == partidos[i].equipo_local) {
                actualizar_estadisticas(&tabla[j], partidos[i].goles_local, partidos[i].goles_visitante,
                                        puntos_victoria, puntos_empate, puntos_derrota);
            } else if (tabla[j].nombre == partidos[i].equipo_visitante) {
                actualizar_estadisticas(&tabla[j], partidos[i].goles_visitante, partidos[i].goles_local,
                                        puntos_victoria, puntos_empate, puntos_derrota);
            }
        }
    }

    return tabla;
}

// Ordena la tabla por puntos (mayor a menor)
void ordenar_tabla(std::vector<Equipo>& tabla) {
    for (int i = 0; i < (int)tabla.size() - 1; i++) {
        for (int j = i + 1; j < (int)tabla.size(); j++) {
            if (tabla[j].puntos > tabla[i].puntos) {
                Equipo temp = tabla[i];
                tabla[i] = tabla[j];
                tabla[j] = temp;
            }
        }
    }
}

void mostrar_tabla(const std::vector<Equipo>& tabla) {
    std::cout << std::left
              << std::setw(4)  << "#"
              << std::setw(25) << "Equipo"
              << std::setw(5)  << "PJ"
              << std::setw(5)  << "PG"
              << std::setw(5)  << "PE"
              << std::setw(5)  << "PP"
              << std::setw(5)  << "GF"
              << std::setw(5)  << "GC"
              << std::setw(6)  << "DG"
              << std::setw(5)  << "PTS"
              << std::endl;

    std::cout << std::string(70, '-') << std::endl;

    for (int i = 0; i < (int)tabla.size(); i++) {
        std::string dg = (tabla[i].diferencia_goles >= 0 ? "+" : "") + std::to_string(tabla[i].diferencia_goles);
        std::cout << std::left
                  << std::setw(4)  << (i + 1)
                  << std::setw(25) << tabla[i].nombre
                  << std::setw(5)  << tabla[i].partidos_jugados
                  << std::setw(5)  << tabla[i].partidos_ganados
                  << std::setw(5)  << tabla[i].partidos_empatados
                  << std::setw(5)  << tabla[i].partidos_perdidos
                  << std::setw(5)  << tabla[i].goles_favor
                  << std::setw(5)  << tabla[i].goles_contra
                  << std::setw(6)  << dg
                  << std::setw(5)  << tabla[i].puntos
                  << std::endl;
    }
}

// ===================== FUNCIONES DE INTERFAZ =====================

void registrar_partidos(std::vector<std::string>& equipos, int& numero_jornadas, int& numero_partidos_jornada_actual) {
    std::cout << "Estos son los equipos disponibles:" << std::endl;
    for (int pos = 0; pos < (int)equipos.size(); pos++) {
        std::cout << " - " << equipos[pos] << std::endl;
    }

    std::string EquipoLocal;
    std::cout << "Introduzca el equipo local:" << std::endl;
    std::getline(std::cin, EquipoLocal);

    std::string EquipoVisitante;
    std::cout << "Introduzca el equipo visitante:" << std::endl;
    std::getline(std::cin, EquipoVisitante);

    int puntaje_local, puntaje_visitante;
    std::cout << "Introduzca el puntaje del equipo local" << std::endl;
    std::cin >> puntaje_local;
    std::cout << "Introduzca el puntaje del equipo visitante" << std::endl;
    std::cin >> puntaje_visitante;

    // Validar que los equipos existan
    bool encontrado_local = false;
    for (int i = 0; i < (int)equipos.size(); i++) {
        if (equipos[i] == EquipoLocal) {
            encontrado_local = true;
        }
    }
    if (!encontrado_local) {
        std::cout << "No se encontro el equipo local ingresado" << std::endl;
        return;
    }

    bool encontrado_visitante = false;
    for (int i = 0; i < (int)equipos.size(); i++) {
        if (equipos[i] == EquipoVisitante) {
            encontrado_visitante = true;
        }
    }
    if (!encontrado_visitante) {
        std::cout << "No se encontro el equipo visitante ingresado" << std::endl;
        return;
    }

    // Validar que los equipos sean distintos
    if (EquipoLocal == EquipoVisitante) {
        std::cout << "El equipo local y el visitante no pueden ser el mismo." << std::endl;
        return;
    }

    // Validar puntajes no negativos
    if (puntaje_local < 0 || puntaje_visitante < 0) {
        std::cout << "Los puntajes no pueden ser negativos." << std::endl;
        return;
    }

    Partido p;
    p.fecha = fecha_actual();
    p.equipo_local = EquipoLocal;
    p.equipo_visitante = EquipoVisitante;
    p.goles_local = puntaje_local;
    p.goles_visitante = puntaje_visitante;

    guardar_partido_archivo(p);
    guardar_jornada_archivo(p, numero_jornadas, numero_partidos_jornada_actual, equipos.size());

    std::cout << "Partido registrado correctamente." << std::endl;
}

void ver_partidos() {
    std::ifstream partidos("../../data/partidos.txt");
    if (!partidos.is_open()) {
        std::cout << "Error: No se pudo abrir partidos.txt" << std::endl;
        return;
    }
    std::string linea;
    while (getline(partidos, linea)) {
        std::string dato;
        std::stringstream separador(linea);
        while (getline(separador, dato, ';')) {
            std::cout << dato << " | ";
        }
        std::cout << "\n";
    }
    partidos.close();
}

void fechas_partidos() {
    std::ifstream fechas("../../data/fechas.txt");
    if (!fechas.is_open()) {
        std::cout << "Error: No se pudo abrir fechas.txt" << std::endl;
        return;
    }
    std::string linea;
    while (getline(fechas, linea)) {
        if (linea.substr(0, 7) == "JORNADA") {
            std::cout << "=== JORNADA " << linea.substr(8, linea.size() - 8) << " ===\n";
        } else if (linea == "FIN_JORNADA") {
            std::cout << "=== FIN JORNADA ===\n";
        } else {
            std::string dato;
            std::stringstream separador(linea);
            while (getline(separador, dato, ';')) {
                std::cout << dato << " | ";
            }
            std::cout << "\n";
        }
    }
    fechas.close();
}

int mostrar_menu(const std::string& titulo) {
    int numero;
    std::cout << "\nMenu: " << titulo << std::endl;
    std::cout << "Tiene las siguientes opciones:" << std::endl;
    std::cout << "1. Ver tabla de posiciones" << std::endl;
    std::cout << "2. Registrar resultado de un partido" << std::endl;
    std::cout << "3. Ver historial de jornadas" << std::endl;
    std::cout << "4. Ver todos los partidos jugados" << std::endl;
    std::cout << "5. Salir" << std::endl;
    std::cout << "Escoger un numero: ";
    std::cin >> numero;
    return numero;
}

// ===================== MAIN =====================

int main() {
    std::string titulo;
    int puntos_victoria, puntos_empate, puntos_derrota;
    std::vector<std::string> equipos;

    if (cargar_configuracion(titulo, puntos_victoria, puntos_empate, puntos_derrota, equipos) != 0) {
        return 1;
    }

    int numero_jornadas = 0;
    int numero_partidos_jornada_actual = 0;
    std::string linea;
    std::ifstream fechas("../../data/fechas.txt");
    while (getline(fechas, linea)) {
        if (linea.substr(0, 7) == "JORNADA") {
            numero_jornadas++;
        } else if (linea == "FIN_JORNADA") {
            numero_partidos_jornada_actual = 0;
        } else {
            numero_partidos_jornada_actual++;
        }
    }
    fechas.close();

    if (numero_jornadas == 0) {
        std::ofstream init_fechas("../../data/fechas.txt");
        if (!init_fechas.is_open()) {
            std::cout << "Error: No se pudo crear fechas.txt" << std::endl;
            return 1;
        }
        init_fechas << "JORNADA=1\n";
        init_fechas.close();
        numero_jornadas++;
    }

    bool menu = true;
    do {
        int numero = mostrar_menu(titulo);

        switch (numero) {
            case 1: {
                std::cin.ignore(1000, '\n');
                std::vector<Partido> partidos = leer_partidos();
                std::vector<Equipo> tabla = construir_tabla(equipos, partidos, puntos_victoria, puntos_empate, puntos_derrota);
                ordenar_tabla(tabla);
                mostrar_tabla(tabla);
                break;
            }
            case 2: {
                std::cin.ignore(1000, '\n');
                registrar_partidos(equipos, numero_jornadas, numero_partidos_jornada_actual);
                break;
            }
            case 3: {
                std::cin.ignore(1000, '\n');
                fechas_partidos();
                break;
            }
            case 4: {
                std::cin.ignore(1000, '\n');
                ver_partidos();
                break;
            }
            case 5:
                menu = false;
                break;
            default:
                std::cin.ignore(1000, '\n');
                std::cout << "Opcion invalida, vuelva al inicio." << std::endl;
                break;
        }
    } while (menu);

    return 0;
}

