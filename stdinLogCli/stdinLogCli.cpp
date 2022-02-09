// stdinLogCli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <regex>

int wmain()
{
    std::wstring commandline = L"logcli query ";
    if (std::wcin)
    {
        std::wstring input_line, options, query;

        getline(std::wcin, options);
        std::wcout << options << std::endl;

        do
        {
            getline(std::wcin, input_line);
            std::wcout << input_line << std::endl;

            if (input_line[0] == L'#' || input_line[0] == L';')
                continue;

            if (input_line.empty())
                break;
            else
                query += input_line;
        }
        while (true);

        // query = std::regex_replace(query, std::regex("\\\\"), "\\\\");
        query = std::regex_replace(query, std::wregex(L"\""), L"\\\"");
        commandline += options + L" \"" + query + L"\"";

        std::wcout << L"# " << commandline << std::endl << std::endl;

        SetStdHandle(STD_ERROR_HANDLE, GetStdHandle(STD_OUTPUT_HANDLE));

        PROCESS_INFORMATION pi{};
        STARTUPINFO si{};
        si.cb = sizeof(si);
        if (::CreateProcess(nullptr, (LPWSTR)commandline.c_str(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
        {
            if (WAIT_OBJECT_0 == WaitForSingleObject(pi.hProcess, 100 * 1000))
            {
                DWORD exitCode;
                if (GetExitCodeProcess(pi.hProcess, &exitCode))
                {
                    std::wcout << L"PreExecuteCommandScript returned:" << exitCode << std::endl;
                }
            }
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        }
    }
    return 0;
}
