
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

#include "DDS/DDSLib.h"
#include "Utils/UtilsLib.h"

using namespace std;
using namespace dds;
using namespace simulation;
using namespace utils;


// ===========================================================================
/**	
	\brief	Main executable TinyBRL library (DDS).
	
	\author	Castronovo Michael

	\date	2014-09-19
*/
// ===========================================================================
// ---------------------------------------------------------------------------
//   Extra structures
// ---------------------------------------------------------------------------
class AgentData
{
     public:
          string name, expName;
          double offlineTime;
          double onlineTime;
          double mean;
          double gap;
};


// ---------------------------------------------------------------------------
//	Extra functions
// ---------------------------------------------------------------------------
void help();
void latex(int argc, char* argv[]) throw (parsing::ParsingException);
void matlab(int argc, char* argv[]) throw (parsing::ParsingException);
void writeLatexTable(ostream& os,
                     string expName, vector<AgentData>& agentDataList);
void writeMatlabFunction(ostream& os, string functionName,
                         vector<AgentData>& agentDataList);
void splitTime(double t,
               unsigned int& days, unsigned int& hours, unsigned int& minutes,
               unsigned int& seconds, unsigned int& milliseconds);

// ---------------------------------------------------------------------------
//	Main function
// ---------------------------------------------------------------------------
int main(int argc, char *argv[])
{	
	//	'DDS' package initilization
	init();

     
     //   Parsing and execution
	    //   1.   Get the right mode
    bool modeIsHelp   = parsing::hasFlag(argc, argv, "--help");
    bool modeIsLatex  = parsing::hasFlag(argc, argv, "--latex");
    bool modeIsMatlab = parsing::hasFlag(argc, argv, "--matlab");
	    
	    
	    //   2.   Launch the selected mode
     try
	{
	    if      (modeIsHelp)   { help();             }
	    else if (modeIsLatex)  { latex(argc, argv);  }
	    else if (modeIsMatlab) { matlab(argc, argv); }
	    else                           { cout << "No mode selected!\n"; }
	}
	
	catch (parsing::ParsingException e)
     {
          cout << "\nparsing exception: " << e.what();
          cout << "\n(see '--help')\n\n";
     }
}

// ----------------------------------------------------------------------------
//	Extra functions implementation
// ----------------------------------------------------------------------------
void help()
{
	ifstream is("doc/command-line manual (TinyBRL-export).txt");
	string line;
	cout << "\n";
	while (!is.eof()) { getline(is, line); cout << line << "\n"; }
}


void latex(int argc, char* argv[]) throw (parsing::ParsingException)
{
     vector<AgentData> agentDataList;

     //   1.   Parse the Agents
     int iAgent = 0;
     while (iAgent < (argc - 1))
     {
          while ((string(argv[iAgent]) != "--agent") && (iAgent < (argc - 1)))
               ++iAgent;
   
          int argcBis = 2;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iAgent++];

          while ((string(argv[iAgent]) != "--agent") && (iAgent < (argc - 1)))
          {               
               argvBis[argcBis] = argv[iAgent];
               ++iAgent;
               ++argcBis;
          }
          
          Agent* agent = Agent::parse(argcBis, argvBis);
          delete[] argvBis;
          
          AgentData agentData;
          agentData.name = agent->getName();
          agentData.offlineTime = agent->getOfflineTime();
          agentDataList.push_back(agentData);
          
          delete agent;
     }
     
     
     //   2.   Parse the Experiments
     int iExp = 0, nExp = 0;
     while (iExp < (argc - 1))
     {
          while ((string(argv[iExp]) != "--experiment") && (iExp < (argc - 1)))
               ++iExp;
          
          int argcBis = 2;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iExp++];
          
          while ((string(argv[iExp]) != "--experiment") && (iExp < (argc - 1)))
               argvBis[argcBis++] = argv[iExp++];
          
          Experiment* exp = Experiment::parse(argcBis, argvBis);         
          delete argvBis;
          
          agentDataList[nExp].expName = (exp->getName());
          agentDataList[nExp].onlineTime = (exp->getTimeElapsed()
                    / (double) exp->getNbOfMDPs());
          
          vector<double> dsrList = exp->computeDSRList();
			pair<double, double> CI95
					= statistics::computeCI95<double>(dsrList);
                    
          agentDataList[nExp].mean = ((CI95.first + CI95.second) / 2.0);;
          agentDataList[nExp].gap = (CI95.second - agentDataList[nExp].mean);
          ++nExp;
          
          delete exp;
     }
     
     
     //   3.   Get 'output'
     string output = parsing::getValue(argc, argv, "--output");
     assert(output != "");
     
     
     //   4.   Run
	ofstream os(output.c_str());
	while (!agentDataList.empty())
	{
		string expName = agentDataList[0].expName;
		vector<AgentData> tAgentDataList;
		
		for (int i = 0; i < (int) agentDataList.size(); ++i)
		{
			if (agentDataList[i].expName == expName)
			{
				tAgentDataList.push_back(agentDataList[i]);
				
				for (int j = (i + 1); j < (int) agentDataList.size(); ++j)
					agentDataList[j - 1] = agentDataList[j];
				
				agentDataList.pop_back();				
				--i;
			}
		}
		
		writeLatexTable(os, expName, tAgentDataList);
	}
	os.close();
}


