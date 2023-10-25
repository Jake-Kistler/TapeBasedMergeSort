#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>

using namespace std;

void mergeSort(const string &inputFileName, const string &outputFileName);
void merge(const string &leftFile, const string &rightFile, const string &outputFile);
int detectAndWriteRuns(const string &inputFileName);
void naturalMergeSort(const string &inputFileName, const string &outputFileName);

int main(int argc, char **argv)
{
    string fileName;
    string tempFileName = "temp.txt";

    if (argc > 1)
    {
        cout << "File name collected as a command-line argument\n";
        fileName = argv[1];
    }
    else
    {
        // Prompt the user to enter the filename
        cout << "File was not provided as a command-line argument, enter the filename: \n";
        getline(cin,fileName);
    }

    // Prompt the user to enter the filename
    cout << "Choose the type of merge sort:\n";
    cout << "1. Binary Merge Sort\n";
    cout << "2. Natural Merge Sort\n";
    cout << "Enter choice (1 or 2): ";

    int choice;
    cin >> choice;

    clock_t start, end;

    switch (choice)
    {
    case 1:
        start = clock();
        mergeSort(fileName, tempFileName);
        end = clock();
        break;

    case 2:
        start = clock();
        naturalMergeSort(fileName, tempFileName);
        end = clock();
        break;

    default:
        cout << "Invalid choice!\n";
        return 1;
    }

    string sortedFileName = fileName.substr(0, fileName.find_last_of('.')) + "_SORTED.txt";
    rename(tempFileName.c_str(), sortedFileName.c_str());
    remove(tempFileName.c_str());

    cout << "Sorted list written to the file " << sortedFileName << endl;
    cout << "Time taken for merge sort: " << ((double)(end - start)) / CLOCKS_PER_SEC << " seconds " << endl;

    return 0;
}

void mergeSort(const string &inputFileName, const string &outputFileName)
{
    ifstream input(inputFileName);
    if (!input.is_open())
    {
        cerr << "Error opening file: " << inputFileName << endl;
        return;
    }

    string line;
    getline(input, line);
    stringstream ss(line);

    int num, count = 0;
    while (ss >> num)
    {
        ++count;
    }

    if (count <= 1)
    {
        ofstream output(outputFileName);
        output << line;
        return;
    }

    string leftFile = inputFileName + ".left";
    string rightFile = inputFileName + ".right";

    ofstream leftOutput(leftFile);
    ofstream rightOutput(rightFile);

    ss.clear();
    ss.str(line);

    int midpoint = count / 2;
    for (int i = 0; i < count; ++i)
    {
        ss >> num;
        if (i < midpoint)
        {
            leftOutput << num << " ";
        }
        else
        {
            rightOutput << num << " ";
        }
    }

    input.close();
    leftOutput.close();
    rightOutput.close();

    string leftSorted = leftFile + ".sorted";
    string rightSorted = rightFile + ".sorted";

    mergeSort(leftFile, leftSorted);
    mergeSort(rightFile, rightSorted);
    merge(leftSorted, rightSorted, outputFileName);

    remove(leftFile.c_str());
    remove(rightFile.c_str());
    remove(leftSorted.c_str());
    remove(rightSorted.c_str());
}

void merge(const string &leftFile, const string &rightFile, const string &outputFile)
{
    ifstream leftInput(leftFile);
    ifstream rightInput(rightFile);
    ofstream output(outputFile);

    int leftNum, rightNum;

    bool hasLeft = static_cast<bool>(leftInput >> leftNum);
    bool hasRight = static_cast<bool>(rightInput >> rightNum);

    while (hasLeft && hasRight)
    {
        if (leftNum <= rightNum)
        {
            output << leftNum << " ";
            hasLeft = static_cast<bool>(leftInput >> leftNum);
        }
        else
        {
            output << rightNum << " ";
            hasRight = static_cast<bool>(rightInput >> rightNum);
        }
    }

    while (hasLeft)
    {
        output << leftNum << " ";
        hasLeft = static_cast<bool>(leftInput >> leftNum);
    }

    while (hasRight)
    {
        output << rightNum << " ";
        hasRight = static_cast<bool>(rightInput >> rightNum);
    }

    leftInput.close();
    rightInput.close();
    output.close();
}

int detectAndWriteRuns(const string &inputFileName)
{
    ifstream input(inputFileName);
    int num, prevNum;
    int runCount = 0;
    bool firstNum = true;
    ofstream runOutput;

    while (input >> num)
    {
        if (firstNum || num >= prevNum)
        {
            if (firstNum)
            {
                runOutput.close(); // Close previous file (if any)
                runOutput.open(inputFileName + ".run" + to_string(runCount), ofstream::out);
                firstNum = false;
            }
        }
        else
        {
            runOutput.close();
            runCount++;
            runOutput.open(inputFileName + ".run" + to_string(runCount), ofstream::out);
        }
        runOutput << num << " ";
        prevNum = num;
    }
    runOutput.close();
    input.close();
    return runCount + 1; // return number of runs
}

void naturalMergeSort(const string &inputFileName, const string &outputFileName)
{
    int runCount = 0;
    ifstream inputFile(inputFileName);

    int num, lastNum;
    if (!(inputFile >> lastNum))
        return; // Handle empty file

    // Split into runs
    while (true)
    {
        string runFileName = inputFileName + ".run" + to_string(runCount);
        ofstream runFile(runFileName);

        runFile << lastNum << " ";

        bool runEnd = false;
        while (!runEnd && inputFile >> num)
        {
            if (num >= lastNum)
            {
                runFile << num << " ";
                lastNum = num;
            }
            else
            {
                runEnd = true;
            }
        }

        runFile.close();
        runCount++;

        if (runEnd)
            lastNum = num;
        else
            break;
    }
    inputFile.close();

    // Merge runs
    int mergeCount = 0;
    while (runCount > 1)
    {
        int i = 0;

        while (i < runCount)
        {
            string firstRun = inputFileName + ".run" + to_string(i);
            string secondRun = (i + 1 < runCount) ? inputFileName + ".run" + to_string(i + 1) : "";
            string mergedRun = inputFileName + ".merged" + to_string(mergeCount);

            if (!secondRun.empty())
            {
                merge(firstRun, secondRun, mergedRun);
                remove(firstRun.c_str());
                remove(secondRun.c_str());
            }
            else
            {
                rename(firstRun.c_str(), mergedRun.c_str());
            }

            i += 2;
            mergeCount++;
        }

        // Rename merged files for next iteration
        for (int j = 0; j < mergeCount; j++)
        {
            string mergedRun = inputFileName + ".merged" + to_string(j);
            string newRun = inputFileName + ".run" + to_string(j);
            rename(mergedRun.c_str(), newRun.c_str());
        }

        runCount = mergeCount;
        mergeCount = 0;
    }

    // Rename the final run to the outputFileName
    string finalRun = inputFileName + ".run0";
    rename(finalRun.c_str(), outputFileName.c_str());
}
