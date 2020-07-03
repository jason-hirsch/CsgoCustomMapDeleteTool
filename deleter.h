#pragma once

#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <TlHelp32.h>


extern std::string csgoPath;
extern std::vector<std::string> tags;

void log(std::string str);

bool readCfg();

int isCsgoRunning();

void deleteLoop();
