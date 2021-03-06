#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Station {
	int Q;
	int Y;
};
struct EquivalenceClass {
	int value;
	vector<int> members;
};


void ExpandVector(vector<vector<Station>>& machine, int x, int q) {
	machine.resize(x);
	for (int i = 0; i < x; ++i) {
		machine[i].resize(q);
	}
}

void GetMur(vector<vector<Station>>& machine, const vector<int>& elements, int x, int q) {

	int j = 0;
	Station element;

	ExpandVector(machine, x + 1, q);

	for (int i = 0; i < q; i++) {
		element.Y = elements[i];
		machine[j][i] = element;
	}

	int lastIndex = (x + 1)*q;
	j++;

	for (int i = q; i < lastIndex; i++) {
		element.Q = elements[i];
		machine[j][i % q] = element;

		if (i % q == q-1) {
			j++;
		}
	}
}

void GetMili(vector<vector<Station>>& machine, const vector<int>& elements, int x, int q) {

	int j = 0;
	int k = 0;
	Station element;
	int lastIndex = 2 * q * x;

	ExpandVector(machine, x, q);

	for (int i = 0; i < lastIndex - 1; i = i + 2) {

		if (((i % (2 * q)) == 0) && (i > 0)) {
			k = 0;
			j++;
		}

		element.Q = elements[i];
		element.Y = elements[i+1];
		machine[j][k] = element;
		k++;
	}	
}

void GetData(char* fileName, vector<vector<Station>>& machine, int& machineType, int& yAmount, vector<int>& statesToClass) {

	ifstream ifs(fileName);

	if (ifs.is_open())
	{
		ifs >> machineType;

		int x;
		ifs >> x;

		ifs >> yAmount;

		int q;
		ifs >> q;
		statesToClass.resize(q);

		int element;
		vector<int> elements;

		int elementAmount;

		if (machineType == 1) {
			elementAmount = (x + 1) * q;

			for (int i = 0; i < elementAmount; i++) {
				ifs >> element;
				elements.push_back(element);
			}

			GetMur(machine, elements, x, q);
		}
		else {
			elementAmount = 2 * x * q;

			for (int i = 0; i < elementAmount; i++) {
				ifs >> element;
				elements.push_back(element);
			}

			GetMili(machine, elements, x, q);			
		}
	}
}

void AddElement(vector<EquivalenceClass>& classToStates,  int value, int element) {
	int vector_size = classToStates.size();

	bool isExist = false;

	for (int j = 0; j < vector_size; j++) {
		if (classToStates[j].value == value) {
			isExist = true;
			classToStates[j].members.push_back(element);
		}
	}

	if (!isExist) {
		EquivalenceClass member;
		member.value = value;
		member.members.push_back(element);
		classToStates.push_back(member);
	}
}

void DefineZeroEquivalence(int machineType, const vector<vector<Station>>&  machine, vector<EquivalenceClass>& classToStates,
	vector<int>& statesToClass) {

	int vector_size1 = machine.size();
	int vector_size2 = machine[0].size();

	if (machineType == 1) {
		for (int i = 0; i < vector_size2; i++) {
			AddElement(classToStates, machine[0][i].Y, i);
		}
	}
	else {
		for (int j = 0; j < vector_size2; j++) {
			int value = 0;
			for (int i = 0; i < vector_size1; i++) {
				value = value * 10 + machine[i][j].Y;
			}

			AddElement(classToStates, value, j);
		}
	}

	int vector_size = classToStates.size();

	for (int i = 0; i < vector_size; i++) {
		vector<int> m = classToStates[i].members;
		int vector_size1 = m.size();

		for (int j = 0; j < vector_size1; j++) {
			statesToClass[m[j]] = i;
		}
	}
}

vector<vector<int>> GetStates(vector<vector<Station>>& machine, int machineType) {

	vector<vector<int>> table;
	int vector_size1 = machine.size();
	int vector_size2 = machine[0].size();

	int i = 0;
	if (machineType == 1) {
		i = 1;
	}

	for (i; i < vector_size1; i++) {
		vector<int> line;
		for (int j = 0; j < vector_size2; j++) {
			line.push_back(machine[i][j].Q);
		}
		table.push_back(line);
	}

	return table;
}

void Minimize(const vector<vector<int>>& stateToState, vector<EquivalenceClass>& classToStates,
	vector<int>& statesToClass) {

	int vector_size1 = classToStates.size();
	vector<EquivalenceClass> newClassToStates;

	for (int i = 0; i < vector_size1; i++) {
		int vector_size2 = classToStates[i].members.size();

		vector<EquivalenceClass> additionalClassToStates;

		for (int j = 0; j < vector_size2; j++) {

			int station = classToStates[i].members[j];
			int vector_size3 = stateToState.size();
			int value = 0;

			for (int k = 0; k < vector_size3; k++) {
				 int index = stateToState[k][station];
				value = value * 10 + statesToClass[index];
			}

			AddElement(additionalClassToStates, value, classToStates[i].members[j]);
			value = 0;
		}

		int vector_size = additionalClassToStates.size();

		for (int i = 0; i < vector_size; i++) {
			newClassToStates.push_back(additionalClassToStates[i]);
		}
	}

	int vector_size = newClassToStates.size();

	for (int i = 0; i < vector_size; i++) {
		vector<int> m = newClassToStates[i].members;
		int vector_size1 = m.size();
		for (int j = 0; j < vector_size1; j++) {
			statesToClass[m[j]] = i;
		}
	}

	int last_vector_size = classToStates.size();
	int new_vector_size = newClassToStates.size();

	if (last_vector_size < new_vector_size) {
		classToStates = newClassToStates;
		Minimize(stateToState, classToStates, statesToClass);
	}
}

