/**********************************************************
*                                                         *
*  IOTMA - IoT Mananger  Agent                            *
*                                                         *
*  Author: Alexandre Heideker                             *
*  e-mail: alexandre.heideker@ufabc.edu.br                *
*  UFABC - 2019                                           *
*  statistic data structure class                         *
***********************************************************/

using namespace std;
#define STAT_USER 0
#define STAT_NICE 1
#define STAT_SYSTEM 2
#define STAT_IDLE 3
#define STAT_IOWAIT 4
#define STAT_IRQ 5
#define STAT_SOFTIRQ 6
#define STAT_STEAL 7
#define STAT_GUEST 8
#define STAT_GUEST_NICE 9

enum ProcessType {
    HARDWARE,
    SYSTEM,
    DOCKER
};

struct MonEntity{
    std::string host;
    std::string service;
    std::string servicepath;
    std::string entityid;
    std::string entitypath;
    ProcessType processtype; 
    std::string processname;
    uint16_t port;
    std::string avaiability;
    std::string startscript;
    std::string stopscript;
    std::string action;
    std::string lastlog;
    bool Alive;
    int Cpu;
    long Memory;
};


class Agent{
    private:
        bool DockerStats;
        unsigned int setupTimeStamp;
        std::string setupHost;
        std::string setupEntity;
        std::string setupServicePath;
        std::string setupService;

    public:
        IMAIoTVar IMvar;
        std::string CPUPathStat;
        std::string NetworkPathStat;
        std::string CPUPathArch;
        bool debugMode;
        std::string Host;
        std::string Entity;
        std::string ServicePath;
        std::string Service;
        unsigned int Timestamp; //timestamp
        long MemorySize;
        long MemoryAvailable;
        long cpuOldStats[10];
        long cpuStats[10];
        int cpuLevel;
        std::string arch;
        networkData netData;
        int publishinterval;
        int actioninterval;
        int sampleinterval;
        std::vector<networkAdapter> NetAdapters;
        std::vector<MonEntity> entities;
        
        std::string getJsonStorage();
        std::string getJsonNetworkStats();
        std::string getJsonNetworkAdapters();
        std::string getJsonProcess();
        std::string getTxtStorage();
        std::string getTxtNetworkStats();
        std::string getTxtNetworkAdapters();
        std::string getTxtProcess();
        void RefreshSetup();
        void Refresh();
        bool newSetupEntity();
        Agent(string setupHost, std::string setupEntity, std::string setupServicePath, std::string setupService);
        Agent();
        void getSetup();
        bool setupAgent(Json::Value & obj);
};

Agent::Agent(string setupHost, std::string setupEntity, 
        std::string setupServicePath, std::string setupService){
    this->setupEntity = setupEntity;
    this->setupHost = setupHost;
    this->setupService = setupService;
    this->setupServicePath = setupServicePath;
    getSetup();
}
Agent::Agent(){
}

bool Agent::setupAgent(Json::Value & obj) {
    this->publishinterval = obj["publishinterval"].asInt();
    this->actioninterval = obj["actioninterval"].asInt();
    this->sampleinterval = obj["sampleinterval"].asInt();
    this->entities.clear();
    Json::Reader reader;
    Json::Value obj2 = obj["entities"]["value"];
    
    for( Json::Value::const_iterator itr = obj2.begin() ; itr != obj2.end() ; itr++ ) {
        cout << itr.key() << endl;
        
    }
    for (Json::Value::ArrayIndex i = 0; i != obj2.size(); i++) {
        MonEntity MnE;
        MnE.host = obj2[i]["host"].asString();
        AQUI
        MnE.entityid = obj2[i]["host"].asString();
        MnE.servicepath = obj2[i]["servicepath"].asString();
        MnE.service = obj2[i]["service"].asString();
        MnE.processtype = (obj2[i]["processtype"].asString() == "" ? HARDWARE : (obj2[i]["processtype"].asString() == "docker" ? DOCKER : SYSTEM));
        MnE.processname = obj2[i]["processname"].asString();
        MnE.port = obj2[i]["port"].asUInt();
        MnE.avaiability = obj2[i]["avaiability"].asString();
        MnE.startscript = obj2[i]["startscript"].asString();
        MnE.stopscript = obj2[i]["stopscript"].asString();
        MnE.action = obj2[i]["action"].asString();
        MnE.lastlog = obj2[i]["lastlog"].asString();
        MnE.Alive = false;
        MnE.Cpu = 0;
        MnE.Memory = 0;
    }
    return true;
}

