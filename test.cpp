#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cerrno>
#include <cstring>

using namespace std;

int main(void) {
	ifstream in("config/ops.list");
	string line, name, pass;

	if (!in)
			throw std::runtime_error(strerror(errno));
	while (getline(in, line))
	{
		size_t pos;
    	while ((pos = line.find('\'')) != string::npos)
		{
    		line.replace(pos, 1, " ");
		}
		istringstream is(line);
		is >> name >> pass;
		cout << name << ',' << pass << endl;
	}
	return 0;
}