#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <chrono> 

using namespace std;

int main() {
	ifstream archivo; // Archivo con las distancias
	string ruta, linea, substr;
	vector<int> elementos; // Vector con los elementos no seleccionados
	vector<int>::iterator it;
	list<int> seleccion; // Lista con los elementos seleccionados
	list<int>::iterator it2, it3;
	int i, j, n, m;
	int indice;
	double suma, max;
	float valor;
	chrono::time_point<chrono::system_clock> start, end;
	
	cout << "Archivo con casos: (QUIT para terminar)" << endl;
	cin >> ruta;
	
	while (ruta != "QUIT") {
		archivo.open(ruta);
		
		if (archivo.is_open()) {
			// Lee el n y el m de la primera l�nea
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
				elementos.push_back(i);
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

			start = chrono::system_clock::now();
			
			// Busca el elemento mas alejado del resto, que sera el primer seleccionado
			suma = 0;
			max = 0;
			
			for (i = 0; i < n; i++) {
				for (j = 0; j < n; j++) {
					suma += distancias[i][j];
				}
				
				if (suma > max) {
					max = suma;
					indice = i;
				}
				
				suma = 0;
			}
			
			seleccion.push_back(indice);
			elementos.erase(elementos.begin() + indice);
			
			// Busca el elemento mas alejado de los ya seleccionados hasta encontrar m elementos.
			while (seleccion.size() < m) {
				suma = 0;
				max = 0;
				
				for (it = elementos.begin(); it != elementos.end(); ++it) {
					for (it2 = seleccion.begin(); it2 != seleccion.end(); ++it2) {
						suma += distancias[(*it)][(*it2)];
					}
					
					if (suma > max) {
						max = suma;
						indice = (*it);
					}
					
					suma = 0;
				}
				
				seleccion.push_back(indice);
				elementos.erase(remove(elementos.begin(), elementos.end(), indice), elementos.end());
			}
			
			end = chrono::system_clock::now();
			chrono::duration<double> elapsed_seconds = end - start; 
			
			// Calculo de la distancia total
			suma = 0;
			for (it2 = seleccion.begin(); it2 != seleccion.end(); ++it2) {
				for (it3 = next(it2); it3 != seleccion.end(); ++it3) {
					suma = suma + distancias[(*it2)][(*it3)];
				}
			}
			
			cout << endl;
			cout << "--- Distancia: " << fixed << suma << endl;
			cout << "--- Tiempo (ms): " << fixed << elapsed_seconds.count() * 1000 << endl; 
			cout << endl;
			
			elementos.clear();
			seleccion.clear();
		}
		else
			cout << "No se ha podido abrir el archivo." << endl;
		
		cout << "Archivo con casos: (QUIT para terminar)" << endl;
		cin >> ruta;
	}
}
