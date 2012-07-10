#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <stdexcept>
#include <memory>
#include <vector>
#include <signal.h>
#include <inttypes.h>
#include <mysql.h>

#include "common_declarations.h"
#include "exception.h"
#include "configurator.h"
#include "logger.h"
#include "mysql_writer_factory.h"
#include "reciever.h"
#include "scheduled_action.h"
#include "scheduled_actions.h"

std::string program_name = "electricity_meter";

void signal_handler(int sig) /* signal handler function */
{
		switch(sig){
			case SIGHUP:
				/* rehash the server */
				break;		
			case SIGTERM:
				/* finalize the server */
				exit(0);
				break;		
		}	
}
	
void daemonize(const std::string& working_dir, const std::string& lock_file_name)
{
int i,lfp;
	if(getppid()==1) return; /* already a daemon */

	struct stat sb;
	if (stat(working_dir.c_str(), &sb) != 0 ||
		!(S_ISDIR(sb.st_mode)))
	{
		std::cerr<<"The directory "<<working_dir<<" does not exist!"<<std::endl;
		exit(0);
	}
	int ret = chdir(working_dir.c_str()); /* change running directory */
	if(ret!=0){
		std::cerr<<"Cannot chdir to "<<working_dir<<std::endl;
		exit(0);	
	};

	i=fork();
	if (i<0) exit(1); /* fork error */
	if (i>0) exit(0); /* parent exits */
	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
	
	for (i=getdtablesize();i>=0;--i) close(i); /* close all descriptors */
	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */

	umask(027); /* set newly created file permissions */

	lfp=open(lock_file_name.c_str(),O_RDWR|O_CREAT,0640);
	if (lfp<0) exit(1); /* can not open */
	if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */
	/* first instance continues */
	std::ostringstream info_for_lock;
	info_for_lock<<getpid();
 	write(lfp, info_for_lock.str().c_str(), strlen(info_for_lock.str().c_str())); /* record pid to lockfile */

	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,signal_handler); /* catch hangup signal */
	signal(SIGTERM,signal_handler); /* catch kill signal */
}

int main(int argc, char **argv)
{
	if (argc>1)
	{
		program_name = argv[0];
	}
	
	//prepares the new process for being daemon
	std::string working_dir = "/usr/sbin/";
	
	std::string lock_file_name =  "/var/run/";
	lock_file_name += program_name;
	lock_file_name += ".pid";
	daemonize(working_dir, lock_file_name);
	//starts working
	try
	{
		electricity_meter::logger& logg = electricity_meter::logger::instance();
		logg.set_app_name(program_name);
		logg.initialize();
		electricity_meter::configurator& config = electricity_meter::configurator::instance();
		logg.set_app_version(config.application_version());
		config.load_configuration(program_name+".xml");
		
		electricity_meter::mysql_writer_factory& mysql_fact = electricity_meter::mysql_writer_factory::instance();

		mysql_fact.connect(config.database_host(), 
							config.database_name(),
							config.database_user_name(),
							config.database_password(),
							config.database_port());
		
		electricity_meter::reciever& reciev = electricity_meter::reciever::instance();
		electricity_meter::logger::instance().log_message(electricity_meter::INFO, "Program started, connection to MySQL done. Ready to recieve data packets.");
		electricity_meter::scheduled_actions::instance().start_actions(); // started scheduled actions processing
		
		reciev.start_recieve(config.tcp_port(),
							config.tcp_recv_send_timeout_sec(),
							config.tcp_max_connect_queue());
	} 
	catch (electricity_meter::exception exc) 
	{
		electricity_meter::logger::instance().log_message(exc);
		return 1;
	}
	catch (std::exception exc)
	{
		electricity_meter::logger::instance().log_message(electricity_meter::ERROR, exc.what());
		return 1;
	};
	return 0;
}
