#include "stdafx.h"
#include <vector>
#include <iostream>
#include <fstream>
#include<algorithm>
#include <string>

using namespace std;

struct Station {
	int identificator = 0;
	int number;
	vector<int> statesList;
};
struct NewStation {
	int identificator = 0;
	int number;
	vector<int> possibleState;
	vector<Station> statesList;
};

void SplitString(const string str, vector<int>& elements) {
	int length = str.length();

	bool isProcessed = true;
	int element = 0;
	int x = 0;

	for (int i = 0; i < length; i++) {
		if (str[i] != ' ') {
			isProcessed = false;
			x = (int)str[i] - 48;
			element = element * 10 + x;
		}
		else {
			elements.push_back(element);
			element = 0;
			isProcessed = true;
		}
	}

	if (!isProcessed) {
		elements.push_back(element);
		element = 0;
		isProcessed = true;
	}
}

int GetIdentificator(vector<int> & vect) {

	int identificator = 0;

	sort(vect.begin(), vect.end());

	int vectorSize = vect.size();

	for (int i = 0; i < vectorSize; i++) {

		if ((vectorSize == 1) && (vect[i] == 0)) {
			identificator = 0;
		}
		else {
			identificator = identificator * 10 + vect[i] + 1;
		}
	}

	return identificator;
}

void FillStateVector(const int stateNumber, const string str, vector<vector<Station>>& machine) {
	vector<int> elements;
	SplitString(str, elements);

	int vectorSize = elements.size();
	int z;
	int x;
	for (int i = 0; i < vectorSize; i++) {
		z = elements[i];
		i++;
		x = elements[i];
		machine[stateNumber][x].statesList.push_back(z);
	}
}

void ProcessInputData(char* fileName, int& xAmount, int& zAmount, int& fAmount, vector<int>& finalStates, vector<vector<Station>>& machine) {

	ifstream ifs(fileName);

	if (ifs.is_open())
	{
		ifs >> xAmount;
		ifs >> zAmount;
		ifs >> fAmount;

		int finalState;
		for (int i = 0; i < fAmount; i++) {
			ifs >> finalState;
			finalStates.push_back(finalState);
		}

		//увеличиваем размер массива
		machine.resize(zAmount);
		for (int i = 0; i < zAmount; i++) {
			machine[i].resize(xAmount);
		}

		string str;
		getline(ifs, str);
		int stateNumber = 0;
		while (!ifs.eof() && (stateNumber < zAmount))
		{
			getline(ifs, str);
			FillStateVector(stateNumber, str, machine);

			for (int i = 0; i < xAmount; i++) {
				machine[stateNumber][i].identificator = GetIdentificator(machine[stateNumber][i].statesList);
			}
			stateNumber++;
		};
	}
}

void AddNewState(vector<NewStation>& newMachine, vector<vector<Station>>& machine, Station& newState, vector<Station>& newStates, const int xAmount) {

	NewStation addedState;
	addedState.identificator = newState.identificator;
	addedState.possibleState = newState.statesList;
	addedState.statesList.resize(xAmount);

	int stAmount = newState.statesList.size();

	int currState;

	for (int i = 0; i < xAmount; i++) {
		Station currStation;

		for (int k = 0; k < stAmount; k++) {
			currState = newState.statesList[k];

			int listSize = machine[currState][i].statesList.size();

			for (int j = 0; j < listSize; j++) {
				bool isExistingElement = find(currStation.statesList.begin(), currStation.statesList.end(), machine[currState][i].statesList[j]) != currStation.statesList.end();

				if (!isExistingElement) {
					currStation.statesList.push_back(machine[currState][i].statesList[j]);
				}
				
			}
		}

		currStation.identificator = GetIdentificator(currStation.statesList);
		addedState.statesList[i] = currStation;

		newStates.push_back(currStation);
	}

	newMachine.push_back(addedState);
}

void CreateNewMachine(vector<NewStation>& newMachine, vector<vector<Station>>& machine, vector<int>& existingStates, const int xAmount, vector<Station>& newStates) {

	int currStateAmount = newMachine.size();

	Station currState = newStates[0];
	newStates.erase(newStates.begin());

	if (find(existingStates.begin(), existingStates.end(), currState.identificator) != existingStates.end()) {
		int newStatesSize = newStates.size();
		if (newStatesSize > 0) {
			CreateNewMachine(newMachine, machine, existingStates, xAmount, newStates);
		}
	}
	else {
		AddNewState(newMachine, machine, currState, newStates, xAmount);
		existingStates.push_back(currState.identificator);
		CreateNewMachine(newMachine, machine, existingStates, xAmount, newStates);
	}

}

