#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>

std::string fecha_actual(){
    return "Hoy";
}

std::string separar_texto(std::string texto, char cc){
    std::string palabra_indivual;
    std::stringstream separador(texto);
    getline(separador, palabra_indivual, cc);
    getline(separador, palabra_indivual, cc);

    return palabra_indivual;
}

int cargar_configuracion(std::string& titulo, int& puntos_victoria, int& puntos_empate, int& puntos_derrota, std::vector<std::string>& equipos){
    std::ifstream configuracion("../data/config.txt");

    if (!configuracion.is_open()){
        std::cout << "Error: No se pudo abrir config.txt Verifique que el archivo existe." << std::endl;
        return 1;
    }

    std::string linea;

    if (!getline(configuracion, linea)){
        std::cout << "Error: Formato invalido en config.txt" << std::endl;
        return 1;
    }
    titulo = separar_texto(linea, '=');
    if (!getline(configuracion, linea)){
        std::cout << "Error: Falta el campo de puntos por victoria." << std::endl;
        return 1;
    }
    puntos_victoria = std::stoi(separar_texto(linea, '='));

    if (!getline(configuracion, linea)){
        std::cout << "Error: Falta el campo de puntos por empate." << std::endl;
        return 1;
    }
    puntos_empate = std::stoi(separar_texto(linea, '='));

    if (!getline(configuracion, linea)){
        std::cout << "Error: Falta el campo de puntos por derrota." << std::endl;
        return 1;
    }
    puntos_derrota = std::stoi(separar_texto(linea, '='));

    while(getline(configuracion, linea)){
        std::string equipo = separar_texto(linea, '=');
        if (!equipo.empty()){
            equipos.push_back(equipo);
        }
    }

    if (equipos.empty()){
        std::cout << "Error: No se encontraron equipos en config.txt" << std::endl;
        return 1;
    }

}

void registrar_partidos(std::vector<std::string>& equipos, int& numero_jornadas, int& numero_partidos_jornada_actual){
    std::cout << "Estos son los equipos disponibles:" << std::endl;
    for(int pos = 0; pos < equipos.size(); pos++){
        std::cout << " - " << equipos[pos] << std::endl;
    }

    std::string EquipoLocal;
    std::cout<< "Introduzca el equipo local:" << std::endl;
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
    for(int i=0; i<equipos.size(); i++){
        if(equipos[i] == EquipoLocal){
            encontrado_local = true;
        }
    }

    if(encontrado_local != true){
        std::cout<< "No se encontro el equipo local ingresado" <<std::endl;
        return;
    }

    bool encontrado_visitante = false;
    for(int i=0; i<equipos.size(); i++){
        if(equipos[i] == EquipoVisitante){
            encontrado_visitante = true;
        }
    }

    if(encontrado_visitante != true){
        std::cout<< "No se encontro el equipo visitante ingresado" << std::endl;
        return;
    }

    
    if(!(puntaje_local >= 0 && puntaje_visitante >= 0)){
        std:: cout<< "No es valido"<< std::endl;
        return;
    }

    

    std::ofstream partidos("../data/partidos.txt", std::ios::app); 
    partidos << fecha_actual() << ";" << EquipoLocal << ";" << EquipoVisitante << ";" << puntaje_local << ";" << puntaje_visitante << "\n";
    partidos.close();

    std::ofstream fecha ("../data/fechas.txt", std::ios::app);
    fecha << fecha_actual() << ";" << EquipoLocal << ";" << EquipoVisitante << ";" << puntaje_local << ";" << puntaje_visitante << "\n";
    numero_partidos_jornada_actual++;
    if (numero_partidos_jornada_actual == equipos.size()/2){
       fecha << "FIN_JORNADA\n";
       numero_jornadas++; 
       numero_partidos_jornada_actual = 0;
       fecha << "JORNADA=" << numero_jornadas <<"\n";
    }
    fecha.close();
}   

void ver_partidos(){
    std::ifstream partidos ("../data/partidos.txt");
    std::string linea;
    while(getline(partidos, linea)){
        std::string dato;
        std::stringstream separador (linea);
        while(getline(separador,dato, ';')){
            std::cout <<dato << " | ";
        }
        std::cout << "\n";
    }
    partidos.close();
}    

void fechas_partidos(){
    std::ifstream fechas ("../data/fechas.txt");
    std::string linea;
    while(getline (fechas, linea)){
        if (linea.substr(0, 7) == "JORNADA"){
            std::cout << "=== JORNADA " << linea.substr(8, linea.size() - 8) << " ===\n";
        } 
        else if (linea == "FIN_JORNADA"){
            std::cout << "=== FIN JORNADA ===\n";
        }
        else{
            std::string dato;
            std::stringstream separador(linea);
            while(getline(separador,dato, ';')){
                std::cout <<dato << " | ";
            }
            std::cout << "\n";
        }
    }  
    fechas.close();     
}


int main(){
    std::string titulo; 
    int puntos_victoria, puntos_empate, puntos_derrota;
    std::vector<std::string> equipos;
    cargar_configuracion(titulo, puntos_victoria, puntos_empate, puntos_derrota, equipos);
    
    int numero_jornadas = 0;
    int numero_partidos_jornada_actual = 0;
    std::string linea;
    std::ifstream fechas ("../data/fechas.txt");
    while (getline(fechas, linea)){       
        if(linea.substr(0,7) == "JORNADA"){
            numero_jornadas++;
        }
        else if(linea == "FIN_JORNADA"){
            numero_partidos_jornada_actual = 0;
        }
        else{
            numero_partidos_jornada_actual++;
        }
    }
    if(numero_jornadas == 0){
        std::ofstream fechas ("../data/fechas.txt");    
        fechas << "JORNADA=1\n";
        numero_jornadas++;
    }
    

    bool menu = true;
    do{
        int numero;
        std::cout << "Menu:"<< titulo << std::endl;
        std::cout << "Tiene las siguientes opciones:" << std::endl;
        std::cout << "1. Ver tabla de posiciones" << std::endl;
        std::cout << "2. Registrar resultado de un partido" << std::endl;
        std::cout << "3. Ver historial de jornadas" << std::endl;
        std::cout << "4. Ver todos los partidos jugados" << std::endl;
        std::cout << "5. Salir" << std::endl;

        std::cout << "Escoger un numero:"<< std::endl;
        std::cin >> numero;

        switch (numero){
            case 1:
                //
                break;
            case 2:{
                std::cin.ignore(1000, '\n');
                registrar_partidos(equipos, numero_jornadas, numero_partidos_jornada_actual);
                break;
            }
            case 3:{
                std::cin.ignore(1000, '\n');
                fechas_partidos();
                break;
            }
            case 4:{
                std::cin.ignore(1000, '\n');
                ver_partidos();
                break;
            }
            case 5:
                menu = false;
                break;
            default: 
                std::cin.ignore(1000, '\n');
                std::cout << "Volver al inicio" << std::endl;
                break;
        }
    } while(menu);

    return 0;
}