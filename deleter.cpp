#include "deleter.h"


std::string csgoPath;
std::vector<std::string> tags;


void log(std::string str)
{
	std::ofstream fout("log.txt", std::ofstream::app);
	if (!fout.is_open()) return;
	fout << str << std::endl;
	fout.close();
}


bool readCfg()
{
	csgoPath = "";
	tags = {};
	std::string currentLine;
	std::string currentKey;
	std::string currentValues;
	std::string value;
	std::vector<std::string> values;

	log("Reading config file...");

	std::ifstream fin("settings.cfg"); //Open settings file
	if (!fin.is_open())
	{
		log("Error: settings.cfg does not exist. Example file is\n\tPath: C:/Program Files (x86)/Steam/steamapps/common/Counter-Strike Global Offensive\n\tTags: bhop_, surf_,");
		return false;
	}

	while (std::getline(fin, currentLine))
	{
		log("Reading line: " + currentLine);
		if (currentLine.at(0) == '#') continue; //Skip commented lines
		currentKey = currentLine.substr(0, currentLine.find(":") + 1); //Get line key
		currentValues = currentLine.substr(currentLine.find(":") + 2); //Get line value

		for (auto& c : currentValues) //Extract the values for the key
		{
			if (c == ',')
			{
				values.push_back(value);
				value.clear();
			}
			else
			{
				value += c;
			}
		}
		values.push_back(value);

		if (currentKey.compare("Path:") == 0) //If the key is "Path:" set the csgo path
		{
			log("Csgo path read: " + values.at(0));
			csgoPath.insert(0, values.at(0));
		}
		if (currentKey.compare("Tags:") == 0) //If the key is "Tags:" add the values to the tags vector
		{
			for (auto& str : values)
			{
				if (str.at(0) == ' ') str = str.substr(1);
				log("Tag read: " + str);
				tags.push_back(str);
			}
		}

		value.clear();
		values.clear();
	}
	csgoPath += "/csgo/maps";
	log("Config file read");
	return true;
}


int isCsgoRunning()
{
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		log("Error: Process snapshot failed to be created");
		return -1;
	}

	PROCESSENTRY32 proc;
	proc.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(snapshot, &proc)) {
		log("Error: Failed to get first process of process snapshot");
		CloseHandle(snapshot);
		return -1;
	}

	do {
		size_t sz = wcslen(proc.szExeFile) + 1;
		char* nameP = new char[sz];
		size_t temp;
		if (wcstombs_s(&temp, nameP, sz, proc.szExeFile, sz) != 0)
		{
			log("Error: Failed to convert wide string to multibyte string while checking if csgo is running");
			return -1;
		}
		std::string ret(nameP);
		delete[] nameP;
		if (ret == "csgo.exe")
		{
			CloseHandle(snapshot);
			return 1;
		}
	} while (Process32Next(snapshot, &proc));

	CloseHandle(snapshot);
	return 0;
}


void deleteLoop()
{
	log("Map deletion loop started");
	try //Catch error is path is incorrect
	{
		while (true)
		{
			int running = isCsgoRunning();
			if (running == 0)
			{
				for (auto& file : std::filesystem::directory_iterator(csgoPath))
				{
					for (auto& tag : tags)
					{
						if (file.path().filename().string().find(tag) != std::string::npos)
						{
							log("Removing file: " + file.path().filename().string());
							std::filesystem::remove(file);
						}
					}
					Sleep(10);
				}
			}
			else if (running == -1)
			{
				log("Error: There was an error while checking if csgo is running");
			}
			Sleep(60000);
		}
	}
	catch (...)
	{
		log("Error: Csgo Path is invalid");
		return; 
	}
}