void matlab(int argc, char* argv[]) throw (parsing::ParsingException)
{
     vector<AgentData> agentDataList;

     //   1.   Parse the Agents
     int iAgent = 0;
     while (iAgent < (argc - 1))
     {
          while ((string(argv[iAgent]) != "--agent") && (iAgent < (argc - 1)))
               ++iAgent;
   
          int argcBis = 2;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iAgent++];

          while ((string(argv[iAgent]) != "--agent") && (iAgent < (argc - 1)))
          {               
               argvBis[argcBis] = argv[iAgent];
               ++iAgent;
               ++argcBis;
          }
          
          Agent* agent = Agent::parse(argcBis, argvBis);
          delete[] argvBis;
          
          AgentData agentData;
          agentData.name = agent->getName();
          agentData.offlineTime = agent->getOfflineTime();
          agentDataList.push_back(agentData);
          
          delete agent;
     }
     
     
     //   2.   Parse the Experiments
     int iExp = 0, nExp = 0;
     while (iExp < (argc - 1))
     {
          while ((string(argv[iExp]) != "--experiment") && (iExp < (argc - 1)))
               ++iExp;
          
          int argcBis = 2;
          char** argvBis = new char*[argc];
          argvBis[0] = argv[0];
          argvBis[1] = argv[iExp++];
          
          while ((string(argv[iExp]) != "--experiment") && (iExp < (argc - 1)))
               argvBis[argcBis++] = argv[iExp++];
          
          Experiment* exp = Experiment::parse(argcBis, argvBis);         
          delete argvBis;
          
          agentDataList[nExp].expName = (exp->getName());
          agentDataList[nExp].onlineTime = (exp->getTimeElapsed()
                    / (double) exp->getNbOfMDPs());
          
          vector<double> dsrList = exp->computeDSRList();
			pair<double, double> CI95
					= statistics::computeCI95<double>(dsrList);
                    
          agentDataList[nExp].mean = ((CI95.first + CI95.second) / 2.0);;
          agentDataList[nExp].gap = (CI95.second - agentDataList[nExp].mean);
          ++nExp;
          
          delete exp;
     }
     
     
     //   3.   Get 'output'
     string output = parsing::getValue(argc, argv, "--output");
     
     
     //   4.   Run
     string tmp = output.substr(output.find_last_of('/') + 1);
	string functionName = tmp.substr(0, tmp.find_last_of('.'));	
	
	ofstream os(output.c_str());
     writeMatlabFunction(os, functionName, agentDataList);
	os.close();
}


