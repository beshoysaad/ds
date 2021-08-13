// this is the lock server
// the lock client has a similar interface

#ifndef lock_server_h
#define lock_server_h

#include <string>
#include "lock_protocol.h"
#include "lock_client.h"
#include "rpc.h"
#include <map>
#include <pthread.h>

typedef enum _lock_status_t
{
	LOCK_FREE, LOCK_LOCKED
} lock_status_t;

typedef struct _lock_t
{
	lock_status_t status;
	pthread_mutex_t m;
	pthread_cond_t c;
} lock_t;

class lock_server
{

protected:
	std::map<lock_protocol::lockid_t, lock_t*> map_locks;
	int nacquire;

public:
	lock_server();
	~lock_server()
	{
	}
	;
	lock_protocol::status stat(int clt, lock_protocol::lockid_t lid, int &);
	lock_protocol::status acquire(int clt, lock_protocol::lockid_t lid, int &);
	lock_protocol::status release(int clt, lock_protocol::lockid_t lid, int &);
	lock_protocol::status subscribe(int clt, lock_protocol::lockid_t lid,
			int &);
};

#endif 

