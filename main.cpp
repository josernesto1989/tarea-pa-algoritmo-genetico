#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// Algoritmo Genetico - Optimizacion de f(x,y) = 20 + (x^2 - 10*cos(2*pi*x)) + (y^2 - 10*cos(2*pi*y))
// Dominio acotado: x,y en [-5, 5] enteros | Minimo global en (0,0) con f(0,0) = 0

const double PI = 3.14159;

struct Ind {
    int x;
    int y;
};

const int MIN_COORD = -5;
const int MAX_COORD = 5;
const int POBLACION_SIZE = 50;
const int GENERATIONS_MAX = 100;
const int ELITE_SIZE = 3;
const int TOURNAMENT_SIZE = 3;
const double MUTATION_RATE = 0.15;
const int BITS_PER_COORD = 4;
const int RIGHT_BITS = BITS_PER_COORD / 2;
const unsigned int COORD_MASK = (1u << BITS_PER_COORD) - 1u;
const unsigned int RIGHT_MASK = (1u << RIGHT_BITS) - 1u;
const unsigned int LEFT_MASK = COORD_MASK ^ RIGHT_MASK;

using namespace std;

// Fitnes menor valor = mejor individuo
double f(double x, double y) {
    return 20.0 + (x * x - 10.0 * cos(2.0 * PI * x)) + (y * y - 10.0 * cos(2.0 * PI * y));
}

double fitness(const Ind& i) {
    return f(i.x, i.y);
}

Ind randomInd() {
    Ind a;
    a.x = MIN_COORD + rand() % (MAX_COORD - MIN_COORD + 1);
    a.y = MIN_COORD + rand() % (MAX_COORD - MIN_COORD + 1);
    return a;
}

// Seleccion por torneo
Ind tournament(const vector<Ind>& poblacion) {
    Ind best = poblacion[rand() % poblacion.size()];
    for (int i = 1; i < TOURNAMENT_SIZE; i++) {
        Ind cand = poblacion[rand() % poblacion.size()];
        if (fitness(cand) < fitness(best)) {
            best = cand;
        }
    }
    return best;
}

// Cruce: mitad izquierda de a y mitad derecha de b para cada coordenada
Ind crossover(const Ind& a, const Ind& b) {
    Ind c;

    unsigned int ax = static_cast<unsigned int>(a.x - MIN_COORD);
    unsigned int bx = static_cast<unsigned int>(b.x - MIN_COORD);
    unsigned int ay = static_cast<unsigned int>(a.y - MIN_COORD);
    unsigned int by = static_cast<unsigned int>(b.y - MIN_COORD);

    unsigned int childX = (ax & LEFT_MASK) | (bx & RIGHT_MASK);
    unsigned int childY = (ay & LEFT_MASK) | (by & RIGHT_MASK);

    childX = min(childX, static_cast<unsigned int>(MAX_COORD - MIN_COORD));
    childY = min(childY, static_cast<unsigned int>(MAX_COORD - MIN_COORD));

    c.x = static_cast<int>(childX) + MIN_COORD;
    c.y = static_cast<int>(childY) + MIN_COORD;
    return c;
}

// Mutacion: invierte un bit de una coordenada codificada
void mutate(Ind& i) {
    if ((double)rand() / RAND_MAX < MUTATION_RATE) {
        int& coordinate = (rand() % 2 == 0) ? i.x : i.y;
        unsigned int encoded = static_cast<unsigned int>(coordinate - MIN_COORD);
        vector<unsigned int> validMutations;

        for (int bit = 0; bit < BITS_PER_COORD; bit++) {
            unsigned int mutated = encoded ^ (1u << bit);
            if (mutated <= static_cast<unsigned int>(MAX_COORD - MIN_COORD)) {
                validMutations.push_back(mutated);
            }
        }

        unsigned int mutated = validMutations[rand() % validMutations.size()];
        coordinate = static_cast<int>(mutated) + MIN_COORD;
    }
}

int main() {
    srand((unsigned)time(nullptr));

    // Inicializar poblacion
    vector<Ind> poblacion(POBLACION_SIZE);
    for (int i = 0; i < POBLACION_SIZE; i++) {
        poblacion[i] = randomInd();
    }

    for (int gen = 0; gen < GENERATIONS_MAX; gen++) {
        // Evaluar fitness de la poblacion
        vector<double> fit(POBLACION_SIZE);
        for (int i = 0; i < POBLACION_SIZE; i++) {
            fit[i] = fitness(poblacion[i]);
        }

        // Ordenar poblacion del mejor al peor
        vector<int> idx(POBLACION_SIZE);
        for (int i = 0; i < POBLACION_SIZE; i++) idx[i] = i;
        sort(idx.begin(), idx.end(), [&](int a, int b) {
            return fit[a] < fit[b];
        });

        Ind best = poblacion[idx[0]];
        cout << "Generacion " << gen << " | Mejor f(" << best.x << "," << best.y << ") = " << fitness(best) << endl;

        // Detener si se encontro el minimo global (f = 0 en (0,0))
        if (fitness(best) < 0.0001) {
            break;
        }

        // Crear nueva poblacion: elitismo + seleccion/cruce/mutacion
        vector<Ind> nueva_poblacion;
        for (int e = 0; e < ELITE_SIZE; e++) {
            nueva_poblacion.push_back(poblacion[idx[e]]);
        }
        while ((int)nueva_poblacion.size() < POBLACION_SIZE) {
            Ind a = tournament(poblacion);
            Ind b = tournament(poblacion);
            Ind child = crossover(a, b);
            mutate(child);
            nueva_poblacion.push_back(child);
        }
        poblacion = nueva_poblacion;
    }

    // Evaluar la mejor individuo final
    vector<double> fit(POBLACION_SIZE);
    for (int i = 0; i < POBLACION_SIZE; i++) fit[i] = fitness(poblacion[i]);
    vector<int> idx(POBLACION_SIZE);
    for (int i = 0; i < POBLACION_SIZE; i++) idx[i] = i;
    sort(idx.begin(), idx.end(), [&](int a, int b) { return fit[a] < fit[b]; });

    Ind best = poblacion[idx[0]];
    cout << "\nMejor individuo: x = " << best.x << " | y = " << best.y << endl;
    cout << "Fitness final: f(" << best.x << "," << best.y << ") = " << fitness(best) << endl;

    return 0;
}