void writeLatexTable(ostream& os,
                     string expName, vector<AgentData>& agentDataList)
{
     os << "\n";
     os << "\\begin{table}\n";
	os << "\t\\centering\n";
	os << "\t\\begin{tabular}{l|c|c|c}\n";
	os << "\t\tAgent & Offline time & Online time & Mean score\\\\\n";
	os << "\t\t\\hline\n";
	
	for (unsigned int i = 0; i < agentDataList.size(); ++i)
	{
          AgentData& agentData = agentDataList[i];
          
          //   Write 'name'
		os << "\t\t" << agentData.name;

          //   Write 'offline time'
          unsigned int days, hours, minutes, seconds, milliseconds;
          splitTime(agentData.offlineTime, days, hours, minutes,
                    seconds, milliseconds);
          
          os << " & ";
          if (days > 0) { os << "$" << days << "$d "; }
          if ((days > 0) || (hours > 0))
               os << "$" << hours << "$h ";
          if ((days > 0) || (hours > 0) || (minutes > 0))
               os << "$" << minutes << "$m ";
          if ((days > 0) || (hours > 0) || (minutes > 0) || (seconds > 0))
               os << "$" << seconds << "$s ";
          os << "$" << setprecision(ceil(log10(milliseconds) + 2));
          os << milliseconds << "$ms ";
		
		
          //   Write 'online time'
          splitTime(agentData.onlineTime, days, hours, minutes,
                    seconds, milliseconds);

		os << " & ";
          if (days > 0) { os << "$" << days << "$d "; }
          if ((days > 0) || (hours > 0)) { os << "$" << hours << "$h "; }
          if ((days > 0) || (hours > 0) || (minutes > 0))
               os << "$" << minutes << "$m ";
          if ((days > 0) || (hours > 0) || (minutes > 0) || (seconds > 0))
               os << "$" << seconds << "$s ";
          os << "$" << setprecision(ceil(log10(milliseconds) + 2));
          os << milliseconds << "$ms ";
		
		
          //   Write 'score'
		os << " & $";
		os << setprecision(ceil(log10(agentData.mean) + 2));
		os << agentData.mean;
		os << " \\pm ";
		os << setprecision(ceil(log10(agentData.gap) + 2));
		os << agentData.gap << "$";
		
		os << "\\\\\n";
	}
	os << "\t\\end{tabular}\n";
	os << "\t\\caption{" << expName << "}\n";
	os << "\\end{table}\n\n";
}


void writeMatlabFunction(ostream& os, string functionName,
                         vector<AgentData>& agentDataList)
{
     os << "\n";
	os << "function " << functionName << "()\n";
	os << "\tY =\t[\n\t\t";
	vector<string> agentNameList;
	vector<string> expNameList;
	while (!agentDataList.empty())
	{
		string expName = agentDataList[0].expName;
		expNameList.push_back(expName);
		
		vector<AgentData> tAgentDataList;
		for (unsigned int i = 0; i < agentDataList.size(); ++i)
		{
			if (agentDataList[i].expName == expName)
			{
				tAgentDataList.push_back(agentDataList[i]);
				
				for (int j = (i + 1); j < (int) agentDataList.size(); ++j)
					agentDataList[j - 1] = agentDataList[j];
				
				agentDataList.pop_back();				
				--i;
			}
		}

		os << "\t";
		for (unsigned int i = 0; i < tAgentDataList.size(); ++i)
		{
			if (expNameList.size() == 1)
				agentNameList.push_back(tAgentDataList[i].name);

			os << " " << tAgentDataList[i].mean;
		}
		os << "\n\t\t";
	}

	    //	1 group case
	if (expNameList.size() == 1)
	{
		os << "\t";
		for (int i = 0; i < (int) agentNameList.size(); ++i) { os << " 0"; }
		os << "\n\t\t";
	}
	
	os << "];\n\n";

	os << "\tLabels = {";
	for (unsigned int i = 0; i < (expNameList.size() - 1); ++i)
		os << "\'" << expNameList[i] << "\', ";
	os << "\'" << expNameList.back() << "\'";
	
	    //	1 group case
	if (expNameList.size() == 1) { os << ", \' \'"; }
	
	os << "};\n\n";

	os << "\tbar(Y, 'group');\n";
	os << "\thold on;\n";
	os << "\tset(gca, 'XTick', 1:";
	
	    //	1 group case
	if (expNameList.size() == 1)	{ os << 2;				}
	else						{ os << expNameList.size();	}
	
	os << ", 'XTickLabel', Labels);\n";
	
	    //	1 group case
	if (expNameList.size() == 1) { os << "\txlim([0, 2]);\n"; }
	
	os << "\tylabel(\'Mean score\');\n";

	os << "\tlegend(";
	for (unsigned int i = 0; i < (agentNameList.size() - 1); ++i)
		os << "\'" << agentNameList[i] << "\', ";
	os << "\'" << agentNameList.back() << "\');\n";
	os << "\tlegend(\'Location\', \'NorthEastOutside\');\n";
}


void splitTime(double t,
               unsigned int& days, unsigned int& hours, unsigned int& minutes,
               unsigned int& seconds, unsigned int& milliseconds)
{   
     days = floor(t / (24*60*60*1000));
     t -= (days * (24*60*60*1000));
     
     hours = floor(t / (60*60*1000));
     t -= (hours * (60*60*1000));
     
     minutes = floor(t / (60*1000));
     t -= (minutes * (60*1000));
     
     seconds = floor(t / 1000);
     t -= (seconds * 1000);
     
     milliseconds = round(t);
}
