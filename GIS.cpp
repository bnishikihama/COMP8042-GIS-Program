//
// Created by Braeden on 2022-11-29.
//
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <sstream>


using namespace std;

const string divider = "----------------------------------------------------------------------------------";

class GISRecord{

};
class NameIndex {

};
class CoordinateIndex {

};
class BufferPool {

};
class Logger {
public:
    static fstream logging;

public:
    static void logInit(const string& db, const string& script, const string& log) {
        logging.open(log, ios::out);
        if (!logging){
            cout << "Error in opening file: " << log << endl;
            exit(0);
        }
        else {
            time_t now = time(0);
            char* date = ctime(&now);
            logging << "Course Project for COMP 8042" << endl;
            logging << "Student Name: Braeden Nishikihama, Student ID: A01046511" << endl;
            logging << "Begin of GIS Program log:" << endl;
            logging << "dbFile: " << db << endl;
            logging << "script: " << script << endl;
            logging << "Start Time: " << date << endl;
        }

    }

public:
    static void log(const string& line) {
        logging << line << endl;
    }

public:
    static void stop() {
        // put end of log file stuff here
        time_t now = time(0);
        char* date = ctime(&now);
        logging << divider << endl;
        logging << "End Time: " << date << endl;
        logging.close();
    };
};
fstream Logger::logging;

class SystemManager {

};

// Manages retrieving commands from the script file
// Makes necessary calls to other components to carry out the commands
class CommandProcessor {
public:
    string scriptFile;
    fstream scriptStream;
    fstream dbFile;

    CommandProcessor(const string& db, const string& script) {
        scriptFile = script;
        dbFile.open(db, ios::out);
        if (!dbFile) {
            cout << "Error in opening File: " << db << endl;
            exit(0);
        }
        else {
            cout << "DB file created" << endl;  //testing delete later
            dbFile << "This is a new DB File";
        }
        dbFile.close();
    }

    void readScript(){
        string line;
        vector<string> output;
        scriptStream.open(scriptFile, ios::in);
        if (!scriptStream) {
            cout << "Error in opening File: " << scriptFile << endl;
            exit(0);
        }
        else {
            while (getline(scriptStream, line)) {
                //TODO: start logging here
                if (!line.empty()){
                    //logs line in script
                    Logger::log(line);
                }

                // If the line starts with ';' then skip it
                if (line[0] == ';' || line.empty())
                    continue;

                istringstream iss(line);
                string div;
                while (getline(iss, div, '\t'))
                    output.push_back(div);

                for (const auto& i : output) {
                    cout << i << endl;
                }
                output.clear();
            }
        }
        scriptStream.close();
    }
};
enum Command {

};
struct DMS {

};

int main(int argc, char* argv[]){
    if (argc != 4)
        return 0;

    string db = argv[1];
    string script = argv[2];
    string log = argv[3];

    Logger::logInit(db, script, log);
    CommandProcessor CommProc(db, script);
    CommProc.readScript();
    Logger::stop();
    return 0;
}