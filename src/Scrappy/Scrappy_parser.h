#include <nlohmann/json.hpp>
#include <vector>
#include <limits>

using namespace nlohmann;
using namespace std;

string splitters[18] = { "=", "+", "<-", "-" , "\"", "'", ";", "<", ">", "\\" , "(", ")", "{", "}", "OR", "AND", "NOT", "Global" };
string vartype[5] = { "int", "string", "float", "char" , "bool" };
string statements[3] = { "if", "for", "while" };

vector<string> split_line(string line)
{
	if (line != "") {
		int pos;
		vector<string> lineparse;
		int index;
		int next = numeric_limits<int>::max();;
		int iofnext = -1;
		for (int i = 0; i < 15; i++)
		{
			pos = line.find(splitters[i]);
			if (pos != string::npos) {
				if (pos < next) {
					next = pos;
					iofnext = i;
				}
			}
		}
		pos = line.find(" ");
		do {
			string sect = "";
			if (pos != next - 1 && pos < next && pos != string::npos) {
				sect = line.substr(0, pos);
				while ((index = sect.find(" ")) != string::npos) {
					sect.replace(index, 1, "");
				}
				if (sect != "") {
					lineparse.push_back(sect);
				}
				lineparse.push_back(" ");
				line.erase(line.begin(), line.begin() + pos + 1);
			}
			else {
				sect = line.substr(0, next);
				while ((index = sect.find(" ")) != string::npos) {
					sect.replace(index, 1, "");
				}
				if (sect != "") {
					lineparse.push_back(sect);
				}
				lineparse.push_back(splitters[iofnext]);
				line.erase(line.begin(), line.begin() + next + splitters[iofnext].length());
			}
			next = numeric_limits<int>::max();
			iofnext = -1;
			for (int i = 0; i < 18; i++)
			{
				pos = line.find(splitters[i]);
				if (pos != string::npos) {
					if (pos < next) {
						next = pos;
						iofnext = i;
					}
				}
			}
			pos = line.find(" ");
		} while (pos != string::npos || next != numeric_limits<int>::max());
		return lineparse;
	}
	else {
		return {};
	}
}

vector<string> format_split(vector<string> split)
{
	vector<string> formatted_split;
	bool instring = false;
	bool inchar = false;
	for (int i = 0; i < split.size(); i++)
	{
		if (split.at(i) == " " && (instring == true || inchar == true))
		{
			formatted_split.push_back(split.at(i));
		}
		else if (split.at(i) == "\"" && inchar == false && !(instring == true && split.at(i - 1) == "\\")) {
			instring = !instring;
			formatted_split.push_back(split.at(i));
		}
		else if (split.at(i) == "'" && instring == false && !(inchar == true && split.at(i - 1) == "\\")) {
			inchar = !inchar;
			formatted_split.push_back(split.at(i));
		}
		else if (split.at(i) != " " && split.at(i) != "") {
			formatted_split.push_back(split.at(i));
		}
	}
	return formatted_split;
}

void find_seperate(vector<vector<string>> splittext, vector<vector<vector<string>>>& returnvals)
{
	vector<vector<string>> currentsect;
	int lev = 0;
	bool entered = false;
	bool tobreak = false;
	for (int i = 0; i < splittext.size(); i++)
	{
		bool instring = false;
		bool inchar = false;
		if (splittext.at(i).size() != 0) {
			for (int t = 0; t < splittext.at(i).size(); t++) {
				if (splittext.at(i).at(t) == "{" && (instring == false && inchar == false)) {
					lev++;
					entered = true;
				}
				else if (splittext.at(i).at(t) == "}" && (instring == false && inchar == false)) {
					lev--;
					if (lev == 0 && entered == true) {
						entered = false;
						tobreak = true;
					}
				}
				else if (splittext.at(i).at(t) == "\"" && inchar == false && !(instring == true && splittext.at(i).at(t - 1) == "\\")) {
					instring = !instring;
				}
				else if (splittext.at(i).at(t) == "'" && instring == false && !(inchar == true && splittext.at(i).at(t - 1) == "\\")) {
					inchar = !inchar;
					tobreak = false;
				}
			}
			currentsect.push_back(splittext.at(i));
			if (tobreak == true) {
				returnvals.push_back(currentsect);
				currentsect.clear();
			}
		}
	}
}

