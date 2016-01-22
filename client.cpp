#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include <sys/times.h>


using namespace std;
const unsigned MAXBUFLEN = 512;
int sockfd;
int sockfd1,sockfd2;
int cflag=0;
int check=0;
char myname[MAXBUFLEN];
unsigned port = 7200;
int global=1;
int timectr=1;
/* timing variables */
struct tms buffer;        /* used for timing */
clock_t start, finish, itime=600;
clock_t tleft=600,tleftother=600;
char mygameport[MAXBUFLEN];

struct list
{
	char rname[MAXBUFLEN];
	char rport[MAXBUFLEN];
};

struct game
{
	char player1[MAXBUFLEN],player2[MAXBUFLEN];
	char a[3],b[3],c[3];
	int chance;
};



list templist1;
list templist2;

vector <list> rlist;
vector <list> slist;

game glist;

vector <list> templist;
vector <string> obslist;

void *writeto_clientserver(void *);
void *readingfrom_clientserver(void *);
void showgame ();
int makemove(char []);


int makemove(char move[2])
{

	int position = (move[1]-'0')-1;
	char ch= move[0];
	clock_t timeover;
	int val;

	if(ch=='a' && position<3 && glist.a[position]=='*'){val=1;}
	else if(ch=='b' && position<3 && glist.b[position]=='*'){val=2;}
	else if(ch=='c' && position<3 && glist.c[position]=='*'){val=3;}
	else {val=4;}

	switch(val)
	{
		case 1:
			{
				if(glist.chance%2==0)
				{
					finish = times(&buffer);
					timeover = (finish - start);
					tleft-=(timeover/100);

					glist.a[position]='X';

				}
				else if(glist.chance%2!=0)
				{
					finish = times(&buffer);
					timeover = (finish - start);
					tleft-=(timeover/100);
					glist.a[position]='O';

				}

				break;
			}
		case 2:
			{
				if(glist.chance%2==0)
				{

					finish = times(&buffer);
					timeover = (finish - start);

					tleft-=(timeover/100);
					glist.b[position]='X';

				}
				else if(glist.chance%2!=0)
				{

					finish = times(&buffer);
					timeover = (finish - start);
					tleft-=(timeover/100);
					glist.b[position]='O';

				}

				break;
			}

		case 3:
			{
				if(glist.chance%2==0)
				{

					finish = times(&buffer);
					timeover = (finish - start);
					tleft-=(timeover/100);
					glist.c[position]='X';

				}
				else if(glist.chance%2!=0)
				{

					finish = times(&buffer);
					timeover = (finish - start);

					tleft-=(timeover/100);
					glist.c[position]='O';

				}

				break;
			}
		case 4:
			{
				cout<<"WRONG MOVE !!!\n";
				break;
			}
	}

	if(val!=4)
	{
		glist.chance++;

		if(tleft>0)
		{

			if((glist.a[0]==glist.b[0]&& glist.b[0]==glist.c[0] && glist.b[0] != '*')||
					(glist.a[1]==glist.b[1] && glist.b[1]==glist.c[1] && glist.b[1] != '*')||
					(glist.a[2]==glist.b[2] && glist.b[2]==glist.c[2] && glist.b[2] != '*')||
					(glist.a[0]==glist.a[1] && glist.a[1]==glist.a[2] && glist.a[1] != '*')||
					(glist.b[0]==glist.b[1] && glist.b[1]==glist.b[2] && glist.b[1] != '*')||
					(glist.c[0]==glist.c[1] && glist.c[1]==glist.c[2] && glist.c[1] != '*')||
					(glist.a[2]==glist.b[1] && glist.b[1]==glist.c[0] && glist.b[1] != '*')||
					(glist.a[0]==glist.b[1] && glist.b[1]==glist.c[2] && glist.b[1] != '*'))
			{
				if(glist.chance%2==0)
				{
					cout<<"\n O WINS !!!! \n";
					val=100;
					//clear gamelist, condition for the observers;

				}
				else
				{

					cout<<"\n X WINS !!!! \n";
					val=100;
					//clear gamelist, condition for the observers;
				}
			}
			else if(glist.chance >= 9)
			{
				cout<<"\n Game Draw!!!! \n";
				val=100;
				//clear gamelist, condition for the observers;
			}

		}
		else
		{
			if(glist.chance%2==0)
			{
				cout<<"\n X WINS by time !!!! \n";
				val=200;
				//clear gamelist, condition for the observers;

			}
			else
			{

				cout<<"\n O WINS by time!!!! \n";
				val=200;
				//clear gamelist, condition for the observers;
			}
		}

	}
if (strcmp(glist.player2,myname)==0 && (val==100 || val==200))
{
/////////////////////////////////////
        if(!obslist.empty())
        {
                char ob1[512],ob2[512],ob[512];
                int obssock;
                int n;
                for(int i=0;i<obslist.size();i++)
                {
                        sscanf(obslist[i].c_str(),"%s %s",ob1,ob2);
                        obssock=atoi(ob2);
			if(glist.chance%2==0 && val == 100)
			{sprintf(ob,"O wins!!!\n\n");}
			else if(glist.chance%2==0 && val == 200)
			{sprintf(ob,"X wins!!!\n\n");}
			else if(glist.chance%2!=0 && val == 100)
			{sprintf(ob,"X wins!!!\n\n");}
			else if(glist.chance%2!=0 && val == 200)
			{sprintf(ob,"O wins!!!\n\n");}
                        write(obssock,ob,strlen(ob));
                }
        }

/////////////////////////////////////


}
	return val;
}






