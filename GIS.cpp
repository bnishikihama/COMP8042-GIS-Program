//
// Created by Braeden on 2022-11-29.
//
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <sstream>

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

    static string dbLog(const string& line) {
        fstream db;
        string off;
        int i = 0;
        db.open(dbPath, ios::in);
        if (db){
            while (getline(db, off)){
                if (i == stoi(line)){
                    return off;
                }
                i++;
                if (i > stoi(line)+1){
                    return "";
                }
            }
            return "";
        }
        return "";
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

struct Point {
    int lon;
    int lat;
    vector<int> offset{};
    explicit Point(int _lon = -1, int _lat = -1, int _offset = {}){
        lon = _lon;
        lat = _lat;
        offset.push_back(_offset);
    };
};

struct QuadTree {
    bool leaf = false;
    Point p;
    Point min, max, centerL, centerR;

    QuadTree *NW = nullptr;
    QuadTree *NE = nullptr;
    QuadTree *SW = nullptr;
    QuadTree *SE = nullptr;

    QuadTree(Point newMin, Point newMax) : min(newMin), max(newMax){
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
                    this->p.offset = p.offset;
                    return;
                }
        }

        // If duplicate, add to list of duplicates
        if (p.lat == this->p.lat && p.lon == this->p.lon){
            vector<int> temp;
            for (int i : this->p.offset){
                temp.push_back(i);
            }
            temp.push_back(p.offset[0]);
            this->p.offset = temp;
            return;
        }

        // No longer a leaf. Set false
        leaf = false;
        // Splitting QuadTree. Set new quadrants
        if (!NW && !NE && !SW && !SE) {
            Point p1 = Point(min.lon, centerR.lat);
            Point p2 = Point(centerL.lon, max.lat);
            NW = new QuadTree(p1, p2);

            p1 = Point(centerR.lon, centerR.lat);
            p2 = Point(max.lon, max.lat);
            NE = new QuadTree(p1, p2);

            p1 = Point(min.lon, min.lat);
            p2 = Point(centerL.lon, centerL.lat);
            SW = new QuadTree(p1, p2);

            p1 = Point(centerR.lon, min.lat);
            p2 = Point(max.lon, centerL.lat);
            SE = new QuadTree(p1, p2);

            // Insert old point before new point
            if (NW->checkBoundary(this->p))
                NW->insert(this->p);
            else if (NE->checkBoundary(this->p))
                NE->insert(this->p);
            else if (SW->checkBoundary(this->p))
                SW->insert(this->p);
            else if (SE->checkBoundary(this->p))
                SE->insert(this->p);
            else
                return;
        }
        // Insert new point
        if (NW->checkBoundary(p))
            NW->insert(p);
        else if (NE->checkBoundary(p))
            NE->insert(p);
        else if (SW->checkBoundary(p))
            SW->insert(p);
        else if (SE->checkBoundary(p))
            SE->insert(p);
        else
            return;
    }

    vector<int> search(Point p) const{
        if (leaf) {
            if (this->p.lat == p.lat && this->p.lon == p.lon)
                return this->p.offset;
            else
                return {};
        }

        if (NW->checkBoundary(p))
            return NW->search(p);
        else if (NE->checkBoundary(p))
            return NE->search(p);
        else if (SW->checkBoundary(p))
            return SW->search(p);
        else if (SE->checkBoundary(p))
            return SE->search(p);
        else
            return {};
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
        currentSize = 0;
        makeEmpty();
    }

    void printTable(){
        string temp;
        Logger::log("Current Table Size: " + to_string(array.size()));
        Logger::log("Number of Elements in the table: " + to_string(currentSize) + "\n");
        for (int i = 0; i < array.size(); i++){
            temp = "";
            if (array[i].isActive){
                for (int j = 0; j < array[i].value.size(); j++) {
                    temp += to_string(array[i].value[j]) + " ";
                    Logger::log('\t' + to_string(i) + ": [" + array[i].element + ": [" + temp + "]]");
                }
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

    vector<int> find(const string& name) {
        int i = 0;
        int index = elfHash(name) % array.size();
        while (isActive(index)){
            if (name == array[index].element) {
                return array[index].value;
            }
            i++;
            index += ((i*i + i)/2);
            index = index % array.size();

        }
        return {};
    }

    int insert(T &&x, int offset) {
        const int COUNT_LIMIT = 100;
        int currProbe;

        if (currentSize >= array.size() * MAX_LOAD)
            expand();

        while (true){
            currProbe = 0;
            int index = elfHash(x) % array.size();
            for (int count = 0; count < COUNT_LIMIT; ++count){

                if (!isActive(index)) {
//                    array[index] = std::move(HashEntry{std::move(x), true, offset});
                    array[index].element = x;
                    array[index].value.push_back(offset);
                    array[index].isActive = true;
                    ++currentSize;
                    return currProbe;
                }
                else if (x == array[index].element) {
                    array[index].value.push_back(offset);
                    return currProbe;
                }

                index = index + ((count*count + count)/2);
                currProbe++;
                index = index % array.size();

            }

        }

    }

private:
    struct HashEntry {
        T element;
        bool isActive;
        vector<int> value;
        HashEntry(){
            isActive = false;
        }
    };

    void rehash(int newSize) {
        vector<HashEntry> oldArray = array;

        // Create new double-sized, empty table
        array.resize(nextPrime(newSize));
        for (auto &entry: array) {
            entry.isActive = false;
            entry.value.empty();
        }

        // Copy table over
        currentSize = 0;
        for (auto &entry: oldArray)
            if (entry.isActive)
                for (int i = 0; i < entry.value.size(); i++)
                    insert(std::move(entry.element), std::move(entry.value[i]));
    }

    void expand() {
        rehash(static_cast<int>( array.size() / MAX_LOAD ));
    }

    vector<HashEntry> array;
    int currentSize;

    bool isActive(int currentPos) const { return currentPos != -1 && array[currentPos].isActive; }

};
struct BufferItem{
    string line;
    int offset;
    BufferItem(string _line = "", int _offset = -1) {
        line = _line;
        offset = _offset;
    }
};

class BufferPool {
public:
    static vector<BufferItem> pool;
    int max_size = 15;

    static string dbsearch(const int& offset){
        // open and search the db by offset
        fstream db;
        string line;
        int i = 0;
        db.open(Logger::dbPath, ios::in);
        if (db){
            while (getline(db, line)){
                if (i == offset){
                    return line;
                }
                if (i > offset+1){
                    return "";
                }
                i++;
            }
            return "";
        }
        return "";
    }

    static string checkPool(const int& offset){
        string line;
        BufferItem temp;
        if (pool.empty()){
            line = dbsearch(offset);
            pool.push_back(BufferItem(line, offset));
            return line;
        }
        else{
            for (int i = 0; i < pool.size(); i++){
                if (pool[i].offset == offset){
                    temp = pool[i];
                    pool.erase(pool.begin() + i);
                    pool.push_back(temp);
                    return temp.line;
                }
            }

            line = dbsearch(offset);
            temp = BufferItem(line, offset);
            if (pool.size() >= 15) {
                pool.erase(pool.begin());
                pool.push_back(temp);
            }
            else {
                pool.push_back(temp);
            }

            return line;
        }
    }

    static void print(){
        for (auto i = pool.rbegin(); i != pool.rend(); i++)
            Logger::log(to_string(i->offset) + ": " + i->line);
    }

};
vector<BufferItem> BufferPool::pool;


class CommandProcessor {
public:
    static int westLong;
    static int eastLong;
    static int southLat;
    static int northLat;
    static vector<string> output;
    static HashTable<string> hashTable;
    static int maxProbe;
    static int offset;

    static vector<string> getCommand(const string& line){
        istringstream iss(line);
        string div;
        offset = 0;
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

    static void world(const vector<string>& line, QuadTree *quad){
        westLong = worldBound(line, 1);
        eastLong = worldBound(line, 2);
        southLat = worldBound(line, 3);
        northLat = worldBound(line, 4);
        Point pMin = *new Point(westLong, southLat);
        Point pMax = *new Point(eastLong, northLat);
        quad->min=pMin;
        quad->max=pMax;

        Logger::log("World Boundaries are set to the following in total seconds:");
        string result =
                "\tnorthLat: " + to_string(northLat) + "\n"
                "westLong: " + to_string(westLong) + "\teastLong: " + to_string(eastLong) + "\n"
                "\tsouthLat: " + to_string(southLat);
        Logger::log(result);
    }

    static void import(const vector<string>& command, QuadTree *quad){
        fstream import;
        fstream db;
        string line;
        string element;
        string value;
        char div = '|';
        int count = 0;
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
            //skip the first line
            getline(import, line);
            while(getline(import, line)){

                elements.clear();
                istringstream iss(line);
                while(getline(iss, element, div)){
                    elements.push_back(element);
                }

                // If the Coordinates are "Unknown" or blank, skip
                if (elements[7] == "Unknown" || elements[8] == "Unknown") {
                    continue;
                }

                if (elements[7].empty() || elements[8].empty()) {
                    continue;
                }

                int Long = (
                        (stoi(elements[8].substr(0, 3)) * 3600) +
                        (stoi(elements[8].substr(3, 2))) * 60 +
                        (stoi(elements[8].substr(5, 2)))
                );
                int Lat = (
                        (stoi(elements[7].substr(0, 2)) * 3600) +
                        (stoi(elements[7].substr(2, 2))) * 60 +
                        (stoi(elements[7].substr(4, 2)))
                );
                if (elements[8][7] == 'W')
                    Long *= -1;
                if (elements[7][6] == 'S')
                    Lat *= -1;

                if ((CommandProcessor::westLong < Long) && (Long < CommandProcessor::eastLong) &&
                        (CommandProcessor::southLat < Lat) && (Lat < CommandProcessor::northLat)){
                    db << line << endl;

                    totalNameLength += elements[1].length();
                    // Insert into hash table
                    probe = hashTable.insert(elements[1] + elements[3], offset);
                    if (probe > maxProbe)
                        CommandProcessor::maxProbe = probe;

                    // Insert into quad tree
                    Point p = *new Point(Long, Lat, offset);
                    quad->insert(p);
                    offset++;
                    count++;
                }

            }
        }
        db.close();
        import.close();
        Logger::log("Total Imports: " + to_string(count));
        Logger::log("Longest Probe: " + to_string(CommandProcessor::maxProbe));
        if (count == 0)
            Logger::log("Average Name Length: " + to_string(0));
        else
            Logger::log("Average Name Length: " + to_string(totalNameLength/(count)));
    }

    static void debug(const vector<string>& line){
        if (line[1] == "quad") {
            return;
        }
        else if (line[1] == "hash") {
            CommandProcessor::hashTable.printTable();
        }
        else if (line[1] == "pool") {
            BufferPool::print();
        }
        else return;
    }

    static void quit(){
        Logger::log("Terminating execution of commands.");
        Logger::stop();
        exit(0);
    }

    static void what_is_at(const vector<string>& line, QuadTree *quad){
        vector<int> result;
        string temp;
        string templine;
        vector<string> elements;
        int lon =(
                (stoi(line[2].substr(0, 3)) * 3600) +
                (stoi(line[2].substr(3, 2))) * 60 +
                (stoi(line[2].substr(5, 2)))
        );
        int lat =  (
                (stoi(line[1].substr(0, 2)) * 3600) +
                (stoi(line[1].substr(2, 2))) * 60 +
                (stoi(line[1].substr(4, 2)))
        );
        if (line[2][7] == 'W')
            lon *= -1;
        if (line[1][6] == 'S')
            lat *= -1;
        Point p = *new Point(lon, lat);
        result = quad->search(p);

        if (result.empty())
            Logger::log("No records match: \"" + line[1] + "\" and \"" + line[2] + "\"\n");
        else {
            for (auto i : result){
                temp = BufferPool::checkPool(i);
//                temp = Logger::dbLog(to_string(i-1));
                elements.clear();
                istringstream iss(temp);
                while(getline(iss, templine, '|')){
                    elements.push_back(templine);
                }
                Logger::log(to_string(i) + ": \"" + elements[5] + "\": \"" + elements[7] + "\", \"" + elements[8] + "\"\n");
            }
        }
    }

    static void what_is(const vector<string>& line){
        vector<int> a;
        string temp;
        string templine;
        vector<string> elements;

        a = CommandProcessor::hashTable.find(line[1] + line[2]);
        if (a.empty())
            Logger::log("No records match: \"" + line[1] + "\" and \"" + line[2] + "\"\n");
        else {
            for (auto i : a){
                temp = BufferPool::checkPool(i);
                elements.clear();
                istringstream iss(temp);
                while(getline(iss, templine, '|')){
                    elements.push_back(templine);
                }
                Logger::log(to_string(i) + ": \"" + elements[5] + "\": \"" + elements[7] + "\", \"" + elements[8] + "\"\n");
            }
        }
    }

    static string what_is_in(const vector<string>& line){
        //rip
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
int CommandProcessor::offset;


class SystemManager {
public:
    string scriptFile;
    fstream scriptStream;
    fstream dbFile;
    Point p = *new Point(0,0);
    QuadTree *quad = new QuadTree(p,p);

    SystemManager(const string& db, const string& script) {
        scriptFile = script;

        // Create empty database file if not exists,
        // if exists, empty it
        dbFile.open(db, ios::out);
        if (!dbFile) {
            Logger::log("Cannot open file: " + db);
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
                if (!line.empty()){
                    //logs line in script
                    Logger::log(line);
                }

                // If the line starts with ';' then skip it
                if (line[0] == ';' || line.empty())
                    continue;

                command = CommandProcessor::getCommand(line);
                if (command[0] == "world")
                    CommandProcessor::world(command, quad);

                else if (command[0] == "import") {
                    CommandProcessor::import(command, quad);
                }

                else if (command[0] == "debug")
                    CommandProcessor::debug(command);

                else if (command[0] == "quit")
                    CommandProcessor::quit();

                else if (command[0] == "what_is_at")
                    CommandProcessor::what_is_at(command, quad);

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
    CommandProcessor::quit();

    return 0;
}