vector<int> GetFinalStates(vector<NewStation>& newMachine, vector<int> finalStates){

	int machineSize = newMachine.size();

	vector<int> newFinalStates;

	for (int i = 0; i < machineSize; i++) {

		int statesListSize = newMachine[i].possibleState.size();

		for (int j = 0; j < statesListSize; j++) {

			bool isСoincidentalStates = find(finalStates.begin(), finalStates.end(), newMachine[i].possibleState[j]) != finalStates.end();

			if (isСoincidentalStates) {
				bool isExistingElement = find(newFinalStates.begin(), newFinalStates.end(), newMachine[i].identificator) != newFinalStates.end();

				if (!isExistingElement) {
					newFinalStates.push_back(newMachine[i].identificator);
				}
			}
		}
	}

	return newFinalStates;
};

void CorrectMachine(vector<NewStation>& newMachine, int& xAmount, vector<int>& compoundStates) {

	int machineSize = newMachine.size();

	for (int i = 0; i < machineSize; i++) {
		compoundStates.push_back(newMachine[i].identificator);
		newMachine[i].number = i;

		for (int j = 0; j < xAmount; j++) {
		
			if (newMachine[i].statesList[j].statesList.size() == 0) {
				newMachine[i].statesList[j].identificator = -1;
			};
		}
	}
}

void Visualization(vector<NewStation>& newMachine, int& xAmount, vector<int>& finalStates, vector<int>& compoundStates) {

	ofstream fout;
	fout.open("out.dot");

	int machinieSize = newMachine.size();

	fout << "digraph G{" << endl;

	for (int i = 0; i < machinieSize; i++) {
		bool isFinalState = find(finalStates.begin(), finalStates.end(), newMachine[i].identificator) != finalStates.end();

		if (isFinalState) {
			fout << "node [shape=star]" << "\"" << i << "\"";
		}
		else {
			fout << "node [shape=circle]" << "\"" << i << "\"";
		}

		fout << endl;
	}

	for (int i = 0; i < machinieSize; i++) {
		for (int j = 0; j < xAmount; j++) {
			if (newMachine[i].statesList[j].identificator >= 0) {
				int stateNumber;

				vector<int>::iterator it = find(compoundStates.begin(), compoundStates.end(), newMachine[i].identificator);
				stateNumber = (it - compoundStates.begin());
				fout << "\"" << stateNumber << "\"" << "->";

				it = find(compoundStates.begin(), compoundStates.end(), newMachine[i].statesList[j].identificator);
				stateNumber = (it - compoundStates.begin());
				fout << "\"" << stateNumber << "\"";

				fout << "[label=\"" << j << "\"];" << endl;
			}
		}
	}

	fout << "}" << endl;
}

void PrintResult(vector<NewStation>& newMachine, int& xAmount, vector<int>& finalStates, vector<int>& compoundStates) {

	ofstream fout;
	fout.open("out.txt");

	int machineSize = newMachine.size();

	fout << xAmount << endl;
	fout << machineSize << endl;

	int finalStateAmount = finalStates.size();
	fout << finalStateAmount << endl;

	for (int j = 0; j < finalStateAmount; j++) {
		vector<int>::iterator it = find(compoundStates.begin(), compoundStates.end(), finalStates[j]);
		fout << (it - compoundStates.begin()) << " ";
	}
	fout << endl;


	for (int j = 0; j < xAmount; j++) {
		for (int i = 0; i < machineSize; i++) {
			int id = newMachine[i].statesList[j].identificator;

			if (id > 0) {
				vector<int>::iterator it = find(compoundStates.begin(), compoundStates.end(), id);
				fout << (it - compoundStates.begin()) << " ";
			}
			else {
				fout << id << " ";
			}
		}
		fout << endl;
	}
}


int main(int argc, char* argv[])
{
	int xAmount;
	int zAmount;
	int fAmount = 0;
	vector<int> finalStates;
	vector<vector<Station>> machine;
	ProcessInputData(argv[1], xAmount, zAmount, fAmount, finalStates, machine);

	vector<NewStation> newMachine;
	vector<int> existingStates;
	vector<Station> newStates;

	int startState = 0;
	newMachine.resize(1);
	newMachine[startState].identificator = startState;
	existingStates.push_back(startState);
	newMachine[startState].possibleState.push_back(startState);
	for (int i = 0; i < xAmount; i++) {
		Station newStation;
		newMachine[startState].statesList.push_back(machine[startState][i]);
		newStation = machine[startState][i];
		newStates.push_back(newStation);
	}
	CreateNewMachine(newMachine, machine, existingStates, xAmount, newStates);

	vector<int> newFinalStates = GetFinalStates(newMachine, finalStates);
	vector<int> compoundStates;
	CorrectMachine(newMachine, xAmount, compoundStates);
	Visualization(newMachine, xAmount, newFinalStates, compoundStates);
	PrintResult(newMachine, xAmount, newFinalStates, compoundStates);

	return 0;
}