void showgame()
{
	int i;

	if(strcmp(glist.player1,myname)==0)
	{
		printf("\nBlack: %s\t%d\tWhite: %s\t%d\n",glist.player1,tleft,glist.player2,tleftother);
	}

	else
	{
		printf("\nBlack: %s\t%d\tWhite: %s\t%d\n",glist.player1,tleftother,glist.player2,tleft);
	}

	cout<<"\t1\t2\t3\n";

	cout<<"A\t";
	for(i=0;i<3;i++)
	{
		cout<<glist.a[i];
		cout<<"\t";
	}
	cout<<"\n";

	cout<<"B\t";
	for(i=0;i<3;i++)
	{
		cout<<glist.b[i];
		cout<<"\t";
	}
	cout<<"\n";

	cout<<"C\t";
	for(i=0;i<3;i++)
	{
		cout<<glist.c[i];
		cout<<"\t";
	}
	cout<<"\n";

	if(!obslist.empty())
	{
		char ob1[512],ob2[512],ob[512];
		int obssock;
		int n;
		for(int i=0;i<obslist.size();i++)
		{
			sscanf(obslist[i].c_str(),"%s %s",ob1,ob2);
			obssock=atoi(ob2);
			sprintf(ob,"Black: %s\tWhite: %s\n\t1\t2\t3\nA\t%c\t%c\t%c\nB\t%c\t%c\t%c\nC\t%c\t%c\t%c\n",glist.player1,glist.player2,glist.a[0],glist.a[1],glist.a[2],glist.b[0],glist.b[1],glist.b[2],glist.c[0],glist.c[1],glist.c[2]);
			write(obssock,ob,strlen(ob));
		}
	}
}

void *observe(void *arg)
{
	char buf[512],buffer[512];
	int n;
	pthread_detach(pthread_self());
	while ((n = read(sockfd2, buf, MAXBUFLEN)) > 0)
	{
		cout<<buf<<endl;
		sscanf(buf,"%s",buffer);
		if(strcmp(buffer,myname)==0){break;}
		for(int j=0; j<512 ; j++){buf[j] = '\0';}
	}

	close(sockfd2);
}

void *replyto_observer(void *arg)
{
	int obsfd = *((int *)arg);
	char buf[512],buf1[512],name[512];
	char ob1[512],ob2[512],ob[512];
	int n;
	pthread_detach(pthread_self());
//	cout<<"While begins";
	while ((n = read(obsfd, buf, MAXBUFLEN)) > 0)
	{
		sscanf(buf,"%s %s",name,buf1);
		if(strcmp(buf1,"unobserve")==0 && (!obslist.empty()))
		{
			for(int j=0; j<512 ; j++){buf1[j] = '\0';}
			sprintf(buf1,"%s %d",name,obsfd);
			write(obsfd,buf1,strlen(buf1));
			//	cout<<"obslist before deletion"<<endl;
			//	for(int i=0;i<obslist.size();i++)
			//	{cout<<obslist[i]<<endl;}
			obslist.erase(find(obslist.begin(), obslist.end(),buf1));
			//	cout<<"obslist after deletion"<<endl;
			//	for(int i=0;i<obslist.size();i++)
			//	{cout<<obslist[i]<<endl;}
			close(obsfd);
		}
		else if(strcmp(buf1,"kibitz")==0 || strcmp(buf1,"'")==0)
		{
			////////////////////////////////////////
			if(!obslist.empty())
			{
				//char ob1[512],ob2[512],ob[512];
				int obssock;

				sprintf(ob,"From %s",buf);
				int n;
				for(int i=0;i<obslist.size();i++)
				{
					sscanf(obslist[i].c_str(),"%s %s",ob1,ob2);
					obssock=atoi(ob2);
					write(obssock,ob,strlen(ob));
				}
			}

			//////////////////////////////////////	
		}
		else if(strcmp(buf,"ref")==0)
		{	
			sprintf(ob,"Black: %s\tWhite: %s\n\t1\t2\t3\nA\t%c\t%c\t%c\nB\t%c\t%c\t%c\nC\t%c\t%c\t%c\n",glist.player1,glist.player2,glist.a[0],glist.a[1],glist.a[2],glist.b[0],glist.b[1],glist.b[2],glist.c[0],glist.c[1],glist.c[2]);
			write(obsfd,ob,strlen(ob));


		}
		for(int j=0; j<512 ; j++){buf[j] = '\0';buf1[j]='\0';ob[j]='\0';}
	}
	//cout<<"While end";
}

