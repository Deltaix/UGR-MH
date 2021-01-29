#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

const int CROMOSOMAS = 50;
mt19937 rng(0);

void reparar(bool* &cromosoma, vector<float> &contribucionSel, vector<float> &contribucionNoSel, int n, int m) {
    int contador = 0;
    float maxEvaluacion = 0.0;
    int diferencia;
    int indice, i;
    vector<float>::iterator it;

    // Cuenta de 1s
    for (int i = 0; i < n; i++) {
        if (cromosoma[i] == 1)
            contador++;
    }
	
    // Si hay demasiados seleccionados, quita los que contribuyan más por variedad
    if (contador > m) {
        diferencia = contador - m;

        while (diferencia > 0) {
        	i = 0;
            for (it = contribucionSel.begin(); it != contribucionSel.end(); ++it) {
                if ((*it) > maxEvaluacion) {
                    maxEvaluacion = (*it);
                    indice = i;
                }
                i++;
            }

            cromosoma[indice] = false;
            contribucionSel[indice] = 0.0;
            maxEvaluacion = 0.0;
            diferencia--;
        }
    }

    // Si hay pocos seleccionados, añade los que contribuyan más
    if (contador < m) {
        diferencia = m - contador;

        while (diferencia > 0) {
        	i = 0;
            for (it = contribucionNoSel.begin(); it != contribucionNoSel.end(); ++it) {
                if ((*it) > maxEvaluacion) {
                    maxEvaluacion = (*it);
                    indice = i;
                }
                i++;
            }

            cromosoma[indice] = true;
            contribucionNoSel[indice] = 0.0;
            maxEvaluacion = 0.0;
            diferencia--; 
        }
    }
}

bool* cruce(bool cromosoma1[], bool cromosoma2[], int n) {
    uniform_int_distribution<int> gen(0, 1);
    bool* hijo = new bool[n];

    for (int i = 0; i < n; i++) {
        // Si el gen es igual en los dos cromosomas, lo mantiene
        if (cromosoma1[i] == cromosoma2[i])
            hijo[i] = cromosoma1[i];

        // Si el gen es distinto en los dos padres, coge uno de los dos al azar
        else {
            if (gen(rng) == 0)
                hijo[i] = cromosoma1[i];
            else
                hijo[i] = cromosoma2[i];
        }
    }
    
    return hijo;
}

float evaluar(const bool* cromosoma, float** distancias, int n) {
    int valor = 0;
    
    for (int i = 0; i < n; i++) {
    	if (cromosoma[i]) {
	        for (int j = i; j < n; j++) {
	        	if (cromosoma[j])
	    	        valor += distancias[i][j];
	    	}
		}
    }

    return valor;
}

void calcContribuciones(const bool hijo[], vector<float> &contribucionSel, vector<float> &contribucionNoSel, float *distancias[], int n) {
	contribucionSel.clear();
	contribucionNoSel.clear();
	
    for (int i = 0; i < n; i++) {
        contribucionSel.push_back(0.0);
		contribucionNoSel.push_back(0.0);

		// Si el elemento no está seleccionado, calcula la contribución de ese elemento si se añadiera
        if (!hijo[i]) {
        	for (int j = 0; j < n; j++) {
        		if (hijo[j])
        			contribucionNoSel[i] += distancias[i][j];
			}
		}
		
		// Si el elemento está seleccionado, calcula la contribución de ese elemento en el vector solución
		else {
			for (int j = 0; j < n; j++) {
				if (hijo[j])
					contribucionSel[i] += distancias[i][j];
			}
		}
    }
}

