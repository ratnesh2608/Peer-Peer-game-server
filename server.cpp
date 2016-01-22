#include <iostream>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include<sys/times.h>

using namespace std;

const unsigned port = 2200;
const unsigned MAXBUFLEN = 512;
int i=0;
struct cord
{
	char name[100];
	char portno[100];
}ford[10];

/* global declarations begins */

FILE *current, *statfp,*mailfp;
FILE *tempfile,*tempfile1;
//FILE *passfile;

int rport;
char rcvr[MAXBUFLEN],rcvrport[MAXBUFLEN],msg[MAXBUFLEN];
char clientbuf[MAXBUFLEN],mailbuf[MAXBUFLEN];

struct tms buffer;        /* used for timing */
clock_t start, finish, itime=600;

void putinfo(int);
void runcmd(int);
void gameplay(int,int);
void showgame(int,int);
void makemove(char,char,int,int);
void updatestat(char[],int);
void mailhekya(int);
void gamekiller(int,int);

vector <int> listofcurrgames;

struct sockaddr_in serv_addr;
socklen_t senderSize = sizeof(serv_addr);



struct record{
	int clid;
	char clname[MAXBUFLEN];
	char clport[MAXBUFLEN];
	vector <int> rlist;
	vector <int> slist;
	vector <int> gamelist;
	vector <int> obvlist;
	clock_t tleft,starttime,endtime;
};


vector <string> gameinfo;
record playerinfo[50];


int ctr=0,presult;
char b4colon[100],afcolon[100],templine[100];

/* global declarations ends */

/////////////////////////////////////

void updatestat(char statid[],int result)
{

	fflush(0);
	int i,j;
	char faltu[100];
	sprintf(clientbuf,"%s_stat",statid);
	statfp=fopen(clientbuf,"r");
	tempfile=fopen("tmp.txt","w");
	fclose(tempfile);
	tempfile=fopen("tmp.txt","a");
	ctr=0;
	while(fgets(templine,100,statfp)!=NULL)
	{
		if(ctr==3 || ctr==4 || ctr==2)
		{
			i=0;
			while(templine[i]!= ':')
			{
				b4colon[i]=templine[i];
				i++;
			}
			fflush(0);
			b4colon[i]='\0';i++;j=0;
			while(templine[i]!= '\n' && templine[i]!='\0')
			{
				afcolon[j]=templine[i];
				i++;j++;
			}
			fflush(0);
			afcolon[j]='\0';
			presult=atoi(afcolon);
			fflush(0);

			if(ctr==2 && result==1){presult+=1;}
			else if(ctr==3 && result==0){presult+=1;}
			else if(ctr==4 && result==1){presult+=5;}
			else if(ctr==4 && result==0){presult-=1;}
			printf("In the WHILE 4\n");fflush(0);
			sprintf(templine,"%s:%d\n",b4colon,presult);
			fprintf(tempfile,templine);
		}
		else
		{fprintf(tempfile,templine);}
		ctr++;
	}
	fclose(tempfile);
	fclose(statfp);
	sprintf(faltu,"scp tmp.txt %s",clientbuf);
	system(faltu);
	fflush(0);
}


/////////////////////////////////////
void mailhekya(int mid)
{
        sprintf(mailbuf,"%s_mail",playerinfo[mid].clname);
        mailfp=fopen(mailbuf,"r");
        int i=0;
        while(fgets(templine,100,mailfp)!=NULL)
        {
                if(strcmp(templine,"!@#!\n")==0)
                {
                        if(fgets(templine,100,mailfp)==NULL)
                        {break;}
                        else
                        {
                                if(templine[0]=='N')
                                        i++;
                        }
                }
        }
        fclose(mailfp);
        sprintf(templine,"You have %d unread mails\n",i);
        
        write(playerinfo[mid].clid,templine,strlen(templine));

}
/////////////////////////////////////

void putinfo(int r)
{
	int rank=r;
	current= fopen("current.txt","a");
	fprintf(current,playerinfo[rank].clname);
	fprintf(current,":");
	fprintf(current,"%d",playerinfo[rank].clid);
	fprintf(current,"\n");
	// p[rank]=&playerinfo[rank].clid;
	fclose(current);

}



