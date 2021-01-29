#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <vector>
#include <chrono> 

using namespace std;

const int CROMOSOMAS = 50;
mt19937 rng(0);

void cruce(bool* &hijo1, bool* &hijo2, bool cromosoma1[], bool cromosoma2[], int n) {
    uniform_int_distribution<int> gen(0, 1);
    vector<bool> copia1, copia2;
    vector<int> posiciones1, posiciones2;
    int i, j;
    vector<bool>::iterator it;
	
    for (i = 0; i < n; i++) {
        copia1.push_back(cromosoma1[i]);
        copia2.push_back(cromosoma2[i]);
        posiciones1.push_back(i);
        posiciones2.push_back(i);
        hijo1[i] = false;
        hijo2[i] = false;
    }
	
	j = 0;
    for (i = 0; i < n; i++) {
        // Si el gen es igual en los dos cromosomas, lo mantiene
        if (copia1[i] == copia2[i]) {
            hijo1[j] = copia1[i];
            hijo2[j] = copia1[i];
            copia1.erase(copia1.begin() + i);
            copia2.erase(copia2.begin() + i);
            posiciones1.erase(posiciones1.begin() + i);
            posiciones2.erase(posiciones2.begin() + i);
            i--;
            n--;
        }
        j++;
    }
	
    shuffle(copia1.begin(), copia1.begin() + copia1.size(), rng);
    
    i = 0;
    for (it = copia1.begin(); it != copia1.end() && !posiciones1.empty(); ++it) {
        while (i != posiciones1[0])
            i++;
        
        hijo1[i] = (*it);
        posiciones1.erase(posiciones1.begin());
    }
	
    shuffle(copia1.begin(), copia1.begin() + copia1.size(), rng);
    
    i = 0;
    for (it = copia1.begin(); it != copia1.end() && !posiciones2.empty(); ++it) {
        while (i != posiciones2[0])
            i++;
        
        hijo2[i] = (*it);
        posiciones2.erase(posiciones2.begin());
    }
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

int main() {
    ifstream archivo; // Archivo con las distancias
    string ruta, linea, substr;
    int i, j, n, m;
    float valor;
    float probCruce = 0.7; // Probabilidad de que dos padres se crucen
    float probMutacion = 0.001; // Probabilidad de que un gen mute
    int crucesEsperados, mutacionesEsperadas;
    uniform_int_distribution<int> distribution(0, CROMOSOMAS-1); 
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
            crucesEsperados = probCruce * CROMOSOMAS/2;
            mutacionesEsperadas = probMutacion * n * CROMOSOMAS;
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
            float evaluaciones[CROMOSOMAS], evaluacionesAnterior[CROMOSOMAS];
            float mejorEvaluacion, peorEvaluacion;
            int indexMejorEval, indexPeorEval;
            bool sobrevive;
            
            for (i = 0; i < CROMOSOMAS; i++)
                evaluaciones[i] = evaluar(poblacion[i], distancias, n);
			
			tiempoAcumulado = 0.0;
            // Evalua 100000 veces, 50 veces por cada ejecución (numero de cromosomas)
            for (i = 0; i < 100000; i = i + 50) {
            	start = chrono::system_clock::now();
            	
                // Evalúa la función objetivo y guarda el indice del cromosoma con la mejor evaluación
                mejorEvaluacion = 0.0;
                for (j = 0; j < CROMOSOMAS; j++) {
                    if (evaluacionesAnterior[j] > mejorEvaluacion) {
                        mejorEvaluacion = evaluacionesAnterior[j];
                        indexMejorEval = j;
                    }
                }
				
                // Guardamos la población que tenemos
                bool** poblacionAnterior = new bool*[CROMOSOMAS];
                for (j = 0; j < CROMOSOMAS; j++)
                    poblacionAnterior[j] = new bool[n];
                
                poblacionAnterior = poblacion;
                
                // Operador de selección: torneo binario, tantos como individuos existan en la población genética
                int indice1, indice2;
                for (j = 0; j < CROMOSOMAS; j++) {
                    indice1 = distribution(rng);
                    indice2 = distribution(rng);

                    if (evaluaciones[indice1] > evaluaciones[indice2])
                        poblacion[j] = poblacionAnterior[indice1];
                    else
                        poblacion[j] = poblacionAnterior[indice2];
                }
                
                // Operador de cruce: cruce basado en posición, se mantienen las posiciones que tengan los mismos valores en los padres.
                // El resto de posiciones de cualquiera de los dos padres se barajan dos veces y se insertan en dos hijos.
                bool* hijo1 = new bool[n];
                bool* hijo2 = new bool[n];

                for (j = 0; j < crucesEsperados*2; j = j+2) {
                    cruce(hijo1, hijo2, poblacion[j], poblacion[j+1], n);

                    poblacion[j] = hijo1;
                    poblacion[j+1] = hijo2;
                }
				
                // Operador de mutación: operador de intercambio entre el gen a mutar xi por el de otro gen xj escogido aleatoriamente con el valor contrario
                // distribution(rng) devuelve un numero aleatorio entre 0 y CROMOSOMAS (50) - 1 y distribution2(rng) devuelve un numero aleatorio entre 0 y n-1
                int M, n1, n2;
				for (j = 0; j < mutacionesEsperadas; j++) {
                    M = distribution(rng);
                    n1 = distribution2(rng);
                    n2 = distribution2(rng);
                    
                    while (poblacion[M][n1] == poblacion[M][n2]) {
                        n2 = distribution2(rng);
                    }
                    
                    poblacion[M][n1] != poblacion[M][n1];
                    poblacion[M][n2] != poblacion[M][n2];
                }
				
                // Esquema de reemplazamiento: la población de hijos sustituye automáticamente a la actual.
                // Si la mejor solución de la generación no sobrevive, sustituye directamente la peor solución de la nueva población
                peorEvaluacion = mejorEvaluacion;
                sobrevive = false;

                for (j = 0; j < CROMOSOMAS; j++)
                    evaluaciones[j] = evaluar(poblacion[j], distancias, n);
                    
                for (j = 0; j < CROMOSOMAS && !sobrevive; j++) {
                    if (poblacion[j] == poblacionAnterior[indexMejorEval])
                        sobrevive = true;

                    if (evaluaciones[j] < peorEvaluacion) {
                        peorEvaluacion = evaluaciones[j];
                        indexPeorEval = j;
                    }
                }
                
                if (!sobrevive) {
                    poblacion[indexPeorEval] = poblacionAnterior[indexMejorEval];
                    evaluaciones[indexPeorEval] = evaluacionesAnterior[indexMejorEval];
                }
                
                for (j = 0; j < CROMOSOMAS; j++) {
                	evaluacionesAnterior[j] = evaluaciones[j];
				}
				
				end = chrono::system_clock::now();
				chrono::duration<double> elapsed_seconds = end - start;
				
				tiempoAcumulado += elapsed_seconds.count();
            }
				
            mejorEvaluacion = 0.0;
            
            for (i = 0; i < CROMOSOMAS; i++) {
                evaluaciones[i] = evaluar(poblacion[i], distancias, n);
                
				if (evaluaciones[i] > mejorEvaluacion) {
                    mejorEvaluacion = evaluaciones[i];
                    indexMejorEval = i;
                }
            }
            
			cout << "--- Valor: " << mejorEvaluacion << endl;
            cout << "--- Tiempo medio (ms): " << fixed << (tiempoAcumulado * 1000) / 2000 << endl;
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
        
    
    // Otros datos: 50 cromosomas; Probabilidad de cruce 0,7; Probabilidad de mutación 0,001; 100000 evaluaciones de la función objetivo; 1 sola ejecución por caso
}
