#include <bits/stdc++.h>

using namespace std;

vector<vector<float>> population, offsprings;
vector<pair<float, int>> sortedFitness; //mapped with the old index for each fitness
vector<pair<float, int>> sortedOffspringFitness;
int numberOfPoints, degree, numberOfCoefficients;
const int populationSize = 10, MaxNumberOfGenerations = 10;
const float pc = 0.4, pm = 0.1, LB = -10, UB = 10, b = 1;
float pointX[100], pointY[100], populationFitness[populationSize], offspringsFitness[populationSize];


void clear2DVector(vector<vector<float>> &v)
{
    v.resize(populationSize);
    for(int i = 0; i < populationSize; i++)
    {
        v[i].resize(populationSize);
        for(int j = 0; j<numberOfCoefficients; j++)
        {
            v[i][j] = 0;
        }
    }
}

int randomNumber(int a, int b)
{
    static bool first = true;
    if(first)
    {
        srand(time(NULL));
        first = false;
    }
    return a + (rand() % b);
}

float floatRandomNumber(float a, float b)
{
    static bool second = true;
    if(second)
    {
        srand(time(NULL));
        second = false;
    }
    float result = a + static_cast<float>(rand()) * static_cast<float>(b - a) / RAND_MAX;
    float finalResult = ceil(result * 100.0) / 100.0;
    return finalResult;
}

void sortArr(float arr[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        sortedFitness.push_back(make_pair(arr[i], i));
    }

    sort(sortedFitness.begin(), sortedFitness.end());
}

void sortArrOffsprings(float arr[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        sortedOffspringFitness.push_back(make_pair(arr[i], i));
    }

    sort(sortedOffspringFitness.begin(), sortedOffspringFitness.end());
}

void initializePopulation()
{
    population.resize(populationSize);
    offsprings.resize(populationSize);
    for(int i=0; i<populationSize; i++)
    {
        population[i].resize(numberOfCoefficients);
        offsprings[i].resize(numberOfCoefficients);
        populationFitness[i] = 0;
        for(int j=0; j<numberOfCoefficients; j++)
        {
            population[i][j] = 0;
            offsprings[i][j] = 0;

            float randomCoefficient = floatRandomNumber(-10.0, 10.0);
            population[i][j] = randomCoefficient;
        }
    }
}

void printPopulation()
{
    for(int i=0; i<populationSize; i++)
    {
        for(int j=0; j<numberOfCoefficients; j++)
        {
            cout<< population[i][j] << " ";
        }
        cout<<endl;
    }
}

float errorAtOnePoint(vector<float> chromosome, float x, float y)
{
    float sum = 0;
    for(int j=0; j<numberOfCoefficients; j++)
    {
        sum += chromosome[j] * pow(x, j);
    }
    sum -= y;
    float result = pow(sum, 2);
    return result;
}

float MSE(vector<float> chromosome)
{
    float sum = 0;
    for(int i=0; i<numberOfPoints; i++)
    {
        sum += errorAtOnePoint(chromosome, pointX[i], pointY[i]);
    }
    sum /= numberOfPoints;
    return sum;
}

void evaluateFitness()
{
    for(int i=0; i<populationSize; i++)
    {
        //using total error to represent fitness, the smaller the better
        populationFitness[i] = MSE(population[i]);
    }
    sortArr(populationFitness, populationSize);
}

void calcOffspringsFitness()
{
    for(int i=0; i<populationSize; i++)
    {
        //using total error to represent fitness, the smaller the better
        offspringsFitness[i] = MSE(offsprings[i]);
    }
}

void tournamentSelection()
{
    for(int i=0; i<populationSize; i++)
    {
        int randomIndex1 = randomNumber(0, populationSize);
        int randomIndex2 = randomNumber(0, populationSize);
        if(populationFitness[randomIndex1] <= populationFitness[randomIndex2])
        {
            offsprings[i] = population[randomIndex1];
        }
        else
        {
            offsprings[i] = population[randomIndex2];
        }
    }
    calcOffspringsFitness();
}

