#include <iostream>
#include <random>
#include <list>
#include <utility>
#include <algorithm>
#include<vector>
#include <chrono>
#include <ctime>
#include <cstdlib> 
#include <fstream>
#include <string>

using namespace std;

/*
* Common settings
*/
const bool VIZUALIZATION_ENABLED = false;
const bool EXPORT_TO_FILE_ENABLED = true;
const string EXPORT_FILE_PATH = "D:\distribution_per_step.txt";

/*
* Parameters
*/
const int NUMBER_OF_CELL_TYPES = 12;
const int NUMBER_OF_CELLS_FOR_EACH_TYPE = 400;
const int TOTAL_NUMBER_OF_CELLS = 5000;

const int NUMBER_OF_EXAMPLES_TYPE_ONE = 10000;
const int NUMBER_OF_EXAMPLES_TYPE_TWO = 10000;
const int TOTAL_NUMBER_OF_TRAINING_EXAMPLES = NUMBER_OF_EXAMPLES_TYPE_ONE + NUMBER_OF_EXAMPLES_TYPE_TWO;
const double MEAN_TYPE_ONE = 6.0;
const double STD_DEVIATION = 1.0;
const double MEAN_TYPE_TWO_FIRST = 3.0;
const double MEAN_TYPE_TWO_SECOND = 9.0;

const int TOTAL_TIME = 10;
const double TIME_STEP_VALUE = 0.5;
const int NUMBER_OF_STEPS = TOTAL_TIME/TIME_STEP_VALUE;

const int EMPTY_CELL = 0;
const int POSSITIVE_RESPONCE = 1;
const int NO_RESPONCE = 0;

/*
* Calculate cell responce to training example value
*/
double calculateResponce(double trainingExampleValue, int cellType)
{
    double delta = 1;
    double responce = 1 - abs(trainingExampleValue / delta - cellType + 1);
    if (responce < 0) {
        responce = 0;
    }
    return responce;
}

/*
* Clear currentDistribution massive and then calculate amount o cells for each type
*/
void calculateCurrentDistribution(int * cellMassive, int* currentDistribution) {

    for (int j = 0; j <= NUMBER_OF_CELL_TYPES; j++) {
        currentDistribution[j] = 0;
    }

    for (int i = 0; i < TOTAL_NUMBER_OF_CELLS; i++) {
        currentDistribution[cellMassive[i]]++;
    }
}