void *replyto_client(void *arg) {
	//int sockfd1;
	char ol[MAXBUFLEN],ol1[MAXBUFLEN], ol2[MAXBUFLEN],ol3[MAXBUFLEN],ol4[512];
	char buf[MAXBUFLEN],linetosend[MAXBUFLEN];
	ssize_t n;
	int k;
	sockfd1 = *((int *)arg);
	free(arg);
//	cout<<"4. equal to 3 == sockfd1-->"<<sockfd1<<"XXXXXX\n";

	pthread_detach(pthread_self());
//	cout<<"I was in client connection"<<"\n";
	while ((n = read(sockfd1, buf, MAXBUFLEN)) > 0) {
		buf[n] = '\0';
//		cout<<"Reading from other client on sockfd1-->"<<sockfd1<<"XXXXXX\n";
		cout << buf << endl;
		////////////////////////////////////
		sscanf(buf,"%s %s %s %s",ol1,ol2,ol3,ol4);

		if(strcmp(ol1,"move")==0)
		{
			//if(timectr==1)
			start=times(&buffer);

			tleftother=atoi(ol4);
			k=makemove(ol2);
			start=times(&buffer);

			showgame();
			glist.chance=atoi(ol3);
//			cout<<"Move made by other player"<<ol2<<"---"<<ol3;	
			if (k==100)
			{

				cflag=0;
				sprintf(linetosend,"GAMEOVER %d %s %s %d",port,glist.player1,glist.player2,0);
				write(sockfd,linetosend,strlen(linetosend));
				memset(&glist, 0, sizeof(glist));
				obslist.clear();
				global=1;
				break;
				//close(sockfd1);
				//	exit(0);

			}

		}
		/////////////////////////////////
		/*	else if(strcmp(ol1,"TIMEOVER")==0)
			{
			cout<<"You win by TIME\n";
			cflag=0;
			sprintf(linetosend,"GAMEOVER %d %s %s %d",port,glist.player1,glist.player2,1);
			write(sockfd,linetosend,strlen(linetosend));
			memset(&glist, 0, sizeof(glist));
			obslist.clear();
			global=1;
			break;

			}*/

		else if(strcmp(ol1,"IQUIT")==0 || strcmp(ol1,"TIMEOVER")==0)
		{
			cflag=0;
			if(strcmp(ol1,"IQUIT")==0)
			{cout<<"You WIN...!!! Other player resigned\n";}
			else if(strcmp(ol1,"TIMEOVER")==0)
			{cout<<"You WIN...!!! By TIME\n";}

			//loop for observers
			if(!obslist.empty())
			{
				char ob1[512],ob2[512],ob[512];
				int obssock;
				int n;
				for(int i=0;i<obslist.size();i++)
				{
					sscanf(obslist[i].c_str(),"%s %s",ob1,ob2);
					obssock=atoi(ob2);
					sprintf(ob,"%s WON !!!",glist.player2);
					write(obssock,ob,strlen(ob));
					close(obssock);
				}
			}
			sprintf(linetosend,"GAMEOVER %d %s %s %d",port,glist.player1,glist.player2,1);
			write(sockfd,linetosend,strlen(linetosend));
			memset(&glist, 0, sizeof(glist));
			obslist.clear();
			global=1;
			//	close(sockfd1);
			break;
		}

		for(int i=0;i<512;i++){linetosend[i]='\0';}
		//write(sockfd1, buf, strlen(buf));
	}
	if (n == 0) {
		cflag=0;
		memset(&glist, 0, sizeof(glist));
		//cout << "client closed replyto_client" << endl;
	} else {
		cflag=0;
		memset(&glist, 0, sizeof(glist));
	//	cout << "something wrong replyto_client" << endl;
	}
	close(sockfd1);
	return(NULL);
}


