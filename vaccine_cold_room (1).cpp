//Maintaining Temperature in Vaccine Cold Room
#include <iostream>
#include <ctime>
#include <fstream>
using namespace std;

const int GENE = 12;
const float DESIRED_TEMP = -15.0;
const int POP_SIZE = 100;
const float TEMP[GENE] = { -0.1,-0.2,-0.3,-0.4,-0.5,-0.6,-0.7,-0.8,-1,-1,0.5,0.5 };
const float COST[GENE] = { 1.5,1.6,1.7,1.8,1.9,2.0,2.1,2.2,8,8,15,15 };
const float weight_cost = 0.3;
const float weight_temp = 0.7;
const float CO_probability = 0.9;
const float sumMaxSpeedXCost = 425.6;
const float  MUT_probability = 0.2;

int chromosome[POP_SIZE][GENE];
float fitness[POP_SIZE];
int parents[2][GENE];
int children[2][GENE];
float CURRENT_TEMP = 0.0;

int newChromosome[POP_SIZE][GENE];
int newChromoCounter = 0;
ofstream bestFitnessFile, avgFitnessFile, bestChromosomeFile;

const int MAX_GENERATION = 30;

int indexParents[2];
void initializePopulation() {
    int randNum;
    //initialize random seed
    srand(time(NULL));
    for (int c = 0; c < POP_SIZE; c++) {
        for (int i = 0; i < GENE; i++) {
            randNum = rand() % 8;
            chromosome[c][i] = randNum;
        }
    }
}

void printChromosome() {
    for (int c = 0; c < POP_SIZE; c++) {
        cout << "\tC" << c << "\t";
        for (int i = 0; i < GENE; i++) {
            cout << chromosome[c][i] << " ";
        }
        cout << endl;
    }
}

void evaluateChromosome() {
    float tempChanges = 0;
    float cost = 0;
    for (int c = 0;c < POP_SIZE;c++) {
        tempChanges = 0;
        cost = 0;
        for (int g = 0;g < GENE;g++) {
            tempChanges = tempChanges + TEMP[g] * chromosome[c][g];
            cost = cost + COST[g] * chromosome[c][g];
        }
        fitness[c] = weight_temp * (abs((CURRENT_TEMP - DESIRED_TEMP) + tempChanges) / abs(CURRENT_TEMP - DESIRED_TEMP)) + 
            weight_cost * (cost / sumMaxSpeedXCost);
        cout << "\tC " << c << " temperature changes  = " << tempChanges << "\tFV\t" << fitness[c] << endl;
    }

}

void parentSelection() {
    int player1, player2;


    do {
        for (int p = 0; p < 2; p++) {//tournament selection
            player1 = rand() % POP_SIZE;
            do {
                player2 = rand() % POP_SIZE;
            } while (player1 == player2);

            if (fitness[player1] <= fitness[player2]) {
                indexParents[p] = player1;
            }
            else {
                indexParents[p] = player2;
            }
            cout << "\n\t Players: " << player1 << " vs " << player2;
            cout << "\n\t Fitness: " << fitness[player1] << " vs " << fitness[player2];
            cout << "\n\t Winner: " << indexParents[p];
        }//end of tournament
    } while (indexParents[0] == indexParents[1]);

    for (int p = 0; p < 2; p++) {
        cout << "\n\t Parents " << p + 1 << " : ";
        for (int g = 0; g < GENE; g++) {
            parents[p][g] = chromosome[indexParents[p]][g];
            cout << parents[p][g] << " ";
        }
    }
}

