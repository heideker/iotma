/**********************************************************
*                                                         *
*  IOTMA - IoT Mananger  Agent                            *
*                                                         *
*  Author: Alexandre Heideker / Dener Otoni               *
*  e-mail: alexandre.heideker@ufabc.edu.br                *
*  UFABC - 2020                                           *
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

typedef struct
{
    int pid;
    std::string Name;
    float cpu;
    long DataMem;
    long VirtMem;
} processData;

typedef struct
{
    long UDPtxQueue;
    long UDPrxQueue;
    long TCPtxQueue;
    long TCPrxQueue;
    unsigned int TCPMaxWindowSize; //only TCP
} networkData;

typedef struct
{
    std::string Name;
    long RxBytes;
    long RxPackets;
    long RxErrors;
    long TxBytes;
    long TxPackets;
    long TxErrors;
} networkAdapter;

struct MonEntity{
    bool exist;
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
        std::string CPUPathStat;
        std::string NetworkPathStat;
        std::string CPUPathArch;
        bool debugMode;
        std::string host;
        std::string entity;
        std::string servicepath;
        std::string service;
        std::string entitypath;
        int heartbeatinterval;
        int heartbeatretry;
        std::string action;
        int publishintervall;
        int actioninterval;
        int samplinginterval;
        int setupinterval;
        unsigned int setuptimestamp;
        float longitude;
        float latitude;


        unsigned int timestamp;
        long MemorySize;
        long MemoryAvailable;
        long cpuOldStats[10];
        long cpuStats[10];
        int cpuLevel;
        std::string arch;
        networkData netData;
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
        bool setupAgent(Json::Value & obj);
};

Agent::Agent(string setupHost, std::string setupEntity, 
        std::string setupServicePath, std::string setupService){
    this->setupEntity = setupEntity;
    this->setupHost = setupHost;
    this->setupService = setupService;
    this->setupServicePath = setupServicePath;
}
Agent::Agent(){
}

bool Agent::setupAgent(Json::Value & obj) {

    this->entity = obj["id"].asString();
    this->host = obj["host"]["value"].asString();
    this->service = obj["service"]["value"].asString();
    this->servicepath = obj["servicepath"]["value"].asString();
    this->entitypath = obj["entitypath"]["value"].asString();
    this->heartbeatinterval = obj["heartbeatinterval"]["value"].asInt();
    this->heartbeatretry = obj["heartbeatretry"]["value"].asInt();
    this->action = obj["action"]["value"].asString();
    this->publishintervall = obj["publishintervall"]["value"].asInt();
    this->actioninterval = obj["actioninterval"]["value"].asInt();
    this->samplinginterval = obj["samplinginterval"].asInt();
    this->setupinterval = obj["setupinterval"].asInt();
    this->setuptimestamp = obj["setuptimestamp"].asInt();
    this->entities.clear();
    for (Json::Value::ArrayIndex i = 0; i != obj["entities"].size(); i++) {
        MonEntity MnE;
        MnE.host = obj["entities"][i]["host"]["value"].asString();
        MnE.entityid = obj["entities"][i]["entityid"]["value"].asString();
        MnE.servicepath = obj["entities"][i]["servicepath"]["value"].asString();
        MnE.service = obj["entities"][i]["service"]["value"].asString();
        MnE.processtype = (obj["entities"][i]["processtype"]["value"].asString() == "" ? HARDWARE : (obj["entities"][i]["processtype"]["value"].asString() == "docker" ? DOCKER : SYSTEM));
        MnE.processname = obj["entities"][i]["processname"]["value"].asString();
        MnE.port = obj["entities"][i]["port"]["value"].asUInt();
        MnE.avaiability = obj["entities"][i]["avaiability"]["value"].asString();
        MnE.startscript = obj["entities"][i]["startscript"]["value"].asString();
        MnE.stopscript = obj["entities"][i]["stopscript"]["value"].asString();
        MnE.action = obj["entities"][i]["action"]["value"].asString();
        MnE.lastlog = obj["entities"][i]["lastlog"]["value"].asString();
        MnE.Alive = false;
        MnE.Cpu = 0;
        MnE.Memory = 0;
        MnE.exist = false;
        this->entities.push_back (MnE);
    }
    return true;
}

void Agent::Refresh(){
    //Refresh system data
    this->timestamp = std::time(0);
    struct sysinfo memInfo;
    sysinfo (&memInfo);
    this->MemorySize = memInfo.totalram;
    this->MemoryAvailable = memInfo.freeram;
    std::ifstream File;
    std::string line;

    File.open (this->CPUPathStat, std::fstream::in );
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

    //reading processes stats...using ps shell command
    std::string stringOut;
    processData ps;

    struct dk {
        std::string Name;
        unsigned int cpu;
        unsigned int m1;
        unsigned int m2;
    };
    std::vector<dk> dks;
    stringOut = run("docker stats --all --no-stream --format \"{{.Name}};{{.CPUPerc}};{{.MemUsage}}\"");
    dk d;
    if (stringOut.size()>0) {
        if (debug) cout << "Docker Stats: " << stringOut << endl;
        replaceAll(stringOut, " / ", ";");
        replaceAll(stringOut, "MiB", "E06");
        replaceAll(stringOut, "GiB", "E09");
        replaceAll(stringOut, "B", "");
        replaceAll(stringOut, "%", "");
        replaceAll(stringOut, ";", " ");
        std::stringstream ss(stringOut);
        while (ss >> d.Name >> d.cpu >> d.m1 >> d.m2){
            dks.push_back(d);
        }
    }
    for (auto &p: this->entities) {
        unsigned int m1=0, m2=0;
        unsigned int cpu=0;
        p.Alive = false;
        if (p.processtype == DOCKER) {
            for (auto d: dks) {
                if (d.Name == p.processname) {
                    p.Memory = (unsigned int) (d.m2 - d.m1);
                    p.Cpu = d.cpu;
                    p.Alive = true;
                    break;
                }
            }
        } else if (p.processtype == SYSTEM) {
            stringOut = run("ps  --no-headers -C " + p.processname + " -o pid,%cpu,rss,vsz");
            std::stringstream ss(stringOut);
            while (ss >> ps.pid >> cpu >> m1 >> m2){
                p.Memory += (m1 + m2);
                p.Cpu += cpu; 
            }
            p.Alive = true;
        } else {
            p.Alive = true;
            p.Cpu = this->cpuLevel; 
            p.Memory = this->MemorySize - this->MemoryAvailable;
        }

    }
}
