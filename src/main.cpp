#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>

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

// Rellena un string con espacios hasta el ancho deseado
std::string columna(std::string texto, int ancho) {
    if ((int)texto.size() < ancho) {
        texto += std::string(ancho - texto.size(), ' ');
    }
    return texto;
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
    std::ifstream archivo("../../data/partidos.txt");

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
    equipo->goles_favor  += goles_favor;
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

std::vector<Equipo> construir_tabla(const std::vector<std::string>& nombres_equipos,
                                    const std::vector<Partido>& partidos,
                                    int puntos_victoria, int puntos_empate, int puntos_derrota) {
    std::vector<Equipo> tabla;

    for (int i = 0; i < (int)nombres_equipos.size(); i++) {
        Equipo e;
        e.nombre             = nombres_equipos[i];
        e.partidos_jugados   = 0;
        e.partidos_ganados   = 0;
        e.partidos_empatados = 0;
        e.partidos_perdidos  = 0;
        e.goles_favor        = 0;
        e.goles_contra       = 0;
        e.diferencia_goles   = 0;
        e.puntos             = 0;
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

// Ordena por puntos, desempata por diferencia de goles y luego por goles a favor
void ordenar_tabla(std::vector<Equipo>& tabla) {
    for (int i = 0; i < (int)tabla.size() - 1; i++) {
        for (int j = i + 1; j < (int)tabla.size(); j++) {
            bool intercambiar = false;

            if (tabla[j].puntos > tabla[i].puntos) {
                intercambiar = true;
            } else if (tabla[j].puntos == tabla[i].puntos) {
                if (tabla[j].diferencia_goles > tabla[i].diferencia_goles) {
                    intercambiar = true;
                } else if (tabla[j].diferencia_goles == tabla[i].diferencia_goles) {
                    if (tabla[j].goles_favor > tabla[i].goles_favor) {
                        intercambiar = true;
                    }
                }
            }

            if (intercambiar) {
                Equipo temp = tabla[i];
                tabla[i]    = tabla[j];
                tabla[j]    = temp;
            }
        }
    }
}
// Imprime una fila de la tabla usando columna() en vez de setw
void imprimir_fila(int pos, const Equipo& e) {
    std::string dg = (e.diferencia_goles >= 0 ? "+" : "") + std::to_string(e.diferencia_goles);
    std::cout << columna(std::to_string(pos), 4)
              << columna(e.nombre, 25)
              << columna(std::to_string(e.partidos_jugados), 5)
              << columna(std::to_string(e.partidos_ganados), 5)
              << columna(std::to_string(e.partidos_empatados), 5)
              << columna(std::to_string(e.partidos_perdidos), 5)
              << columna(std::to_string(e.goles_favor), 5)
              << columna(std::to_string(e.goles_contra), 5)
              << columna(dg, 6)
              << columna(std::to_string(e.puntos), 5)
              << std::endl;
}

void imprimir_encabezado() {
    std::cout << columna("#", 4)
              << columna("Equipo", 25)
              << columna("PJ", 5)
              << columna("PG", 5)
              << columna("PE", 5)
              << columna("PP", 5)
              << columna("GF", 5)
              << columna("GC", 5)
              << columna("DG", 6)
              << columna("PTS", 5)
              << std::endl;
    std::cout << std::string(70, '-') << std::endl;
}

void mostrar_tabla(const std::vector<Equipo>& tabla) {
    imprimir_encabezado();
    for (int i = 0; i < (int)tabla.size(); i++) {
        imprimir_fila(i + 1, tabla[i]);
    }
}

void mostrar_top3(const std::vector<Equipo>& tabla) {
    if (tabla.empty()) {
        std::cout << "No hay partidos registrados aun." << std::endl;
        return;
    }

    int limite = (int)tabla.size() < 3 ? (int)tabla.size() : 3;

    std::cout << "\n=== TOP 3 ===" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    imprimir_encabezado();
    for (int i = 0; i < limite; i++) {
        imprimir_fila(i + 1, tabla[i]);
    }
    std::cout << std::string(70, '=') << std::endl;
}

void exportar_tabla(const std::vector<Equipo>& tabla) {
    std::ofstream archivo("../../data/tabla.txt");
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo crear tabla.txt" << std::endl;
        return;
    }

    archivo << columna("#", 4)
            << columna("Equipo", 25)
            << columna("PJ", 5)
            << columna("PG", 5)
            << columna("PE", 5)
            << columna("PP", 5)
            << columna("GF", 5)
            << columna("GC", 5)
            << columna("DG", 6)
            << columna("PTS", 5)
            << "\n";
    archivo << std::string(70, '-') << "\n";

    for (int i = 0; i < (int)tabla.size(); i++) {
        std::string dg = (tabla[i].diferencia_goles >= 0 ? "+" : "") + std::to_string(tabla[i].diferencia_goles);
        archivo << columna(std::to_string(i + 1), 4)
                << columna(tabla[i].nombre, 25)
                << columna(std::to_string(tabla[i].partidos_jugados), 5)
                << columna(std::to_string(tabla[i].partidos_ganados), 5)
                << columna(std::to_string(tabla[i].partidos_empatados), 5)
                << columna(std::to_string(tabla[i].partidos_perdidos), 5)
                << columna(std::to_string(tabla[i].goles_favor), 5)
                << columna(std::to_string(tabla[i].goles_contra), 5)
                << columna(dg, 6)
                << columna(std::to_string(tabla[i].puntos), 5)
                << "\n";
    }

    archivo.close();
    std::cout << "Tabla exportada correctamente a tabla.txt" << std::endl;
}

// Permite corregir el marcador de un partido ya registrado
void editar_resultado(const std::vector<std::string>& equipos) {
    std::vector<Partido> partidos = leer_partidos();

    if (partidos.empty()) {
        std::cout << "No hay partidos registrados aun." << std::endl;
        return;
    }

    // Mostrar todos los partidos numerados
    std::cout << "\n=== PARTIDOS REGISTRADOS ===" << std::endl;
    for (int i = 0; i < (int)partidos.size(); i++) {
        std::cout << (i + 1) << ". "
                  << partidos[i].fecha << "  |  "
                  << partidos[i].equipo_local << " "
                  << partidos[i].goles_local << " - "
                  << partidos[i].goles_visitante << " "
                  << partidos[i].equipo_visitante << std::endl;
    }

    std::cout << "Ingrese el numero del partido a editar (0 para cancelar): ";
    int seleccion;
    std::cin >> seleccion;
    std::cin.ignore(1000, '\n');

    if (seleccion == 0) return;

    if (seleccion < 1 || seleccion > (int)partidos.size()) {
        std::cout << "Numero de partido invalido." << std::endl;
        return;
    }

    int indice = seleccion - 1;
    std::cout << "Partido seleccionado: "
              << partidos[indice].equipo_local << " "
              << partidos[indice].goles_local << " - "
              << partidos[indice].goles_visitante << " "
              << partidos[indice].equipo_visitante << std::endl;

    int nuevo_local, nuevo_visitante;
    std::cout << "Nuevo puntaje " << partidos[indice].equipo_local << ": ";
    std::cin >> nuevo_local;
    std::cout << "Nuevo puntaje " << partidos[indice].equipo_visitante << ": ";
    std::cin >> nuevo_visitante;
    std::cin.ignore(1000, '\n');

    if (nuevo_local < 0 || nuevo_visitante < 0) {
        std::cout << "Los puntajes no pueden ser negativos." << std::endl;
        return;
    }

    // Aplicar el cambio en memoria
    partidos[indice].goles_local      = nuevo_local;
    partidos[indice].goles_visitante  = nuevo_visitante;

    // Reescribir partidos.txt completo con el partido corregido
    std::ofstream archivo("../../data/partidos.txt");
    if (!archivo.is_open()) {
        std::cout << "Error: No se pudo abrir partidos.txt para escritura." << std::endl;
        return;
    }
    for (int i = 0; i < (int)partidos.size(); i++) {
        archivo << partidos[i].fecha << ";"
                << partidos[i].equipo_local << ";"
                << partidos[i].equipo_visitante << ";"
                << partidos[i].goles_local << ";"
                << partidos[i].goles_visitante << "\n";
    }
    archivo.close();

    std::cout << "Resultado actualizado correctamente." << std::endl;
}

// ===================== FUNCIONES DE INTERFAZ =====================

int mostrar_menu(const std::string& titulo) {
    int numero;
    std::cout << "\nMenu: " << titulo << std::endl;
    std::cout << "Tiene las siguientes opciones:" << std::endl;
    std::cout << "1. Ver tabla de posiciones" << std::endl;
    std::cout << "2. Registrar resultado de un partido" << std::endl;
    std::cout << "3. Ver historial de jornadas" << std::endl;
    std::cout << "4. Ver todos los partidos jugados" << std::endl;
    std::cout << "5. Ver Top 3 de la tabla" << std::endl;
    std::cout << "6. Exportar tabla a tabla.txt" << std::endl;
    std::cout << "7. Historial de enfrentamientos" << std::endl;
    std::cout << "8. Editar resultado de un partido" << std::endl;
    std::cout << "9. Salir" << std::endl;
    std::cout << "Escoger un numero: ";
    std::cin >> numero;
    return numero;
}

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

    if (EquipoLocal == EquipoVisitante) {
        std::cout << "El equipo local y el visitante no pueden ser el mismo." << std::endl;
        return;
    }

    if (puntaje_local < 0 || puntaje_visitante < 0) {
        std::cout << "Los puntajes no pueden ser negativos." << std::endl;
        return;
    }

    // Deteccion de partido duplicado en la jornada actual
    std::vector<Partido> partidos_existentes = leer_partidos();
    // Contar cuantos partidos hay en la jornada actual para saber donde empieza
    int partidos_jornadas_anteriores = 0;
    std::ifstream fechas_check("../../data/fechas.txt");
    std::string linea_check;
    int jornada_actual = 1;
    int contador_jornada = 0;
    while (getline(fechas_check, linea_check)) {
        if (linea_check == "FIN_JORNADA") {
            partidos_jornadas_anteriores += contador_jornada;
            contador_jornada = 0;
            jornada_actual++;
        } else if (linea_check.substr(0, 7) != "JORNADA" && !linea_check.empty()) {
            contador_jornada++;
        }
    }
    fechas_check.close();

    // Revisar solo los partidos de la jornada actual
    for (int i = partidos_jornadas_anteriores; i < (int)partidos_existentes.size(); i++) {
        bool mismo_enfrentamiento =
            (partidos_existentes[i].equipo_local == EquipoLocal && partidos_existentes[i].equipo_visitante == EquipoVisitante) ||
            (partidos_existentes[i].equipo_local == EquipoVisitante && partidos_existentes[i].equipo_visitante == EquipoLocal);
        if (mismo_enfrentamiento) {
            std::cout << "Este enfrentamiento ya fue registrado en la jornada actual." << std::endl;
            return;
        }
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

void historial_enfrentamientos(const std::vector<std::string>& equipos) {
    std::cout << "Estos son los equipos disponibles:" << std::endl;
    for (int i = 0; i < (int)equipos.size(); i++) {
        std::cout << " - " << equipos[i] << std::endl;
    }

    std::string equipo1, equipo2;
    std::cout << "Introduzca el primer equipo:" << std::endl;
    std::getline(std::cin, equipo1);
    std::cout << "Introduzca el segundo equipo:" << std::endl;
    std::getline(std::cin, equipo2);

    bool encontrado1 = false, encontrado2 = false;
    for (int i = 0; i < (int)equipos.size(); i++) {
        if (equipos[i] == equipo1) encontrado1 = true;
        if (equipos[i] == equipo2) encontrado2 = true;
    }
    if (!encontrado1 || !encontrado2) {
        std::cout << "Uno o ambos equipos no existen en la liga." << std::endl;
        return;
    }
    if (equipo1 == equipo2) {
        std::cout << "Los dos equipos deben ser distintos." << std::endl;
        return;
    }

    std::vector<Partido> partidos = leer_partidos();
    int victorias1 = 0, victorias2 = 0, empates = 0;
    bool hay_partidos = false;

    std::cout << "\n=== HISTORIAL: " << equipo1 << " vs " << equipo2 << " ===" << std::endl;
    std::cout << std::string(70, '-') << std::endl;

    for (int i = 0; i < (int)partidos.size(); i++) {
        bool es_enfrentamiento =
            (partidos[i].equipo_local == equipo1 && partidos[i].equipo_visitante == equipo2) ||
            (partidos[i].equipo_local == equipo2 && partidos[i].equipo_visitante == equipo1);

        if (!es_enfrentamiento) continue;

        hay_partidos = true;

        int goles_e1, goles_e2;
        if (partidos[i].equipo_local == equipo1) {
            goles_e1 = partidos[i].goles_local;
            goles_e2 = partidos[i].goles_visitante;
        } else {
            goles_e1 = partidos[i].goles_visitante;
            goles_e2 = partidos[i].goles_local;
        }

        std::string resultado;
        if (goles_e1 > goles_e2) {
            resultado = "Gana " + equipo1;
            victorias1++;
        } else if (goles_e1 < goles_e2) {
            resultado = "Gana " + equipo2;
            victorias2++;
        } else {
            resultado = "Empate";
            empates++;
        }

        std::cout << partidos[i].fecha << "  |  "
                  << equipo1 << " " << goles_e1 << " - " << goles_e2 << " " << equipo2
                  << "  |  " << resultado << std::endl;
    }

    if (!hay_partidos) {
        std::cout << "Estos equipos aun no se han enfrentado." << std::endl;
        return;
    }

    std::cout << std::string(70, '-') << std::endl;
    std::cout << equipo1 << ": " << victorias1 << " victorias  |  "
              << equipo2 << ": " << victorias2 << " victorias  |  "
              << "Empates: " << empates << std::endl;
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
            case 5: {
                std::cin.ignore(1000, '\n');
                std::vector<Partido> partidos = leer_partidos();
                std::vector<Equipo> tabla = construir_tabla(equipos, partidos, puntos_victoria, puntos_empate, puntos_derrota);
                ordenar_tabla(tabla);
                mostrar_top3(tabla);
                break;
            }
            case 6: {
                std::cin.ignore(1000, '\n');
                std::vector<Partido> partidos = leer_partidos();
                std::vector<Equipo> tabla = construir_tabla(equipos, partidos, puntos_victoria, puntos_empate, puntos_derrota);
                ordenar_tabla(tabla);
                exportar_tabla(tabla);
                break;
            }
            case 7: {
                std::cin.ignore(1000, '\n');
                historial_enfrentamientos(equipos);
                break;
            }
            case 8: {
                std::cin.ignore(1000, '\n');
                editar_resultado(equipos);
                break;
            }
            case 9:
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