void crossover() {
    float prob = 0.0;
    int co_point;

    for (int p = 0; p < 2; p++) {//Copy parents to children
        for (int g = 0; g < GENE; g++) {
            children[p][g] = parents[p][g];
        }
    }
    //generate 0-1
    prob = ((rand() % 10) + 1) / 10.0;
    if (prob < CO_probability) {//crossover happen
        co_point = rand() % GENE;
        cout << "\n\t Children crossover at " << co_point;
        for (int g = co_point; g < GENE; g++) {
            children[0][g] = parents[1][g];
            children[1][g] = parents[0][g];
        }
    }
    else {//crossover did not happen
        cout << "\n\t Crossover did not happen ";
    }
    //output
    for (int c = 0; c < 2; c++) {
        cout << "\n\t Children " << c + 1 << " : ";
        for (int g = 0; g < GENE; g++) {
            cout << children[c][g] << " ";
        }
    }
}
void mutation() {
    float prob;
    int mut_point;
    int mut_point_1, mut_point_2, value_point_1;
    for (int c = 0;c < 2;c++) {
        prob = (rand() % 101) / 100.0; 
        if (prob < MUT_probability) {
            //if random number less than mutation probability, mutation will happen, else, mutation wont happen.

            do {
                mut_point_1 = rand() % GENE;
                mut_point_2 = rand() % GENE;
            } while (mut_point_1 == mut_point_2);
            cout << "\n\t Children mutated at " << mut_point_1;
            cout << "\n\t Children mutated at " << mut_point_2;

            value_point_1 = children[c][mut_point_1];
            children[c][mut_point_1] = children[c][mut_point_2];
            children[c][mut_point_2] = value_point_1;
        }
        else
        {
            cout << "\n\t Mutation did not happen";
        }

    }

    for (int c = 0; c < 2; c++) {
        cout << "\n\t Children " << c + 1 << " after mutation : ";
        for (int g = 0; g < GENE; g++) {
            cout << children[c][g] << " ";
        }
    }
}

void survivalSelection() {//all children replace parents
    for (int c = 0; c < 2; c++) {
        for (int g = 0; g < GENE; g++) {
            newChromosome[newChromoCounter][g] = children[c][g];
        }
        newChromoCounter++;

    }

    for (int c = 0; c < newChromoCounter; c++) {
        cout << "\n\t New chromosome " << c << "  : ";
        for (int g = 0; g < GENE; g++) {
            cout << newChromosome[c][g] << " ";
        }
    }
}
void copyChromosome() {
    for (int c = 0; c < POP_SIZE; c++) {//copy newChromosome to chromosome
        for (int g = 0; g < GENE; g++) {
            chromosome[c][g] = newChromosome[c][g];
        }
    }
}
void calculateAverageFitness() {//average fitness for each generations
    double avgFitness = 0.0;
    double sum = 0.0;
    for (int c = 0; c < POP_SIZE; c++) {
        sum += fitness[c];
    }
    avgFitness = sum / POP_SIZE;
    cout << "\n	AVERAGE FITNESS = " << avgFitness;

    avgFitnessFile << avgFitness << endl;
}
void recordBestFitness() {//Best fitness for each generation
    double bestFitness = 99.9;
    int bestChromosome[GENE];
    for (int c = 0;c < POP_SIZE;c++) {
        if (bestFitness > fitness[c]) {
            bestFitness = fitness[c];
            for (int g = 0;g < GENE;g++) {
                bestChromosome[g] = chromosome[c][g];
            }
        }

    }
    cout << "\nBest Fitness = " << bestFitness;
    cout << "\nBest Chromosome = ";

    for (int g = 0;g < GENE;g++) {
        cout << bestChromosome[g] << " ";
    }
    cout << endl;
    bestFitnessFile << bestFitness << endl;
    for (int g = 0;g < GENE;g++) {
        bestChromosomeFile << bestChromosome[g] << " ";
    }
    bestChromosomeFile << endl;

}
int main() {
    bestFitnessFile.open("bestFitness.txt");
    avgFitnessFile.open("avgFitness.txt");
    bestChromosomeFile.open("bestChromosome.txt");


    initializePopulation();
    for (int g = 0;g < MAX_GENERATION;g++) {//will end the iteration if it hit maximum generation
        newChromoCounter = 0;
        printChromosome();
        evaluateChromosome();
        recordBestFitness();
        calculateAverageFitness();
        cout << "\n\tGENERARTION " << g + 1 << endl;
        for (int i = 0;i < (POP_SIZE / 2);i++) {
            parentSelection();
            crossover();
            mutation();
            survivalSelection();
        }
        copyChromosome();
    }
    bestFitnessFile.close();
    avgFitnessFile.close();
    bestChromosomeFile.close();
}