void translate_line(vector<string> line, vector<vector<string>>& returnvec)
{
	vector<string> seperate;
	for (int i = 0; i < line.size(); i++)
	{
		bool notany = false;
		if (line.at(i) == "{") {
			if (seperate.size() == 0) {
				seperate.push_back("{");
				returnvec.push_back(seperate);
				seperate.clear();
			}
			else {
				returnvec.push_back(seperate);
				seperate.clear();
				seperate.push_back("{");
				returnvec.push_back(seperate);
				seperate.clear();
			}
			notany = true;
		}
		else if (line.at(i) == "}") {
			if (seperate.size() == 0) {
				seperate.push_back("}");
				returnvec.push_back(seperate);
				seperate.clear();
			}
			else {
				returnvec.push_back(seperate);
				seperate.clear();
				seperate.push_back("}");
				returnvec.push_back(seperate);
				seperate.clear();
			}
			notany = true;
		}
		else if (line.at(i) == "Global") {
			seperate.push_back("Globalvals");
			notany = true;
		}
		else if (line.at(i) == "func") {
			seperate.push_back("funcdec");
			notany = true;
		}
		for (int t = 0; t < 5; t++)
		{
			if (line.at(i) == vartype[t]) {
				try {
					if (line.at(i - 1) == "func") {
						seperate.push_back("funcret" + vartype[t]);
					}
					else {
						seperate.push_back("varcreate" + vartype[t]);
					}
				}
				catch (exception) {
					seperate.push_back("varcreate" + vartype[t]);
				}
				notany = true;
			}
		}
		for (int t = 0; t < 3; t++)
		{
			if (line.at(i) == statements[t]) {
				seperate.push_back(statements[t]);
				notany = true;
			}
		}
		try {
			if (seperate.at(seperate.size() - 1) == "funcret" || seperate.at(seperate.size() - 1) == "varcreate") {
				seperate.push_back(line.at(i));
				notany = true;
			}
		}
		catch (exception) {
			;
		}
		for (int t = 0; t < 15; t++)
		{
			if (splitters[t] == line.at(i)) {
				seperate.push_back(line.at(i));
				notany = true;
			}
		}
		if (notany == false) {
			seperate.push_back(line.at(i));
		}
	}
	if (seperate.size() != 0) {
		returnvec.push_back(seperate);
	}
}

void prepare(vector<string> splitline, vector<string> functions, vector<vector<string>> funcargtypes, vector<string> variables, vector<string> variablestype, vector<string> builtins, json& returnjson, string& newitems)
{
	newitems = "none";
	if (splitline.at(0) == "{") {
		returnjson["sep"] = "+";
		return;
	}
	if (splitline.at(0) == "}") {
		returnjson["sep"] = "-";
		return;
	}
	if (splitline.at(0) == "funcdec") {
		returnjson["funcdec"] = { {"returnvals", splitline.at(1).substr(7, splitline.at(1).size() - 7)}, {"funcname", splitline.at(2)} };
		int count = 0;
		for (int i = 0; i < (splitline.size() - 5) / 3; i++) {
			returnjson["funcdec"]["arg" + to_string(i)] = { {"argtype", splitline.at(4 + i * 3)}, {"argname", splitline.at(5 + i * 3)} };
			count++;
		}
		returnjson["funcdec"]["argam"] = count;
		newitems = "function";
		return;
	}
	if (splitline.at(0).find("varcreate") != string::npos) {
		returnjson["varcreate"] = { {"vartype", splitline.at(0).substr(9, splitline.at(1).size() - 9)}, {"varname", splitline.at(0)} };
		for (int i = 0; i < splitline.size() - 4; i++)
		{
			returnjson["varcreate"]["arg" + to_string(i)] = splitline.at(i + 3);
		}
		newitems = "variable";
		return;
	}
	for (int i = 0; i < builtins.size(); i++)
	{
		if (builtins.at(i) == splitline.at(0)) {
			int count = 0;
			for (int i = 0; i < splitline.size() - 3; i++)
			{
				if (splitline.at(i + 2) != "<-") {
					returnjson[builtins.at(i)][to_string(count)]["args" + to_string(i)] = splitline.at(i + 2);
				}
				else {
					count++;
				}
			}
			return;
		}
	}
}

void condition_prep(vector<string> splitline, vector<string> usedvars, vector<string> usedfuncs, json& conditions)
{
}