void *readfrom_server(void *arg) {
	int n;
	char ol[MAXBUFLEN],ol1[MAXBUFLEN], ol2[MAXBUFLEN],ol3[MAXBUFLEN];
	char buf[MAXBUFLEN];
	pthread_detach(pthread_self());
	while (n = read(sockfd, buf, MAXBUFLEN)>0)
	{
		/*	n = read(sockfd, buf, MAXBUFLEN);
			if (n <= 0) {
			if (n == 0) {
			cout << "server closed" << endl;
			} else {
			cout << "something wrong" << endl;
			}
			close(sockfd);
		// we directly exit the whole process.
		exit(1);
		}
		buf[n] = '\0';
		 */	
		cout << buf<<endl;
		sscanf(buf,"%s %s %s",ol1,ol2,ol3);
		if(strcmp(ol1,"connect")==0)
		{
			strcpy(templist1.rname,ol2);
			strcpy(templist1.rport,ol3);
			slist.push_back(templist1);
		}
		else if(strcmp(ol1,"Observe")==0)
		{
			check=0;
			int rv, flag;
			ssize_t n;
			struct addrinfo hints, *res, *ressave;
			pthread_t tid;
			char pno[MAXBUFLEN];


			bzero(&hints, sizeof(struct addrinfo));
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;

			if ((rv = getaddrinfo("0.0.0.0", ol2, &hints, &res)) != 0) {
				cout << "getaddrinfo wrong: " << gai_strerror(rv) << endl;
				exit(1);
			}

			ressave = res;
			flag = 0;
			do {
				sockfd2 = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
				//cout<<"Sockfd1 when requesting-->"<<sockfd1<<"XXXXXX";
				if (sockfd2 < 0)
					continue;
				if (connect(sockfd2, res->ai_addr, res->ai_addrlen) == 0) {
					flag = 1;
					//	cflag=1;
					break;
				}
				close(sockfd2);
			} while ((res = res->ai_next) != NULL);
			freeaddrinfo(ressave);

			if (flag == 0) {
				fprintf(stderr, "cannot connect\n");
				exit(1);
			}
			//cout<<"5. will now connect to new sockfd1-->"<<sockfd2<<"XXXXXX\n";
			write(sockfd2,myname,strlen(myname));
			pthread_create(&tid, NULL, &observe, NULL);
			// pthread_create(&tid, NULL, &readingfrom_clientserver, NULL);
		}
		else if(strcmp(ol1,"Wants")==0)
		{
			strcpy(templist1.rname,ol2);
			strcpy(templist1.rport,ol3);
			rlist.push_back(templist1);
		}
		for(int i=0;i<MAXBUFLEN;i++){buf[i]='\0';}
	}
	if (n == 0) {
		cout <<"Server closed\n"<< endl;
	} else {
		cout <<".Server closed.\n"<< endl;
	}
	close(sockfd);
	return(NULL);
}


void *readingfrom_clientserver(void *arg) {
	int n,k;
	char ol[MAXBUFLEN],ol1[MAXBUFLEN], ol2[MAXBUFLEN],ol3[MAXBUFLEN],ol4[512];
	char buf[MAXBUFLEN],linetosend[MAXBUFLEN];
	pthread_detach(pthread_self());

	while (n = read(sockfd1, buf, MAXBUFLEN)>0)
	{
		//	n = read(sockfd1, buf, MAXBUFLEN);
		/*	if (n <= 0) 
			{
			if (n == 0) {
			cout << "server closed" << endl;
			} 
			else {
			cout << "something wrong" << endl;
			}
		//	close(sockfd1);
		// we directly exit the whole process.
		exit(1);
		}*/
		//buf[n] = '\0';
		cout << buf << endl;
		sscanf(buf,"%s %s %s %s",ol1,ol2,ol3,ol4);

		if(strcmp(ol1,"move")==0)
		{
			start=times(&buffer);
			k=makemove(ol2);

			start=times(&buffer);

			tleftother=atoi(ol4);
			showgame();
			glist.chance=atoi(ol3);
			//cout<<"Move made by other player"<<ol2;	
			if (k==100)
			{
				//close(sockfd1);
				cflag=0;
				memset(&glist, 0, sizeof(glist));
				obslist.clear();
				break;
				//exit(0);
			}

		}

		else if(strcmp(ol1,"IQUIT")==0 || strcmp(ol1,"TIMEOVER")==0)
		{
			cflag=0;
			if(strcmp(ol1,"IQUIT")==0)
			{cout<<"You WIN...!!! Other player resigned\n";}
			else if(strcmp(ol1,"TIMEOVER")==0)
			{cout<<"You WIN...!!! By TIME\n";}

			memset(&glist, 0, sizeof(glist));
			obslist.clear();
			break;
		}
		for(int i=0;i<512;i++){buf[i]='\0';}
	}
	if (n == 0) {
		cflag=0;
		memset(&glist, 0, sizeof(glist));
		//cout << "client closed readingfrom_CS" << endl;
	} else {
		cflag=0;
		memset(&glist, 0, sizeof(glist));
		//cout << "something wrong readingfrom_CS" << endl;
	}
	close(sockfd1);
	return(NULL);
}


