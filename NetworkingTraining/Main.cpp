#include "Client/Client.h"
#include <Header Files/DataFrame.h>
#include <iostream>

using namespace std;


int main()
{
    string dataToSend = "Angelo is here";

    Client c1("127.0.0.1", 5000);
    Client c2("127.0.0.1", 5000);

    DataFrame df1(1);
    DataFrame df2(2);
    df1.setSendString("User 1 sending data");
    df1.setxPosRandom(34);
    df1.setyPosRandom(56);
    df2.setSendString("User 2 sending data");
    df2.setxPosRandom(35);
    df2.setyPosRandom(57);

    try
    {
        string temp = df1.Pack();
        string temp2 = df2.Pack();

        for (int x = 0; x < 100; x++)
        {
            try
            {
                Client c1("127.0.0.1", 5000);
                Client c2("127.0.0.1", 5000);

                c1.Connect();
                cout << "Client data to send, length: " << temp.size() << endl;
                cout << "Client data to send: " << temp << endl;
                c1.SendData(temp.c_str(), temp.size());
                c1.Disconnect();

                c2.Connect();
                cout << "Client2 data to send, length: " << temp2.size() << endl;
                cout << "Client2 data to send: " << temp2 << endl;
                c2.SendData(temp2.c_str(), temp2.size());
                c2.Disconnect();
            }
            catch (std::exception e)
            {
                cout << e.what() << endl;
                break;
            }
        }
    }
    catch (std::exception ex)
    {
        cout << ex.what() << endl;
    }
}
