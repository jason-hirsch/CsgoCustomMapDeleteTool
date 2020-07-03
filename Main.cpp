#include "deleter.h"


int main() {
	ShowWindow(GetConsoleWindow(), SW_HIDE);

	std::ofstream fout("log.txt");
	fout.close();

	if (readCfg()) deleteLoop();

	return 0;
}