/*
   void *writeto_clientserver(void *arg) 
   {

   pthread_detach(pthread_self());
   string oneline;
   cout<<"6 should be ==5. sockfd1-->"<<sockfd1<<"XXXXXX";
   while (getline(cin, oneline)) {
   cout<<"Inside talk_client's getline-->"<<sockfd1<<"XXXXXX\n";
   if (oneline == "quit") {
   close(sockfd1);
   break;
   }
   else {
   write(sockfd1, oneline.c_str(), oneline.length());
   }
   }
   }
 */
void *writeto_server(void *arg) 
{
	int check=0;
	char ol[MAXBUFLEN],ol1[MAXBUFLEN], ol2[MAXBUFLEN],ol3[MAXBUFLEN];
	char buf[MAXBUFLEN],linetosend[MAXBUFLEN];
	pthread_detach(pthread_self());
	string oneline;
cout<<"<"<<myname<<":>";
	while (getline(cin, oneline)) 
	{
		strcpy(buf,oneline.c_str());
		sscanf(buf,"%s %s",ol1,ol2);

		int sss;

		if(strcmp(buf,"quit")==0 || strcmp(buf,"exit")==0){sss=1;}
		else if(strcmp(ol1,"match")==0){sss=2;}
		else if((oneline=="a1" ||oneline=="a2" ||oneline=="a3" ||oneline=="b1" ||oneline=="b2" ||oneline=="b3" ||oneline=="c1" ||oneline=="c2" ||oneline=="c3")&& cflag==1){sss=3;}
		else if(oneline=="unobserve"){sss=4;}
		else if((strcmp(ol1,"kibitz")==0) || (strcmp(ol1,"'")==0)){sss=5;}
		else if(oneline=="resign"){sss=6;}
		else if(oneline=="ref"){sss=8;}
		else if(strcmp(buf,"stats")==0){sss=9;}
		else if(strcmp(buf,"mail")==0){sss=10;}
		else if(strcmp(buf,"help")==0 || strcmp(buf,"?")==0){sss=11;}
		else{sss=7;}

		switch(sss)
		{
			case 1:
				//if (oneline == "quit")
				{
					if(cflag==1)
					{
						cflag=0;
						sprintf(linetosend,"IQUIT");
						write(sockfd1,linetosend,strlen(linetosend));
						close(sockfd1);

						if(strcmp(glist.player2,myname)==0)
						{
							char ob1[512],ob2[512],ob[512];
							int obssock;
							int n;
							for(int i=0;i<obslist.size();i++)
							{
								sscanf(obslist[i].c_str(),"%s %s",ob1,ob2);
								obssock=atoi(ob2);
								sprintf(ob,"%s WON !!!",glist.player1);
								write(obssock,ob,strlen(ob));
								close(obssock);
							}
							sprintf(linetosend,"GAMEOVER %d %s %s",port,glist.player1,glist.player2,0);
							write(sockfd,linetosend,strlen(linetosend));
						}

						memset(&glist, 0, sizeof(glist));
						obslist.clear();
						global=1;

					}
					if(sockfd2!=0)
					{sprintf(linetosend,"%s unobserve",myname);
					write(sockfd2,linetosend,strlen(linetosend));
					cout<<endl;
					}
					write(sockfd,oneline.c_str(),oneline.length());
					close(sockfd);
					exit(NULL);
					break;
				}

			case 2:
				//else if(strcmp(ol1,"match")==0)
				{
					char gametime[512];

					if(sscanf(buf,"%s %s %s %s",ol1,ol2,ol3,ol)==4)
					{
						tleft=atoi(ol);
					}
					else{tleft=itime;}

					if(!rlist.empty())
					{
						check=0;
						for(int i=0; i<=rlist.size();i++)
						{
							if(strcmp(rlist[i].rname,ol2)==0)
							{strcpy(ol3,rlist[i].rport);
								//cout<<"XXXX"<<ol2<<"XX"<<ol3<<endl;
								check=1;
								strcpy(mygameport,ol3);

								strcpy(glist.player1,myname);
								strcpy(glist.player2,ol2);
								glist.chance=0;

								break;
							}
							else
							{
								cout<<"NOT FOUND\n";
							}
						}
					}
					else 
					{
					//cout<<"rlist is empty, but now wont be empty";
					}
					if(check!=1)
					{
						write(sockfd, oneline.c_str(), oneline.length());
					}

					/////////////////////////////
					if(check==1)
					{
						check=0;
						//cout <<"Here----"<<ol1 << endl;
						check=0;
						int rv, flag;
						ssize_t n;
						struct addrinfo hints, *res, *ressave;
						pthread_t tid;
						char pno[MAXBUFLEN];


						bzero(&hints, sizeof(struct addrinfo));
						hints.ai_family = AF_UNSPEC;
						hints.ai_socktype = SOCK_STREAM;

						if ((rv = getaddrinfo("0.0.0.0", ol3, &hints, &res)) != 0) {
							cout << "getaddrinfo wrong: " << gai_strerror(rv) << endl;
							exit(1);
						}

						ressave = res;
						flag = 0;
						do {
							sockfd1 = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
							//cout<<"Sockfd1 when requesting-->"<<sockfd1<<"XXXXXX";
							if (sockfd1 < 0)
								continue;
							if (connect(sockfd1, res->ai_addr, res->ai_addrlen) == 0) {
								flag = 1;
								cflag=1;
								break;
							}
							close(sockfd1);
						} while ((res = res->ai_next) != NULL);
						freeaddrinfo(ressave);

						if (flag == 0) {
							fprintf(stderr, "cannot connect\n");
							exit(1);
						}
						//cout<<"5. will now connect to new sockfd1-->"<<sockfd1<<"XXXXXX\n";

						//put gamelist here

						//glist.player

						//cout<<"Uparwala-->>"<<glist.player1<<"X"<<glist.player2<<"X"<<glist.chance<<"\n";
						//empty the rlist
						if(!rlist.empty())
						{
							//        rlist.rname.erase(find(rlist.rname.begin(), rlist.rname.end(),glist.player2));

							for(int i=0; i<=rlist.size();i++)
							{
								if(strcmp(rlist[i].rname,glist.player2)==0)
								{
									rlist.erase(rlist.begin()+i);
									break;
								}
								else
								{
									cout<<"NOT FOUND\n";
								}
							}



						}

						//Sending game info to server

						sprintf(linetosend,"Newgameinfo %s %s %s",ol3,glist.player1,glist.player2);
						write(sockfd,linetosend,strlen(linetosend));


						//Game initialization
						for(int i=0;i<3;i++)
						{
							glist.a[i]='*';
							glist.b[i]='*';
							glist.c[i]='*';
						}


						//	showgame();

						//read(sockfd1,pno,MAXBUFLEN);

						sprintf(linetosend,"%s %s %d",glist.player1,glist.player2,glist.chance);
						write(sockfd1,linetosend,strlen(linetosend));
						if(n=read(sockfd1,gametime,MAXBUFLEN)>0)
						{sscanf(gametime,"%s",gametime);}

						tleft=atoi(gametime);
						tleftother=tleft;

						start=times(&buffer);

						showgame();

						//pthread_create(&tid, NULL, &writeto_clientserver, NULL);
						pthread_create(&tid, NULL, &readingfrom_clientserver, NULL);

					}

					break;
					//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

				}

			case 3:
				//else if((oneline=="a1" ||oneline=="a2" ||oneline=="a3" ||oneline=="b1" ||oneline=="b2" ||oneline=="b3" ||oneline=="c1" ||oneline=="c2" ||oneline=="c3")&& cflag==1)
				{
					//jisne ye command dala he uski gamelist....... check chance in that game...... if no..den do nothing.... if yes, make move....
					//bakchodi macha
					int n;

					if(strcmp(glist.player1,myname)==0 && glist.chance%2==0)
					{
						//glist.chance++;
						if((n=makemove(ol1))!=4)
						{
							showgame();

							if(n==200)
							{
								sprintf(linetosend,"TIMEOVER %s %d",ol1,glist.chance);
							}
							else
							{
								sprintf(linetosend,"move %s %d %d",ol1,glist.chance,tleft);
							}
							write(sockfd1, linetosend, strlen(linetosend));

							if (n==100 || n==200)
							{
								//	close(sockfd1);
								memset(&glist, 0, sizeof(glist));
								obslist.clear();
								cflag=0;
							}
						}
					}

					else if(strcmp(glist.player2,myname)==0 && glist.chance%2!=0)
					{
						//glist.chance++;
						if((n=makemove(ol1))!=4)
						{
							showgame();

							if(n==200)
							{
								sprintf(linetosend,"TIMEOVER %s %d",ol1,glist.chance);
							}
							else
							{
								sprintf(linetosend,"move %s %d %d",ol1,glist.chance,tleft);
							}

							write(sockfd1, linetosend, strlen(linetosend));

							if (n==100 || n==200)
							{
								//close(sockfd1);
								sprintf(linetosend,"GAMEOVER %d %s %s %d",port,glist.player1,glist.player2,1);
								write(sockfd,linetosend,strlen(linetosend));
								cflag=0;
								memset(&glist, 0, sizeof(glist));
								obslist.clear();
								global=1;
							}				
						}
					}

					else
					{
						cout<<"Not your Chance\n";
					}
					if(n==100){close(sockfd1);}
					break;

				}

			case 4:
				//else if(oneline=="unobserve")
				{
					//char linetosend[MAXBUFLEN];
					sprintf(linetosend,"%s %s",myname,oneline.c_str());
					write(sockfd2,linetosend,strlen(linetosend));
					break;
				}

			case 5:
				//else if((strcmp(ol1,"kibitz")==0) || (strcmp(ol1,"'")==0))
				{
					//char linetosend[MAXBUFLEN];
					sprintf(linetosend,"%s: %s",myname,oneline.c_str());
					write(sockfd2,linetosend,strlen(linetosend));
					break;
				}

			case 6:				//resign
				{
					if(cflag==1)
					{
						cflag=0;
						sprintf(linetosend,"IQUIT");
						write(sockfd1,linetosend,strlen(linetosend));
						close(sockfd1);


						if(strcmp(glist.player2,myname)==0)
						{
							char ob1[512],ob2[512],ob[512];
							int obssock;
							int n;
							for(int i=0;i<obslist.size();i++)
							{
								sscanf(obslist[i].c_str(),"%s %s",ob1,ob2);
								obssock=atoi(ob2);
								sprintf(ob,"%s WON !!!",glist.player1);
								write(obssock,ob,strlen(ob));
								close(obssock);
							}
							sprintf(linetosend,"GAMEOVER %d %s %s",port,glist.player1,glist.player2,0);
							write(sockfd,linetosend,strlen(linetosend));
						}


						memset(&glist, 0, sizeof(glist));
						obslist.clear();
						global=1;
					}
					else
					{
						cout<<"You are not playing a game\n";
					}	
					break;
				}

			case 8:
				{
					if (cflag==1)
					{
						printf("Black: %s\tWhite: %s\n\t1\t2\t3\nA\t%c\t%c\t%c\nB\t%c\t%c\t%c\nC\t%c\t%c\t%c\n",glist.player1,glist.player2,glist.a[0],glist.a[1],glist.a[2],glist.b[0],glist.b[1],glist.b[2],glist.c[0],glist.c[1],glist.c[2]);

					}

					write(sockfd2,ol1,strlen(ol1));
					break;
				}
			case 9:
				{
					write(sockfd,oneline.c_str(),oneline.length());
					break;
				}

			case 10:
				{
					write(sockfd,oneline.c_str(),oneline.length());

					while (getline(cin, oneline)) 
					{
						if(oneline==".")
						{break;}

						write(sockfd,oneline.c_str(),oneline.length());
					}
					break;
				}
case 11:
{
printf("Commands supported:\n?\t\t\t# print this message\n' <msg>\t\t\t# Comment on a game\ndeletemail <msg_num>\t# Delete the particular mail\nexit\t\t\t# quit the system\ngame\t\t\t# list all current games\nhelp\t\t\t# print this message\ninfo <msg>\t\t# change your information to <msg>\nkibitz <msg>\t\t# Comment on a game\nlistmail\t\t# List the header of the mails\nmail <id> <title>\t# Send id a mail\nmatch <name> <b|w> t\t# Trying to start a game\nobserve <game_num>\t# Observe a game\npasswd <new>\t\t# change password\nquit\t\t\t# quit the system\nreadmail <msg_num>\t# Read the particular mail\nresign\t\t\t# Resign a game\nrefresh\t\t\t# Refresh a game\nshout <msg>\t\t# shout <msg> to every one online\nstats [name]\t\t# Display user information\ntell <name> <msg>\t# tell user <name> message\nunobserve\t\t# Unobserve a game\nwho\t\t\t# List all online users\n");

break;}

			case 7:
				//else
				{
					/*	if(cflag==1)
						{
						cout<<"Trying to write on sockfd1-->"<<sockfd1<<"XXXXXX\n";
						write(sockfd1, oneline.c_str(), oneline.length());
						}
					 */
					write(sockfd, oneline.c_str(), oneline.length());
					//cout<<"oneline--->>"<<oneline;
					//cout<<"\noneline.cstr--->>"<<oneline.c_str()<<endl;

					break;
				}
				for(int i=0;i<512;i++){linetosend[i]='\0';}
		}
	if(oneline=="")
	{cout<<"<"<<myname<<":>";}
	}

}
//const unsigned serv_port = 5100;

