// yfs client.  implements FS operations using extent and lock server
#include "yfs_client.h"
#include "extent_client.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const yfs_client::inum ROOT_INUM = 1;

yfs_client::yfs_client(std::string extent_dst, std::string lock_dst)
{
	ec = new extent_client(extent_dst);
	lc = new lock_client(lock_dst);
	yfs_client::dirinfo inf;
	if (getdir(ROOT_INUM, inf) != OK)
	{
		lc->acquire(ROOT_INUM);
		assert(put(ROOT_INUM, "") == OK);
		lc->release(ROOT_INUM);
	}
}

yfs_client::inum yfs_client::n2i(std::string n)
{
	std::istringstream ist(n);
	unsigned long long finum;
	ist >> finum;
	return finum;
}

std::string yfs_client::filename(inum inum)
{
	std::ostringstream ost;
	ost << inum;
	return ost.str();
}

bool yfs_client::isfile(inum inum)
{
	if (inum & 0x80000000)
		return true;
	return false;
}

bool yfs_client::isdir(inum inum)
{
	return !isfile(inum);
}

yfs_client::inum yfs_client::ilookup(yfs_client::inum di, std::string name)
{
	if (yfs_client::isdir(di))
	{
		std::string buff;
		if (getbuff(di, buff) == yfs_client::OK)
		{
			size_t idx = buff.find(name + ";");
			if (idx != std::string::npos)
			{
				size_t idx2 = buff.find(";", idx);
				idx2++;
				assert(idx2 != std::string::npos);
				size_t idx3 = buff.find(";", idx2);
				assert(idx3 != std::string::npos);
				std::string temp = buff.substr(idx2, idx3 - idx2);
				return std::stoull(temp);
			}
		}
	}
	return 0;
}

int yfs_client::getfile(inum inum, fileinfo &fin)
{
	int r = OK;

	extent_protocol::attr a;
	if (ec->getattr(inum, a) != extent_protocol::OK)
	{
		r = IOERR;
		goto release;
	}

	fin.atime = a.atime;
	fin.mtime = a.mtime;
	fin.ctime = a.ctime;
	fin.size = a.size;

	release:

	return r;
}

int yfs_client::getdir(inum inum, dirinfo &din)
{
	int r = OK;

	extent_protocol::attr a;
	if (ec->getattr(inum, a) != extent_protocol::OK)
	{
		r = IOERR;
		goto release;
	}
	din.atime = a.atime;
	din.mtime = a.mtime;
	din.ctime = a.ctime;

	release: return r;
}

int yfs_client::getbuff(inum inum, std::string &str)
{
	if (ec->get(inum, str) != extent_protocol::OK)
	{
		return IOERR;
	}
	return OK;

}

int yfs_client::put(inum inum, std::string buff)
{
	if (ec->put(inum, buff) != extent_protocol::OK)
	{
		return IOERR;
	}
	return OK;
}

int yfs_client::erase(inum inum)
{
	if (ec->remove(inum) != extent_protocol::OK)
	{
		return IOERR;
	}
	return OK;
}

int yfs_client::acquire(inum inum)
{
	lock_protocol::status st;
	st = lc->acquire(inum);
	if (st != lock_protocol::OK)
	{
		return IOERR;
	}
	return OK;
}

int yfs_client::release(inum inum)
{
	if (lc->release(inum) != lock_protocol::OK)
	{
		return IOERR;
	}
	return OK;
}

