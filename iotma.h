/**********************************************************
*                                                         *
*  IOTMA - IoT Mananger  Agent                            *
*                                                         *
*  Author: Alexandre Heideker / Dener Ottoni              *
*  e-mail: alexandre.heideker@ufabc.edu.br                *
*  UFABC - 2019                                           *
*  Ver. 0.2                                               *
***********************************************************/

typedef struct
{
    std::string NodeUUID;
    bool debugMode;
    std::string EntityPath;
    int PublishIntervall;
    int ActionIntervall;
    int HeartBeatInterval;
    int HeartBeatRetry;
    std::string OrionURL;
    std::string FiwareService;
    std::string FiwareServicepath;
    std::string CPUPathStat;
    std::string CPUPathArch;
    std::string NetworkPathStat;
    u_int16_t SampplingTime;

    u_int16_t OrionPublisherTime;
    u_int16_t LogIntervall;
    bool DockerStat;
    bool CPUStat;
    bool NetworkStat;
    bool DiskStat;
    std::vector<std::string> ProcessNames;
    std::vector<std::string> DockerNames;
    float longitude;
    float latitude;
    bool WatchDogMode;
    u_int16_t WatchDogTime;
    u_int16_t WatchDogAlertTime;
    u_int16_t WatchDogRetryCount;
    std::string WatchDogHeartbeatScript;
    std::string WatchDogHeartbeatOrionURL;
    std::string WatchDogRebootCommand;
} IMAIoTVar;


void dumpVar(Agent & IMvar)
{
    cout << "NodeUUID:\t" << IMvar.entity << endl;
    cout << "Longitude:\t" << IMvar.longitude << endl;
    cout << "Latitude:\t" << IMvar.latitude << endl;
    cout << "debugMode:\t" << IMvar.debugMode << endl;
    cout << "OrionURL:\t" << IMvar.host << endl;
    cout << "FiwareService:\t" << IMvar.service << endl;
    cout << "FiwareServicepath:\t" << IMvar.servicepath << endl;
    cout << "SampplingTime:\t" << IMvar.samplinginterval << endl;
    cout << "PublishIntervall:\t" << IMvar.publishintervall << endl;
    cout << "CPUPathStat:\t" << IMvar.CPUPathStat << endl;
    cout << "CPUPathArch:\t" << IMvar.CPUPathArch << endl;
    cout << "HeartBeatInterval:\t" << IMvar.heartbeatinterval << endl;
    cout << "HeartBeatRetry:\t" << IMvar.heartbeatretry << endl;
}

bool parseVar(Agent & IMvar, string token, string value)
{
    //cout << "Token:" << token << "\tValue:" << value << endl; 
    if (token == "debugMode")
    {
        if (value == "1")
            IMvar.debugMode = true;
        else
            IMvar.debugMode = false;
    }
    // Essential Setup 
    else if (token == "NodeUUID")
    {
        IMvar.entity = value;
    }
    else if (token == "OrionURL")
    {
        IMvar.host = value;
    }
    else if (token == "FiwareService")
    {
        IMvar.service = value;
    }
    else if (token == "FiwareServicepath")
    {
        IMvar.servicepath = value;
    }
    else if (token == "CPUPathStat")
    {
        IMvar.CPUPathStat = value;
    }
    else if (token == "CPUPathArch")
    {
        IMvar.CPUPathArch = value;
    }
    else if (token == "NetworkPathStat")
    {
        IMvar.NetworkPathStat = value;
    }
    else if (token == "SampplingTime")
    {
        IMvar.samplinginterval = stoi(value);
    }
    // End of essential setup...

    else if (token == "Latitude")
    {
        IMvar.latitude = stof(value);
    }
    else if (token == "Longitude")
    {
        IMvar.longitude = stof(value);
    }
    else if (token == "OrionPublisherTime")
    {
        IMvar.publishintervall = stoi(value);
    }

    /*
    else if (token == "ProcessNames")
    {
        IMvar.ProcessNames = splitString(trim(value), ' ');
    }
    else if (token == "DockerNames")
    {
        IMvar.DockerNames = splitString(trim(value), ' ');
    }
    else if (token == "DiskStat")
    {
        if (value == "1")
            IMvar.DiskStat = true;
        else
            IMvar.DiskStat = false;
    }
    */
    else
    {
        cout << "Invalid argument: Token=" << token << "  Value=" << value << endl;
        return false;
    }
    return true;
}

bool readSetup(Agent & IMvar)
{
    bool error = false;
    ifstream File;
    string line;
    File.open("iotma.conf");
    if (File.is_open())
    {
        string token;
        string value;
        while (!File.eof() && !error)
        {
            getline(File, line);
            if (line[0] != '#' && line != "")
            {
                token = trim(line.substr(0, line.find("=")));
                value = trim(line.substr(line.find("=") + 1, line.length() - 1));
                if (line.find("=") + 1 == line.length()) value = "";
                error = !parseVar(IMvar, token, value);
            }
        }
        File.close();
    }
    return !error;
}

bool readSetupFromCL(Agent & IMvar, int argc, char *argv[])
{
    int i;
    bool error = false;
    string token;
    string value;
    for (i = 1; i < argc; i++)
    {
        string line(argv[i]);
        if (line[0] == '-' && line[1] == '-')
        {
            token = trim(line.substr(2, line.find("=") - 2));
            value = trim(line.substr(line.find("=") + 1, line.length() - 1));
            if (line.find("=") + 1 == line.length()) value = "";
            error = !parseVar(IMvar, token, value);
        }
        else
        {
            error = true;
        }
        if (error)
            break;
    }
    if (error)
    {
        cout << "Error: Invalid command line argument" << endl;
        cout << "imaiot --help for usage" << endl;
        return false;
    }
    else
    {
        return true;
    }
}
