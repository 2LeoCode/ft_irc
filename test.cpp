#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <cstdlib>

using namespace std;

std::string randomString( size_t size )
{
	std::string ret;
	size_t added;
	long randNum;
	char *ptr;

	while (ret.size() < size)
	{
		randNum = random();
		ptr = reinterpret_cast< char* >(&randNum);
		for (int i = 0; ret.size() < size && i < sizeof(long) / sizeof(char); ++i)
		{
			if (isgraph(*ptr))
				ret += *ptr;
			randNum <<= sizeof(char) * 8;
		}
	}
	return ret;
}

int main(void)
{
	srandom(time(NULL));
	cout << randomString(10) << endl;
	return 0;
}