void Agent::getSetup(){
    //read the timestamp from setup entity. if is early than actual, refresh the setup...
    //std::string txt = getRest(url, header, "");
    //Json::Reader rd;
    //Json::Value js; 
    //rd.parse(txt, js);

}


bool Agent::newSetupEntity() {
/*
    //Create a new setup entity 
    ostringstream json;
    SEM_WAIT
    json << "{\"id\":\"" << this->setupEntity << "\",\"type\":{\"type\":\"text\",\"value\":\"IoTMA\"}," <<
            "\"host\":{\"type\":\"text\",\"value\":\"" << this->setupHost << "\"}," <<
            "\"service\":{\"type\":\"text\",\"value\":\"" << this->setupService << "\"}," <<
            "\"servicepath\":{\"type\":\"text\",\"value\":\"" << this->setupServicePath << "\"}," <<
            "\"entitypath\":{\"type\":\"text\",\"value\":\"\"}," <<
            "\"publishintervall\":{\"type\":\"number\",\"value\":" << this->publishInterval << "}," <<
            "\"actionintervall\":{\"type\":\"number\",\"value\":" << this->actionInterval << "}," <<
            "\"action\":{\"type\":\"text\",\"value\":\"\"}," <<
            "\"timestamp\":\"" << std::time(0) << "\",\"entities\":[]}";
    SEM_POST
    ostringstream url;

    url << this->setupHost << "/v2/entities?options=keyValues";
    if (this->debugMode) cout << "URL:\t" << url.str() << endl;
    if (this->debugMode) cout << "JSON:\t" << json.str() << endl;
    struct curl_slist *chunk = NULL;
    chunk = curl_slist_append(chunk, "Content-Type: application/json");
    chunk = curl_slist_append(chunk, ("fiware-service: " +  this->setupService).c_str());
    chunk = curl_slist_append(chunk, ("fiware-servicepath: " +  this->setupServicePath).c_str());

    string retStr = getRestFiware(url.str(), chunk, json.str());
    if (retStr.find("{\"error\":",0)) 
        return false;
*/
    return true;
}



