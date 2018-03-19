#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <map>
#include <time.h>
#include <conio.h>
using namespace std;

/*
 * Struct: bucket
 * --------------
 * This struct is used to represent the buckets in the hash table.
 * Each bucket can store 10 pairs of keys & values, and the values
 * here mean their position in the data file ("db.dat"). "BucketN"
 * is the number of this bucket.
 */

struct bucket {
    char keys[10][15];
    int values[10];
    int bucketN;
    bucket(int n) : bucketN(n) {
        string temp = "";
        for (int i = 0; i < 10; ++i) {
            strcpy_s(keys[i], temp.c_str());
            values[i] = -1;
        }
    }
};

#define bucketSize sizeof(bucket)
#define valueSize sizeof(char[15])

/*
 * Class: dataBase
 * --------------
 * This class is used to represent the data base. There are 2 private
 * variable in it, the index file and the data file. 6 public
 * functions are provided for the required operation of this data
 * base. In order to reduce conflict, I set 700001 buckets for the
 * hash table.
 */

class dataBase {
    fstream idx;
    fstream dat;
    string deleted;
    int hashFunc(string &key) {
        // This function gets a hashed number from a string.
        unsigned int len = key.size();
        size_t hash = 0;
        for (unsigned int i = 0; i < len; ++i) {
            hash = hash * 31 + key[i];
        }
        return hash % 700001;
    }
    int ins_check(bucket &b, string &key) {
        /* This function finds the first usable position in the array 
         * of keys in the bucket.
         */
        int first = -1;
        for (int i = 0; i < 10; ++i) {
            if (b.keys[i] == key) {
                first = i;
                break;
            }
            if (b.keys[i][0] == '\0' || b.keys[i] == deleted) {
                first = i;
                break;
            }
        }
        return first;
    }
    void rehash(int &bucketN) {
        bucketN += 1;
    }
    void idx_ins(string &key, int &bucketN, int &p, bucket &b,
         int &first) {
        /*
         * This function inserts a key and the position of the value in
         * the data file into a bucket according to the number of it.
         */
        strcpy_s(b.keys[first], key.c_str());
        b.values[first] = p;
        idx.seekp(bucketN * bucketSize);
        idx.write((char*)(&b), bucketSize);
    }
    int dat_ins(string &value, int &ps) {
        /*
         *This function inserts a value and return its position in the
         * data file. Where it is inserted depends on "ps", which is
         * probably an effective position.
         */
        if (ps != -1) dat.seekp(ps);
        else dat.seekp(0, ios::end);
        int p = dat.tellp();
        int len = value.size();
        char buf[15];
        strcpy_s(buf, value.c_str());
        dat.write((char*)(&buf), valueSize);
        return p;
    }
    int idx_find(string &key, int &bucketN, bucket &b) {
        // This function finds the value's position in the data file.
        idx.seekg(bucketN * bucketSize);
        idx.read((char*)(&b), bucketSize);
        int kn = -1;
        for (int i = 0; i < 10; ++i) {
            if (b.keys[i] == key) {
                kn = i;
                break;
            }
        }
        if (kn == -1) {
            for (int i = 0; i < 10; ++i) {
                if (b.keys[i][0] == '\0' || b.keys[i] == deleted) {
                    return -2;
                }
            }
            return -1;
        }
        int p = b.values[kn];
        return p;
    }
    void idx_find_ins(string &key, int &ps, int &bucketN, bucket &b)
    {
        /*
         * This function finds a posible position for a value to insert
         * in the data file.
         */
        idx.seekg(bucketN * bucketSize);
        idx.read((char*)(&b), bucketSize);
        int kn = -1;
        for (int i = 0; i < 10; ++i) {
            if (b.keys[i] == key) {
                kn = i;
                break;
            }
        }
        if (kn != -1) {
            ps = b.values[kn];
            return;
        }
        for (int i = 0; i < 10; ++i) {
            if (b.keys[i][0] == '\0' || b.keys[i] == deleted) {
                kn = i;
                break;
            }
        }
        if (kn != -1) {
            ps = b.values[kn];
            return;
        }
    }
    string dat_find(int &p) {
        // This function finds the data with its position.
        string value;
        dat.seekg(p);
        char buf[15];
        dat.read((char*)(&buf), valueSize);
        value = buf;
        return value;
    }
    bool idx_delete(string &key, int &bucketN) {
        // This function deletes a data in the index file.
        idx.seekg(bucketN * bucketSize);
        bucket b(bucketN);
        idx.read((char*)(&b), bucketSize);
        int kn = -1;
        for (int i = 0; i < 10; ++i) {
            if (b.keys[i] == key) {
                kn = i;
                break;
            }
        }
        if (kn == -1) {
            for (int i = 0; i < 10; ++i) {
                if (b.keys[i][0] == '\0') {
                    return true;
                }
            }
            return false;
        }
        strcpy_s(b.keys[kn], deleted.c_str());
        idx.seekp(bucketN * bucketSize);
        idx.write((char*)(&b), bucketSize);
        return true;
    }
public:
    dataBase(string &path) {
        deleted = "[d]";
        string i = path;
        string d = path;
        i.append(".idx");
        d.append(".dat");
        // Fitrstly use "app" mode to ensure that the files can be 
        // created.
        idx.open(i.c_str(), ios::binary | ios::in | ios::out | ios::app);
        dat.open(d.c_str(), ios::binary | ios::in | ios::out | ios::app);
        idx.close();
        dat.close();
        idx.open(i.c_str(), ios::binary | ios::in | ios::out);
        dat.open(d.c_str(), ios::binary | ios::in | ios::out);
        idx.seekg(0, ios::end);
        if ((int)idx.tellg() == 0) {
            // initialize the data base
            int total = 700001;
            bucket b(0);
            while (total > 0) {
                idx.write((char*)(&b), bucketSize);
                b.bucketN += b.bucketN;
                total--;
            }
        }
    };
    dataBase(){};
    void open(string &path) {
        string i = path;
        string d = path;
        i.append(".idx");
        d.append(".dat");
        // Fitrstly use "app" mode to ensure that the files can be 
        // created.
        idx.open(i.c_str(), ios::binary | ios::in | ios::out | ios::app);
        dat.open(d.c_str(), ios::binary | ios::in | ios::out | ios::app);
        idx.close();
        dat.close();
        idx.open(i.c_str(), ios::binary | ios::in | ios::out);
        dat.open(d.c_str(), ios::binary | ios::in | ios::out);
        idx.seekg(0, ios::end);
        if ((int)idx.tellg() == 0) {
            // initialize the data base
            int total = 700001;
            bucket b(0);
            while (total > 0) {
                idx.write((char*)(&b), bucketSize);
                b.bucketN += b.bucketN;
                total--;
            }
        }
    }
    void store(string &key, string &value) {
        // This function inserts a pair of key & value into the data base.
        if (key.size() >= 15) {
            cout << "error: key too long" << endl;
            return;
        }
        if (value.size() >= 15) {
            cout << "error: value too long" << endl;
            return;
        }
        if (key == "" || value == "" || key == deleted) {
            cout << "error: invalid key / value" << endl;
        }
        int bucketN = hashFunc(key);
        int ps = -1;
        bucket b(bucketN);
        int first = -1;
        bool full = false;
        while (first == -1) {
            if (bucketN == 700001) {
                cout << "error: data base already full" << endl;
                return;
            }
            idx_find_ins(key, ps, bucketN, b);
            first = ins_check(b, key);
            if (first == -1) rehash(bucketN);
        }
        int p = dat_ins(value, ps);
        idx_ins(key, bucketN, p, b, first);
    }
    void del(string &key) {
        // This function deletes a pair of key & value from the data base.
        if (key.size() >= 15) {
            cout << "error: key too long" << endl;
            return;
        }
        if (key == "" || key == deleted) {
            cout << "error: invalid key" << endl;
        }
        int bucketN = hashFunc(key);
        while (!idx_delete(key, bucketN)) {
            if (bucketN == 700001) {
                break;
            }
            rehash(bucketN);
        }
        return;
    }
    string find(string &key) {
        // This function finds a pair of key & value in the data base.
        if (key.size() >= 15) {
            cout << "error: key too long" << endl;
            return "";
        }
        if (key == "" || key == deleted) {
            cout << "error: invalid key" << endl;
            return "";
        }
        int bucketN = hashFunc(key);
        int p = -1;
        string result = "";
        bool full = false;
        while (p == -1) {
            bucket b(bucketN);
            bool flag = false;
            if (bucketN == 700001) {
                full = true;
                break;
            }
            p = idx_find(key, bucketN, b);
            if (p == -1) rehash(bucketN);
            else if (p == -2) break;
            else result = dat_find(p);
        }
        if (full || p == -2) {
            return "";
        }
        return result;
    }
    void close() {
        // This function closes the data base.
        idx.close();
        dat.close();
    }
};