void twoPointCrossover()
{
    int siz;
    //handle if the size is odd
    (populationSize%2!=0)? siz=populationSize-1 : siz=populationSize;

    for(int i=0; i<siz; i+=2)
    {
        float r2 = floatRandomNumber(0.0, 1.0);

        //perform crossover only if r2 <= pc
        if(r2 <= pc)
        {
            int separationPoint1 = randomNumber(1, numberOfCoefficients/2);
            int separationPoint2 = randomNumber(1, numberOfCoefficients-1);
            while(separationPoint1 == separationPoint2)
            {
                separationPoint2 = randomNumber(1, numberOfCoefficients-1);
            }

            vector<float> offspring1(numberOfCoefficients, 0), offspring2(numberOfCoefficients, 0);


            //same
            for(int j=0; j<separationPoint1; j++)
            {
                offspring1[j] = offsprings[i][j];
                offspring2[j] = offsprings[i+1][j];
            }
            //crossover happens here
            for(int j=separationPoint1; j<separationPoint2; j++)
            {
                offspring1[j] = offsprings[i+1][j];
                offspring2[j] = offsprings[i][j];
            }
            //same
            for(int j=separationPoint2; j<numberOfCoefficients; j++)
            {
                offspring1[j] = offsprings[i][j];
                offspring2[j] = offsprings[i+1][j];
            }

            offsprings[i] = offspring1;
            offsprings[i+1] = offspring2;
            calcOffspringsFitness();
        }

    }
}

void nonUniformMutation(int t)
{
    for(int i=0; i<populationSize; i++)
    {

        for(int j=0; j<numberOfCoefficients; j++)
        {
            float rm = floatRandomNumber(0.0, 1.0);
            if(rm < pm)
            {
                float y, deltaL, deltaU;
                deltaL = offsprings[i][j] - LB;
                deltaU = UB - offsprings[i][j];
                float r1 = floatRandomNumber(0.0, 1.0);

                if(r1 <= 0.5)
                    y = deltaL;
                else
                    y = deltaU;

                float r = floatRandomNumber(0.0, 1.0);
                float term = pow( 1 - (t/MaxNumberOfGenerations) , b);
                float change = y * (1 - pow(r, term));
                if(y == deltaL)
                    offsprings[i][j] -= change;
                else
                    offsprings[i][j] += change;

                calcOffspringsFitness();
            }
        }
    }
}

void elitismReplacement()
{
    sortArrOffsprings(offspringsFitness, populationSize);


    vector<vector<float>> bestOfBothWorlds;
    clear2DVector(bestOfBothWorlds);


    int cntp=0, cnto=0;
    for(int i=0; i<populationSize; i++)
    {

        int parentIndex = sortedFitness[cntp].second;
        float parentValue = sortedFitness[cntp].first;
        int offspringIndex = sortedOffspringFitness[cnto].second;
        float offspringValue = sortedOffspringFitness[cnto].first;

        if(parentValue <= offspringValue)
        {
            cntp++;
            bestOfBothWorlds[i] = population[parentIndex];
        }
        else{
            cnto++;
            bestOfBothWorlds[i] = offsprings[offspringIndex];
        }
    }

    clear2DVector(population);
    population = bestOfBothWorlds;
    clear2DVector(offsprings);
    //cout<<"\n ------------------------------- \n";

}

int getFittestIndex()
{
    int fittestIndex;
    evaluateFitness();
    float minFitness = populationFitness[0];

    for(int i = 0; i < populationSize; i++)
    {
        if(populationFitness[i] <= minFitness)
        {
            minFitness = populationFitness[i];
            fittestIndex = i;
        }
    }
    return fittestIndex;
}

int main()
{

    freopen("input.txt", "rt", stdin);
    freopen("output.txt", "wt", stdout);
    int testCase, t = 1;
    cin>>testCase;
    while(testCase--)
    {
        clear2DVector(offsprings);
        cin>> numberOfPoints >> degree;

        numberOfCoefficients = degree+1;

        for(int i=0; i<numberOfPoints; i++)
        {
           cin>>pointX[i]>> pointY[i];
        }

        initializePopulation();

        for(int i=0; i < MaxNumberOfGenerations; i++)
        {
            evaluateFitness();
            tournamentSelection();
            twoPointCrossover();
            nonUniformMutation(i);
            elitismReplacement();
        }

        cout<<"\nTest Dataset Index: "<< t << endl;
        int fittestIndex = getFittestIndex();
        vector<float> fittestChromosome = population[fittestIndex];

        cout<<"\nCoefficients: \n";
        for(int i = 0; i < numberOfCoefficients; i++)
        {
            cout << "a" << i << " = ";
            cout << fittestChromosome[i] << endl;
        }

        cout<<"\nMean Square Error: " << populationFitness[fittestIndex] <<endl;
        cout<<"\n****************************************\n";
        t++;
    }
    return 0;
}
