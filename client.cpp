// udp client driver program 
#include <stdio.h> 
#include <strings.h> 
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include<netinet/in.h> 
#include<unistd.h> 
#include<stdlib.h> 
#include <iostream>

#define PORT 5000 
#define MAXLINE 1000 

using namespace std;

// Driver code 
int main() 
{ 
	char buffer[1000]; 
	char message[50] = "Client connecting to server."; 
	int sockfd, n; 
	struct sockaddr_in servaddr; 
    char option[2];
    char name[50];
	string invoice;
	
	// clear servaddr 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_family = AF_INET; 
	
	// create datagram socket 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	
	// connect to server 
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	{ 
		printf("\n Error : Connect Failed \n"); 
		exit(0); 
	} 

    //Client enters his name
    cout<<"Enter your name = "; 
    cin>>name;
    sendto(sockfd, name , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));       //Sending his name

	while(1)
	{
		recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n); //Recieving status
		int activitystatus = atoi(buffer);

		if (activitystatus == 1)	//Execute
		{
			char choice[2];
			cout<<"The menu of options are as follows: "<<endl<<"1: Read Record \n2: Delete Record \n3: Add Record \n4: Request Log\n";   //Displaying the list of options
			cin>> choice;
			int opt = atoi(choice);

			sendto(sockfd, choice , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));       //Sending his choice

			if (opt == 1)		//Reading
			{
				cout<<"Enter the invoice number = ";
				cin>>invoice;
				sendto(sockfd, invoice.c_str() , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));       //Sending his invoice number
				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n); //Recieving invoices
				cout<<"The records corresponding to the invoice number "<<invoice<<" are as follows: "<<endl;
				cout<<buffer<<endl;
			}

			else if (opt == 2)
			{
				cout<<"Enter the invoice number = ";
				cin>>invoice;
				sendto(sockfd, invoice.c_str() , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));       //Sending his invoice number

				char msg[100];
				recvfrom(sockfd, msg, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
				cout << msg << endl;
			}

			else if (opt == 3)
			{
				char invoice[10],description[50],quantity[5],price[10], id[6], country[20];
				cout<<"Enter invoice number = ";
				cin>> invoice;
				sendto(sockfd, invoice , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));    //Sending invoice

				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n); //receiving description demand
				cout<<buffer;
				cin>>description;
				sendto(sockfd, description , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));    //Sending description

				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n); //receiving quantity demand
				cout<<buffer;
				cin>>quantity;
				sendto(sockfd, quantity , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));    //Sending quantity

				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n); //receiving price in $ demand
				cout<<buffer;
				cin>>price;
				sendto(sockfd, price , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));    //Sending price in $

				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n); //receiving customer id demand
				cout<<buffer;
				cin>>id;
				sendto(sockfd, id , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));    //Sending customer id

				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n); //receiving country demand
				cout<<buffer;
				cin>>country;
				sendto(sockfd, country , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));    //Sending country

			}

		}

		else if (activitystatus == 0)
		{
			recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n); //receiving Authentication Message
			int auth = atoi(buffer);

			cout << auth << endl;

			if (auth == 0)
			{
				cout << "Idle" << endl;
			}

			else if (auth == 1)
			{
				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
				cout << buffer << endl;

				cout<<"Enter 1 to Authenticate and 2 to Decline"<<endl;
				string choice;
				cin>>choice;
				sendto(sockfd, choice.c_str() , MAXLINE, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));    //Sending Authentication/Declining

				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
				cout << buffer << endl;

			}

			else if (auth == 2)
			{
				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&servaddr, (socklen_t*)&n);
				cout << buffer << endl;
			}

		}

	}
    
	close(sockfd); 
}
