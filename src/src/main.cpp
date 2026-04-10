#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>

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

int cargar_configuracion(std::string& titulo, int& puntos_victoria, int& puntos_empate, int& puntos_derrota, std::vector<std::string>& equipos) {
    std::ifstream configuracion("../data/config.txt");

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

std::vector<Partido> leer_partidos() {
    std::vector<Partido> partidos;
    std::ifstream archivo("../data/partidos.txt");

    if (!archivo.is_open()) {
        return partidos;
    }

    std::string linea;
    while (getline(archivo, linea)) {
        if (linea.empty()) continue;
        std::stringstream ss(linea);
        Partido p;

        getline(ss, p.fecha, ';');
        getline(ss, p.equipo_local, ';');
        getline(ss, p.equipo_visitante, ';');

        std::string goles_l, goles_v;
        getline(ss, goles_l, ';');
        getline(ss, goles_v, ';');

        p.goles_local     = std::stoi(goles_l);
        p.goles_visitante = std::stoi(goles_v);

        partidos.push_back(p);
    }

    archivo.close();
    return partidos;
}

void guardar_partido_archivo(const Partido& p) {
    std::ofstream archivo("../data/partidos.txt", std::ios::app);
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir partidos.txt para escritura." << std::endl;
        return;
    }
    archivo << p.fecha << ";" << p.equipo_local << ";" << p.equipo_visitante << ";"
            << p.goles_local << ";" << p.goles_visitante << "\n";
    archivo.close();
}

void guardar_jornada_archivo(const Partido& p, int& numero_jornadas, int& numero_partidos_jornada_actual, int total_equipos) {
    std::ofstream archivo("../data/fechas.txt", std::ios::app);
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

void registrar_partidos(std::vector<std::string>& equipos, int& numero_jornadas, int& numero_partidos_jornada_actual) {
    std::cout << "Estos son los equipos disponibles:" << std::endl;
    for (int pos = 0; pos < equipos.size(); pos++) {
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
    std::cout << "Introduzca el puntaje el equipo visitante" << std::endl;
    std::cin >> puntaje_visitante;

    bool encontrado_local = false;
    for (int i = 0; i < equipos.size(); i++) {
        if (equipos[i] == EquipoLocal) {
            encontrado_local = true;
        }
    }
    if (encontrado_local != true) {
        std::cout << "No se encontro el equipo local ingresado" << std::endl;
        return;
    }

    bool encontrado_visitante = false;
    for (int i = 0; i < equipos.size(); i++) {
        if (equipos[i] == EquipoVisitante) {
            encontrado_visitante = true;
        }
    }
    if (encontrado_visitante != true) {
        std::cout << "No se encontro el equipo visitante ingresado" << std::endl;
        return;
    }

    if (!(puntaje_local >= 0 && puntaje_visitante >= 0)) {
        std::cout << "No es valido" << std::endl;
        return;
    }

    Partido p;
    p.fecha            = fecha_actual();
    p.equipo_local     = EquipoLocal;
    p.equipo_visitante = EquipoVisitante;
    p.goles_local      = puntaje_local;
    p.goles_visitante  = puntaje_visitante;

    guardar_partido_archivo(p);
    guardar_jornada_archivo(p, numero_jornadas, numero_partidos_jornada_actual, equipos.size());

    std::cout << "Partido registrado correctamente." << std::endl;
}

void ver_partidos() {
    std::ifstream partidos("../data/partidos.txt");
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
    std::ifstream fechas("../data/fechas.txt");
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
    std::ifstream fechas("../data/fechas.txt");
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
        std::ofstream init_fechas("../data/fechas.txt");
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
            case 1:
                // Me falta aun
                break;
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