void runcmd(int r)
{
	int rank=r,n,cno,a,i,j;
	char cmd[MAXBUFLEN],cmd1[MAXBUFLEN],line[MAXBUFLEN],removeuser[MAXBUFLEN],command[MAXBUFLEN],cmd2[MAXBUFLEN];
	char linetosend[MAXBUFLEN];

//	sprintf(linetosend,"\n<%s:>",playerinfo[rank].clname);
//	write(playerinfo[rank].clid,linetosend,strlen(linetosend));
	//write(playerinfo[rank].clid,"\n<",2);
	//write(playerinfo[rank].clid,playerinfo[rank].clname,strlen(playerinfo[rank].clname));
	//write(playerinfo[rank].clid,":>",2);

	while((n= read(playerinfo[rank].clid,command,MAXBUFLEN))>0)
	{
		command[n]='\0';
		printf("\n Read %d Characters : [%s]", n, command);
		fflush(0);
		sscanf(command,"%s %s",cmd,cmd1);
		printf("\n %d Characters : [%s]", n, cmd);
		fflush(0);

		if(strcmp(cmd,"who")==0){cno=1;}
		else if(strcmp(cmd,"shout")==0){cno=3;}
		else if(strcmp(cmd,"tell")==0){cno=4;}
		else if(strcmp(cmd,"match")==0){cno=5;}
		else if(strcmp(cmd,"Newgameinfo")==0){cno=7;}
		else if(strcmp(cmd,"game")==0){cno=8;}
		else if(strcmp(cmd,"observe")==0){cno=9;}
		else if(strcmp(cmd,"GAMEOVER")==0){cno=10;}
		else if(strcmp(cmd,"stats")==0){cno=11;}
		else if(strcmp(cmd,"info")==0){cno=12;}
		else if(strcmp(cmd,"passwd")==0){cno=13;}
		else if(strcmp(cmd,"mail")==0){cno=14;}
		else if(strcmp(cmd,"listmail")==0){cno=15;}
		else if(strcmp(cmd,"readmail")==0){cno=16;}
		else if(strcmp(cmd,"deletemail")==0){cno=17;}
		else if(strcmp(cmd,"quit")==0 || strcmp(cmd,"exit")==0){cno=18;}
		else {cno=6;}


		switch(cno)
		{
			case 1:         /*who*/
				{

					current=fopen("current.txt","r");
					while(fgets(line,MAXBUFLEN,current)!=NULL)
					{
						write(playerinfo[rank].clid,line,strlen(line));
					}
					fclose(current);
					break;
				}

			case 3:         /*shout*/
				{
					i=6,a=0;
					while(command[i]!='\0')
					{
						cmd1[a]=command[i];
						i++;a++;
					}
					cmd1[a]='\0';
					for(i=0;i<50;i++)
					{
						if(getpeername(playerinfo[i].clid, (struct sockaddr *)&serv_addr, (socklen_t *)&senderSize)!=0)
						{fflush(0);}
						else
						{
							write(playerinfo[i].clid,"\nBroadcast msg from : ",22);
							write(playerinfo[i].clid,playerinfo[rank].clname,strlen(playerinfo[rank].clname));
							write(playerinfo[i].clid,"\n",1);
							write(playerinfo[i].clid,cmd1,strlen(cmd1));
						}

					}
					cmd1[0]='\0';
					break;
				}



			case 4:         /*tell*/
				{
					int ctr=0;
					sscanf(command,"%s %s",cmd,cmd1);
					i=0;
					while(command[i]!=' '){i++;}
					i++;
					while(command[i]!=' '){i++;}
					j=0;i++;
					while(command[i]!='\0' && command[i] != '\n'){msg[j]=command[i];i++;j++;}
					msg[j]='\0';
					fflush(0);
					i=0;
					while(i<50)
					{
						if(strcmp(playerinfo[i].clname,cmd1)==0)
						{ctr=1;
							if(getpeername(playerinfo[i].clid, (struct sockaddr *)&serv_addr, (socklen_t *)&senderSize)<0)
							{
								write(playerinfo[rank].clid,"User offline\n",13);
								break;
							}
							else
							{
								write(playerinfo[i].clid,"Message from ",13);
								write(playerinfo[i].clid,playerinfo[rank].clname,strlen(playerinfo[rank].clname));
								write(playerinfo[i].clid,"\n",1);
								write(playerinfo[i].clid,msg,strlen(msg));
							}
							break;
						}
						i++;
					}
					if(ctr==0){
						write(playerinfo[rank].clid,"User offline\n",13);
					}
					fflush(0);
					break;
				}

			case 5:										//match
				{
					int i=0;
					//sscanf(command,"%s %s",cmd,cmd1);
					cout<<cmd<<cmd1<<endl;fflush(0);
					while(i<50)
					{
						if(strcmp(playerinfo[i].clname,cmd1)==0)
						{
							sprintf(linetosend,"connect %s",playerinfo[i].clport);
							break;
						}
						i++;
					}
					sprintf(linetosend,"connect %s %s",playerinfo[i].clname,playerinfo[i].clport);
					write(playerinfo[rank].clid,linetosend,strlen(linetosend));
					sprintf(linetosend,"Wants %s %s",playerinfo[rank].clname,playerinfo[rank].clport);
					write(playerinfo[i].clid,linetosend,strlen(linetosend));
					break;
				}
			case 7:							//newgameinfo
				{
					char ol[MAXBUFLEN],ol1[MAXBUFLEN],ol2[MAXBUFLEN],ol3[MAXBUFLEN],ol4[MAXBUFLEN];
					sscanf(command,"%s %s %s %s",ol1,ol2,ol3,ol4);
					sprintf(ol,"%s %s %s",ol2,ol3,ol4);
					gameinfo.push_back(ol);
					break; 
				}
			case 8:							//game
				{
					char ol[MAXBUFLEN],ol1[MAXBUFLEN],ol2[MAXBUFLEN],ol3[MAXBUFLEN],ol4[MAXBUFLEN];
					if(!gameinfo.empty())
					{
						for(int i=0;i<gameinfo.size();i++)
						{
							sscanf(gameinfo[i].c_str(),"%s %s %s",ol1,ol2,ol3);
							sprintf(linetosend,"Game(%d)\t%s VS %s\t%s\n",i,ol2,ol3,ol1);
							write(playerinfo[rank].clid,linetosend,strlen(linetosend));
						}
					}
					else
					{
						sprintf(linetosend,"There are no games in play\n");
						write(playerinfo[rank].clid,linetosend,strlen(linetosend));
					}
					break;
				}

			case 9:							//Observe
				{
					char ol[MAXBUFLEN],ol1[MAXBUFLEN],ol2[MAXBUFLEN],ol3[MAXBUFLEN];
					int gno;
					if(sscanf(command,"%s %s",ol1,ol2)==2)
					{
						gno=atoi(ol2);
						if(!gameinfo.empty() || gameinfo.size()>gno)
						{
							cout<<"\nGAME name "<<gameinfo[gno]<<"XCXCXCCXCCCXXX"<<gno<<endl;
							sscanf(gameinfo[gno].c_str(),"%s",ol3);
							sprintf(linetosend,"Observe %s",ol3);
							write(playerinfo[rank].clid,linetosend,strlen(linetosend));
						}
						else
						{
							sprintf(linetosend,"There is no such game to Observe\n");
							write(playerinfo[rank].clid,linetosend,strlen(linetosend));
						}
						for(int i=0;i<512;i++){linetosend[i]='\0';}
					}
					else
					{
						cout<<"wrong observe condition";
					}
					break;
				}
			case 10:						//gameover
				{
					char linetodelete[512],ol[MAXBUFLEN],ol1[MAXBUFLEN],ol2[MAXBUFLEN],ol3[MAXBUFLEN],ol4[512];
					sscanf(command,"%s %s %s %s %s",ol,ol1,ol2,ol3,ol4);
					sprintf(linetodelete,"%s %s %s",ol1,ol2,ol3);
					cout<<"GAME INFO received --->>>"<<command;
					cout<<"GAME INFO --->>>"<<gameinfo[0];
					gameinfo.erase(find(gameinfo.begin(),gameinfo.end(),linetodelete));
					cout<<"GAME INFO after game deletion-->>>"<<gameinfo[0];

					//updating stats ol2=player1 ol3=plyer2 ol4= 1 if clientserver won: else 0

					if(atoi(ol4)==1)
					{
						updatestat(ol3,1);
						updatestat(ol2,0);
					}
					else if(atoi(ol4)==0)
					{
						updatestat(ol2,1);
						updatestat(ol3,0);
					}

					break;
				}

			case 11:
				{
					int i,ctr,ctr1=0;
					if(sscanf(command,"%s %s",cmd1,cmd)==2)
					{
						sprintf(cmd2,"%s_stat",cmd);
					}
					else
					{
						sprintf(cmd2,"%s_stat",playerinfo[rank].clname);
						strcpy(cmd,playerinfo[rank].clname);
					}
					fflush(0);

					if((statfp=fopen(cmd2,"r")))
					{ctr=0;
						while(fgets(cmd1,200,statfp)!=NULL)
						{
							if(ctr==5)
							{i=0;
								while(i<50)
								{
									if(strcmp(playerinfo[i].clname,cmd)==0)
									{ctr1=1;
										if(getpeername(playerinfo[i].clid, (struct sockaddr *)&serv_addr, (socklen_t *)&senderSize)==0)
										{
											sprintf(cmd1,"Status: User is Online\n");
											break;
										}
										else
										{
											sprintf(cmd1,"Status: User is Offline\n");
										}
									}
									i++;
								}
								if(ctr1==0)
								{
									sprintf(cmd1,"Status: User is Offline\n");
								}
							}
							write(rank,cmd1,strlen(cmd1));
							ctr++;
						}
						fclose(statfp);
					}
					else
					{
						write(rank,"No such user\n",13);

					}
					for(int i=0;i<200;i++){command[i]='\0';}
					break;
				}

			case 12:                                                                /*Info*/
				{
					char *info;
					if(sscanf(command,"%s %s",cmd,cmd1)==2)
					{
						fflush(0);
						sprintf(cmd2,"%s_stat",playerinfo[rank].clname);
						tempfile=fopen("tmp.txt","a");

						if((statfp=fopen(cmd2,"r")))
						{
							ctr=0;
							while(fgets(templine,200,statfp)!=NULL)
							{
								if(ctr==1)
								{
									info=strstr(command," ");
									sprintf(templine,"Info:%s\n",info);
								}
								fprintf(tempfile,templine);
								ctr++;
							}
							fclose(statfp);fclose(tempfile);
							sprintf(templine,"scp tmp.txt %s",cmd2);
							system(templine);
							write(rank,"Info Updated\n",13);

						}
						else{write(rank,"\nCannot Update\n",14);}
					}
					else{write(rank,"\nUsage info <msg>\n",17);}
					break;
				}

			case 13:
				{
					FILE *passfile;
					if(sscanf(command,"%s %s",cmd,cmd1)==2)
					{
						fflush(0);
						passfile=fopen("clpassfile","r");
						tempfile=fopen("tmp.txt","a");
						while(fgets(templine,50,passfile)!= NULL)
						{
							int i=0;
							while(templine[i]!= ':')
							{
								cmd2[i]=templine[i];
								i++;
							}
							cmd2[i]='\0';

							if(strcmp(cmd2,playerinfo[rank].clname)==0)
							{
								sprintf(templine,"%s:%s\n",cmd2,cmd1);
							}
							fprintf(tempfile,templine);

						}
						fclose(tempfile);/*fclose(passfile);*/
						system("scp tmp.txt clpassfile");
						write(rank,"Password changed\n",17);
					}
					else{write(rank,"Usage password <newpassword>\n",29);}
					break;
				}

			case 14:
				///////////////////////////////////

				{
					char body[MAXBUFLEN],mailto[100],title[100];
					time_t rawtime;
					struct tm * timeinfo;
					char mailtime[100],tempbody[MAXBUFLEN],fulltitle[100];

					if(sscanf(command,"%s %s %s",cmd,mailto,title)==3)
					{
						fflush(0);
						int i=0,j;

						/*Getting the full title*/

						while(command[i]!=' '){i++;}i++;
						while(command[i]!=' '){i++;}i++;j=0;

						while(command[i]!='\0')
						{
							fulltitle[j]=command[i];
							i++;j++;
						}
						fulltitle[j]='\0';

						//sscanf(fulltitle,"%s",fulltitle);
						printf("XXX%sXXX",fulltitle);fflush(0);

						strcpy(title,fulltitle);

						sprintf(mailbuf,"%s_mail",mailto);
						mailfp=fopen(mailbuf,"a");

						time ( &rawtime );
						timeinfo = localtime ( &rawtime );
						strcpy(mailtime,asctime(timeinfo));

						fprintf(mailfp,"N %s %s : %s\n",playerinfo[rank].clname,title,mailtime);
						//tempfile=fopen("tmp.txt","a");

						sprintf(linetosend,"Mail body begins, put '.' to end mail\n");
						write(rank,linetosend,strlen(linetosend));

						while((n=read(rank,body,MAXBUFLEN)>0))
						{

							fflush(0);
							sscanf(body,"%s",tempbody);
							if(strcmp(tempbody,".")==0)
							{break;}
							fprintf(mailfp,"%s\n",body);
							for(int i=0;i<512;i++){body[i]='\0';}
						}
						fprintf(mailfp,"!@#!\n");
						fclose(mailfp);
						i=0;
						while(i<50)
						{
							if(strcmp(playerinfo[i].clname,mailto)==0)
							{
								if(getpeername(playerinfo[i].clid, (struct sockaddr *)&serv_addr, (socklen_t *)&senderSize)<0)
								{break;}
								else
								{
									write(playerinfo[i].clid,"You have a mail from ",21);
									write(playerinfo[i].clid,playerinfo[rank].clname,strlen(playerinfo[rank].clname));
								}
								break;
							}
							i++;
						}

					}
					else{write(rank,"mail receiver title\n",20);}

					break;
				}


				///////////////////////////////////
				///////////////////////////////////

			case 15:
				{
					if(sscanf(command,"%s",cmd)==1)
					{
						fflush(0);
						sprintf(mailbuf,"%s_mail",playerinfo[rank].clname);
						mailfp=fopen(mailbuf,"r");
						int i=0;
						char chi[MAXBUFLEN];
						while(fgets(templine,100,mailfp)!=NULL)
						{
							if(strcmp(templine,"!@#!\n")==0)
							{
								if(fgets(templine,100,mailfp)==NULL)
								{break;}
								else
								{
									sprintf(chi,"%d %s",i,templine);
									write(playerinfo[rank].clid,chi,strlen(chi));
									i++;
								}
							}
						}
						fclose(mailfp);
					}
					break;
				}
			case 16:
				{
					int ctr=0,innerctr=0;
					int icmd1;
					char w1[100],w2[100],w4[100];
					char templine1[MAXBUFLEN];
					char *w3;
					if(sscanf(command,"%s",cmd)==1)
					{

						if(sscanf(command,"%s %s",cmd,cmd1)==2)
						{icmd1=atoi(cmd1);}

						else{icmd1=0;}
						fflush(0);

						sprintf(mailbuf,"%s_mail",playerinfo[rank].clname);
						tempfile=fopen("tmp.txt","a");
						mailfp=fopen(mailbuf,"r");
						while(fgets(templine1,MAXBUFLEN,mailfp)!=NULL)
						{
							if(strcmp(templine1,"!@#!\n")==0)
							{
								if(ctr==icmd1)
								{
									fprintf(tempfile,"%s",templine1);
									while(fgets(templine1,MAXBUFLEN,mailfp)!=NULL)
									{
										if(strcmp(templine1,"!@#!\n")==0)
										{
											break;
										}
										if(innerctr==0)
										{
											innerctr=1;
											sscanf(templine1,"%s%s%s",w4,w1,w2);
											w3=strstr(templine1,":");
											sprintf(templine,"Y %s %s %s",w1,w2,w3);
											fprintf(tempfile,"%s",templine);

											write(playerinfo[rank].clid,"From: ",6);
											write(playerinfo[rank].clid,w1,strlen(w1));
											write(playerinfo[rank].clid,"\n",1);
											write(playerinfo[rank].clid,"Title: ",7);
											write(playerinfo[rank].clid,w2,strlen(w2));
											write(playerinfo[rank].clid,"\n",1);
											write(playerinfo[rank].clid,"Time",4);
											write(playerinfo[rank].clid,w3,strlen(w3));
										}
										else
										{
											write(playerinfo[rank].clid,templine1,strlen(templine1));
											fprintf(tempfile,"%s",templine1);
										}
									}
								}ctr++;
								fprintf(tempfile,"%s",templine1);
							}
							else
							{
								fprintf(tempfile,"%s",templine1);
							}
						}
						fclose(mailfp);
						fclose(tempfile);
						sprintf(templine1,"scp tmp.txt %s",mailbuf);
						system(templine1);
					}
					for(int i=0;i<100;i++){templine1[i]='\0';}
					break;
				}
				///////////////////////////////////



			case 17:
				{
					int ctr=0;
					int icmd1;
					/*char w1[100],w2[100],w4[100];*/
					char templine1[MAXBUFLEN];
					/*char *w3;*/
					if(sscanf(command,"%s",cmd)==1)
					{
						if(sscanf(command,"%s %s",cmd,cmd1)==2)
						{icmd1=atoi(cmd1);}

						else{icmd1=0;}
						fflush(0);

						sprintf(mailbuf,"%s_mail",playerinfo[rank].clname);
						tempfile=fopen("tmp.txt","a");
						mailfp=fopen(mailbuf,"r");
						while(fgets(templine1,MAXBUFLEN,mailfp)!=NULL)
						{
							if(strcmp(templine1,"!@#!\n")==0)
							{
								if(ctr==icmd1)
								{
									while(fgets(templine1,MAXBUFLEN,mailfp)!=NULL)
									{
										if(strcmp(templine1,"!@#!\n")==0)
										{
											write(playerinfo[rank].clid,"Mail Deleted\n",13);
											break;
										}
									}
								}ctr++;
								fprintf(tempfile,"%s",templine1);
							}
							else
							{
								fprintf(tempfile,"%s",templine1);
							}
						}
						fclose(mailfp);
						fclose(tempfile);
						sprintf(templine1,"scp tmp.txt %s",mailbuf);
						system(templine1);

					}
					break;
				}
				////////////////////////////////
			case 18:

				{
					current=fopen("current.txt","r");
					tempfile1=fopen("tmp1.txt","a");
					while(fgets(line,MAXBUFLEN,current)!=NULL)
					{       a=0;

						while(line[a]!= ':')
						{
							removeuser[a]=line[a];
							a++;
						}
						removeuser[a]='\0';
						if(strcmp(removeuser,playerinfo[rank].clname)!=0)
						{
							fprintf(tempfile1,line);
						}
						else
						{a++;}
					}
					fclose(current);
					fclose(tempfile1);

					sprintf(line,"scp tmp1.txt current.txt");
					system(line);

					tempfile1=fopen("tmp1.txt","w");
					fclose(tempfile1);

					close(playerinfo[rank].clid);
					memset(&playerinfo[rank], 0, sizeof(playerinfo[rank]));
					break;
				}


				///////////////////////////////

		}


		/*to clear command */
		for(unsigned int i=0;i<MAXBUFLEN;i++){command[i]='\0';cmd[i]='\0';}
		fflush(0);
		tempfile=fopen("tmp.txt","w");fclose(tempfile);

	//	sprintf(linetosend,"\n<%s:>",playerinfo[rank].clname);
//		write(playerinfo[rank].clid,linetosend,strlen(linetosend));
	}
	if (n == 0) {
		cout << "client closed" << endl;
	} else {
		cout << "something wrong" << endl;
	}
	close(playerinfo[rank].clid);

}