int ten(int& n) {
    if (n == 0) return 1;
    int to = 1;
    int i = 0;
    while (i < n) {
        to = to * 10;
        ++i;
    }
    return to;
}

string int_to_string(int& n) {
    int i = 0;
    while (n / ten(i) != 0) i++;
    if (i == 0) return "0";
    string result(i, '0');
    for (int j = 1; j < i; ++j) {
        int t = ten(j);
        result[i - j] = char(n % t * 10 / t + '0');
    }
    i = i - 1;
    result[0] = char(n / ten(i) + '0');
    return result;
}

/* TEST 0
int main() {
    string path = "test0";
    dataBase db(path);
    const int total = 100;
    int n = total;
    string key = "";
    string value = "";
    map<string, string> mmap;
    while (n--) {
        key = int_to_string(n);
        value = key;
        value.append(1, 'v');
        mmap[key] = value;
        db.store(key, value);
    }
    n = total;
    while (n--) {
        key = int_to_string(n);
        if (db.find(key) != mmap[key]) cout << "error" << endl;
    }
    n = total;
    while (n--) {
        key = int_to_string(n);
        db.del(key);
    }
    n = total;
    while (n--) {
        if (db.find(key) != "") cout << "error" << endl;
    }
    return 0;
}
*/

/* TEST 1
int main() {
    double t1 = clock();
    string path = "test";
    string key = "";
    string value = "";
    dataBase db(path);
    int n = 1000000;
    while (n--) {
        key = int_to_string(n);
        value = key;
        value.append(1, 'v');
        db.store(key, value);
    }
    double t2 = clock();
    cout << (t2 - t1) / CLOCKS_PER_SEC;
    return 0;
}
*/