vector<vector<Station>> GetNewMachine(const vector<vector<Station>>& machine, int machineType, const vector<EquivalenceClass>& classToStates,
	const vector<int>& statesToClass) {

	vector<vector<Station>> newMachine;

	int statesAmount = classToStates.size();
	int lineAmount = machine.size();
	int elementFromClass;

	ExpandVector(newMachine, lineAmount, statesAmount);

	if (machineType == 1) {
		for (int i = 0; i < statesAmount; i++) {
			elementFromClass = classToStates[i].members[0];
			newMachine[0][i].Y = machine[0][elementFromClass].Y;
		}

		for (int i = 0; i < statesAmount; i++) {
			for (int j = 1; j < lineAmount; j++) {
				elementFromClass = classToStates[i].members[0];
				int elementFromMachine = machine[j][elementFromClass].Q;
				newMachine[j][i].Q = statesToClass[elementFromMachine];
			}
		}
	}
	else {
		for (int i = 0; i < statesAmount; i++) {

			elementFromClass = classToStates[i].members[0];

			for (int j = 0; j < lineAmount; j++) {
				int elementFromMachine = machine[j][elementFromClass].Q;
				newMachine[j][i].Q = statesToClass[elementFromMachine];
				newMachine[j][i].Y = machine[j][elementFromClass].Y;
			}
		}
	}

	return newMachine;
}

void PrintResult(vector<vector<Station>> machine, int machineType, int yAmount) {
	ofstream fout;
	fout.open("out.txt");

	fout << machineType << endl;

	int vector_size1 = machine.size();
	int vector_size2 = machine[0].size();

	if (machineType == 1) {
		fout << vector_size1 - 1 << endl;
		fout << yAmount << endl;
		fout << vector_size2 << endl;

		for (int i = 0; i < vector_size2; i++) {
			fout << machine[0][i].Y << " ";
		}
		fout << endl;

		for (int i = 1; i < vector_size1; i++) {
			for (int j = 0; j < vector_size2; j++) {
				fout << machine[i][j].Q << " ";
			}
			fout << endl;
		}	
	}
	else {
		fout << vector_size1 << endl;
		fout << yAmount << endl;
		fout << vector_size2 << endl;

		for (int i = 0; i < vector_size1; i++) {
			for (int j = 0; j < vector_size2; j++) {
				fout << machine[i][j].Q << " ";
				fout << machine[i][j].Y << " ";
			}
			fout << endl;
		}
	}
}

void Visualization(vector<vector<Station>> machine, int machineType) {

	ofstream fout;
	fout.open("out.dot");

	int vector_size1 = machine.size();
	int vector_size2 = machine[0].size();
	int point;

	fout << "digraph G{" << endl;

	if (machineType == 1) {
		for (int j = 0; j < vector_size2; j++) {
			for (int i = 1; i < vector_size1; i++) {
				fout << "\"" << j << "/" << machine[0][j].Y << "\"" << "->";
				point = machine[i][j].Q;
				fout << "\"" << point << "/" << machine[0][point].Y << "\"";
				fout << "[label=\"" << i - 1 << "\"];" << endl;
			}
		}
	}
	else {
		for (int j = 0; j < vector_size2; j++) {
			for (int i = 0; i < vector_size1; i++) {
				fout << "\"" << j << "\"" << "->";
				point = machine[i][j].Q;
				fout << "\"" << point << "\"";
				fout << "[label=\"" << i << "/" << machine[i][j].Y << "\"];" << endl;
			}
		}
	}
	fout << "}" << endl;
}


int main(int argc, char* argv[])
{
	int machineType;
	int yAmount;
	vector<int> statesToClass;
	vector<vector<Station>> machine;
	vector<EquivalenceClass> classToStates;

	GetData(argv[1], machine, machineType, yAmount, statesToClass);

	DefineZeroEquivalence(machineType, machine, classToStates, statesToClass);

	vector<vector<int>> stateToState;

	stateToState = GetStates(machine, machineType);

	Minimize(stateToState, classToStates, statesToClass);

	vector<vector<Station>> newMachine;
	newMachine = GetNewMachine(machine, machineType, classToStates, statesToClass);

	PrintResult(newMachine, machineType, yAmount);

	Visualization(newMachine, machineType);

    return 0;
}