void *process_connection(void *arg) {
	/*	int sockfd;
		`	ssize_t n;
		char buf[MAXBUFLEN],rec1[MAXBUFLEN],rec2[MAXBUFLEN],buf1[MAXBUFLEN],buf2[MAXBUFLEN];

		sockfd = *((int *)arg);
		free(arg);
		cout<<"socketfd server---->"<<sockfd<<"XXXXX\n";

	 */
	//	pthread_detach(pthread_self());

	/* registration */

	int exitflag=0;
	int rank = *(int*)arg;
	int cli_sockfd = *(int*)arg;
	struct sockaddr_in cli_addr;
	socklen_t sock_len;
	ssize_t n;
	char buf[MAXBUFLEN];
	char usrname[MAXBUFLEN],password[MAXBUFLEN],older[50],ouser[50],opassword[50],statbuf[MAXBUFLEN],keyword[MAXBUFLEN];
	FILE *passfile;
	int value,value1;
	int choice,i,j,ctr=0;
	int tid = *((int *)arg);
	int useristhere=0;
	free(arg);
	char kickuser[MAXBUFLEN],kickno[MAXBUFLEN];
	char line[200],removeuser[200];
	int a,z,remove;
	char linetosend[MAXBUFLEN],linetoread[MAXBUFLEN];
	pthread_detach(pthread_self());

	//	for (; ;) {
	/*sock_len = sizeof(cli_addr);
	  pthread_mutex_lock(&accept_lock);
	  cli_sockfd = accept(serv_sockfd, (struct sockaddr *)&cli_addr, &sock_len);
	  pthread_mutex_unlock(&accept_lock);

	  cout << "thread " << tid << ": ";
	  cout << "remote client IP == " << inet_ntoa(cli_addr.sin_addr);
	  cout << ", port == " << ntohs(cli_addr.sin_port) << endl;
	 */
	//cout<<"I was here --> "<<cli_sockfd;fflush(0);

	sprintf(linetosend,"Press 1: To Login\nPress Enter : To register\n");
	write(cli_sockfd,linetosend,strlen(linetosend));
	while(n= read(cli_sockfd,buf,MAXBUFLEN)>0)
	{
		choice = buf[0]- '0';

		if(choice==1){value=1;}
		else{value=2;}

		switch(value)
		{
			case 1:
			{
				do{
					passfile=fopen("clpassfile","a+r");

					for(int clean=0;clean<MAXBUFLEN;clean++){usrname[clean]='\0';password[clean]='\0';}		
					write(cli_sockfd,"Username:",9);
					n= read(cli_sockfd,usrname,MAXBUFLEN);
					sscanf(usrname,"%s",usrname);
					write(cli_sockfd,"Password:",9);
					n= read(cli_sockfd,password,MAXBUFLEN);
					sscanf(password,"%s",password);
					cout<<usrname<<":"<<password<<"YYY";fflush(0);
					ctr=0;
					useristhere=0;
					while(fgets(older,50,passfile)!= NULL)
					{
						i=0;
						while(older[i]!= ':')
						{
							ouser[i]=older[i];
							i++;
						}
						ouser[i]='\0';
						j=0;i++;
						if(strcmp(ouser,usrname)==0)
						{
							while(older[i]!= '\n' && older[i] != '\0')
							{
								opassword[j]=older[i];
								i++,j++;
							}
							opassword[j]='\0';
							if(strcmp(opassword,password) != 0)
							{
								write(cli_sockfd,"Wrong Password\n",15);
								ctr=2;
								break;
							}
							else
							{
								current=fopen("current.txt","r");
								while(fgets(older,50,current)!=NULL)
								{
									i=0;
									while(older[i]!= ':')
									{
										ouser[i]=older[i];
										i++;
									}
									ouser[i]='\0';
									j=0;i++;
									if(strcmp(ouser,usrname)==0)
									{
										useristhere=1;
										strcpy(kickuser,ouser);
									}
								}
								fclose(current);
								cout<<"\n 1.OK \n";fflush(0);
								if(useristhere==0)
								{

									cout<<"\n OK \n";fflush(0);
									write(cli_sockfd,"OK",2);
									if(n=read(cli_sockfd,linetoread,MAXBUFLEN)>0)
									{
										strcpy(playerinfo[rank].clport,linetoread);
										cout<<"Portnorecv"<<playerinfo[rank].clport<<"XX";fflush(0);
										write(cli_sockfd,usrname,strlen(usrname));
									}

									ctr=1;
									rank=cli_sockfd;
									playerinfo[rank].gamelist.push_back('\0');
									playerinfo[rank].clid=cli_sockfd;

									playerinfo[rank].tleft=itime;

									strcpy(playerinfo[rank].clname,usrname);
									
									putinfo(rank);
									mailhekya(rank);
									runcmd(rank);
									exitflag=1;
									ctr=1;
								}
								else{
									ctr=2;
									useristhere=0;
									write(cli_sockfd,"User already in the current session\n",36);
									/*      //////////////////////////////////////////////////*/
								/*	current=fopen("current.txt","r");
									tempfile=fopen("tmp.txt","a");
									while(fgets(line,MAXBUFLEN,current)!=NULL)
									{       a=0;

										while(line[a]!= ':')
										{
											removeuser[a]=line[a];
											a++;
										}

										if(strcmp(removeuser,kickuser)==0)
										{z=0;a++;
											while(line[a]!= '\n')
											{
												kickno[z]=line[a];
												a++,z++;
											}
											kickno[z]='\0';
										}
										else
										{
											fprintf(tempfile,line);
											a++;}
									}
									fclose(current);
									fclose(tempfile);

									remove=atoi(kickno);

									sprintf(line,"scp tmp.txt current.txt");
									system(line);

									tempfile=fopen("tmp.txt","w");
									fclose(tempfile);
									if(!playerinfo[remove].gamelist.empty())
									{
										write(remove,"You logged from another machine\n",32);
										//gamekiller(playerinfo[remove].gamelist[0],remove);
									}
									//p[remove]=NULL;               
									close(remove);
									/*\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
								/*	write(cli_sockfd,"Match found\n",12);
									ctr=1;
									rank=cli_sockfd;
									playerinfo[rank].gamelist.push_back('\0');
									playerinfo[rank].clid=cli_sockfd;

									playerinfo[rank].tleft=itime;

									strcpy(playerinfo[rank].clname,usrname);

									fflush(0);

									/* putinfo(rank);
									   mailhekya(rank);
									   runcmd(rank);*/
								}
								//call function to proceed here

								break;
							}

						}
					}
					if(ctr==0)
					{
						write(cli_sockfd,"No User Found\n",14);
					}
					fclose(passfile);

				}while(ctr != 1);
				break;
			}
			case 2:
			{
				do{
					write(cli_sockfd,"The only allowed command is <<register username password>>\n",59);
					if(n= read(cli_sockfd,usrname,MAXBUFLEN)>0)
					sscanf(usrname,"%s%s%s",keyword,usrname,password);
					if(strcmp(keyword,"register")==0)
					{
						useristhere=0;
						passfile=fopen("clpassfile","a+r");
						while(fgets(older,50,passfile)!=NULL)
						{
							i=0;
							while(older[i]!= ':')
							{
								ouser[i]=older[i];
								i++;
							}
							ouser[i]='\0';
							i++;
							if(strcmp(ouser,usrname)==0)
							{useristhere=1;
								break;}
						}
						fclose(passfile);
						if(useristhere==0)
						{
							passfile=fopen("clpassfile","a");
							fprintf(passfile,usrname);
							fprintf(passfile,":");
							fprintf(passfile,password);
							fprintf(passfile,"\n");

							write(cli_sockfd,"New ID created",14);

							sprintf(statbuf,"%s_stat",usrname);
							statfp=fopen(statbuf,"w");

							fprintf(statfp,"User:%s\n",usrname);
							fprintf(statfp,"Info:\n");
							fprintf(statfp,"Wins:0\n");
							fprintf(statfp,"Loses:0\n");
							fprintf(statfp,"Rating:0\n");
							fprintf(statfp,"Online:Y\n");
							fclose(statfp);

							useristhere=1;

							fclose(passfile);

							sprintf(mailbuf,"%s_mail",usrname);
							mailfp=fopen(mailbuf,"w");
							fprintf(mailfp,"!@#!\n");
							fclose(mailfp);

						}
						else
						{
							write(cli_sockfd,"A user by this name already exists\n",35);
						}
					}
					else
					{
					//write(cli_sockfd,"The only allowed command is <<register username password>>\n",59);
					write(cli_sockfd,"To exit type exit, else press enter\n",37);
					}
					//write(cli_sockfd,"To exit type exit, else press enter\n",37);

					for(int clean=0;clean<512;clean++){mailbuf[clean]='\0';}
					
					if(n= read(cli_sockfd,mailbuf,MAXBUFLEN)>0)
					{sscanf(mailbuf,"%s",usrname);
					if(strcmp(usrname,"exit")==0){useristhere=0;exitflag=1;}
					else{useristhere=1;}
					}
					else
						{
						if (n == 0) {
						cout << "client closed" << endl;
						} else {
						cout << "something wrong" << endl;
						}
						close(cli_sockfd);
						useristhere=0;exitflag=1;
						}
				}while(useristhere==1);
				break;
			}
		}

		/*
		   if (n == 0) {
		   cout << "client closed" << endl;
		   } else {
		   cout << "something wrong" << endl;
		   }*/
		//close(cli_sockfd);
		if(exitflag==1){exitflag=0;break;}
	}
	//	return 0;
	if (n == 0) {
		cout << "client closed" << endl;
	} else {
		cout << "something wrong" << endl;
	}
	close(cli_sockfd);


	/* registration ends */


	return(NULL);
}

