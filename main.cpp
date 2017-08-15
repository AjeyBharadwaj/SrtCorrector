#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <cstdlib>

using namespace std;

struct srtTime {

    srtTime() {
        hour = min = sec = msec = 0;
    }

    int hour;
    int min;
    int sec;
    int msec;
};

typedef struct srt {
    int srtNo;

    struct srtTime fTime;
    struct srtTime tTime;

    string srt;

    struct srt* next;

}srt;

srt *head = NULL;

void getTime(ifstream &fp, struct srtTime &ptr) {
    char c[2];

    fp >> ptr.hour;
    fp.read(c, 1);
    fp >> ptr.min;
    fp.read(c, 1);
    fp >> ptr.sec;
    fp.read(c, 1);
    fp >> ptr.msec;
}

void getTime(string str, struct srtTime &ptr) {
    char c[2];
    stringstream ss(str);

    ss >> ptr.hour;
    ss.read(c, 1);
    ss >> ptr.min;
    ss.read(c, 1);
    ss >> ptr.sec;
    ss.read(c, 1);
    ss >> ptr.msec;
}

srt* getNextSrt(ifstream &fp) {
    if (fp.eof())
        return NULL;

    char c[6];

    srt *ptr = new srt;
    fp >> ptr->srtNo;

    //cout << "Str NO : " << ptr->srtNo << endl;

    getTime(fp, ptr->fTime);
    //cout << ptr->fTime.hour << " : " << ptr->fTime.min << " : " << ptr->fTime.sec << " : " << ptr->fTime.msec << endl;

    fp.read(c, sizeof(c)-1);
    //cout << c << endl;

    getTime(fp, ptr->tTime);
    //cout << ptr->tTime.hour << " : " << ptr->tTime.min << " : " << ptr->tTime.sec << " : " << ptr->tTime.msec << endl;

    string s1;
    getline(fp, s1);

    ptr->srt = s1;

    while(1) {
        getline(fp, s1);
        if (s1.length() < 2) {
            break;
        }
        ptr->srt += "\n" + s1;
    }
    //cout << "String : " << ptr->srt << endl;

    ptr->next = NULL;
    return ptr;
}

int timeToMsec(struct srtTime &t1) {
    return (t1.msec + t1.sec*1000 + t1.min*60*1000 + t1.hour*60*60*1000);
}

struct srtTime msecToTime(int msec) {
    struct srtTime time;

    time.hour = msec/(60*60*1000);
    msec %= (60*60*1000);

    time.min = msec/(60*1000);
    msec %= (60*1000);

    time.sec = msec/(1000);
    msec %= (1000);

    time.msec = msec;

    return time;
};

int timeDiff(struct srtTime &t1, struct srtTime &t2) {
    int msec1, msec2;

    msec1 = timeToMsec(t1);
    msec2 = timeToMsec(t2);

    return msec1 - msec2;
}

bool correct(int srtNo, string tTime) {
    struct srtTime time;
    srt *ptr = head;
    bool found = false;

    getTime(tTime, time);

    while(ptr) {
        if (ptr->srtNo == srtNo) {
            found = true;
            break;
        }
        ptr = ptr->next;
    }

    if (found == false) {
        return false;
    }

    int t1, t2, diff;

    t1 = timeToMsec(ptr->fTime);
    t2 = timeToMsec(time);
    ptr->fTime = msecToTime(t2);

    diff = t2-t1;
    ptr->tTime = msecToTime(timeToMsec(ptr->tTime)+diff);

    ptr = ptr->next;
    while(ptr) {
        ptr->fTime = msecToTime(timeToMsec(ptr->fTime)+diff);
        ptr->tTime = msecToTime(timeToMsec(ptr->tTime)+diff);
        ptr = ptr->next;
    }

    return true;
}

bool correct(int from, int to) {
    srt *ptr = head;

    while(ptr) {
        if (ptr->srtNo == from) {
            head = ptr;
            while(ptr) {
                ptr->srtNo = to;
                to++;
                ptr = ptr->next;
            }
            break;
        }
        ptr = ptr->next;
    }
}

void prettyPrint(char *file) {
    srt *ptr = head;

    ofstream fp(file);
    if (!(fp.is_open())) {
        cout << "File open error\n";
        return;
    }

    while(ptr) {
        fp << ptr->srtNo << endl <<
                        setfill('0') << setw(2) << ptr->fTime.hour << ":" <<
                        setfill('0') << setw(2) << ptr->fTime.min << ":" <<
                        setfill('0') << setw(2) << ptr->fTime.sec << "," <<
                        setfill('0') << setw(3) << ptr->fTime.msec
                        << " --> " <<
                        setfill('0') << setw(2) << ptr->tTime.hour << ":" <<
                        setfill('0') << setw(2) << ptr->tTime.min << ":" <<
                        setfill('0') << setw(2) << ptr->tTime.sec << "," <<
                        setfill('0') << setw(3) << ptr->tTime.msec
                        << ptr->srt << endl << endl;
        ptr = ptr->next;
    }
}

int main(int argc, char *argv[]) {
    srt *ptr, *ptr1;

    if (argc < 6) {
        // FIXME:
        cout << "./a.exe <infile> <outfile> <1|2> <from> <to>";
        return -1;
    }

    ifstream fp (argv[1]);
    if (!(fp.is_open())) {
        cout << "File open error\n";
        return -1;
    }

    head = getNextSrt(fp);
    if (head == NULL) {
        cout << "Error in getting first SRT\n";
        fp.close();
        return -1;
    }

    ptr = head;
    while((ptr1 = getNextSrt(fp))) {
        if ((ptr->srtNo+1) != (ptr1->srtNo)) {
            cout << "SRT file is not correct at : " << ptr->srtNo << endl;
            //fp.close();
            //return -1;
        }
        ptr->next = ptr1;
        ptr = ptr1;
    }

    for(int i = 3; i < argc; i = i+3) {
        switch(atoi(argv[i])) {
        case 1:
            correct(atoi(argv[i+1]), argv[i+2]);
            break;
        case 2:
            correct(atoi(argv[i+1]), atoi(argv[i+2]));
            break;

        }
    }
    prettyPrint(argv[2]);
    fp.close();
}



