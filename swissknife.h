/**********************************************************
*                                                         *
*  IMAIoT - Infrastructure Monitoring Agent for IoT       *
*                                                         *
*  Author: Alexandre Heideker                             *
*  e-mail: alexandre.heideker@ufabc.edu.br                *
*  UFABC - 2019                                           *
*  Utility library                                        *
***********************************************************/

using namespace std;

string trim(const string& str){
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> splitString(std::string strToSplit, char delimeter) {
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
       splittedStrings.push_back(item);
    }
    return splittedStrings;
}

string UCase(string strIn) {
    string p;
    for (unsigned int i=0; i<strIn.length(); i++) {
        p += toupper(strIn[i]);
    }
    return p;
}

string run(string command) {
    ostringstream out;
    FILE *in;
    char buff[512];
    if(!(in = popen(command.c_str(), "r"))){
        return NULL;
    }
    while(fgets(buff, sizeof(buff), in)!=NULL)
        out << buff;
    pclose(in);
    return out.str();
}

void replaceAll(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::string ReplaceForbidden(std::string s){
    if (s.find("("))
        replaceAll(s, "(", " "); 
    if (s.find(")"))
        replaceAll(s, ")", " "); 
    if (s.find("<"))
        replaceAll(s, "<", " "); 
    if (s.find(";"))
        replaceAll(s, ";", " "); 
    if (s.find("'"))
        replaceAll(s, "'", " "); 
    if (s.find("\""))
        replaceAll(s, "\"", " "); 
    if (s.find("="))
        replaceAll(s, "=", " "); 
    return s;
}

size_t curlCallback(char *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string getRest(string url, string header, string data) {
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (debug) curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        if (data != "") {
            struct curl_slist *chunk = NULL;
            chunk = curl_slist_append(chunk, header.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1L);
        }
        string Buffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return "";
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return Buffer;
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return "";
}



string getRest(string url, curl_slist *chunk, string data) {
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
        }
        string Buffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return "";
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return Buffer;
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return "";
}

void PrintJSONValue( const Json::Value &val )
{
    if( val.isString() ) {
        printf( "string(%s)", val.asString().c_str() ); 
    } else if( val.isBool() ) {
        printf( "bool(%d)", val.asBool() ); 
    } else if( val.isInt() ) {
        printf( "int(%d)", val.asInt() ); 
    } else if( val.isUInt() ) {
        printf( "uint(%u)", val.asUInt() ); 
    } else if( val.isDouble() ) {
        printf( "double(%f)", val.asDouble() ); 
    }
    else 
    {
        printf( "unknown type=[%d]", val.type() ); 
    }
}

bool PrintJSONTree( const Json::Value &root, unsigned short depth /* = 0 */) 
{
    depth += 1;
    printf( " {type=[%d], size=%d}", root.type(), root.size() ); 

    if( root.size() > 0 ) {
        printf("\n");
        for( Json::Value::const_iterator itr = root.begin() ; itr != root.end() ; itr++ ) {
            // Print depth. 
            for( int tab = 0 ; tab < depth; tab++) {
               printf("-"); 
            }
            printf(" subvalue(");
            PrintJSONValue(itr.key());
            printf(") -");
            PrintJSONTree( *itr, depth); 
        }
        return true;
    } else {
        printf(" ");
        PrintJSONValue(root);
        printf( "\n" ); 
    }
    return true;
}