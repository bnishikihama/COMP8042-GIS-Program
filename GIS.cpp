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

enum Command {
    world,
    import,
    debug,
    quit,
    what_is_at,
    what_is,
    what_is_in
};

class CommandProcessor {
public:
    static int westLong;
    static int eastLong;
    static int southLat;
    static int northLat;
    static vector<string> output;

    static vector<string> getCommand(const string& line){
        istringstream iss(line);
        string div;
        while (getline(iss, div, '\t'))
            output.push_back(div);

//        for (const auto& i : output) {
//            cout << i << endl;
//        }
        return output;
    }

    static void world(const vector<string>& line){
        westLong = (
                (stoi(line[1].substr(0,3)) * 3600) +
                (stoi(line[1].substr(3,2))) * 60 +
                (stoi(line[1].substr(5,2)))
                );
        if (line[1][7] == 'W')
            westLong *= -1;

        eastLong = (
                (stoi(line[2].substr(0,3)) * 3600) +
                (stoi(line[2].substr(3,2))) * 60 +
                (stoi(line[2].substr(5,2)))
                );
        if (line[2][7] == 'W')
            eastLong *= -1;

        southLat = (
                (stoi(line[3].substr(0,3)) * 3600) +
                (stoi(line[3].substr(3,2))) * 60 +
                (stoi(line[3].substr(5,2)))
                );
        if (line[3][7] == 'S')
            southLat *= -1;

        northLat = (
                (stoi(line[4].substr(0,3)) * 3600) +
                (stoi(line[4].substr(3,2))) * 60 +
                (stoi(line[4].substr(5,2)))
                );
        if (line[3][7] == 'S')
            northLat *= -1;

        Logger::log(to_string(westLong));
    }
    static string import(const vector<string>& line){
        return "";
    }
    static string debug(const vector<string>& line){
        return "";
    }
    static string quit(const vector<string>& line){
        return "";
    }
    static string what_is_at(const vector<string>& line){
        return "";
    }
    static string what_is(const vector<string>& line){
        return "";
    }
    static string what_is_in(const vector<string>& line){
        return "";
    }

};
int CommandProcessor::westLong;
int CommandProcessor::eastLong;
int CommandProcessor::southLat;
int CommandProcessor::northLat;
vector<string> CommandProcessor::output;


class SystemManager {
public:
    string scriptFile;
    fstream scriptStream;
    fstream dbFile;

    SystemManager(const string& db, const string& script) {
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
        vector<string> command;
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

                command = CommandProcessor::getCommand(line);
                if (command[0] == "world")
                    CommandProcessor::world(command);
                else if (command[0] == "import")
                    CommandProcessor::import(command);
                else if (command[0] == "debug")
                    CommandProcessor::debug(command);
                else if (command[0] == "quit")
                    CommandProcessor::quit(command);
                else if (command[0] == "what_is_at")
                    CommandProcessor::what_is_at(command);
                else if (command[0] == "what_is")
                    CommandProcessor::what_is(command);
                else if (command[0] == "what_is_in")
                    CommandProcessor::what_is_in(command);
                else
                    Logger::log("Command not found error");
            }
        }
        scriptStream.close();
    }
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
    SystemManager system(db, script);
    system.readScript();
    Logger::stop();
    return 0;
}