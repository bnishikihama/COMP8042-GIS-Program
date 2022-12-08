//
// Created by Braeden on 2022-11-29.
//
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <sstream>
#include <cmath>

#define MAX_LOAD 0.70

using namespace std;


const string divider = "----------------------------------------------------------------------------------";

class Logger {
public:
    static fstream logging;
    static string dbPath;

    static void logInit(const string& db, const string& script, const string& log) {
        logging.open(log, ios::out);
        if (!logging){
            cout << "Error in opening file: " << log << endl;
            exit(0);
        }
        else {
            Logger::dbPath = db; // set db path
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

    static void log(const string& line) {
        logging << line << endl;
    }

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
string Logger::dbPath;

class GISRecord{

};
class NameIndex {

};

class CoordinateIndex {

};

struct Point {
    int lon = -1;
    int lat = -1;
    int offset = -1;
};

struct QuadTree {
    bool leaf = false;
    Point p;
    Point min, max, centerL, centerR;
    vector<int> offsets;
//    vector<Point> children;
//    const int max_node = 4;

    QuadTree *NW = nullptr;
    QuadTree *NE = nullptr;
    QuadTree *SW = nullptr;
    QuadTree *SE = nullptr;

    QuadTree(Point newMin, Point newMax, int of = -1) : min(newMin), max(newMax){
//        min = newMin;
//        max = newMax;
        offsets.push_back(of);
        centerL.lat = (min.lat + max.lat)/2;
        centerL.lon = (min.lon + max.lon)/2;
        centerR.lat = (min.lat + max.lat)/2 + 1;
        centerR.lon = (min.lon + max.lon)/2 + 1;
    }

    bool checkBoundary(Point p) const{
        if ((min.lon <= p.lon) &&
            (min.lat <= p.lat) &&
            (max.lon >= p.lon) &&
            (max.lat >= p.lat))
            return true;
        return false;
    }

    void insert(Point p){
        // Check if this node is a leaf
        if (!leaf){
            // check it does not have children
                if (!NW && !NE && !SW && !SE) {
                    leaf = true;
                    this->p = p;
                    return;
                }
        }

        // If duplicate, add to list of duplicates
        if (p.lat == this->p.lat && p.lon == this->p.lon){
            this->offsets.push_back(p.offset);
            return;
        }

        // No longer a leaf. Set false
        leaf = false;
        // Splitting QuadTree. Set new quadrants
        if (!NW && !NE && !SW && !SE) {
            NW = new QuadTree({min.lon, centerR.lat}, {centerL.lon, max.lat});
            NE = new QuadTree({centerR.lon, centerR.lat}, {max.lon, max.lat});
            SW = new QuadTree({min.lon, min.lat}, {centerL.lon, centerL.lat});
            SE = new QuadTree({centerR.lon, min.lat}, {max.lon, centerL.lat});

            // Insert old point before new point
            if (NW->checkBoundary(this->p))
                NW->insert(this->p);
            if (NE->checkBoundary(this->p))
                NE->insert(this->p);
            if (SW->checkBoundary(this->p))
                SW->insert(this->p);
            if (SE->checkBoundary(this->p))
                SE->insert(this->p);
        }
        // Insert new point
        if (NW->checkBoundary(p))
            NW->insert(p);
        if (NE->checkBoundary(p))
            NE->insert(p);
        if (SW->checkBoundary(p))
            SW->insert(p);
        if (SE->checkBoundary(p))
            SE->insert(p);
    }

    bool search(Point p) const{
        if (leaf) {
            if (this->p.lat == p.lat && this->p.lon == p.lon)
                return true;
            else
                return false;
        }
        if (NW->checkBoundary(p))
            return NW->search(p);
        if (NE->checkBoundary(p))
            return NE->search(p);
        if (SW->checkBoundary(p))
            return SW->search(p);
        if (SE->checkBoundary(p))
            return SE->search(p);
        return false;
    }

};

// TODO: isPrime and nextPrime code taken from Assignment 2 code
bool isPrime(int n) {
    if (n == 2 || n == 3)
        return true;

    if (n == 1 || n % 2 == 0)
        return false;

    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0)
            return false;

    return true;
}
int nextPrime(int n) {
    if (n % 2 == 0)
        ++n;

    for (; !isPrime(n); n += 2);

    return n;
}

// TODO: Most HashTable code taken from Assignment 2 code
template<typename T>
class HashTable {
public:
    explicit HashTable(int size = 1024) : array(size) {
//        numHashFunctions = hashFunctions.getNumberOfFunctions();
        rehashes = 0;
        currentSize = 0;
        makeEmpty();
    }

    void printTable(){
        Logger::log("Current Table Size: " + to_string(array.size()));
        Logger::log("Number of Elements in the table: " + to_string(currentSize) + "\n");
        for (int i = 0; i < array.size(); i++){
            if (array[i].isActive){
                Logger::log('\t' + to_string(i) + ": [" +
                            array[i].element + ": [" + to_string(array[i].value-1) + "]]");
            }
        }
    }

    void makeEmpty() {
        currentSize = 0;
        for (auto &entry: array)
            entry.isActive = false;
    }

    unsigned int elfHash(const string& x){
        unsigned int hash = 0, y = 0;
        for (int i = 0; i < x.length(); i++){
            hash = (hash << 4) + x[i];
            if ((y = hash & 0xF0000000) != 0){
                hash ^= (y >> 24);
            }
            hash &= ~y;
        }
        return hash;
    }

    int insert(T &&x, int offset) {
        const int COUNT_LIMIT = 100;
        int currProbe;

        if (currentSize >= array.size() * MAX_LOAD)
            expand();

        while (true){
            int lastPos = -1;
            currProbe = 0;
            int index = elfHash(x) % array.size();
            for (int count = 0; count < COUNT_LIMIT; ++count){

                if (!isActive(index)) {
                    array[index] = std::move(HashEntry{std::move(x), true, offset});
                    ++currentSize;
                    return currProbe;
                }
                lastPos = index;
                index = ((pow(index, 2) + index) / 2);
                currProbe++;
                index = index % array.size();

                swap(x, array[lastPos].element);
                swap(offset, array[lastPos].value);
            }

        }

    }

private:
    struct HashEntry {
        T element;
        bool isActive;
        int value;
        HashEntry(const T &e = T(), bool a = false, int i = -1): element{e}, isActive{a}, value{i} {}
        HashEntry(T &&e, bool a = false, int i = -1): element{std::move(e)}, isActive{a}, value{std::move(i)} {}
    };

    void rehash(int newSize) {
        vector<HashEntry> oldArray = array;

        // Create new double-sized, empty table
        array.resize(nextPrime(newSize));
        for (auto &entry: array) {
            entry.isActive = false;
            entry.value = -1;
        }

        // Copy table over
        currentSize = 0;
        for (auto &entry: oldArray)
            if (entry.isActive)
                insert(std::move(entry.element), std::move(entry.value));
    }

    void expand() {
        rehash(static_cast<int>( array.size() / MAX_LOAD ));
    }

    vector<HashEntry> array;
    int currentSize;
//    int numHashFunctions;
    int rehashes;

    bool isActive(int currentPos) const { return currentPos != -1 && array[currentPos].isActive; }

};

class BufferPool {
public:
    static vector<string> bufferPool;

    static void add(const string& element) {
        // first check if exists in buffer
        // loop through to make sure

        // if exists, move element to front as it is now most recently used

        // if it does not exist in buffer, check if buffer exceed 15
        if (bufferPool.size() >= 15) {
            // replace least used
        }
        else {
            // buffer does not exceed limit, add element to front
            bufferPool.insert(bufferPool.begin(), element);
        }

    }
};
vector<string> BufferPool::bufferPool;


class CommandProcessor {
public:
    static int westLong;
    static int eastLong;
    static int southLat;
    static int northLat;
    static vector<string> output;
    static HashTable<string> hashTable;
    static int maxProbe;

    static vector<string> getCommand(const string& line){
        istringstream iss(line);
        string div;
        output.clear();
        while (getline(iss, div, '\t'))
            output.push_back(div);

        return output;
    }

    static int worldBound(const vector<string>& line, int n) {
        int DMS;
        if (n == 1 || n == 2){
            DMS = (
                    (stoi(line[n].substr(0,3)) * 3600) +
                    (stoi(line[n].substr(3,2))) * 60 +
                    (stoi(line[n].substr(5,2)))
            );
        }
        else {
            DMS = (
                    (stoi(line[n].substr(0,2)) * 3600) +
                    (stoi(line[n].substr(2,2))) * 60 +
                    (stoi(line[n].substr(4,2)))
            );
        }
        if (line[n][7] == 'W' || line[n][6] == 'S')
            DMS *= -1;
        return DMS;
    }

    static void world(const vector<string>& line){
        westLong = worldBound(line, 1);
        eastLong = worldBound(line, 2);
        southLat = worldBound(line, 3);
        northLat = worldBound(line, 4);

        Logger::log("World Boundaries are set to the following in total seconds:");
        string result =
                "\tnorthLat: " + to_string(northLat) + "\n"
                "westLong: " + to_string(westLong) + "\teastLong: " + to_string(eastLong) + "\n"
                "\tsouthLat: " + to_string(southLat);
        Logger::log(result);
    }

    static void import(const vector<string>& command, QuadTree quad){
        fstream import;
        fstream db;
        string line;
        string element;
        string value;
        char div = '|';
        int offset = 0, count;
        vector<string> elements;
        int probe;
        unsigned int totalNameLength = 0;


        import.open(command[1], ios::in);
        db.open(Logger::dbPath, ios::app);
        probe = 0;
        CommandProcessor::maxProbe = 0;
        if (!import){
            Logger::log("Could not open file: " + command[1]);
        }
        else if (!db) {

            Logger::log("Could not open file: " + Logger::dbPath);
        }
        else {
        // TODO: Append record to database file, find offset and add to HashTable

            while(getline(import, line)){
                //skip the first line
                if (offset == 0) {
                    offset++;
                    continue;
                }

                elements.clear();
                istringstream iss(line);
                while(getline(iss, element, div)){
                    elements.push_back(element);
                }
                int Long = (
                        (stoi(elements[8].substr(0,3)) * 3600) +
                        (stoi(elements[8].substr(3,2))) * 60 +
                        (stoi(elements[8].substr(5,2)))
                        );
                int Lat = (
                        (stoi(elements[7].substr(0,2)) * 3600) +
                        (stoi(elements[7].substr(2,2))) * 60 +
                        (stoi(elements[7].substr(4,2)))
                        );
                if (elements[8][7] == 'W')
                    Long *= -1;
                if (elements[7][6] == 'S')
                    Lat *= -1;

                if ((CommandProcessor::westLong < Long) && (Long < CommandProcessor::eastLong) &&
                        (CommandProcessor::southLat < Lat) && (Lat < CommandProcessor::northLat)){
                    db << line << endl;
                    offset++;

                    totalNameLength += elements[1].length();
                    // Insert into hash table
                    probe = hashTable.insert(elements[1] + elements[3], offset-1);
                    if (probe > maxProbe)
                        CommandProcessor::maxProbe = probe;

                    // Insert into quad tree
                    quad.insert({Long,Lat, offset});
//                    cout << quad.search({Long, Lat}) << endl;
                }

            }
        }
        db.close();
        import.close();
        Logger::log("Total Imports: " + to_string(offset-1));
        Logger::log("Longest Probe: " + to_string(CommandProcessor::maxProbe));
        if (offset == 0)
            Logger::log("Average Name Length: " + to_string(0));
        else
            Logger::log("Average Name Length: " + to_string(totalNameLength/offset));
    }

    static void debug(const vector<string>& line){
        if (line[1] == "quad") {
            return;
        }
        else if (line[1] == "hash") {
            CommandProcessor::hashTable.printTable();
        }
        else if (line[1] == "pool") {
            return;
        }
        else return;
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
HashTable<string> CommandProcessor::hashTable;
int CommandProcessor::maxProbe = 0;


class SystemManager {
public:
    string scriptFile;
    fstream scriptStream;
    fstream dbFile;

    SystemManager(const string& db, const string& script) {
        scriptFile = script;

        // Create empty database file if not exists,
        // if exists, empty it
        dbFile.open(db, ios::out);
        if (!dbFile) {
            Logger::log("Cannot open file: " + db);
        }
//        else {
//            cout << "DB file created" << endl;  //testing delete later
//            dbFile << "This is a new DB File";
//        }
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

                else if (command[0] == "import") {
                    auto *quad = new QuadTree({CommandProcessor::westLong,CommandProcessor::southLat},
                                              {CommandProcessor::eastLong,CommandProcessor::northLat});
                    CommandProcessor::import(command, *quad);
                }

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