void Agent::Refresh(){
    //Refresh system data
    this->Timestamp = std::time(0);
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    this->MemorySize = memInfo.totalram;
    this->MemoryAvailable = memInfo.freeram;
    std::ifstream File;
    std::string line;
    File.open (this->CPUPathStat);
    if (File.is_open()) {
        std::string cpuId;
        File >> cpuId >> this->cpuStats[STAT_USER]
                >> this->cpuStats[STAT_NICE]
                >> this->cpuStats[STAT_SYSTEM]
                >> this->cpuStats[STAT_IDLE]
                >> this->cpuStats[STAT_IOWAIT]
                >> this->cpuStats[STAT_IRQ]
                >> this->cpuStats[STAT_SOFTIRQ]
                >> this->cpuStats[STAT_STEAL]
                >> this->cpuStats[STAT_GUEST]
                >> this->cpuStats[STAT_GUEST_NICE];
        long cpuOldIdle, cpuIdle, cpuOldAct, cpuAct, totalOld, total, totalDelta, idleDelta;

        cpuOldIdle = this->cpuOldStats[STAT_IDLE] + this->cpuOldStats[STAT_IOWAIT];
        cpuIdle = this->cpuStats[STAT_IDLE] + this->cpuStats[STAT_IOWAIT];

        cpuOldAct = this->cpuOldStats[STAT_USER] + this->cpuOldStats[STAT_NICE] + this->cpuOldStats[STAT_SYSTEM] + 
                    this->cpuOldStats[STAT_IRQ] + this->cpuOldStats[STAT_SOFTIRQ] + this->cpuOldStats[STAT_STEAL];
        cpuAct = this->cpuStats[STAT_USER] + this->cpuStats[STAT_NICE] + this->cpuStats[STAT_SYSTEM] + 
                    this->cpuStats[STAT_IRQ] + this->cpuStats[STAT_SOFTIRQ] + this->cpuStats[STAT_STEAL];
        totalOld = cpuOldIdle + cpuOldAct;
        total = cpuIdle + cpuAct;
        totalDelta = total - totalOld;
        idleDelta = cpuIdle - cpuOldIdle;

        this->cpuLevel = (int) ((totalDelta - idleDelta) * 100 / totalDelta); 
        if (this->debugMode) cout << cpuId << "  " << this->cpuLevel << endl;
        for (int j=0; j<10; j++){
            this->cpuOldStats[j] = this->cpuStats[j];
        }
        File.close();
    }
    //reading network stats
    networkData nd;
    nd.TCPtxQueue = 0;
    nd.TCPrxQueue = 0;
    nd.TCPMaxWindowSize = 0;
    std::string trash;
    File.open (this->NetworkPathStat + "/tcp");
    if (File.is_open()) {
        //header
        getline(File, line);
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            unsigned long v = 0;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            nd.TCPtxQueue += v;
            ss >> v;
            nd.TCPrxQueue += v;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            if (v > nd.TCPMaxWindowSize) nd.TCPMaxWindowSize = v;
        }
        File.close();
    }
    File.open (this->NetworkPathStat + "/tcp6");
    if (File.is_open()) {
        //header
        getline(File, line);
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            long v = 0;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            nd.TCPtxQueue += v;
            ss >> v;
            nd.TCPrxQueue += v;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            if (v > nd.TCPMaxWindowSize) nd.TCPMaxWindowSize = v;
        }
        File.close();
    }
    File.open (this->NetworkPathStat + "/udp");
    if (File.is_open()) {
        //header
        getline(File, line);
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            long v = 0;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            nd.UDPtxQueue += v;
            ss >> v;
            nd.UDPrxQueue += v;
        }
        File.close();
    }
    File.open (this->NetworkPathStat + "/udp6");
    if (File.is_open()) {
        //header
        getline(File, line);
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            long v = 0;
            ss >> trash >> trash >> trash >> trash >> trash >> trash >> v;
            nd.UDPtxQueue += v;
            ss >> v;
            nd.UDPrxQueue += v;
        }
        File.close();
    }
    this->netData = nd;
    //reading network adapters stats...
    this->NetAdapters.clear();
    File.open (this->NetworkPathStat + "/dev");
    if (File.is_open()) {
        //header
        getline(File, line);
        getline(File, line);
        networkAdapter na;
        while (! File.eof()){
            getline(File, line);
            replaceAll(line, ":", " ");
            std::stringstream ss(line);
            ss >> na.Name >> na.RxBytes >> na.RxPackets >> na.RxErrors;
            ss >> trash >> trash >> trash >> trash >> trash;
            ss >> na.TxBytes >> na.TxPackets >> na.TxErrors;
            this->NetAdapters.push_back (na);
        }
        File.close();
    }
    this->netData = nd;
    //reading architeture info...
    File.open (this->CPUPathArch);
    if (File.is_open()) {
        while (! File.eof()){
            getline(File, line);
            if (line.substr(0, 10)=="model name"){
                this->arch = trim(ReplaceForbidden(line.substr(line.find(":")+2, line.length()-1)));
                break;
            }
        }
        File.close();
    }
    //reading processes stats...using ps shell command
    std::string stringOut;
    processData ps;
    /*
    this->Processes.clear();
    for (auto p: this->IMvar.ProcessNames) {
        stringOut = run("ps  --no-headers -C " + p + " -o pid,%cpu,rss,vsz");
        std::stringstream ss(stringOut);
        while (ss >> ps.pid >> ps.cpu >> ps.DataMem >> ps.VirtMem){
            ps.Name = ReplaceForbidden(p);
            ps.DataMem *= 1000;
            ps.VirtMem *= 1000;
            if (this->debugMode) cout << ps.Name << "\t" << ps.cpu << "\t" << ps.DataMem << endl;
            this->Processes.push_back (ps);
        }
    }
    if (this->IMvar.DockerStat) {
        this->Dockers.clear();
        stringOut = run("docker stats --all --no-stream --format \"{{.Name}};{{.CPUPerc}};{{.MemUsage}}\"");
        if (this->debugMode) cout << "Docker Stats: " << stringOut << endl;
        if (stringOut.size()>0) {
            replaceAll(stringOut, " / ", ";");
            replaceAll(stringOut, "MiB", "E06");
            replaceAll(stringOut, "GiB", "E09");
            replaceAll(stringOut, "B", "");
            replaceAll(stringOut, "%", "");
            replaceAll(stringOut, ";", " ");
            std::stringstream ss(stringOut);
            float dm, n;
            ps.pid=0;
            ps.VirtMem=0;
            while (ss >> ps.Name >> ps.cpu >> dm >> n){
                ps.DataMem = (long) dm;
                if (this->debugMode) cout << ps.Name << "\t" << ps.cpu << "\t" << ps.DataMem << endl;
                if (this->IMvar.DockerNames.size()>0) {
                    if (this->IMvar.DockerNames[0] == "*") {
                        this->Dockers.push_back(ps);
                    } else {
                        for (auto d: this->IMvar.DockerNames) {
                            if (d == ps.Name)
                                this->Dockers.push_back (ps);
                        }  
                    }
                }
            }
        }   
    }
    */
}
/*
std::string MonData::getTxtStorage(){
    return "Storage";
}

std::string MonData::getJsonNetworkStats(){
    std::ostringstream s;
    s << "{\"TCPrxQueue\":" << this->netData.TCPrxQueue 
        << ", \"TCPtxQueue\":" << this->netData.TCPtxQueue 
        << ", \"TCPMaxWindowSize\":" << this->netData.TCPMaxWindowSize 
        << ", \"UDPrxQueue\":" << this->netData.TCPtxQueue 
        << ", \"UDPtxQueue\":" << this->netData.TCPtxQueue << "}";
    return s.str();
}

std::string MonData::getJsonNetworkAdapters(){
    std::ostringstream s;
    s << "[";
    bool cy = false;
    for (auto p: this->NetAdapters) {
        if (cy) s << ",";
        cy = true;
        s << "{\"name\":\"" << p.Name << "\", \"rxBytes\":" << p.RxBytes 
            << ", \"rxPackets\":" << p.RxPackets
            << ", \"rxErrors\":" << p.RxErrors
            << ", \"txBytes\":" << p.TxBytes 
            << ", \"txPackets\":" << p.TxPackets
            << ", \"txErrors\":" << p.TxErrors << "}";
    }
    s << "]";
    return s.str();
}
std::string MonData::getTxtNetworkStats(){
    return "Network";
}
std::string MonData::getTxtNetworkAdapters(){
    return "Network";
}
std::string MonData::getJsonProcess(){
    std::ostringstream s;
    s << "[";
    bool cy = false;
    for (auto p: this->Processes) {
        if (cy) s << ",";
        cy = true;
        s << "{\"type\":\"system\", \"pid\":" << p.pid << ", \"name\": \"" << p.Name
            << "\", \"memory\": " << p.DataMem << ", \"cpu\":" << p.cpu << "}";
    }
    for (auto p: this->Dockers) {
        if (cy) s << ",";
        cy = true;
        s << "{\"type\":\"docker\", \"pid\":" << p.pid << ", \"name\": \"" << p.Name
            << "\", \"memory\": " << p.DataMem << ", \"cpu\":" << p.cpu << "}";
    }
    s << "]";
    return s.str();
}
std::string MonData::getTxtProcess(){
    return "Process";
}
*/




