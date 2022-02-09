// stdinLogCli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <regex>

int main()
{
    std::string commandline = "logcli query ";
    if (std::cin)
    {
        std::string input_line, options, query;

        getline(std::cin, options);
        std::cout << options << std::endl;

        do
        {
            getline(std::cin, input_line);
            std::cout << input_line << std::endl;

            if (input_line[0] == '#' || input_line[0] == ';')
                continue;

            if (input_line.empty())
                break;
            else
                query += input_line;
        }
        while (true);

        query = std::regex_replace(query, std::regex("\\\\"), "\\\\");
        query = std::regex_replace(query, std::regex("\""), "\\\"");
        commandline += options + " \"" + query + "\"";

        std::cout << "# " << commandline << std::endl << std::endl;

        SetStdHandle(STD_ERROR_HANDLE, GetStdHandle(STD_OUTPUT_HANDLE));

        PROCESS_INFORMATION pi{};
        STARTUPINFOA si{};
        si.cb = sizeof(si);
        if (::CreateProcessA(nullptr, (LPSTR)(LPCSTR)commandline.c_str(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
        {
            if (WAIT_OBJECT_0 == WaitForSingleObject(pi.hProcess, 100 * 1000))
            {
                DWORD exitCode;
                if (GetExitCodeProcess(pi.hProcess, &exitCode))
                {
                    std::cout << "PreExecuteCommandScript returned:" << exitCode << std::endl;
                }
            }
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }
    return 0;
}
