/**********************************************************
*                                                         *
*  IOTMA - IoT Mananger  Agent                            *
*                                                         *
*  Author: Alexandre Heideker / Dener Ottoni              *
*  e-mail: alexandre.heideker@ufabc.edu.br                *
*  UFABC - 2020                                           *
*  Ver. 1.0                                               *
*                                                         *
***********************************************************/


#include <thread>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <curl/curl.h>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <mutex>
#include <jsoncpp/json/json.h>
#define SEM_WAIT mt.lock();
#define SEM_POST mt.unlock();
#define AQUI cout << "aqui" << endl;

bool debug = false;

#include "swissknife.h"
#include "mondata.h"
#include "iotma.h"

using namespace std;

std::mutex mt;
bool createEntity(string OrionUrl, string FWService, string FWSPath, std::string entity);
void thrSampling();
void thrWatchDog();
void thrSetup();
void thrAction();
void thrPublisher();
bool WDogBark();
bool getOrionSetup(Agent);
bool ckEntityById(string OrionUrl, string FWService, string FWSPath, string Id);
bool getRestFiware(string url, curl_slist *chunk, string data, string request, string &bufferOut);
unsigned int getSetupTimestamp();
void updateMainEntity();
void updateChildrenEntities();


Agent MFstats;

int main(int argc, char *argv[]) {
    //Default settings...
    MFstats.IMvar.CPUPathStat = "/proc/stat";
    MFstats.IMvar.CPUPathArch = "/proc/cpuinfo";
    MFstats.IMvar.NetworkPathStat = "/proc/net";
    MFstats.IMvar.SampplingTime = 5;

    if (!readSetup(MFstats)) return -1;

    if (argc>1) {
        if (!readSetupFromCL(MFstats, argc, argv)) return -1;
    }
    debug = MFstats.debugMode;
    if (debug) dumpVar(MFstats);

    getOrionSetup(MFstats);
    
    std::thread refreshStats (thrSampling);
    sleep(5);
    std::thread refreshSetup (thrSetup);
    //std::thread action (thrAction);
    std::thread pubOrion (thrPublisher);
    //std::thread tWtDog (thrWatchDog);
    refreshStats.join();
    refreshSetup.join();
    //action.join();
    pubOrion.join();
    //tWtDog.join();
    
    return 0;
}

void thrAction(){
    
}

void thrSetup() {
    int intervall;
    unsigned int OldSetupTimestamp = getSetupTimestamp();
    intervall = MFstats.setupinterval;
    if (debug) cout << "Setup thread OK -> interval:" << intervall << endl;
    while (true) {
        sleep(intervall);
        unsigned int ts = getSetupTimestamp();
        if (OldSetupTimestamp != ts) {
            getOrionSetup(MFstats);
            SEM_WAIT
            intervall = MFstats.setupinterval;
            SEM_POST
            OldSetupTimestamp = ts;
        }
    }
}


unsigned int getSetupTimestamp(){
    ostringstream url;
    SEM_WAIT
    url << MFstats.host << "/v2/entities/" << MFstats.entity << "/attrs/setuptimestamp/value";
    if (debug) cout << "URL:\t" << url.str() << endl;
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Accept: text/plain");
    chunk = curl_slist_append(chunk, ("fiware-service: " +  MFstats.service).c_str());
    chunk = curl_slist_append(chunk, ("fiware-servicepath: " +  MFstats.servicepath).c_str());
    SEM_POST
    std::string out = getRest(url.str(), chunk, "");
    if (out != "[]") 
        if (out.find("{\"error\":",0)) 
            return 0;
    return std::stoul(out);
}

void thrSampling(){
    SEM_WAIT
    int intervall = MFstats.samplinginterval;
    SEM_POST
    if (debug) cout << "Sampling thread OK -> interval:" << intervall << endl;
    while (true) {
        std::time_t tsIni = std::time(0);
        SEM_WAIT
        if (debug) cout << "Sampling..." << tsIni << endl;
        MFstats.Refresh();
        intervall = MFstats.samplinginterval;
        SEM_POST        
        int taskTime = (int) (std::time(0) - tsIni);
        int sleepTime = intervall - taskTime;
        if (taskTime <= intervall) 
            sleep(sleepTime);
    }
}

void thrWatchDog(){
/*
    if (!MFstats.IMvar.WatchDogMode) return;
    while (true) {
        sleep(MFstats.IMvar.WatchDogTime);
        if (!WDogBark()) {
            if (debug) cout << "Watch Dog Alert!" << endl;
            //Alert Mode!
            int cnt = MFstats.IMvar.WatchDogRetryCount;
            do {
                sleep(MFstats.IMvar.WatchDogAlertTime);
                cnt--;
                if (cnt==0) {
                    system(MFstats.IMvar.WatchDogRebootCommand.c_str());
                    break;
                    //exit(1);
                }
                if (debug) cout << "Watch Dog retry number " << (MFstats.IMvar.WatchDogRetryCount-cnt) << endl;
            } while (!WDogBark());
        }
    }
*/
}

