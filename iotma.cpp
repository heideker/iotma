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
#include "iotma.h"
#include "mondata.h"

using namespace std;

std::mutex mt;
bool createEntity(string OrionUrl, string FWService, string FWSPath, std::string entity);
bool ckEntity();
bool updateEntity();
void thrSampling();
void thrWatchDog();
void thrOrionPublisher();
bool WDogBark();
bool getOrionSetup(Agent);
bool ckEntityById(string OrionUrl, string FWService, string FWSPath, string Id);



Agent MFstats;
IMAIoTVar Setup;

int main(int argc, char *argv[]) {
    if (!readSetup(Setup)) return -1;

    if (argc>1) {
        if (!readSetupFromCL(Setup, argc, argv)) return -1;
    }
    debug = Setup.debugMode;
    if (debug) dumpVar(Setup);


    getOrionSetup(MFstats);
    return 0;

    std::thread refresh (thrSampling);
    std::thread tWtDog (thrWatchDog);
    std::thread tOrion (thrOrionPublisher);
    tOrion.join();
    refresh.join();
    return 0;
}

void thrSampling(){
    while (true) {
        std::time_t tsIni = std::time(0);
        SEM_WAIT
        if (debug) cout << "Sampling..." << tsIni << endl;
        MFstats.Refresh();
        SEM_POST        
        int taskTime = (int) (std::time(0) - tsIni);
        int sleepTime = MFstats.IMvar.SampplingTime - taskTime;
        if (taskTime <= MFstats.IMvar.SampplingTime) 
            sleep(sleepTime);
    }
}

void thrWatchDog(){
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
}

bool WDogBark(){
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
    } else return true;
}

void thrOrionPublisher(){
    while (true) {
        std::time_t tsIni = std::time(0);
        if (debug) cout << "Updating Orion Context-broker..." << endl;
        updateEntity();
        int taskTime = (int) (std::time(0) - tsIni);
        int sleepTime = MFstats.IMvar.OrionPublisherTime - taskTime;
        if (taskTime <= MFstats.IMvar.OrionPublisherTime) 
            sleep(sleepTime);
    }
}

bool updateEntity(){
    if (!ckEntity()) return false;
    ostringstream json;
    SEM_WAIT
/*
    json << "{\"MFType\":{\"type\":\"Text\", \"value\":\""
            << MFstats.IMvar.KindOfNode << "\"}, \"Architecture\":{\"type\":\"Text\", \"value\":\"" << MFstats.arch 
            << "\"},\"MemorySize\":{\"type\":\"Integer\", \"value\": " << MFstats.MemorySize 
            << "},\"MemoryAvailable\":{\"type\":\"Integer\", \"value\": " << MFstats.MemoryAvailable
            << "},\"LocalTimestamp\":{\"type\":\"Integer\", \"value\": " << MFstats.Timestamp
            << "},\"SampplingTime\":{\"type\":\"Integer\", \"value\": " << MFstats.IMvar.SampplingTime
            << "},\"CPU\":{\"type\":\"Integer\", \"value\": " << MFstats.cpuLevel //:" << MFstats.getJsonCpu() 
            << "},\"Storage\":" << MFstats.getJsonStorage() 
            << ",\"NetworkStats\":" << MFstats.getJsonNetworkStats() 
            << ",\"NetworkAdapters\":" << MFstats.getJsonNetworkAdapters() 
            << ",\"Process\":" << MFstats.getJsonProcess() 
            << "}";

    SEM_POST
    ostringstream url;
    
    url << MFstats.IMvar.OrionURL << "/v2/entities/" << MFstats.IMvar.NodeUUID << "/attrs?options=keyValues";
    if (debug) cout << "URL:\t" << url.str() << endl;
    if (debug) cout << "JSON:\t" << json.str() << endl;
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, ("fiware-service: " +  MFstats.IMvar.FiwareService).c_str());
    chunk = curl_slist_append(chunk, ("fiware-servicepath: " +  MFstats.IMvar.FiwareServicepath).c_str());
    string retStr = getRest(url.str(), chunk, json.str());
    if (retStr.find("{\"error\":",0)) 
        return false;
*/
    return true;
}

bool createEntity(string OrionUrl, string FWService, string FWSPath, std::string entity){
    ostringstream url;

    url << OrionUrl << "/v2/entities";
    if (debug) cout << "URL:\t" << url.str() << endl;
    if (debug) cout << "JSON:\t" << entity << endl;
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, ("fiware-service: " +  FWService).c_str());
    chunk = curl_slist_append(chunk, ("fiware-servicepath: " +  FWSPath).c_str());

    string retStr = getRest(url.str(), chunk, entity);
    if (retStr.find("{\"error\":",0)) 
        return false;
    return true;
}
bool ckEntity(){
/*
    ostringstream url;

    url << MFstats.IMvar.OrionURL << "/v2/entities?type=IMAIoT&id=" << MFstats.IMvar.NodeUUID;
    if (debug) cout << "URL:\t" << url.str() << endl;
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, ("fiware-service: " +  MFstats.IMvar.FiwareService).c_str());
    chunk = curl_slist_append(chunk, ("fiware-servicepath: " +  MFstats.IMvar.FiwareServicepath).c_str());

    string retStr = getRest(url.str(), chunk, "");
    if (retStr != "[]") 
        return true;
*/
    return false;
}

bool ckEntityById(string OrionUrl, string FWService, string FWSPath, string Id, string & out){
    ostringstream url;

    url << OrionUrl << "/v2/entities?id=" << Id;
    if (debug) cout << "URL:\t" << url.str() << endl;
    struct curl_slist *chunk = NULL;
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

void ServerMode(int MonTime, int TCPPort, int SamplingTime){
    cout << "Not Implemented!" << endl;
    return;
}

bool getOrionSetup(Agent){
    Json::Reader reader;
    Json::Value obj;
    std::string entity;
    if (ckEntityById(Setup.OrionURL, Setup.FiwareService, Setup.FiwareServicepath, Setup.NodeUUID, entity)) {
        //Read setup from entity
        reader.parse(entity, obj);

        //PrintJSONTree(obj[0],2);

        obj = obj[0];
        if (obj["id"].asString()==Setup.NodeUUID) {
            //fill agent data
            MFstats.setupAgent(obj);


            return true;
        } else return false;
    } else {
        //create new entity with basic setings
        ostringstream json;
        json << "{\"id\":\"" << Setup.NodeUUID << "\",\"type\":\"IoTMA\"," <<
                "\"host\":{\"type\":\"text\",\"value\":\"" << Setup.OrionURL <<
                "\"},\"service\":{\"type\":\"text\",\"value\":\"" << Setup.FiwareService << 
                "\"},\"servicepath\":{\"type\":\"text\",\"value\":\"" << Setup.FiwareServicepath << 
                "\"},\"entitypath\":{\"type\":\"text\",\"value\":\"" << 
                "\"},\"publishintervall\":{\"type\":\"number\",\"value\":" << Setup.PublishIntervall << 
                "},\"actionintervall\":{\"type\":\"number\",\"value\":0" <<
                "},\"heartbeatinterval\":{\"type\":\"number\",\"value\":0" << 
                "},\"heartbeatretry\":{\"type\":\"number\",\"value\":0" << 
                "},\"action\":{\"type\":\"text\",\"value\":\"" <<
                "\"},\"timestamp\":{\"type\":\"number\",\"value\":0},\"entities\":{}}";
        cout << json.str() << endl;
        return createEntity(Setup.OrionURL, Setup.FiwareService, Setup.FiwareServicepath, json.str());
    }
}




