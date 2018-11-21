#include <bluetooth_socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

extern "C"
{
#include "bluetooth_interface.h"
}

enum cmd_t
{
	stop_az        = 0,
	play_az           ,
	pause_az          ,
	next_az           ,
	pre_az            ,
	btconnect_az      ,
	btdisconnect_az   ,//6
	bstop_az          ,
	sstop_az          ,
	quick_az          ,
	rpause_az         ,
};

static int status = 0;
static int playing = 0;
static int sockfd ;
c_bt c;

int play_cmd = stop_az, btconnect = 0,recv_cmd = stop_az;

void send_cmd_az(int cmd_x)
{

	send(sockfd,&cmd_x,sizeof(cmd_x),0);
}

void send_cmd(int cmd_q)
{
	switch(cmd_q)
	{
		case stop_az:
			play_cmd = cmd_q;
			printf("play_stop\n");
			s_avk_stop();
			break;
		case play_az:
			play_cmd = cmd_q;
			printf("play_play\n");
			c.avk_play();
			send_cmd_az(cmd_q);
			break;
		case pause_az:
			if(playing)
			{
				c.avk_pause();
			}
			play_cmd = cmd_q;
			send_cmd_az(cmd_q);
			printf("play_pause\n");
			break;
		case next_az:
			play_cmd = cmd_q;
			printf("play_next\n");
			c.avk_next();
			break;
		case pre_az:
			play_cmd = cmd_q;
			printf("play_pre\n");
			c.avk_previous();
			break;
		case rpause_az:
			if(playing)
			{
				c.avk_pause();
			}
			play_cmd = cmd_q;
			printf("play_rpause\n");
			send_cmd_az(cmd_q);
			break;
		default:
			play_cmd = cmd_q;
			break;
	}
}
void bt_event_f(BT_EVENT event)
{
	switch(event)
	{
		case BT_AVK_CONNECTED_EVT:
			{
				printf("Media audio connected!\n");
				if(	btconnect ++ > 1)
					send_cmd_az(btconnect_az);
				c.set_dev_discoverable(0);
				c.set_dev_connectable(0);
				status = 1;
				break;
			}

		case BT_AVK_DISCONNECTED_EVT:
			{
				printf("Media audio disconnected!\n");
				c.set_dev_connectable(1);
				c.set_dev_discoverable(1);
				status = 0;
				if(btconnect > 2)
					send_cmd_az(btdisconnect_az);

				break;
			}

		case BT_AVK_START_EVT:
			{
				printf("Media start playing!\n");
				playing = 1;
				send_cmd_az(play_az);
				break;
			}

		case BT_AVK_STOP_EVT:
			{
				printf("Media stop playing!\n");
				playing = 0;
				if(recv_cmd == play_az || recv_cmd == pre_az || recv_cmd == next_az )
					send_cmd_az(bstop_az);
				else
					send_cmd_az(stop_az);
				break;
			}

		default:
			break;
	}
}

void *bt_read(void *dg)
{
	int res = -1, recv_b, i = 0;
	sockfd = socket(AF_UNIX,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		perror("socket"),exit(-1);
	}

	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path,"builesock");

	while((res == -1) &&( i < 200))
	{
		res = connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
		if(-1 == res){
			perror("connect"),exit(-1);
			usleep(100000);
		}
		i++;
	}
	if(res != -1)
		printf("bt connect socket success\n");

	while(recv_b != quick_az)
	{
		res = recv(sockfd,&recv_b,sizeof(recv_b),0);
		printf("bt recv recv_b=%d,res =%d\n",recv_b,res);
		if(0 >= res)
		{
			recv_cmd = quick_az;
			recv_b = quick_az;
		}
		recv_cmd = recv_b;
		send_cmd(recv_b);

	}
	send_cmd(pause_az);
	send_cmd_az(recv_b);
	close(sockfd);
	printf("bt quick socket success\n");
}

int main(int argc, char *args[])
{
	c.bt_off();
	c.set_callback(bt_event_f);

	if(argc >= 2){
		c.bt_on(args[1]);
	} else {
		c.bt_on(NULL);
	}
	char bt_name[64] = {0};
	srand(time(NULL));
	sprintf(bt_name,"EnskySE1-%04d",rand()%10000);
	c.set_bt_name(bt_name);

	pthread_t  th_read;
	pthread_create(&th_read,NULL,bt_read,NULL);
	usleep(100000);
	pthread_detach(th_read);

	while(recv_cmd != quick_az)
	{

		if(!status &&btconnect > 1  )
			c.connect_auto();

		if(!status && btconnect >1)
			sleep(5);
		else if(!status)
			usleep(500000);
		else
			sleep(5);
		if(status && btconnect == 1)
		{
			btconnect ++;
			c.reset_avk_status();
		}
	}	
	app_avk_close_all();
	c.bt_off();
}