bool WDogBark(){
    /*
    if (trim(MFstats.IMvar.WatchDogHeartbeatScript).length() > 1 ) {

        return false;
    } else if (trim(MFstats.IMvar.WatchDogHeartbeatOrionURL).length() > 1 ) {
        ostringstream url;
        url << MFstats.IMvar.WatchDogHeartbeatOrionURL << "/version";
        if (debug) cout << "URL:\t" << url.str() << endl;
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Accept: application/json");
        string retStr = getRest(url.str(), chunk, "");
        if (debug) cout << "WatchDog return:\t" << retStr << endl;
        if (retStr.find("orion",0)!=std::string::npos) 
            return true;
        return false;
    } else
    */ 
        return true;

}

void thrPublisher(){
    SEM_WAIT
    int intervall = MFstats.publishintervall;
    SEM_POST
    if (debug) cout << "Publisher thread OK -> interval:" << intervall << endl;
    while (true) {
        sleep(intervall);
        //publish in main entity
        updateMainEntity();
        //publish in children etities
        updateChildrenEntities();
        SEM_WAIT
        intervall = MFstats.publishintervall;
        SEM_POST
    }
}
void updateMainEntity() {
    ostringstream url;
    ostringstream json;
    std::string out;
    struct curl_slist *chunk = NULL;
    SEM_WAIT
    url << MFstats.host << "/v2/entities/" << MFstats.entity << "/attrs?options=keyValues";
    if (debug) cout << "URL:\t" << url.str() << endl;
    json << "{\"cpu\":{\"type\":\"number\",\"value\":" << MFstats.cpuLevel 
        << "}, \"memoryavailable\":{\"type\":\"number\",\"value\":" << MFstats.MemoryAvailable
        << "}, \"memorysize\":{\"type\":\"number\",\"value\":" << MFstats.MemorySize
        << "},\"timestamp\":" << MFstats.timestamp << "}";
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, ("fiware-service: " +  MFstats.service).c_str());
    chunk = curl_slist_append(chunk, ("fiware-servicepath: " +  MFstats.servicepath).c_str());
    SEM_POST
    getRestFiware(url.str(), chunk, json.str(), "POST", out);
}

void updateChildrenEntities() {
    struct batch {
        std::string host;
        std::string entity;
        std::string service;
        std::string servicepath;
    };
    int cpu, memorysize, memoryavailable, n;
    unsigned int ts;
    ostringstream url;
    ostringstream json;
    std::string out;
    struct curl_slist *chunk = NULL;
    SEM_WAIT
    cpu = MFstats.cpuLevel;
    memoryavailable = MFstats.MemoryAvailable;
    memorysize = MFstats.MemorySize;
    ts = MFstats.timestamp;
    n = MFstats.entities.size();
    std::vector<batch> ent;
    for (auto e: MFstats.entities) {   
        batch t; 
        t.host = e.host;
        //cout << e.host << endl;
        t.entity = e.entityid;
        //cout << e.entityid << endl;
        t.service = e.service;
        t.servicepath = e.servicepath;
        ent.push_back (t);
    }
    SEM_POST
    for (int i = 0; i < n; i++) {
        ostringstream url;
        ostringstream json;
        url << ent[i].host << "/v2/entities/" << ent[i].entity << "/attrs?options=keyValues";
        if (debug) cout << "URL:\t" << url.str() << endl;
        json << "{\"cpu\":{\"type\":\"number\",\"value\":" << cpu 
            << "}, \"memoryavailable\":{\"type\":\"number\",\"value\":" << memoryavailable
            << "}, \"memorysize\":{\"type\":\"number\",\"value\":" << memorysize
            << "},\"timestamp\":" << ts << "}";
        chunk = NULL;
        chunk = curl_slist_append(chunk, "Content-Type: application/json");
        chunk = curl_slist_append(chunk, ("fiware-service: " +  ent[i].service).c_str());
        chunk = curl_slist_append(chunk, ("fiware-servicepath: " +  ent[i].servicepath).c_str());
        getRestFiware(url.str(), chunk, json.str(), "POST", out);
    }
}

bool createEntity(string OrionUrl, string FWService, string FWSPath, std::string entity){
    ostringstream url;

    url << OrionUrl << "/v2/entities?options=keyValues";
    if (debug) cout << "URL:\t" << url.str() << endl;
    if (debug) cout << "JSON:\t" << entity << endl;
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, ("fiware-service: " +  FWService).c_str());
    chunk = curl_slist_append(chunk, ("fiware-servicepath: " +  FWSPath).c_str());

    string retStr = getRest(url.str(), chunk, entity);
    cout << retStr << endl;
    if (retStr.find("{\"error\":",0)) 
        return false;
    return true;
}

