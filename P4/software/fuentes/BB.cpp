#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <random>
#include <chrono>
#include <ctime>
#include <algorithm>

using namespace std;

int ROWCOL = 20;
int FOODCOUNT = 40;
int MAXBEES = 200;
mt19937 rng(0);

class Bumblebee {
    vector<bool> solution;
    int lifespan;
    float cost;
    pair<int, int> position;

  public:
  	Bumblebee() {
  		cost = 0;	
	}
	
    Bumblebee(vector<bool> solution) {
        this->solution = solution;
    }

    int getLifespan() {
        return lifespan;
    }

    void setLifespan(int quantity) {
        lifespan = quantity;
    }

    float getCost() {
        return cost;
    }

    float setCost(float cost) {
        this->cost = cost;
    }

    vector<bool> getSolution() {
        return solution;
    }

    pair<int, int> getPosition() {
        return position;
    }

    bool getElement(int index) {
        return solution[index];
    }

    void changeLifespan(int quantity) {
        lifespan += quantity;

        if (lifespan > 100)
            lifespan = 100;
        else if (lifespan < 0)
            lifespan = 0;
    }

    bool mutate(int position1, int position2, bool &first) {
        bool valid = false;

        if (solution[position1] != solution[position2]) {
            solution[position1] = !solution[position2];
            solution[position2] = !solution[position2];
            valid = true;
        }
        
        if (solution[position1])
            first = false;
        else
            first = true;

        return valid;
    }

    void move(pair<int, int> movement) {
        position = movement;
    }
};

struct Cell {
    int food;
    bool taken;

    Cell() {
        food = 0;
        taken = false;
    }
};

pair<int, int> newCell(pair<int, int> pos, int movement) {
    pair<int, int> position = pos;
    switch (movement) {
        case 1:
            position.first += -2;
            position.second += -2;
            break;
        case 2:
            position.first += -2;
            position.second += -1;
            break;
        case 3:
            position.first += -2;
            break;
        case 4:
            position.first += -2;
            position.second += 1;
            break;
        case 5:
            position.first += -2;
            position.second += 2;
            break;
        case 6:
            position.first += -1;
            position.second += -2;
            break;
        case 7:
            position.first += -1;
            position.second += -1;
            break;
        case 8:
            position.first += -1;
            break;
        case 9:
            position.first += -1;
            position.second += 1;
            break;
        case 10:
            position.first += -1;
            position.second += 2;
            break;
        case 11:
            position.second += -2;
            break;
        case 12:
            position.second += -1;
            break;
        case 13:
            position.second += 1;
            break;
        case 14:
            position.second += 2;
            break;
        case 15:
            position.first += 1;
            position.second += -2;
            break;
        case 16:
            position.first += 1;
            position.second += -1;
            break;
        case 17:
            position.first += 1;
            break;
        case 18:
            position.first += 1;
            position.second += 1;
            break;
        case 19:
            position.first += 1;
            position.second += 2;
            break;
        case 20:
            position.first += 2;
            position.second += -2;
            break;
        case 21:
            position.first += 2;
            position.second += -1;
            break;
        case 22:
            position.first += 2;
            break;
        case 23:
            position.first += 2;
            position.second += 1;
            break;
        case 24:
            position.first += 2;
            position.second += 2;
            break;
    }

	if (position.first < 0) {
		position.first += ROWCOL;
	}
	if (position.second < 0) {
		position.second += ROWCOL;
	}
	
    position.first = position.first % ROWCOL;
    position.second = position.second % ROWCOL;
    return position;
}

