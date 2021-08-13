// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server() :
		nacquire(0)
{
}

lock_protocol::status lock_server::stat(int clt, lock_protocol::lockid_t lid,
		int &r)
{
	lock_protocol::status ret = lock_protocol::OK;
	r = nacquire;
	return ret;
}

lock_protocol::status lock_server::acquire(int clt, lock_protocol::lockid_t lid,
		int &r)
{
	std::map<lock_protocol::lockid_t, lock_t*>::iterator it = map_locks.find(
			lid);
	if (it == map_locks.end())
	{ // new lock
		lock_t *l = (lock_t*) malloc(sizeof(lock_t));
		assert(l != NULL);
		assert(pthread_mutex_init(&l->m, NULL) == 0);
		pthread_mutex_lock(&l->m);
		l->status = LOCK_LOCKED;
		pthread_mutex_unlock(&l->m);
		assert(pthread_cond_init(&l->c, NULL) == 0);
		map_locks.insert(std::make_pair(lid, l));
	}
	else
	{ // existing lock
		lock_t *l = it->second;
		pthread_mutex_lock(&l->m);
		while (l->status == LOCK_LOCKED)
		{
			pthread_cond_wait(&l->c, &l->m);
		}
		l->status = LOCK_LOCKED;
		pthread_mutex_unlock(&l->m);
	}
	return lock_protocol::OK;
}

lock_protocol::status lock_server::release(int clt, lock_protocol::lockid_t lid,
		int &r)
{
	std::map<lock_protocol::lockid_t, lock_t*>::iterator it = map_locks.find(
			lid);
	if (it == map_locks.end())
	{ // lock not found
		return lock_protocol::NOENT;
	}
	else
	{ // lock found
		lock_t *l = it->second;
		pthread_mutex_lock(&l->m);
		l->status = LOCK_FREE;
		pthread_cond_signal(&l->c);
		pthread_mutex_unlock(&l->m);
	}
	return lock_protocol::OK;
}

lock_protocol::status lock_server::subscribe(int clt,
		lock_protocol::lockid_t lid, int &r)
{
	return lock_protocol::RPCERR;
}
