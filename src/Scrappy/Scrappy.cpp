#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Scrappy_parser.h"

using namespace std;

vector<string> read_file(char* filename)
{
	fstream file(filename);
	string current_line;
	vector<string> lines;
	while (getline(file, current_line)) {
		lines.push_back(current_line);
	}
	file.close();
	return lines;
}

int main(int argc, char* argv[])
{
	char filename[200] = "test.txt";
	vector<string> lines = read_file(filename);
	vector<vector<string>> filesplit;
	for (int i = 0; i < lines.size(); i++) {
		vector<string> linesplit = split_line(lines.at(i));
		vector<string> formatted_split = format_split(linesplit);
		filesplit.push_back(formatted_split);
	}
	vector<vector<vector<string>>> seperatesplits;
	find_seperate(filesplit, seperatesplits);
	vector<vector<vector<string>>> seperatetranslated;
	vector<vector<string>> temptranslated;
	for (int i = 0; i < seperatesplits.size(); i++)
	{
		temptranslated.clear();
		for (int t = 0; t < seperatesplits.at(i).size(); t++) {
			translate_line(seperatesplits.at(i).at(t), temptranslated);
		}
		seperatetranslated.push_back(temptranslated);
	}
	for (int i = 0; i < seperatetranslated.size(); i++)
	{
		for (int t = 0; t < seperatetranslated.at(i).size(); t++)
		{
			for (int s = 0; s < seperatetranslated.at(i).at(t).size(); s++)
			{
				cout << seperatetranslated.at(i).at(t).at(s) << ", ";
			}
			cout << endl << endl;
		}
	}
	string newitems;
	vector<string> functions;
	vector<vector<string>> funcargtypes;
	vector<string> variables;
	vector<string> variabletypes;
	json line;
	vector<json> linesjson;
	vector<vector<json>> funclines;
	for (int i = 0; i < seperatetranslated.size(); i++)
	{
		linesjson.clear();
		for (int t = 0; t < seperatetranslated.at(i).size(); t++)
		{
			line.clear();
			prepare(seperatetranslated.at(i).at(t), functions, funcargtypes, variables, variabletypes, { "print" }, line, newitems);
			if (newitems == "function") {
				functions.push_back(line["funcdec"]["funcname"]);
				vector<string> temp;
				temp.clear();
				for (int i = 0; i < line["funcdec"]["argam"]; i++) {
					temp.push_back(line["funcdec"]["arg" + to_string(i)]["argtype"]);
				}
				funcargtypes.push_back(temp);
			}
			else if (newitems == "variable") {
				variables.push_back(line["varcreate"]["varname"]);
				variabletypes.push_back(line["varcreate"]["vartype"]);
			}
			linesjson.push_back(line);
		}
		funclines.push_back(linesjson);
	}
}

//A:\big_projects\Scrappy\Debug\Scrappy hello hello