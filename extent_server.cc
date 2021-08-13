// the extent server implementation

#include "extent_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extent_server::extent_server()
{
}

int extent_server::put(extent_protocol::extentid_t id, std::string buf, int &)
{
	std::map<extent_protocol::extentid_t, node_t>::iterator i =
			extent_server::fs.find(id);
	unsigned int t = time(NULL);
	if (i == extent_server::fs.end())
	{
		node_t node;
		node.buf = buf;
		node.attr.ctime = t;
		node.attr.mtime = t;
		node.attr.size = buf.size();
		extent_server::fs[id] = node;
	}
	else
	{
		i->second.buf = buf;
		i->second.attr.mtime = t;
		i->second.attr.ctime = t;
		i->second.attr.size = buf.size();
	}
	return extent_protocol::OK;
}

int extent_server::get(extent_protocol::extentid_t id, std::string &buf)
{
	std::map<extent_protocol::extentid_t, node_t>::iterator i =
			extent_server::fs.find(id);
	if (i == extent_server::fs.end())
	{
		return extent_protocol::IOERR;
	}
	else
	{
		unsigned int t = time(NULL);
		buf = i->second.buf;
		i->second.attr.atime = t;
		i->second.attr.ctime = t;
		return extent_protocol::OK;
	}
}

int extent_server::getattr(extent_protocol::extentid_t id,
		extent_protocol::attr &a)
{
	std::map<extent_protocol::extentid_t, node_t>::iterator i =
			extent_server::fs.find(id);
	if (i == extent_server::fs.end())
	{
		return extent_protocol::IOERR;
	}
	else
	{
		a = i->second.attr;
		return extent_protocol::OK;
	}
}

int extent_server::remove(extent_protocol::extentid_t id, int &)
{
	std::map<extent_protocol::extentid_t, node_t>::iterator i =
			extent_server::fs.find(id);
	if (i == extent_server::fs.end())
	{
		return extent_protocol::IOERR;
	}
	else
	{
		extent_server::fs.erase(i);
		return extent_protocol::OK;
	}
}