/* TEST 2
int main() {
    double t1 = clock();
    string path = "test";
    string key = "";
    dataBase db(path);
    int n = 10000;
    while (n--) {
        key = int_to_string(n);
        db.del(key);
    }
    double t2 = clock();
    cout << (t2 - t1) / CLOCKS_PER_SEC;
    return 0;
}
*/

/* TEST 3
int main() {
    double t1 = clock();
    string path = "test";
    string key = "";
    dataBase db(path);
    int n = 1000000;
    while (n--) {
        key = int_to_string(n);
        db.find(key);
    }
    double t2 = clock();
    cout << (t2 - t1) / CLOCKS_PER_SEC;
    return 0;
}
*/

/* TEST 4
int main() {
    double t1 = clock();
    string path = "newtest";
    string key = "";
    dataBase db(path);
    db.close();
    double t2 = clock();
    cout << (t2 - t1) / CLOCKS_PER_SEC;
    return 0;
}
*/


int main() {
    string path = "test";
    dataBase db;
    while (true) {
        if (_kbhit()) {
            string com = "";
            getline(cin, com);
            if (com == "quit") exit(0);
            else if (com == "open") {
                db.open(path);
                cout << "ok" << endl;
            }
            else if (com == "close") {
                db.close();
                cout << "ok" << endl;
            }
            else if (com == "store") {
                getline(cin, com);
                stringstream ss(com);
                string key, value;
                getline(ss, key, ' ');
                getline(ss, value, ' ');
                db.store(key, value);
            }
            else if (com == "fetch") {
                getline(cin, com);
                string result = db.find(com);
                if (result == "") cout << "not found" << endl;
                else cout << result << endl;
            }
            else if (com == "delete") {
                getline(cin, com);
                db.del(com);
                cout << "ok" << endl;
            }
            else cout << "error: invalid input" << endl;
        }
    }
}
