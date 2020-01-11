#include <iostream>
#include <string>
#define CURL_STATICLIB
#include <curl.h>
#include <iostream>
#include <rapidjson.h>
#include <stdio.h>
#include <document.h>
using namespace std;
class Reader {
public:
    virtual void read(string* data) = 0;
};

class InfoReader : public Reader {

public:
    void read(string* data) {
        rapidjson::Document document;
        document.Parse(data->c_str());
        const rapidjson::Value& results = document["AirportBoardsResult"];
        const rapidjson::Value& arrivals = results["arrivals"];
        const rapidjson::Value& flights = arrivals["flights"];
        assert(flights.IsArray());
        if (flights.Size() == 0) {
            cout << "Error , please try again" << endl;
        }
        else
        {
            for (rapidjson::SizeType i = 0; i < flights.Size(); i++)
            {
                cout << "All neccessary information about " << i + 1 << " flight : " << endl;
                const rapidjson::Value& val = flights[i];
                cout << "IATA code of airline: " << val["airline"].GetString() << endl;
                const rapidjson::Value& city = val["origin"];
                cout << "City of departure : " << city["city"].GetString() << endl;
                cout << "ID code of flight :" << val["ident"].GetString() << endl;
                cout << "Status : " << val["status"].GetString() << endl;
                cout << endl;
            }
        }

    }
};
class TimeReader : public Reader {
public:
    void read(string* data) {
        cout << "Data about incoming time of flights" << endl;
        rapidjson::Document document;
        document.Parse(data->c_str());
        const rapidjson::Value& results = document["AirportBoardsResult"];
        const rapidjson::Value& val = results["arrivals"];
        const rapidjson::Value& arr = val["flights"];
        assert(arr.IsArray());
        if (arr.Size() == 0) {
            cout << "Error , please try again " << endl;
        }
        else
        {
            for (rapidjson::SizeType i = 0; i < arr.Size(); i++)
            {
                cout << " Data about incoming time of " << i + 1 << " flight :" << endl;
                const rapidjson::Value& val = arr[i];
                const rapidjson::Value& time = val["actual_arrival_time"];
                cout << "Arrival time : " << time["time"].GetString() << endl;
                cout << "Arrival day : " << time["dow"].GetString() << endl;
                cout << "Date of incoming : " << time["date"].GetString() << endl;
                cout << endl;
            }
        }
    }
};

static size_t WriteCallBack(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
int main() {
    CURL* curl;
    string readBuffer;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "http://profile236:a6e664089b67c7786ba57bcf9a77ee935ec004ff@flightxml.flightaware.com/json/FlightXML3/AirportBoards?airport_code=UKKK&type=arrivals&howMany=2&offset=0");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    Reader* infoReader = new InfoReader();
    Reader* timeReader = new TimeReader();
    cout << "Info : " << endl;
    infoReader->read(&readBuffer);
    cout << "Time : " << endl;
    timeReader->read(&readBuffer);
    return 0;
}