bool ckEntityById(string OrionUrl, string FWService, string FWSPath, string Id, string & out){
    ostringstream url;

    url << OrionUrl << "/v2/entities?options=keyValues&id=" << Id;
    if (debug) cout << "URL:\t" << url.str() << endl;
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Accept: application/json");
    chunk = curl_slist_append(chunk, ("fiware-service: " +  FWService).c_str());
    chunk = curl_slist_append(chunk, ("fiware-servicepath: " +  FWSPath).c_str());
    out = "";
    out = getRest(url.str(), chunk, "");
    if (out != "[]") 
        if (out.find("{\"error\":",0)) 
            return true;
    return false;
}


string getDockerProcesses(){
    string Cmd("docker stats --all --no-stream --format \";{{.Name}};{{.CPUPerc}};{{.MemUsage}}\"");
    string ret(run(Cmd));
    replaceAll(ret, "\n", ";");
    return ret;
}

bool getOrionSetup(Agent){
    Json::Reader reader;
    Json::Value obj;
    std::string entity;
    if (ckEntityById(MFstats.host, MFstats.service, MFstats.servicepath, MFstats.entity, entity)) {
        //Read setup from entity
        reader.parse(entity, obj);
        obj = obj[0];
        if (obj["id"].asString()==MFstats.entity) {
            SEM_WAIT
            MFstats.setupAgent(obj);
            string out;
            for (auto &e: MFstats.entities) {
                e.exist = ckEntityById(e.host, e.service, e.servicepath, e.entityid, out);
                if (!e.exist) {
                    //create entity
                    if (debug) cout << "Entity " << e.entityid << " not exists. Creating..." << endl;
                    ostringstream json2;
                    json2 << "{\"id\":\"" << e.entityid << "\"}";
                    if (debug) cout << json2.str() << endl;
                    e.exist = createEntity(MFstats.host, MFstats.service, MFstats.servicepath, json2.str());
                }
            }
            SEM_POST
            return true;
        } else return false;
    } else {
        //create new entity with basic setings
        SEM_WAIT
        ostringstream json;
        json << "{\"id\":\"" << MFstats.entity << "\",\"type\":\"IoTMA\"," <<
                "\"host\":{\"type\":\"text\",\"value\":\"" << MFstats.host <<
                "\"},\"service\":{\"type\":\"text\",\"value\":\"" << MFstats.service << 
                "\"},\"servicepath\":{\"type\":\"text\",\"value\":\"" << MFstats.servicepath << 
                "\"},\"entitypath\":{\"type\":\"text\",\"value\":\"" << 
                "\"},\"publishintervall\":{\"type\":\"number\",\"value\":" << MFstats.publishintervall << 
                "},\"actionintervall\":{\"type\":\"number\",\"value\":0" <<
                "},\"heartbeatinterval\":{\"type\":\"number\",\"value\":0" << 
                "},\"heartbeatretry\":{\"type\":\"number\",\"value\":0" << 
                "},\"action\":{\"type\":\"text\",\"value\":\"" <<
                "\"},\"timestamp\":{\"type\":\"number\",\"value\":0},\"entities\":{}}";
        if (debug) cout << json.str() << endl;
        if (createEntity(MFstats.host, MFstats.service, MFstats.servicepath, json.str())) {
            string out;
            for (auto &e: MFstats.entities) {
                e.exist = ckEntityById(e.host, e.service, e.servicepath, e.entityid, out);
                if (!e.exist) {
                    //create entity
                    if (debug) cout << "Entity " << e.entityid << " not exists. Creating..." << endl;
                    ostringstream json2;
                    json2 << "{\"id\":\"" << e.entityid << "\"}";
                    if (debug) cout << json2.str() << endl;
                    e.exist = createEntity(MFstats.host, MFstats.service, MFstats.servicepath, json2.str());
                }
            }
        }
        SEM_POST
        return true;
    }
}




bool getRestFiware(string url, curl_slist *chunk, string data, string request, string &bufferOut) {
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (debug) curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        if (data != "") {
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
            if (debug) cout << "### Data: " << data << endl;
        }
        string Buffer;
	    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30); //timeout = 30s
	    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10); //connection timeout = 10s
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);
        if (request!="") curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.c_str());
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
	    bufferOut = "";
	    return false;
            //cout << "erro" << endl;
            //return "";
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();
	bufferOut = Buffer;
        return true;
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    bufferOut = "";
    return false;
    //cout << "erro" << endl;
    //return "";
}
