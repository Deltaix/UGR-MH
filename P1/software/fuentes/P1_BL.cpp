#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <random>
#include <ctime>
#include <chrono> 

using namespace std;

int main() {
	ifstream archivo; // Archivo con las distancias
	string ruta, linea, substr;
	vector<int> indices; // Vector con los indices, que se barajara despues
	vector<int> elementos; // Vector con los elementos no seleccionados
	vector<int> seleccion; // Vector con los elementos seleccionados
	vector<int> contribucion; // Vector con la contribucion de cada elemento seleccionado
	vector<int>::iterator it, it2;
	int i, j, n, m;
	int ejecuciones;
	int indice;
	int iteraciones;
	int limite;
	double suma, diff, max, min, maxDist;
	double sumaMax, sumaMin, sumaAcumulada, tiempoAcumulado;
	float valor;
	bool cambio;
	chrono::time_point<chrono::system_clock> start, end; 
	
	cout << "Archivo con casos: (QUIT para terminar)" << endl;
	cin >> ruta;
	
	while (ruta != "QUIT") {
		archivo.open(ruta);
		
		if (archivo.is_open()) {
			cout << "Numero de iteraciones: " << endl;
			cin >> limite;
			
			// Lee el n y el m de la primera linea
			getline(archivo, linea);
			stringstream ss(linea);
			getline(ss, substr, ' ');
			n = stoi(substr);
			getline(ss, substr);
			m = stoi(substr);
			
			float** distancias = new float*[n];
			for (int i = 0; i < n; ++i)
    			distancias[i] = new float[n];
    		
			// Inicializa la diagonal a 0 y crea un vector con todos los elementos que se pueden seleccionar
			for (i = 0; i < n; i++) {
				distancias[i][i] = 0;
				indices.push_back(i);
			}
			
			mt19937 rng(0); // Generador de elementos pseudoaleatorios
			for (ejecuciones = 0; ejecuciones < limite; ejecuciones++) {
				shuffle(indices.begin(), indices.begin() + n, rng);
				
				for (i = 0; i < n; i++) {
					if (i < m)
						seleccion.push_back(indices[i]);
					else
						elementos.push_back(indices[i]);
				}
				
				// Lee el archivo para recoger las distancias y guardarlas en la matriz
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
				
				// Calcula MaxSum de los elementos elegidos
				suma = 0;
				for (it = seleccion.begin(); it != seleccion.end(); ++it) {
					for (it2 = next(it); it2 != seleccion.end(); ++it2) {
						suma = suma + distancias[(*it)][(*it2)];
					}
				}
				maxDist = suma;			
				
				iteraciones = 0;
				cambio = true;
				
				start = chrono::system_clock::now();

				// Algoritmo Busqueda Local
				while (cambio && iteraciones < 100000) {
					cambio = false;
					contribucion.clear();
					
					// Inicializa el vector contibucion
					for (i = 0; i < m; i++) {
						contribucion.push_back(0);
					}
					
					// Calcula la contribucion de cada elemento
					i = 0;
					for (it = seleccion.begin(); it != seleccion.end(); ++it) {
						for (it2 = seleccion.begin(); it2 != seleccion.end(); ++it2) {
							contribucion[i] += distancias[(*it)][(*it2)];
						}
						i++;
					}

					// Busca el elemento que menos contribuya a la distancia
					min = contribucion[0];
					indice = 0;
					for (i = 0; i < contribucion.size(); i++) {
						if (contribucion[i] < min) {
							min = contribucion[i];
							indice = i;
						}
					}
					
					// Comprueba todos los elementos no seleccionados
					for (i = 0; i < elementos.size() && !cambio; i++) {
						diff = 0;
						
						// y para cada uno comprueba la mejora que habría en la distancia
						for (j = 0; j < seleccion.size(); j++) {
							if (j != indice) {
								diff = diff + distancias[elementos[i]][seleccion[j]] - distancias[seleccion[indice]][seleccion[j]];
							}
						}
						
						// si con ese intercambio se incrementara la distancia, se hace ese intercambio
						if (diff > 0) {
							suma += diff;
							cambio = true;
							elementos.push_back(seleccion[indice]);
							seleccion[indice] = elementos[i];
							elementos.erase(elementos.begin() + i);
						}
						
						iteraciones++;
					}
				}
				
				end = chrono::system_clock::now();
				chrono::duration<double> elapsed_seconds = end - start; 
				
				// Calcula la distancia media, la maxima, la minima y el tiempo medio
				if (ejecuciones != 0) {
					if (suma > sumaMax) {
						sumaMax = suma;
					}
					else if (suma < sumaMin) {
						sumaMin = suma;
					}
					sumaAcumulada = sumaAcumulada + (suma - sumaAcumulada)/(ejecuciones + 1);
					tiempoAcumulado += elapsed_seconds.count();
				}
				else {
					sumaMax = suma;
					sumaMin = suma;
					sumaAcumulada = suma;
					tiempoAcumulado = elapsed_seconds.count();
				}
				elementos.clear();
				seleccion.clear();
				indices.clear();
			}
			
			// Muestra los datos por pantalla
			cout << endl;
			cout << "--- Media de distancias: " << fixed << sumaAcumulada << endl;
			cout << "--- Mejor caso: " << fixed << sumaMax << endl;
			cout << "--- Peor caso: " << fixed << sumaMin << endl;
			cout << "--- Tiempo medio (ms): " << fixed << (tiempoAcumulado*1000) / limite << endl;
			cout << endl;
		}
		else
			cout << "No se ha podido abrir el archivo." << endl;
		
		cout << "Archivo con casos: (QUIT para terminar)" << endl;
		cin >> ruta;
	}
}
