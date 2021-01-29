#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <vector>
#include <algorithm>
#include <math.h>
#include <chrono>

using namespace std;

mt19937 rng(0);

int main() {
    ifstream archivo; // Archivo con las distancias
    string ruta, linea, substr;
    uniform_real_distribution<float> randombinary(0, 1);
    int i, j, n, m;
    float valor;
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

			// Generador de n�meros aleatorios uniformemente distribuidos
			uniform_int_distribution<int> distribution1(0, m-1);
    		uniform_int_distribution<int> distribution2(0, n-m-1);
    		
            // Reserva la memoria para guardar las distancias y los elementos
            float** distancias = new float*[n];
            for (i = 0; i < n; i++)
                distancias[i] = new float[n];

            // Esquema de representación: vector que guarda los elementos seleccionados
            vector<int> elementos;
            
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
            for (i = 0; i < n; i++) {
                distancias[i][i] = 0;
                elementos.push_back(i);
            }

            // Generación de la solución inicial: de forma aleatoria      
            int* seleccionados = new int[m];
            int* noSeleccionados = new int[n-m];
            int* mejorSolucion = new int[m];

            // Selecciona m elementos al azar
            for (i = 0; i < m; i++) {
                shuffle(elementos.begin(), elementos.begin() + n, rng);
                
                seleccionados[i] = elementos[0];
                elementos.erase(elementos.begin());
            }
            mejorSolucion = seleccionados;

            // Calcula el coste de la solución generada
            float coste, mejorCoste;
            coste = 0;
            for (int i = 0; i < m; i++) {
                for (int j = i; j < m; j++) {
                    coste += distancias[seleccionados[i]][seleccionados[j]];
                }
            }
            mejorCoste = coste;

            // El resto de elementos los guarda en no seleccionados
            i = 0;
            for (auto it = elementos.begin(); it != elementos.end(); ++it) {
                noSeleccionados[i] = (*it);
                i++;
            }

            // Calculo temperatura inicial
            // fi entre [0,1] es la probabilidad de aceptar una solución un mu por 1 peor que la inicial
            // Se considerará mu = fi = 0.3
            // La temperatura final se fijará a 10^-3 comprobando que sea menor que la inicial
            float temperaturaInicial, temperaturaFinal, temperaturaActual;
            float mu, fi;

            mu = 0.3;
            fi = 0.3;
            temperaturaFinal = 0.001;
            temperaturaInicial = (mu * coste) / (-log(fi));
            temperaturaActual = temperaturaInicial;

            // Si Tf es mayor que T0, Tf se reduce más
            if (temperaturaFinal > temperaturaInicial)
                temperaturaFinal = 0.000001;
            
            // Otros parámetros
            int max_vecinos, max_exitos, n_iter, M;
            int vecinos, exitos;

            max_vecinos = 10 * n;
            max_exitos = 0.1 * max_vecinos;
            n_iter = 100000;
            M = n_iter/max_vecinos;

            i = 0;
            vecinos = 0;
            exitos = 0;

            // Calcula el beta que se usará para el enfriamiento
            float beta = (temperaturaInicial - temperaturaFinal) / (M * temperaturaInicial * temperaturaFinal);
            
            bool final = false;
            tiempoAcumulado = 0.0;

            start = chrono::system_clock::now();
            // Condición de parada: acaba cuando haya alcanzado el número máximo de evaluaciones o bien cuando el número de éxitos en el enfriamiento actual sea 0.
            while (temperaturaFinal < temperaturaActual && i < n_iter && !final) {
                // Condición de enfriamiento L(T): Se enfriará la temperatura, finalizando la iteración actual, bien cuando se haya generado un número máximo de vecinos max_vecinos (independientemente de que se hayan aceptado o no) o bien cuando se haya aceptado un número máximo de los vecinos generados max_exitos.
                while (vecinos < max_vecinos && exitos < max_exitos) {
                    // Guarda el coste en otra variable para calcular el coste de la nueva soluci�n
                    int nuevoCoste;
                    nuevoCoste = coste;
					
                    // Esquema de generación de vecinos: intercambio de un elemento seleccionado por otro que no lo esté aleatorios.
                    int indice1, indice2, tmp;

                    indice1 = distribution1(rng);
                    indice2 = distribution2(rng);
					
                    // Calculamos el coste de esta solución
                    for (j = 0; j < m; j++) {
                        nuevoCoste -= distancias[seleccionados[j]][seleccionados[indice1]];
                        if (j != indice1)
                            nuevoCoste += distancias[seleccionados[j]][noSeleccionados[indice2]];
                    }
					
                    // Comprobamos si aceptamos la solución
                    float diferencia = coste - nuevoCoste;
                    if (diferencia < 0 || randombinary(rng) <= exp(-diferencia/temperaturaActual)) {
                        // Hacemos el intercambio por el vecino
                        tmp = seleccionados[indice1];
                        seleccionados[indice1] = noSeleccionados[indice2];
                        noSeleccionados[indice2] = tmp;
                        coste = nuevoCoste;

                        // Si es mejor que la mejor solución encontrada, la actualiza
                        if (coste > mejorCoste) {
                            mejorCoste = coste;
                            mejorSolucion = seleccionados;
                        }
                        exitos++;
                    }
                    vecinos++;
                    i++;
                }
                
                // Esquema de enfriamiento: Cauchy modificado.
                temperaturaActual = temperaturaActual / (1 + (beta * temperaturaActual));

                // Comprueba si se ha producido al menos un éxito
                if (exitos == 0)
                    final = true;
                
                exitos = 0;
                vecinos = 0;
        	}
            end = chrono::system_clock::now();
            chrono::duration<double> elapsed_seconds = end - start;
			tiempoAcumulado += elapsed_seconds.count();

			cout << "--- Valor: " << fixed << mejorCoste << endl;
            cout << "--- Tiempo medio (ms): " << fixed << tiempoAcumulado * 1000 << endl;
            
            if (final) {
            	cout << "--- Parada por 0 exitos" << endl;
			}
			else if (temperaturaActual < temperaturaFinal) {
				cout << "--- Parada por enfriamiento" << endl;
			}
			else {
				cout << "--- Parada por numero de iteraciones superado" << endl;
			}
            cout << endl;
        }
        else
            cout << "No se ha podido abrir el archivo." << endl;
            
        cout << "Archivo con casos: (QUIT para terminar)" << endl;
		cin >> ruta;    
    }
}
