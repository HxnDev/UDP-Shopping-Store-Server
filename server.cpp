// server program for udp connection 
#include <stdio.h> 
#include <string.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <iostream>
#include <fstream>
#define PORT 5000 
#define MAXLINE 1000 

using namespace std;
// Driver code 

string cptrtos(char* toConvert) 
{
	string str;
	for (int i = 0; toConvert[i]; i++)
		str += toConvert[i];

	return str;
}

void maintinLog(string a)               //Stores the time and date when client accessed the server
{
    fstream obj("accesslog.txt",ios::app);
    time_t t;

    struct tm *tt;
    time(&t);
    tt=localtime(&t);

    obj<<a<<"\t\t"<<asctime(tt);
    obj.close();
}

int main() 
{ 
    char stat[2];       //stat 0 = wait, stat 1 = execute
    string names[3];
	char buffer[100]; 
	char message[50] = "Enter your name =";
    char name[50];
	int listenfd, len; 
    int maxclients = 4;
    int authorization[maxclients-1];
    struct sockaddr_in cliaddr[maxclients]; 
	struct sockaddr_in servaddr; 
	bzero(&servaddr, sizeof(servaddr)); 

	// Create a UDP Socket 
	listenfd = socket(AF_INET, SOCK_DGRAM, 0);		 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_family = AF_INET; 

	// bind server address to socket descriptor 
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
	

    for(int i=0; i<maxclients; i++)
    {
        //Connection with Client 1
        len = sizeof(cliaddr[i]);
        int n = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr[i],(socklen_t*)&len); 	//Recieving from 1
        puts(buffer);
        names[i] = cptrtos(buffer);
    }

    while(1)
    {
    for (int i=0; i<maxclients; i++)     
    {
        maintinLog(names[i]);
        cout<<"Server access recorded"<<endl;

        string stat;
        for (int j = 0; j < maxclients; j++) 
        {
            if (j != i) 
                stat = "0";     //Wait
            else
                stat = "1";     //Execute
            
           sendto(listenfd, stat.c_str(), MAXLINE, 0, (struct sockaddr*)&cliaddr[j], sizeof(cliaddr[j])); 
        }

        recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr[i],(socklen_t*)&len); //receiving the choice
        cout<<"The choice selected is = "<<buffer<<endl;
        int option = atoi(buffer);

        if (option == 1)
        { 
            recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr,(socklen_t*)&len); //receiving the invoice number
            string invoice = cptrtos(buffer);
            cout << "Invoice number enetered = "<<invoice << endl;

            string s,details;
            fstream obj("Assignment 1.txt", ios::in);

            string allDetails;

            if (obj) {
                while(!obj.eof())
                {
                    obj>>s;
                    //cout << "first word : " << s << endl;
                    if(invoice == s)
                    {
                        getline(obj,details);
                        allDetails += details;
                        allDetails.append("\n");
                    }
                    else
                    {
                        char c = obj.get();

                        while (!obj.eof()) {
                            c = obj.get();

                            if (c == 10)
                                break;
                        }
                    }
                }
                obj.close();
                sendto(listenfd, allDetails.c_str(), MAXLINE, 0, (struct sockaddr*)&cliaddr[i], sizeof(cliaddr[i]));
            }

            for (int j = 0; j<maxclients; j++)
            {
                if (j!= i)
                {
                    sendto(listenfd, "0", MAXLINE, 0, (struct sockaddr*)&cliaddr[j], sizeof(cliaddr[j]));
                }

            }
        }

        else if (option == 2)
        {
            recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr,(socklen_t*)&len); //receiving the invoice number
            string invoice = cptrtos(buffer);
            cout << "Invoice number enetered = "<<invoice << endl;

            int k = 0;
            for (int j = 0; j<maxclients; j++)
            {
                if (j!= i)
                {
                    sendto(listenfd, "1", MAXLINE, 0, (struct sockaddr*)&cliaddr[j], sizeof(cliaddr[j]));

                    sendto(listenfd, "A client wants to delete a record. Do you authorize it?", MAXLINE, 0, (struct sockaddr*)&cliaddr[j], sizeof(cliaddr[j]));

                    char buff[10];
                    recvfrom(listenfd, buff, sizeof(buff),0, (struct sockaddr*)&cliaddr[j],(socklen_t*)&len); //receiving Authorization
                    authorization[k] = atoi(buff);
                    k++;
                }

            }

            bool auth = true;

            for (int l = 0; l < k; l++) 
            {
                if (authorization[l] == 2)
                    auth = false;
            }

            if (auth) 
            {
                sendto(listenfd, "You were Authorized", MAXLINE, 0, (struct sockaddr*)&cliaddr[i], sizeof(cliaddr[i]));

                // delete functionality
                ifstream in("Assignment 1.txt");
                if (in) 
                {
                    string str;
                    ofstream out("new.txt");
                    
                    getline(in, str);
                    out << str;
                    getline(in,str);
                    out << str;

                    if (out) 
                    {
                        while(!in.eof())
                        {
                            string line, s;
                            in>>s;
                            getline(in,str);
                            //cout << "first word : " << s << endl;
                            if(invoice != s)
                            {
                                line += s;
                                line += str;
                                out << line << endl;
                            }
                        }

                        in.close();
                        out.close();
                    }

                    const char * file = "Assignment 1.txt";
                    remove(file);
                    rename("new.txt", file);
                }

                for (int j = 0; j<maxclients; j++)
                {
                    if (j!= i)
                    {
                        sendto(listenfd, "A file was deleted", MAXLINE, 0, (struct sockaddr*)&cliaddr[j], sizeof(cliaddr[j]));
                    }

                }
            }

            else
            {
                sendto(listenfd, "You weren't Authorized", MAXLINE, 0, (struct sockaddr*)&cliaddr[i], sizeof(cliaddr[i]));

                for (int j = 0; j<maxclients; j++)
                {
                    if (j!= i)
                    {
                        sendto(listenfd, "", MAXLINE, 0, (struct sockaddr*)&cliaddr[j], sizeof(cliaddr[j]));
                    }

                }
            }
            
        }

        else if (option == 3)
        {
            
            recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr[i],(socklen_t*)&len); //receiving the invoice number
            string invoice = cptrtos(buffer);
            
            sendto(listenfd, "Enter the Description of the product = ", MAXLINE, 0, (struct sockaddr*)&cliaddr[i], sizeof(cliaddr[i])); //Demanding description
            recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr[i],(socklen_t*)&len); //receiving the description
            string desc = cptrtos(buffer);
            
            sendto(listenfd, "Enter the Quantity of the product = ", MAXLINE, 0, (struct sockaddr*)&cliaddr[i], sizeof(cliaddr[i])); //Demanding quantity
            recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr[i],(socklen_t*)&len); //receiving the quantity
            string quan = cptrtos(buffer);
            
            sendto(listenfd, "Enter the Price of the product in $ = ", MAXLINE, 0, (struct sockaddr*)&cliaddr[i], sizeof(cliaddr[i])); //Demanding price in $
            recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr[i],(socklen_t*)&len); //receiving the price in $
            string price = cptrtos(buffer);
            
            sendto(listenfd, "Enter your 5-digit Customer Id = ", MAXLINE, 0, (struct sockaddr*)&cliaddr[i], sizeof(cliaddr[i])); //Demanding customer id
            recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr[i],(socklen_t*)&len); //receiving the customer id
            string id = cptrtos(buffer);
            
            sendto(listenfd, "Enter your country = ", MAXLINE, 0, (struct sockaddr*)&cliaddr[i], sizeof(cliaddr[i])); //Demanding country
            recvfrom(listenfd, buffer, sizeof(buffer),0, (struct sockaddr*)&cliaddr[i],(socklen_t*)&len); //receiving the country
            string country = cptrtos(buffer);
            
            fstream obj("Assignment 1.txt", ios::app);
            obj<< invoice<<"\t"<<desc<<"\t"<<quan<<"\t"<<price<<"\t"<<id<<"\t"<<country<<endl;
            obj.close();


            for (int k=0; k<maxclients; k++)
            {
                if (k != i)
                    sendto(listenfd, "2", MAXLINE, 0, (struct sockaddr*)&cliaddr[k], sizeof(cliaddr[k])); //Send message
            }

            for (int k=0; k<maxclients; k++)
            {
                sendto(listenfd, "A new entry was added ", MAXLINE, 0, (struct sockaddr*)&cliaddr[k], sizeof(cliaddr[k])); //Send message
            }
        }

        else if (option == 4)
        {
            string s;
            fstream obj("accesslog.txt", ios::in);

            while(!obj.eof())
            {
                getline(obj, s);
                cout << s << endl;
            }
            cout<<endl;
            obj.close();

            for (int j = 0; j<maxclients; j++)
            {
                if (j!= i)
                {
                    sendto(listenfd, "0", MAXLINE, 0, (struct sockaddr*)&cliaddr[j], sizeof(cliaddr[j]));
                }

            }
        }
        else
        {
            cout<<"Invalid Option selected"<<endl;
        }
           
        

    }
    }
	close(listenfd);
} 