int main() {
    ifstream archivo; // Archivo con las distancias
    string ruta, linea, substr;
    int i, j, n, m;
    float valor;
    float probMutacion = 0.001; // Probabilidad de que un gen mute
    int crucesEsperados, mutacionesEsperadas;
    uniform_int_distribution<int> distribution(0, CROMOSOMAS-1);
    uniform_int_distribution<int> distrbinario(0, 1);
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
			
            // Calcula cuantas modificaciones hacer en vez de calcular la probabilidad de cada gen/cromosoma (Esperanza matemática)
            mutacionesEsperadas = probMutacion * n * 2;
            uniform_int_distribution<int> distribution2(0, n-1);
			
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
			
            // Representación binaria, 50 cromosomas.
            bool** poblacion = new bool*[CROMOSOMAS];
            for (i = 0; i < CROMOSOMAS; i++)
                poblacion[i] = new bool[n];
			
            // Crea un cromosoma viable (ocurrencias de 1 = m)
            vector<bool> cromosoma;
            for (i = 0; i < n; i++) {
                if (i < m)
                    cromosoma.push_back(true);
                else
                    cromosoma.push_back(false);
            }
			
            // Generación de la población inicial: baraja el cromosoma viable 50 veces para generar la población
            // También calcula a su vez la evaluación de la función objetivo
            for (i = 0; i < CROMOSOMAS; i++) {
                shuffle(cromosoma.begin(), cromosoma.begin() + n, rng);
                
                for (j = 0; j < n; j++)
                	poblacion[i][j] = cromosoma[j];
            }
			
            // Variables donde guardar las evaluaciones de la función objetivo
            float evaluaciones[CROMOSOMAS];
            int indexPeorEval1, indexPeorEval2;
            float peorEvaluacion;
            
            // Evalúa la función objetivo
			for (j = 0; j < CROMOSOMAS;j++)
                evaluaciones[j] = evaluar(poblacion[j], distancias, n);
			
            tiempoAcumulado = 0.0;
            // Evalua 100000 veces, 2 veces por cada ejecución (numero de cromosomas)
            for (i = 0; i < 100000; i = i + 2) {
            	start = chrono::system_clock::now();
                
                // Operador de selección: torneo binario, solo 2 padres
                int indice1, indice2;
                bool* padre1 = new bool[n];
                bool* padre2 = new bool[n];

                indice1 = distribution(rng);
                indice2 = distribution(rng);

                if (evaluaciones[indice1] > evaluaciones[indice2])
                    padre1 = poblacion[indice1];
                else
                    padre1 = poblacion[indice2];

                indice1 = distribution(rng);
                indice2 = distribution(rng);

                if (evaluaciones[indice1] > evaluaciones[indice2])
                    padre2 = poblacion[indice1];
                else
                    padre2 = poblacion[indice2];
                
                // Operador de cruce: cruce uniforme, genera un hijo a partir de dos padres.
                // Los genes con el mismo valor se mantienen, el resto se elige aleatoriamente entre ambos.
                bool* hijo1 = new bool[n];
                bool* hijo2 = new bool[n];

                hijo1 = cruce(padre1, padre2, n);
                hijo2 = cruce(padre1, padre2, n);
					
                vector<float> contribucionSel, contribucionNoSel;
					
                calcContribuciones(hijo1, contribucionSel, contribucionNoSel, distancias, n);
				reparar(hijo1, contribucionSel, contribucionNoSel, n, m);
					
                calcContribuciones(hijo2, contribucionSel, contribucionNoSel, distancias, n);
                reparar(hijo2, contribucionSel, contribucionNoSel, n, m);
				
                // Operador de mutación: operador de intercambio entre el gen a mutar xi por el de otro gen xj escogido aleatoriamente con el valor contrario
                // distribution(rng) devuelve un numero aleatorio entre 0 y CROMOSOMAS (50) - 1 y distribution2(rng) devuelve un numero aleatorio entre 0 y n-1
                int M, n1, n2;
				for (j = 0; j < mutacionesEsperadas; j++) {
                    M = distrbinario(rng);
                    
                    if (M == 0) {
                    	n1 = distribution2(rng);
                    	n2 = distribution2(rng);
                    	
                    	while (hijo1[n1] == hijo1[n2]) {
                        	n2 = distribution2(rng);
                    	}
                    
                    	hijo1[n1] != hijo1[n1];
                    	hijo1[n2] != hijo1[n2];
                	}
                	else {
                		n1 = distribution2(rng);
	                    n2 = distribution2(rng);
	                    
                		while (hijo2[n1] == hijo2[n2]) {
                        	n2 = distribution2(rng);
                    	}
                    
                    	hijo2[n1] != hijo2[n1];
                    	hijo2[n2] != hijo2[n2];
					}
                }
				
                // Esquema de reemplazamiento: los 2 hijos reemplazan a los peores de la generación anterior.
                peorEvaluacion = -1;
                for (j = 0; j < CROMOSOMAS; j++) {
                    if (evaluaciones[j] < peorEvaluacion || peorEvaluacion == -1) {
                        peorEvaluacion = evaluaciones[j];
                        indexPeorEval1 = j;
                    }
				}
                
                evaluaciones[indexPeorEval1] = INT_MAX;

                peorEvaluacion = -1;
                for (j = 0; j < CROMOSOMAS; j++) {
                    if (evaluaciones[j] < peorEvaluacion || peorEvaluacion == -1) {
                        peorEvaluacion = evaluaciones[j];
                        indexPeorEval2 = j;
                    }
				}
				
                poblacion[indexPeorEval1] = hijo1;
                evaluaciones[indexPeorEval1] = evaluar(hijo1, distancias, n);
                poblacion[indexPeorEval2] = hijo2;
                evaluaciones[indexPeorEval2] = evaluar(hijo2, distancias, n);
                
            	end = chrono::system_clock::now();
				chrono::duration<double> elapsed_seconds = end - start;
				
				tiempoAcumulado += elapsed_seconds.count();
            }
			
            float mejorEvaluacion = 0.0;
            int indexMejorEval;
            
            for (i = 0; i < CROMOSOMAS; i++) {
                evaluaciones[i] = evaluar(poblacion[i], distancias, n);
                
				if (evaluaciones[i] > mejorEvaluacion) {
                    mejorEvaluacion = evaluaciones[i];
                    indexMejorEval = i;
                }
            }

			cout << "--- Valor: " << mejorEvaluacion << endl;
            cout << "--- Tiempo medio (ms): " << fixed << (tiempoAcumulado * 1000) / 50000 << endl;
            cout << endl;
        }
        else
			cout << "No se ha podido abrir el archivo." << endl;
		
		cout << "Archivo con casos: (QUIT para terminar)" << endl;
		cin >> ruta;
    }
    
   
    
    // Operador de cruce: cruce uniforme (con reparación) y cruce basado en posición (2 VERSIONES)
        // Cruce uniforme: genera un hijo a partir de dos padres. Los genes con el mismo valor se mantienen, el resto se elige aleatoriamente entre ambos.
            // Operador de reparación: comprueba que la solución es factible. Si 1 < m, se añaden los que más contribuyan. Si 1 > m se quitan los que más contribuyan (para ganar diversidad).
        // Cruce basado en posición: se mantienen las posiciones con los mismos valores. El resto de posiciones de cualquiera de los dos padres se barajan dos veces y se insertan en dos hijos.
    
    // Otros datos: 50 cromosomas; Probabilidad de cruce 0,7; Probabilidad de mutación 0,001; 100000 evaluaciones de la función objetivo; 1 sola ejecución por caso
}