int main(int argc, char **argv)
{
	int rv, flag;
	ssize_t n;
	struct addrinfo hints, *res, *ressave;
	pthread_t tid;
	char clientinfo[MAXBUFLEN];

	port= port + atoi(argv[3]);
	if (argc != 4) 
	{
		cout << "echo_client server_name_or_ip port increase_pvalue" << endl;
		exit(1);
	}

	cout << argv[1] << " " << argv[2] << endl;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) 
	{
		cout << "getaddrinfo wrong: " << gai_strerror(rv) << endl;
		exit(1);
	}

	ressave = res;
	flag = 0;
	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0) 
			continue;
		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0) {
			flag = 1;
			break;
		}
		close(sockfd);
	} while ((res = res->ai_next) != NULL);
	freeaddrinfo(ressave);

	if (flag == 0) {
		fprintf(stderr, "cannot connect\n");
		exit(1);
	}
	//	cout<<"1. SOCKFD-->"<<sockfd<<"XXXXXX";

	//	sprintf(clientinfo,"%s %d",argv[3],port);
	//	write(sockfd,clientinfo,strlen(clientinfo));
	//	cout<<"\nClient Info --->> "<<clientinfo<<"\n";fflush(0);

	/*login strts */

	string login;


	printf("Press 1: To Login\nPress Enter : To register\n");

	if(n=read(sockfd,clientinfo,MAXBUFLEN)>0)
	for(int clean=0;clean<MAXBUFLEN;clean++){clientinfo[clean]='\0';}
	
	getline(cin,login);
	if (login=="")
	{write(sockfd,"2",1);}
	else	
	{write(sockfd,login.c_str(),login.length());}

	while(n=read(sockfd,clientinfo,MAXBUFLEN)>0)
	{
		//clientinfo[n]='\0';
		cout<<clientinfo;
		if(strcmp(clientinfo,"OK")==0)
		{
			break;
		}
		getline(cin, login);
		if(login=="")
		{
			write(sockfd,"2",1);
		}
		else{
			write(sockfd,login.c_str(),login.length());
		}
		if(login=="exit"){exit(0);}
		for(int clean=0;clean<MAXBUFLEN;clean++){clientinfo[clean]='\0';}
	}

	sprintf(clientinfo,"%d",port);
	write(sockfd,clientinfo,strlen(clientinfo));
	/*login ends */
	/*bakwas to get client name */
	if(n=read(sockfd,myname,MAXBUFLEN)>0)
	{sscanf(myname,"%s",myname);}

	//cout<<"XX"<<myname<<"XX";
	pthread_create(&tid, NULL, &readfrom_server, NULL);
	pthread_create(&tid, NULL, &writeto_server, NULL);



	//Client binding itself to the address and listening port 

	int serv_sockfd, cli_sockfd, *sock_ptr;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t sock_len;
	// pthread_t tid;

	cout << "port = " << port << endl;
	serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);


	//cout<<" 2.sockfd his listin num-->"<<serv_sockfd<<"XXXXXX\n";

	bzero((void*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	bind(serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	listen(serv_sockfd, 5);

	for (; ;)
	{

		char linetoread[MAXBUFLEN],g1[MAXBUFLEN],g2[MAXBUFLEN],g3[MAXBUFLEN];
		int n;
		sock_len = sizeof(cli_addr);
		cli_sockfd = accept(serv_sockfd, (struct sockaddr *)&cli_addr, &sock_len);

		cout << "remote client IP == " << inet_ntoa(cli_addr.sin_addr);
		cout << ", port == " << ntohs(cli_addr.sin_port) << endl;

		sock_ptr = (int *)malloc(sizeof(int));
		*sock_ptr = cli_sockfd;

		//cout<<"3. sockfd accepted client-->"<<cli_sockfd<<"XXXXXX\n";

		if(global>1)
		{
			char obsread[MAXBUFLEN],obsbuf[MAXBUFLEN];
			if(n=read(cli_sockfd,obsread,MAXBUFLEN)>0)
			{
				//cout<<"I was in !glist";
				sscanf(obsread,"%s",obsbuf);
				sprintf(obsbuf,"%s %d",obsbuf,cli_sockfd);
				obslist.push_back(obsbuf);
				pthread_create(&tid, NULL, &replyto_observer, (void*)sock_ptr);
			}
		}
		else
		{
			global+=1;
			char gametime[512];

			if(n=read(cli_sockfd,linetoread,MAXBUFLEN)>0)
			{
				//cout<<"I was here";
				sscanf(linetoread,"%s %s %s",g1,g2,g3);
				strcpy(glist.player1,g1);
				strcpy(glist.player2,g2);

				glist.chance=atoi(g3);
			}
			//cout<<"Neechewali--->"<<glist.player1<<"X"<<glist.player2<<"X"<<glist.chance<<endl;

			sprintf(gametime,"%d",tleft);
			tleftother=tleft;
			write(cli_sockfd,gametime,strlen(gametime));

			//Game initialization
			for(int i=0;i<3;i++)
			{
				glist.a[i]='*';
				glist.b[i]='*';
				glist.c[i]='*';
			}


			showgame();

			//empty rlist and slist

			if(!slist.empty())
			{

				for(int i=0; i<=slist.size();i++)
				{
					if(strcmp(slist[i].rname,glist.player1)==0)
					{
						//playerinfo[white].obslist.push_back(white);   
						slist.erase(slist.begin()+i);	
						break;
					}
					else
					{
						cout<<"NOT FOUND\n";
					}
				}

			}


			//set flag to make moves
			cflag=1;

			pthread_create(&tid, NULL, &replyto_client, (void*)sock_ptr);
		}
	}

	exit(0);
}
