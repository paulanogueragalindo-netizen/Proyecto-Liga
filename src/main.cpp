#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

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

int main(){
    std::string titulo; 
    int puntos_victoria, puntos_empate, puntos_derrota;
    std::vector<std::string> equipos;

    cargar_configuracion(titulo, puntos_victoria, puntos_empate, puntos_derrota, equipos);

    // std::cout << "Liga: " << titulo << std::endl;
    // std::cout << "Puntos victoria: " << puntos_victoria << std::endl;
    // std::cout << "Puntos empate:   " << puntos_empate << std::endl;
    // std::cout << "Puntos derrota:  " << puntos_derrota << std::endl;
    // std::cout << "\nEquipos (" << equipos.size() << "):" << std::endl; // Equipos (4)
    
    // for(int pos = 0; pos < equipos.size(); pos++){
    //     std::cout << " - " << equipos[pos] << std::endl;
    // }

    // - Millos
    // - Nacional

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

        // switch tomar una variable/expresion y hacer casos dependiendo de su valor

        switch (numero){
            case 1:
                //
                break;
            case 2:{
                std::cin.ignore(1000, '\n');
                std::cout << "Estos son los equipos disponibles:" << std::endl;
                for(int pos = 0; pos < equipos.size(); pos++){
                    std::cout << " - " << equipos[pos] << std::endl;
                }
                // Equipo local
                std::string EquipoLocal;
                std::cout<< "Introduzca el equipo local:" << std::endl;
                std::getline(std::cin, EquipoLocal);
                
                // Equipo visitante
                std::string EquipoVisitante;
                std::cout << "Introduzca el equipo visitante:" << std::endl;
                std::getline(std::cin, EquipoVisitante);
                
                // Marcador
                int puntaje_local, puntaje_visitante;
                std::cout << "Introduzca el puntaje del equipo local" << std::endl;
                std::cin >> puntaje_local;
                std::cout << "Introduzca el puntaje el equipo visitante" << std::endl;
                std::cin >> puntaje_visitante;
                
                // Registro en el archivo partidos.txt
                std::ofstream File_2("../data/partidos.txt", std::ios::app); 
                File_2 << EquipoLocal << ";" << EquipoVisitante << ";" << puntaje_local << ";" << puntaje_visitante << "\n";
                break;
            }
            case 3:

                break;
            case 4:

                break;
            case 5:
                menu = false;
                break;
            default: 
                std::cout << "Volver al inicio" << std::endl;
                break;
        }
    } while(menu);
    


    return 0;
}