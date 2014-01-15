/*
Author: Conor Harney
Release: 15/01/2014
Notes:
	This program calculates weight based on the Body Mass Index method.
	Standard BMI formula taken from:
		http://www.wikihow.com/Calculate-Your-Body-Mass-Index-(BMI)
		http://www.bmi-calculator.net/bmi-formula.php
	The file this program reads from can be changed by editing the "fileName" variable near the top of the code.
	For the purposes of a constantly updating file, this program will recalculate values if the same question is 
		asked multiple times rather than retaining the value from the previous time it was asked. However, this
		program will not account for an increase in file size during run time, only data replacements. The program 
		must be restarted if the file is resized.  

Purpose:
	Finds the tallest person(s) and their hight
	Finds the shortest person(s) and their hight
	Finds the youngest person(s) and their age
	Finds the oldest person(s) and their age
	Finds the mean age of all people
	Finds the number of people who are obese, underweight, and of healthy weight
	Finds the state with the biggest number of obese, underweight and people of healthy weight
	Finds the average weight loss/gain required so that 90% of people would be in healthy weight
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <cstdio>
#include <ctime>
#include <math.h>
#include <list>
using namespace std;

string fileName = "people.csv";
class StateWeightAverages;
map<string, string> splitLine(string stringIn);
map<string, int> dMYFromDob(string dobIn);
void splitDOB(string dOB, int* day, int* month, int* year, char splitter);
string getDate();
pair<string, int> isOlder(string personOneLine, string personTwoLine);
void tallyStatePercentages();
pair<string, int> getWeightStatusAndWeight(string weightIn, string hightIn);
void setUpStates();
pair<vector<string>, int> getShortestOrTallest(string find);
pair<vector<string>, int> getOldestOrYoungest(string find);
float getMeanAge();
pair<vector<string>, int> getStateWithMost(string status);
double getChangeWeightNeeded();
map<string, int> populationStatusLevels();

int m_PersonCount;
map<string, StateWeightAverages> m_StateStatisticsMap;
bool m_StatesSetUp = false;
list<string> m_States;
bool m_AppOpen = true;


class StateWeightAverages{//holdes the populations information for each state
private:
	int m_OverWeightCount;
	int m_UnderWeightCount;
	int m_HealthyWeightCount;
	int m_Population;
	int m_TotalWeight;
	int m_Totalhight;

public:
	StateWeightAverages(): m_OverWeightCount(0), m_UnderWeightCount(0), m_HealthyWeightCount(0), m_Population(0), m_TotalWeight(0){}
	string addPerson(string weight, string hight){//adds a person and returns their weight status
		m_Population++;
		pair<string, int> weightStatusAndWeight = getWeightStatusAndWeight(weight, hight);
		if(weightStatusAndWeight.first == "Obese"){m_OverWeightCount++;}
		else if(weightStatusAndWeight.first == "Underweight"){m_UnderWeightCount++;}
		else if(weightStatusAndWeight.first == "Healthy weight"){m_HealthyWeightCount++;}
		m_TotalWeight += weightStatusAndWeight.second;
		m_Totalhight += atoi(hight.c_str());
		return weightStatusAndWeight.first;
	}
	int getOverWeightCount(){return m_OverWeightCount;}
	int getUnderWeightCount(){return m_UnderWeightCount;}
	int getHealthyWeightCount(){return m_HealthyWeightCount;}
	int getTotalWeight(){return m_TotalWeight;}
	int getTotalhight(){return m_Totalhight;}
	int getPopulation(){return m_Population;}
	float getAverageWeight(){return (m_TotalWeight / m_Population);} 
};

int main () {

	setUpStates();
	string choice;

	while(m_AppOpen == true){
		cout << "What do you want to know?" << endl;
		cout << "Enter 1 to " << "find the tallest person(s) and their hight." << endl;
		cout << "Enter 2 to " << "find the shortest person(s) and their hight." << endl;
		cout << "Enter 3 to " << "find the youngest person(s) and their age." << endl;
		cout << "Enter 4 to " << "find the oldest person(s) and their age." << endl;
		cout << "Enter 5 to " << "find the mean age of all people." << endl;
		cout << "Enter 6 to " << "find the number of people who are obese, underweight, and of healthy weight." << endl;
		cout << "Enter 7 to " << "find the state with the biggest number of obese, underweight and people of healthy weight." << endl;
		cout << "Enter 8 to " << "find the average weight loss/gain required so that 90% of people would be in healthy weight." << endl;
		cout << "Enter 0 to " << "exit the aplication." << endl << endl;

		cin >> choice;

		if(choice == "1"){getShortestOrTallest("tallest");}
		else if(choice == "2"){getShortestOrTallest("shortest");}
		else if(choice == "3"){getOldestOrYoungest("youngest");}
		else if(choice == "4"){getOldestOrYoungest("oldest");}
		else if(choice == "5"){getMeanAge();}
		else if(choice == "6"){populationStatusLevels();}
		else if(choice == "7"){getStateWithMost("Obese"); getStateWithMost("HealthyWeight"); getStateWithMost("UnderWeight");}
		else if(choice == "8"){getChangeWeightNeeded();}
		else if(choice == "0"){m_AppOpen = false;}
		else {cout << "Sellection not reconised, please enter a number from the list above." << endl;}
		if(choice != "0"){system("pause"); cout << endl; }
	}
	return 0;
}

pair<vector<string>, int> getShortestOrTallest(string find){//pass back the tallest or shortest people in the file
	vector<string> targetNames;
	targetNames.push_back("");
	int targetNum = 0;
	string line;
	ifstream myfile (fileName);
	map<string, string> personOneMap;
	float counter = 0;
	
	string calcMsg;
	if(find == "shortest"){calcMsg = "Calculating shortest people...   ";}
	else if(find == "tallest"){calcMsg = "Calculating tallest people...   ";}
	else{calcMsg = "";}
	cout << calcMsg;

	if (myfile.is_open())
	{
		float percent = (counter / m_PersonCount) * 100.0f;
		int currentNum;
		while ( getline (myfile,line) )
		{
			personOneMap = splitLine(line);
			if(counter == 1){targetNum = atoi(personOneMap["hight"].c_str());}
			if(counter > 2){
				currentNum = atoi(personOneMap["hight"].c_str());
				cout<<'\b';
				cout<<'\b';
				if(percent >= 10){cout<<'\b';}
				percent = (counter / m_PersonCount) * 100.0f;
				percent = floor(percent);
				cout << percent << "%";
				
				if(find == "tallest"){
					if (currentNum >= targetNum){
						if(currentNum != targetNum){
							targetNames.clear();
						}
						targetNum = currentNum;
						targetNames.push_back(personOneMap["Name"]);
					}
				}
				else if (find == "shortest")
				{
					if (currentNum <= targetNum){
						if(currentNum != targetNum){
							targetNames.clear();
						}
						targetNum = currentNum;
						targetNames.push_back(personOneMap["Name"]);
					}
				}
					
			}
			counter++;	
		}
		myfile.close();
		for(int i = 0; i < calcMsg.size() + 4; i++){cout << '\b';}
		for(int i = 0; i < calcMsg.size() + 4; i++){cout << " ";}
		for(int i = 0; i < calcMsg.size() + 4; i++){cout << '\b';}

	}
	vector<string> tempNameList = targetNames;

	if( tempNameList.size() > 1){
		if(find == "shortest"){	cout << "The "  << tempNameList.size() << " shortest people are: " << endl;}
		else if(find == "tallest"){	cout << "The "  << tempNameList.size() << " tallest people are: " << endl;}
		while(tempNameList.size() > 1){
			cout << tempNameList.back() << ", ";
			tempNameList.pop_back();
		}
		cout << "and " << tempNameList.back() << "." << endl;
		tempNameList.pop_back();
	}
	else{
		if(find == "shortest"){cout << "The shortest person is: " << tempNameList.back()  << endl;}
		else if(find == "tallest"){cout << "The tallest person is: " << tempNameList.back()  << endl;}
	}
	cout << "At a hight of: " << targetNum << "cm"<< endl;

	return pair<vector<string>, int>(targetNames, targetNum);
}

pair<vector<string>, int> getOldestOrYoungest(string find){//pass back the oldest or youngest people in the file
	vector<string> targetNames;
	string line;
	ifstream myfile (fileName);
	string targetLine;
	int targetAge;
	map<string, string> personOneMap;
	float counter = 0;
	
	string calcMsg;
	if(find == "oldest"){calcMsg = "Calculating oldest people...   ";}
	else if(find == "youngest"){calcMsg = "Calculating youngest people...   ";}
	else{calcMsg = "";}
	cout << calcMsg;

	if (myfile.is_open())
	{
		float percent = (counter / m_PersonCount) * 100.0f;
		
		while ( getline (myfile,line) )
		{
			if(counter == 1){
				targetLine = line;
				personOneMap = splitLine(line);
				targetNames.push_back(personOneMap["Name"]);
				map<string, int> curDobMap = dMYFromDob(personOneMap["DOB"]);
				targetAge = curDobMap["Age"];
			}
			if(counter > 1){
				cout<<'\b';
				cout<<'\b';
				if(percent >= 10){cout<<'\b';}
				percent = (counter / m_PersonCount) * 100.0f;
				percent = floor(percent);
				cout << percent << "%";
				
				personOneMap = splitLine(line);
				pair<string,int> olderAndAge = isOlder(line, targetLine);
				if(find == "oldest"){
					if (olderAndAge.first == "Older"){
						targetNames.clear();
						targetAge = olderAndAge.second;
						targetLine = line;
						targetNames.push_back(personOneMap["Name"]);
					}
					else if(find == "Same"){
						targetNames.push_back(personOneMap["Name"]);
					}
				}
				else if (find == "youngest")
				{
					if (isOlder(line, targetLine).first == "Younger"){
						targetNames.clear();
						targetAge = olderAndAge.second;
						targetLine = line;
						targetNames.push_back(personOneMap["Name"]);
					}
					else if(find == "Same"){
						targetNames.push_back(personOneMap["Name"]);
					}
				}
			}
			counter++;	
		}
		myfile.close();
		for(int i = 0; i < calcMsg.size() + 4; i++){cout << '\b';}
		for(int i = 0; i < calcMsg.size() + 4; i++){cout << " ";}
		for(int i = 0; i < calcMsg.size() + 4; i++){cout << '\b';}

	}
	vector<string> tempNameList = targetNames;

	if( tempNameList.size() > 1){
		if(find == "youngest"){	cout << "The "  << tempNameList.size() << " youngest people are: " << endl;}
		else if(find == "oldest"){	cout << "The "  << tempNameList.size() << " oldest people are: " << endl;}
		while(tempNameList.size() > 2){
			cout << tempNameList.back() << ", ";
			tempNameList.pop_back();
		}
		cout << "and " << tempNameList.back() << "." << endl;
		tempNameList.pop_back();
	}
	else{
		if(find == "youngest"){cout << "The youngest person is: " << tempNameList.back()  << endl;}
		else if(find == "oldest"){cout << "The oldest person is: " << tempNameList.back()  << endl;}
	}
	cout << "At an age of: " << targetAge << " years"<< endl;

	return pair<vector<string>, int>(targetNames, targetAge);
}

map<string, string> splitLine(string stringIn){//split the line into a map of details
	std::stringstream lineStream(stringIn);
	string data;
	map<string, string> valueMap;
	int i = 0;
	while(std::getline(lineStream,data, ',')){
		switch (i)
		{
		case 0:
			valueMap["firstName"] = data; 
			break;

		case 1:
			valueMap["secondName"] = data; 
			break;

		case 2:
			valueMap["DOB"] = data; 
			break;

		case 3:
			valueMap["State"] = data; 
			break;

		case 4:
			valueMap["Sex"] = data; 
			break;

		case 5:
			valueMap["hight"] = data; 
			break;

		case 6:
			valueMap["Weight"] = data; 
			break;
		default:
			break;
		}
		i++;
	}
	string fullName = valueMap["firstName"] + " " + valueMap["secondName"];
	valueMap["Name"] = fullName;
	return valueMap;
}

map<string, int> dMYFromDob(string dobIn){//return the days, month, years and age from today
	int *bDay = new int(), *bMonth = new int(), *bYear = new int();//b : Birth
	splitDOB(dobIn, bYear, bMonth, bDay, '-');//year-month-day format
	int *cDay = new int(), *cMonth = new int(), *cYear = new int();//c : current
	string str = getDate();
	splitDOB(getDate(), cYear, cMonth, cDay, '-');
	int dDay, dMonth, dYear;//d : difference
	dYear = *cYear - *bYear;
	dMonth = *cMonth - *bMonth;
	dDay = *cDay - *bDay;

	map<string, int> retMap;
	retMap["Days"] = dDay;
	retMap["Months"] = dMonth;
	retMap["Years"] = dYear;
	if (dDay < 0)	{dMonth--;}
	if (dMonth < 0)	{dYear--;}
	retMap["Age"] = dYear;

	return retMap;
}

void splitDOB(string dOB, int* day, int* month, int* year, char splitter){//splites a string at the splitter character and allocates them to the pointers passed in
	std::stringstream lineStream(dOB);
	string data;
	map<string, string> valueMap;
	int i = 0;
	while(std::getline(lineStream,data, splitter)){
		switch (i)
		{
		case 0:
			*day = atoi(data.c_str()); 
			break;

		case 1:
			*month = atoi(data.c_str()); 
			break;

		case 2:
			*year = atoi(data.c_str());
			break;

		default:
			break;
		}
		i++;
	}
}

string getDate(){//get the current date from the OS
	time_t rawtime;
    tm* timeinfo;
    char buffer [80];
    time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    strftime(buffer,80,"%Y-%m-%d",timeinfo);
	string str = buffer;
	return str;
}

pair<string, int> isOlder(string personOneLine, string personTwoLine){//check is the first person older than the second
	map<string, string> pOneMap = splitLine(personOneLine);
	map<string, string> pTwoMap = splitLine(personTwoLine);
	map<string, int> dMYAOne = dMYFromDob(pOneMap["DOB"]);
	map<string, int> dMYATwo = dMYFromDob(pTwoMap["DOB"]);
	pair<string,int> ret = pair<string,int>("" , dMYAOne["Age"]);

	if (dMYAOne["Years"] == dMYATwo["Years"])
	{
		if (dMYAOne["Months"] == dMYATwo["Months"])
		{
			if (dMYAOne["Days"] == dMYATwo["Days"])
			{
				ret.first = "Same";
			}
			else if(dMYAOne["Days"] > dMYATwo["Days"]){ret.first = "Older";}
			else{ret.first = "Younger";}
		}
		else if(dMYAOne["Months"] > dMYATwo["Months"]){ret.first = "Older";}
		else{ret.first = "Younger";}
	}
	else if(dMYAOne["Years"] > dMYATwo["Years"]){ret.first = "Older";}
	else{ret.first = "Younger";}
	return ret;
}

float getMeanAge(){//get the mean of the populations age
	float meanAge;
	string line;
	ifstream myfile (fileName);
	map<string, string> personOneMap;
	int totalAge = 0;
	string calcMsg = "Calculating mean age... ";
	cout << calcMsg;
	m_PersonCount = -1;
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{	
			if(m_PersonCount > 1){
				personOneMap = splitLine(line);
				map<string, int> ageMap = dMYFromDob(personOneMap["DOB"]);
				totalAge += ageMap["Age"];
			}
			m_PersonCount++;
		}
		myfile.close();
	}
	else cout << "Unable to open file";
	for(int i = 0; i < calcMsg.size(); i++){cout << '\b';}
	for(int i = 0; i < calcMsg.size(); i++){cout << ' ';}
	for(int i = 0; i < calcMsg.size(); i++){cout << '\b';}
	meanAge = totalAge / m_PersonCount;
	cout << "The mean age of the population is: " << meanAge << " years old."<< endl; 
	return meanAge;
}

pair<string, int> getWeightStatusAndWeight(string weightIn, string hightIn){//return the persons weight and their weight status on the BMI chart
	pair<string, int> statusAndWeight;
	int hightInCm = atoi(weightIn.c_str());
	int weightInKG = atoi(hightIn.c_str());

	string status;
	float hightInMeters = hightInCm / 100;
	double bmi = ( weightInKG / ( hightInMeters * hightInMeters ) );
	
	if(bmi < 18){status = "Underweight";}
	else if(bmi > 30){status = "Obese";}
	else{status = "Healthy weight";}
	statusAndWeight = pair<string,int>(status, weightInKG); 

	return statusAndWeight;
}

void setUpStates(){//creates all the states and calculates the number of people in the file
	if (m_StatesSetUp == false)
	{
		string calcMsg = "Calculating, please wait...";
		cout << calcMsg;
		m_StateStatisticsMap.clear();
		m_States.clear();
		string line;
		ifstream myfile (fileName);
		map<string, string> personOneMap;
		int counter = 0;
		  if (myfile.is_open())
		  {
			while ( getline (myfile,line) )
			{
				if(counter > 0){
					personOneMap = splitLine(line);
					m_StateStatisticsMap[personOneMap["State"]].addPerson(personOneMap["Weight"], personOneMap["hight"]);
					m_States.push_back(personOneMap["State"]);
				}
				counter++;
			}
			myfile.close();
		  }
		  m_States.sort();
		  m_States.unique();
		  m_PersonCount = counter -1;
		  m_StatesSetUp = true;
		  for(int i = 0; i < calcMsg.size() + 4; i++){cout << '\b';}
		  for(int i = 0; i < calcMsg.size() + 4; i++){cout << " ";}
		  for(int i = 0; i < calcMsg.size() + 4; i++){cout << '\b';}
	}
}

pair<vector<string>, int> getStateWithMost(string status){// "Obese" / "UnderWeight" / "HealthyWeight"
	int targetNum;
	int currentNum;
	vector<string> targetStates;
	list<string> currentStatesList = m_States;
	string currentState;
	currentState = currentStatesList.front();
	currentStatesList.pop_front();

	if(status == "Obese"){
		targetNum = m_StateStatisticsMap[currentState].getOverWeightCount();
		while (currentStatesList.size() > 0)
		{
			currentState = currentStatesList.front();
			currentNum = m_StateStatisticsMap[currentState].getOverWeightCount();
			if(currentNum > targetNum){targetNum = currentNum; targetStates.clear();}
			if(currentNum >= targetNum){targetStates.push_back(currentState);}
			currentStatesList.pop_front();
		}
	}
	else if(status == "UnderWeight"){
		targetNum = m_StateStatisticsMap[currentState].getUnderWeightCount();
		while (currentStatesList.size() > 0)
		{
			currentState = currentStatesList.front();
			currentNum = m_StateStatisticsMap[currentState].getUnderWeightCount();
			if(currentNum > targetNum){targetNum = currentNum; targetStates.clear();}
			if(currentNum >= targetNum){targetStates.push_back(currentState);}
			currentStatesList.pop_front();
		}
	}
	else if(status == "HealthyWeight"){
		targetNum = m_StateStatisticsMap[currentState].getHealthyWeightCount();	
		while (currentStatesList.size() > 0)
		{
			currentState = currentStatesList.front();
			currentNum = m_StateStatisticsMap[currentState].getHealthyWeightCount();
			if(currentNum > targetNum){targetNum = currentNum; targetStates.clear();}
			if(currentNum >= targetNum){targetStates.push_back(currentState);}
			currentStatesList.pop_front();
		}
	}

	pair<vector<string>, int> retPair = pair<vector<string>, int>(targetStates, targetNum);
	if(targetNum > 0){
		if (targetStates.size() == 1)
		{
			cout << "the most " << status << " state is " << targetStates.front() << " with " << targetNum << " people " << status << "." << endl;
		}
		else if(targetStates.size() > 1)
		{
			cout << "the most " << status << " states are ";
			while(targetStates.size() > 1){
				cout << targetStates.back() << ", ";
				targetStates.pop_back();
			}
			cout << "and " << targetStates.back() << " where " << targetNum << " people are "; 
			if(status != "HealthyWeight"){cout << status << " in each state." << endl;}
			else{cout << " a healthy weight in each state." << endl;}
		}
	}
	else{
		if(status == "HealthyWeight"){cout << "They're no people of healthy weight on file." << endl;}
		else{cout << "They're no people " << status << " on file." << endl;}
	}
	return retPair;
}

map<string, int> populationStatusLevels(){//get the total number of people in the 3 BMI statuses
	map<string, int> statusNumbers;
	statusNumbers["Obese"] = 0;
	statusNumbers["HealthyWeight"] = 0;
	statusNumbers["UnderWeight"] = 0;
	list<string> currentStatesList = m_States;
	
	while(currentStatesList.size() > 0){
		statusNumbers["Obese"] += m_StateStatisticsMap[currentStatesList.back()].getOverWeightCount();
		statusNumbers["HealthyWeight"] += m_StateStatisticsMap[currentStatesList.back()].getHealthyWeightCount();
		statusNumbers["UnderWeight"] += m_StateStatisticsMap[currentStatesList.back()].getUnderWeightCount();
		currentStatesList.pop_back();
	}

	cout << statusNumbers["Obese"] << " people are obese." << endl;
	cout << statusNumbers["UnderWeight"] << " people are underweight." << endl;
	cout << statusNumbers["HealthyWeight"] << " people are at a healthy weight." << endl;
	return statusNumbers;
}

double getChangeWeightNeeded(){
	float highest = 0;
	float totalHight = 0;
	list<float> populationBMIMap;
	string line;
	ifstream myfile (fileName);
	map<string, string> personOneMap;
	float counter = 0;
	string calcMsg = "Calculating change needed...   "; 
	cout << calcMsg;
	if (myfile.is_open())
	{
		float percent = (counter /m_PersonCount) * 100.0f;
		while ( getline (myfile,line) )
		{
			if(counter > 1){
				cout<<'\b';
				cout<<'\b';
				if(percent >= 10){cout<<'\b';}
				percent = (counter /m_PersonCount) * 100.0f;
				percent = floor(percent);
				cout << percent << "%";

				personOneMap = splitLine(line);
				float curWeight = atoi(personOneMap["Weight"].c_str());
				float curhight = atoi(personOneMap["hight"].c_str());
				totalHight += curhight;
				double curBMI = curWeight / ((curhight / 100) *(curhight / 100));
				populationBMIMap.push_back(curBMI);
			}
			counter++;
		}
		myfile.close();
		populationBMIMap.sort();

		for(int i = 0; i < calcMsg.size() + 4; i++){cout << '\b';}
		for(int i = 0; i < calcMsg.size() + 4; i++){cout << " ";}
		for(int i = 0; i < calcMsg.size() + 4; i++){cout << '\b';}
	}

	list<float> tempList = populationBMIMap;
	float topTenPercent = 0;
	for(int i = 0; i < m_PersonCount / 10; i ++){
		topTenPercent += tempList.back();
		tempList.pop_back();
	}
	tempList = populationBMIMap;
	float hundredPercent = 0;
	while(tempList.size() > 0){
		hundredPercent += tempList.back();
		tempList.pop_back();
	}
	float nintyPercent = hundredPercent - topTenPercent;
	float averageBMI = nintyPercent / ((m_PersonCount / 10.0) * 9.0); //get the average BMI for the lowest 90%
	float bmiChangeNeeded;
	
	if(averageBMI < 18){ bmiChangeNeeded = 18 - averageBMI;}
	else if(averageBMI > 30){bmiChangeNeeded = 30 - averageBMI;}
	else{return 0;}

	float averageHeight = totalHight / m_PersonCount;
	float weightChangeNeeded = bmiChangeNeeded * ((averageHeight / 100) *(averageHeight / 100));
	if(weightChangeNeeded < 0){
		float temp = 0 - weightChangeNeeded;
		cout << "the average person would need to lose " << temp << " kg for 90% of the population to be of a healthy weight" << endl;
	}
	else{
		float temp = weightChangeNeeded;
		cout << "the average person would need to gain " << temp << " kg for 90% of the population to be of a healthy weight" << endl;
	}
	return weightChangeNeeded;
}