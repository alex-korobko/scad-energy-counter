//implemenation of socket class
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <termios.h>

#include <vector>
#include <string>
#include <map>
#include <functional>
#include <sstream>
#include <stdexcept>


#include "common_declarations.h"
#include "exception.h"
#include "socket_exception.h"

#include "generic_socket.h"

namespace electricity_meter
{

generic_socket::generic_socket(uint64_t recv_send_timeout_nsec): 
   recv_send_timeout_nseconds(recv_send_timeout_nsec),
   sock (-1){	};

generic_socket::~generic_socket() {
	if ( is_valid()) 
		    shutdown (sock, 2);
     close(sock);
}

bool generic_socket::create() throw (socket_exception){
	sock = ::socket (AF_INET,
							  SOCK_STREAM,
							  0);

	if  (sock==-1){
	        std::ostringstream exception_description;
			exception_description<<"generic_socket::create() ::socket (...) error : "<<strerror( errno);
			throw socket_exception (exception_description.str());
		};

	if ( ! is_valid())	return false;

	int on=1, setsockopt_return;
	setsockopt_return=setsockopt (sock,
							SOL_SOCKET,
							SO_REUSEADDR,
							&on,
							sizeof(on)
							);

	if (setsockopt_return==-1){
	        std::ostringstream exception_description;
			exception_description<<"generic_socket::create() setsockopt (...SO_REUSEADDR...) error : "<<strerror( errno);
			if (shutdown(sock, 1)!=0) {
               exception_description<<"\nCan`t shutdown socket.  Error : "<<strerror( errno);
			};
			throw socket_exception (exception_description.str());
		};

	if (setsockopt_return!=0) return false;
	
	timeval tm;
	tm.tv_sec = recv_send_timeout_nseconds;
	tm.tv_usec = 0;

	setsockopt_return=setsockopt (sock,
							SOL_SOCKET,
							SO_RCVTIMEO,
							&tm,
							sizeof(tm)
							);

	if (setsockopt_return==-1){
	        std::ostringstream exception_description;
			exception_description<<"generic_socket::create() setsockopt (...SO_RCVTIMEO...) error : "<<strerror( errno);
			if (shutdown(sock, 1)!=0) {
               exception_description<<"\nCan`t shutdown socket.  Error : "<<strerror( errno);
			};
			throw socket_exception (exception_description.str());
		};

	if (setsockopt_return!=0) return false;
	
	setsockopt_return=setsockopt (sock,
							SOL_SOCKET,
							SO_SNDTIMEO,
							&tm,
							sizeof(tm)
							);

	if (setsockopt_return==-1){
	        std::ostringstream exception_description;
			exception_description<<"generic_socket::create() setsockopt (...SO_SNDTIMEO...) error : "<<strerror( errno);
			if (shutdown(sock, 1)!=0) {
               exception_description<<"\nCan`t shutdown socket.  Error : "<<strerror( errno);
			};
			throw socket_exception (exception_description.str());
		};

	if (setsockopt_return!=0) return false;
	
	
	return true;
}


bool generic_socket::send (const std::vector<byte> buffer) throw (socket_exception){
   	std::ostringstream exception_description;

	if ( ! is_valid() )
       throw socket_exception ("generic_socket::send() socket not valid");

	if (buffer.empty())
        throw socket_exception ("generic_socket::send() buffer to sending is empty");

	int send_status = ::send( sock, 
								  &buffer[0],
								  buffer.size(),
								  0);

	if (send_status == -1){
			std::string message="generic_socket::send() ::send(...) error : ";
			message+=strerror( errno);
			throw socket_exception (message);
		}

	if (static_cast<std::vector<byte>::size_type>(send_status)<buffer.size()){
			throw socket_exception ("generic_socket::send() ::send(...) error : send_status<buffer.size()");
		}
	return true;
}

int generic_socket::recv (
                std::vector<byte> &received_bytes_buffer,
                int bytes_to_receive_count) throw (socket_exception) {
    int  recv_status=0, received_bytes_count=0;
    std::vector<byte> tmp_buffer(bytes_to_receive_count);
    std::vector<byte>::iterator tmp_iterator;
   	std::ostringstream exception_description;

	if ( ! is_valid())
       throw socket_exception ("generic_socket::recv(...)  socket not valid");

	if (bytes_to_receive_count==0)
       throw socket_exception ("generic_socket::recv(...) trying to receive 0 bytes");

      do {
              recv_status = ::recv ( sock, 
										 	     &tmp_buffer[0],
											     bytes_to_receive_count,
											     0);

                if (recv_status<0){
			            exception_description<<"generic_socket::recv() ::recv(...) error : "<<strerror( errno);
                        throw socket_exception (exception_description.str());
                 }

               if (recv_status==0)
                              break;

                tmp_iterator=tmp_buffer.begin();
                advance(tmp_iterator, recv_status);
                received_bytes_buffer.insert(received_bytes_buffer.end(), tmp_buffer.begin(), tmp_iterator);
                received_bytes_count+=recv_status;

     } while(received_bytes_count<bytes_to_receive_count); 

	if (bytes_to_receive_count!=received_bytes_count){
			exception_description<<"generic_socket::recv (...) : (received bytes count == "<<received_bytes_count;
			exception_description<<" ) != (bytes_to_recieve_count == "<<bytes_to_receive_count<<" )";
			throw socket_exception(exception_description.str());
		};

	return received_bytes_count;
}

}