int main() {
    ifstream archivo; // Archivo con las distancias
    string ruta, linea, substr;
    int i, j, k, n, m;
    float valor;
    bool valid;
    uniform_int_distribution<int> cellgen(0, pow(ROWCOL, 2) - 1);
    uniform_int_distribution<int> bestgen(0, 4);
    uniform_int_distribution<int> movegen(1, 24);
    chrono::time_point<chrono::system_clock> start, end; 
    float tiempoAcumulado;

    cout << "Archivo con casos: (QUIT para terminar)" << endl;
    cin >> ruta;

    while (ruta != "QUIT") {
        archivo.open(ruta);

        if (archivo.is_open()) {
            // Lee el n y el m de la primera linea
            getline(archivo, linea);
            stringstream ss(linea);
            getline(ss, substr, ' ');
            n = stoi(substr);
            getline(ss, substr);
            m = stoi(substr);
			
            // Reserva la memoria para guardar las distancias
            float** distancias = new float*[n];
            for (i = 0; i < n; i++)
                distancias[i] = new float[n];
            
            // Lee las distancias y las guarda
            while (getline(archivo, linea)) {
				stringstream ss2(linea);
				getline(ss2, substr, ' ');
				i = stoi(substr);
				getline(ss2, substr, ' ');
				j = stoi(substr);
				getline(ss2, substr);
				valor = stof(substr);
				distancias[i][j] = valor;
				distancias[j][i] = valor;
			}
				
			archivo.close();
            
            // Inicializa la diagonal a 0
            for (i = 0; i < n; i++)
                distancias[i][i] = 0;
			
            // Generador para la mutación
            uniform_int_distribution<int> mutationgen(0, n-1);
			
            // Genera el grid con la comida
            vector< vector<Cell> > grid;
            int contador = 0;
            int gen;

			for (i = 0; i < ROWCOL; i++) {
				vector<Cell> tmp;
				for (j = 0; j < ROWCOL; j++) {
					Cell c;
					tmp.push_back(c);
				}
				grid.push_back(tmp);
			}
			
            while (contador < FOODCOUNT) {
                gen = cellgen(rng);

                if (grid[gen%ROWCOL][gen/ROWCOL].food == 0) {
                    grid[gen%ROWCOL][gen/ROWCOL].food = 5;
                    contador++;
                }
            }
			
            // Crea una solucion viable (ocurrencias de 1 = m)
            vector<bool> solucion;
            for (i = 0; i < n; i++) {
                if (i < m)
                    solucion.push_back(true);
                else
                    solucion.push_back(false);
            }
			
            // Genera los abejorros con sus soluciones y posiciones
            vector<Bumblebee> poblacion;

            for (i = 0; i < MAXBEES; i++) {
                shuffle(solucion.begin(), solucion.begin() + n, rng);
                poblacion.push_back(solucion);
            }

            for (i = 0; i < MAXBEES; i++) {
                valid = false;

                while (!valid) {
                    gen = cellgen(rng);

                    if (!grid[gen%ROWCOL][gen/ROWCOL].taken) {
                        grid[gen%ROWCOL][gen/ROWCOL].taken = true;
                        poblacion[i].move(make_pair(gen%ROWCOL, gen/ROWCOL));
                        valid = true;
                    }
                }
            }
			
            start = chrono::system_clock::now();
            
            // Calcula los costes
            vector<float> costes;
            for (auto it = poblacion.begin(); it != poblacion.end(); ++it) {
            	vector<bool> temp;
                float costeActual = 0.0;
                
                temp = (*it).getSolution();
                
                for (j = 0; j < n; j++) {
                    for (k = j; k < n; k++) {
                        if (temp[j] && temp[k]) {
                            costeActual += distancias[j][k];
                        }
                    }
                }
                (*it).setCost(costeActual);
                costes.push_back(costeActual);
            }
			
            // Guarda las mejores soluciones
            float max1 = 0.0, max2 = 0.0, max3 = 0.0, max4 = 0.0, max5 = 0.0;
            vector<Bumblebee> mejores;
            
            for (i = 0; i < 5; i++) {
            	Bumblebee b;
            	mejores.push_back(b);
			}
                
            for (j = 0; j < costes.size(); j++) {
                if (costes[j] > max1) {
                    max5 = max4;
                    max4 = max3;
                    max3 = max2;
                    max2 = max1;
                    max1 = costes[j];
                    mejores[0] = poblacion[j];
                }
                else if (costes[j] > max2) {
                    max5 = max4;
                    max4 = max3;
                    max3 = max2;
                    max2 = costes[j];
                    mejores[1] = poblacion[j];
                }
                else if (costes[j] > max3) {
                    max5 = max4;
                    max4 = max3;
                    max3 = costes[j];
                    mejores[2] = poblacion[j];
                }
                else if (costes[j] > max4) {
                    max5 = max4;
                    max4 = costes[j];
                    mejores[3] = poblacion[j];
                }
                else if (costes[j] > max5) {
                    max5 = costes[j];
                    mejores[4] = poblacion[j];
                }
            }
			
            // Asigna lifespan
            for (j = 0; j < costes.size(); j++) {
                // Mappeo de valores (mejor, mejor/2) -> (100, 0)
                int lifespan = (costes[j] - (mejores[0].getCost()-1000)) * 100 / (mejores[0].getCost() - (mejores[0].getCost()-1000));
                if (lifespan < 0)
                    lifespan = 0;
                
                poblacion[j].setLifespan(lifespan);
            }    
			
            // Comienzan las generaciones
            for (i = 1; i <= 1000 && poblacion.size() > 0; i++) {
                // Baja el lifespan de todos los abejorros y borra las abejas que hayan agotado su lifespan.
                j = 0;
                for (auto it = poblacion.begin(); it != poblacion.end(); ++it) {
                    (*it).changeLifespan(-1);
                    
                    if (j == poblacion.size())
                    	break;

                    if ((*it).getLifespan() <= 0) {
                        poblacion.erase(poblacion.begin() + j);
                        costes.erase(costes.begin() + j);
                        j--;
                    }
                    j++;
                }
				
                // Cada 40 generaciones crea una nueva abeja
                if (i % 40 == 0) {
                    int index = bestgen(rng);
                    
					poblacion.push_back(mejores[index]);
					
					valid = false;
	                while (!valid) {
	                    gen = cellgen(rng);
	
	                    if (!grid[gen%ROWCOL][gen/ROWCOL].taken) {
	                        grid[gen%ROWCOL][gen/ROWCOL].taken = true;
	                        poblacion[poblacion.size()-1].move(make_pair(gen%ROWCOL, gen/ROWCOL));
	                        valid = true;
	                    }
	                }
                
                    costes.push_back(mejores[index].getCost());
                }

                // Todas las abejas se mueven
                for (auto it = poblacion.begin(); it != poblacion.end(); ++it) {
                    valid = false;
                    pair<int, int> nuevaCasilla, anteriorCasilla;

                    // Comprueba que la casilla no está ocupada
                    while (!valid) {
                    	int movimiento = movegen(rng);
                        nuevaCasilla = newCell((*it).getPosition(), movimiento);
                        
                        if (!grid[nuevaCasilla.first][nuevaCasilla.second].taken) {
                            anteriorCasilla = (*it).getPosition();
                            grid[anteriorCasilla.first][anteriorCasilla.second].taken = false;
                            (*it).move(nuevaCasilla);
                            grid[nuevaCasilla.first][nuevaCasilla.second].taken = true;
                            valid = true;
                        }
                    }
                    
                    // Si la casilla tiene comida, aumenta el lifespan del abejorro
                    if (grid[nuevaCasilla.first][nuevaCasilla.second].food > 0) {
                        (*it).changeLifespan(6);
                        grid[nuevaCasilla.first][nuevaCasilla.second].food--;
                    }
                }

                // Todas las abejas mutan
                k = 0;
                for (auto it = poblacion.begin(); it != poblacion.end(); ++it) {
                    int index1, index2;
                    valid = false;
                    bool first = true;
                    
                    // Intercambia 2 genes al azar
                    while (!valid) {
                        index1 = mutationgen(rng);
                        index2 = mutationgen(rng);

                        while (index1 == index2)
                            index2 = mutationgen(rng);
                        
                        valid = (*it).mutate(index1, index2, first);
                    }

					// Recalcula el coste
                    int nuevoCoste;
                    vector<bool> solucion;
                    solucion = (*it).getSolution();
                    nuevoCoste = (*it).getCost();

                    if (first) {
                        for (j = 0; j < n; j++) {
                            if (solucion[j] && j != index1 && j != index2) {
                                nuevoCoste = nuevoCoste - distancias[j][index1] + distancias[j][index2];
                            }
                        }
                    }
                    else {
                        for (j = 0; j < n; j++) {
                            if (solucion[j] && j != index1 && j != index2) {
                                nuevoCoste = nuevoCoste - distancias[j][index2] + distancias[j][index1];
                            }
                        }
                    }

                    costes[k] = nuevoCoste;

                    // Si mejoran el coste viven más, si no baja.
                    if (nuevoCoste > (*it).getCost())
                        (*it).changeLifespan(2);
                    else if (nuevoCoste < (*it).getCost())
                        (*it).changeLifespan(-1);
                    
                    k++;
                }

                // Guarda las mejores soluciones
                for (j = 0; j < costes.size(); j++) {
                    if (costes[j] > max1) {
                        max5 = max4;
                        max4 = max3;
                        max3 = max2;
                        max2 = max1;
                        max1 = costes[j];
                        mejores[0] = poblacion[j];
                    }
                    else if (costes[j] > max2) {
                        max5 = max4;
                        max4 = max3;
                        max3 = max2;
                        max2 = costes[j];
                        mejores[1] = poblacion[j];
                    }
                    else if (costes[j] > max3) {
                        max5 = max4;
                        max4 = max3;
                        max3 = costes[j];
                        mejores[2] = poblacion[j];
                    }
                    else if (costes[j] > max4) {
                        max5 = max4;
                        max4 = costes[j];
                        mejores[3] = poblacion[j];
                    }
                    else if (costes[j] > max5) {
                        max5 = costes[j];
                        mejores[4] = poblacion[j];
                    }
                }
            }

            end = chrono::system_clock::now();
			chrono::duration<double> elapsed_seconds = end - start;

            // Muestra los datos por pantalla
			cout << endl;
            cout << "--- Mejor caso: " << fixed << mejores[0].getCost() << endl;
			cout << "--- Tiempo medio (ms): " << fixed << chrono::duration_cast<chrono::milliseconds>(elapsed_seconds).count() << endl;
			cout << endl;
        }
        else
            cout << "No se ha podido abrir el archivo." << endl;

        cout << "Archivo con casos: (QUIT para terminar)" << endl;
		cin >> ruta;
    }
}