int main()
{
    //
    // Prepare file if EXPORT_FILE_ENABLED is equeal to true
    //
    ofstream exportDataFile;
    if (EXPORT_TO_FILE_ENABLED) {
        exportDataFile.open(EXPORT_FILE_PATH);
    }

    cout << "1. CELL MASSIVE INITIALIZATION --->" << endl << endl;

    //
    // An array that represents alveolus in which cells of different types are located
    //
    // Initial distribution consist equal numbers of cells of each type located one after another
    // If TOTAL_NUMBER_OF_CELLS > NUMBER_OF_CELLS_FOR_EACH_TYPE * NUMBER_OF_CELLS_FOR_EACH_TYPE than last alveolus will remain empty
    //

    int cellMassive[TOTAL_NUMBER_OF_CELLS] = { 0 };

    for (int type = 1; type <= NUMBER_OF_CELL_TYPES; type++) {
        for (int position = 0; position < NUMBER_OF_CELLS_FOR_EACH_TYPE; position++) {
            cellMassive[(type - 1) * NUMBER_OF_CELLS_FOR_EACH_TYPE + position] = type;
        }
    }

    //
    // An array containing information about the number of cells of each type in cellMassive
    //
    // Array indexes corresponds to cell type number
    // index 0 correspond to empty cells
    //

    int currentDistribution[NUMBER_OF_CELL_TYPES + 1] = { 0 };
    calculateCurrentDistribution(cellMassive, currentDistribution);

    //
    // Stack that consist indexes of empty alveolus in cellMassive. Initially they added in order.
    // The last element is the last empty alveolus in cellMassive.
    //

    vector<int> latestEmptyCellsIndexes;

    for (int i = 0; i < TOTAL_NUMBER_OF_CELLS; i++) {
        if (cellMassive[i] == 0) {
            latestEmptyCellsIndexes.push_back(i);
        }
    }

    cout
        << "   NUMBER_OF_CELL_TYPES:" << NUMBER_OF_CELL_TYPES << endl
        << "   NUMBER_OF_CELLS_FOR_EACH_TYPE:" << NUMBER_OF_CELLS_FOR_EACH_TYPE << endl
        << "   TOTAL_NUMBER_OF_CELLS:" << TOTAL_NUMBER_OF_CELLS << endl;

    cout << endl << "2. TRAINING EXAMPLES INITIALIZATION --->" << endl << endl;

    //
    // Massive of training examples. First value is a responce, second value is a type of example
    //
    vector<pair<double, int> > trainingExamples;

    default_random_engine generator;
    normal_distribution<double> distributionTypeOne(MEAN_TYPE_ONE, STD_DEVIATION);
    normal_distribution<double> distributionTypeTwoFirst(MEAN_TYPE_TWO_FIRST, STD_DEVIATION);
    normal_distribution<double> distributionTypeTwoSecond(MEAN_TYPE_TWO_SECOND, STD_DEVIATION);

    //
    // initialize the random number generator with time-dependent seed
    //
    std::mt19937_64 randomNumberGenerator;
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq seedSequesnce{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
    randomNumberGenerator.seed(seedSequesnce);

    //
    // initialize a uniform distribution between 0 and 1
    //
    std::uniform_real_distribution<double> uniformRealDistribution(0, 1);
    double expBernuli = uniformRealDistribution(randomNumberGenerator);

    //
    // initialize training examples with no responce
    //
    for (int i = 0; i < NUMBER_OF_EXAMPLES_TYPE_ONE; i++) {
        trainingExamples.push_back(make_pair(distributionTypeOne(generator), NO_RESPONCE));
    }

    //
        // initialize training examples with positive responce
    //
    for (int i = 0; i < NUMBER_OF_EXAMPLES_TYPE_TWO; i++) {
        expBernuli = uniformRealDistribution(randomNumberGenerator);
        if (expBernuli > 0.5) {
            trainingExamples.push_back(make_pair(distributionTypeTwoFirst(generator), POSSITIVE_RESPONCE));
        }
        else {
            trainingExamples.push_back(make_pair(distributionTypeTwoSecond(generator), POSSITIVE_RESPONCE));
        }
    }

    cout
        << "   NUMBER_OF_EXAMPLES_TYPE_ONE" << NUMBER_OF_EXAMPLES_TYPE_ONE << endl
        << "   NUMBER_OF_EXAMPLES_TYPE_TWO" << NUMBER_OF_EXAMPLES_TYPE_TWO << endl
        << "   MEAN_TYPE_ONE" << MEAN_TYPE_ONE << endl
        << "   STD_DEVIATION" << STD_DEVIATION << endl
        << "   MEAN_TYPE_TWO_FIRST" << MEAN_TYPE_TWO_FIRST << endl
        << "   MEAN_TYPE_TWO_SECOND" << MEAN_TYPE_TWO_SECOND << endl;

    cout << endl << "3. EXPERIMENT --->" << endl << endl;

    //
    //initialize random number generator
    //
    srand(timeSeed);

    double expResponce = 0;
    double responce;
    int trainingExampleNumber;

    //
    // start the experiment
    //
    for (int step = 1; step < NUMBER_OF_STEPS; step++) {
        for (int i = 0; i < TOTAL_NUMBER_OF_CELLS; i++) {

            //
            // skip empty cell
            //
            if (cellMassive[i] == EMPTY_CELL) {
                continue;
            }
            
            expBernuli = uniformRealDistribution(randomNumberGenerator);

            if ((TIME_STEP_VALUE/2) > expBernuli) {
                //
                // find random training example wich when applying to the i cell 
                // give us responce higher than random number from 0 to 1
                //
                do {
                    trainingExampleNumber = rand() % TOTAL_NUMBER_OF_TRAINING_EXAMPLES;
                    responce = calculateResponce(trainingExamples[trainingExampleNumber].first, cellMassive[i]);
                    expBernuli = uniformRealDistribution(randomNumberGenerator);
                } while (expBernuli > responce);
                expResponce = trainingExamples[trainingExampleNumber].second;
            }

            if (expResponce == POSSITIVE_RESPONCE) {
                if (!latestEmptyCellsIndexes.empty()) {
                    int index = latestEmptyCellsIndexes.back();
                    latestEmptyCellsIndexes.pop_back();
                    cellMassive[index] = cellMassive[i];
                }
            }

            //
            // species competition
            //
            int randomNumber = rand() % TOTAL_NUMBER_OF_CELLS;
            if (cellMassive[randomNumber] == EMPTY_CELL) {
                continue;
            } else {
                if (cellMassive[randomNumber] == cellMassive[i]) {
                    expBernuli = uniformRealDistribution(randomNumberGenerator);
                    if (TIME_STEP_VALUE > expBernuli) {
                        cellMassive[i] = EMPTY_CELL;
                        latestEmptyCellsIndexes.push_back(i);
                    }
                }
            }

            expBernuli = uniformRealDistribution(randomNumberGenerator);
            if ((TIME_STEP_VALUE/2) > expBernuli) {
                //              //
                // find random training example wich when applying to the i cell 
                // give us responce higher than random number from 0 to 1
                //
                do {
                    trainingExampleNumber = rand() % TOTAL_NUMBER_OF_TRAINING_EXAMPLES;
                    responce = calculateResponce(trainingExamples[trainingExampleNumber].first, cellMassive[i]);
                    expBernuli = uniformRealDistribution(randomNumberGenerator);
                } while (expBernuli > responce);
                expResponce = trainingExamples[trainingExampleNumber].second;
            }

            if (expResponce == POSSITIVE_RESPONCE) {
                cellMassive[i] = EMPTY_CELL;
                latestEmptyCellsIndexes.push_back(i);
            }
        }

        calculateCurrentDistribution(cellMassive, currentDistribution);

        cout << "   step " << step << " -->";
        for (int i : currentDistribution) {
            cout << i << ",";
        }
        cout << endl;

        if (EXPORT_TO_FILE_ENABLED) {
            string preparedString = to_string(step);
            for (int i = 0; i <= NUMBER_OF_CELL_TYPES; i++) {
                preparedString += "," + to_string(currentDistribution[i]);
            }
            preparedString += "\n";
            exportDataFile << preparedString;
        }
    }

    if (EXPORT_TO_FILE_ENABLED) {
        cout << endl <<  "4. EXPORT TO FILE ---> " << EXPORT_FILE_PATH << endl;
        exportDataFile.close();
    }
}