int main() {
	int serv_sockfd, cli_sockfd, *sock_ptr;
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t sock_len;
	pthread_t tid;
	char clientinfo[MAXBUFLEN];
	int n;

	cout << "port = " << port << endl;
	serv_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	current=fopen("current.txt","w");
	fclose(current);
	/*Port reuse*/
	//setsockopt(serv_sockfd, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));


	//cout<<"\n SERVERS BINDING GANMARI VALUE : ---> "<<serv_sockfd<<"\n";fflush(0);

	bzero((void*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	bind(serv_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	listen(serv_sockfd, 5);
	for (; ;) {
		sock_len = sizeof(cli_addr);
		cli_sockfd = accept(serv_sockfd, (struct sockaddr *)&cli_addr, &sock_len);

		cout << "remote client IP == " << inet_ntoa(cli_addr.sin_addr);
		cout << ", port == " << ntohs(cli_addr.sin_port) << endl;

		sock_ptr = (int *)malloc(sizeof(int));
		*sock_ptr = cli_sockfd;

		/*		if(n=read(cli_sockfd,clientinfo,MAXBUFLEN)>0)
				{*/
		//	clientinfo[n]='\0';
		/*

		   strcpy(ford[i].name,clientinfo);
		   i++;			
		   }
		 */
		pthread_create(&tid, NULL, &process_connection, (void
					*)sock_ptr);
	}
}
