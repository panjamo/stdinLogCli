// stdinLogCli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <string>
#include <regex>

int wmain(int argc, wchar_t* argv[])
{
    std::wstring commandline = L"logcli query ";
    std::wstring options = L"--timezone=UTC --since=1h --limit=30";
    std::wstring query = L"{component=`renderserver`,environment=`tst`}";

    if (std::wcin && argc == 2 && wcscmp(argv[1], L"-") == 0 )
    {
        std::wstring input_line;

        // read options
        if (std::wcin)
            options.clear();
        while (std::wcin)
        {
            getline(std::wcin, options);
            if (!std::wcin)
                break;
            std::wcout << options << std::endl;
            if (options[0] != L'#' && options[0] != L';')
                break;
        }

        // read query (multipe lines, ends with empty line or EOF
        if (std::wcin)
            query.clear();
        while (std::wcin)
        {
            getline(std::wcin, input_line);
            if (!std::wcin)
                input_line.clear();

            std::wcout << input_line << std::endl;

            if (input_line[0] == L'#' || input_line[0] == L';')
                continue;

            if (input_line.empty())
                break;
            else
                query += input_line;
        }

    }
    else
    {
        std::wcout << options << std::endl;
        std::wcout << query << std::endl << std::endl;
    }

    std::wcout << L"# options: " << options << std::endl;
    std::wcout << L"# query: " << query << std::endl;
    // query = std::regex_replace(query, std::regex("\\\\"), "\\\\");
    query = std::regex_replace(query, std::wregex(L"\""), L"\\\"");
    commandline += options + L" \"" + query + L"\"";

    std::wcout << L"# commandline: " << commandline << std::endl;
    std::wcout << L"# help *options:* https://grafana.com/docs/loki/latest/getting-started/logcli/" << std::endl;
    std::wcout << L"# help *query:*   https://grafana.com/docs/loki/latest/logql/" << std::endl << std::endl;

    PROCESS_INFORMATION pi{};
    STARTUPINFO si{};
    si.cb = sizeof(si);
    SetStdHandle(STD_ERROR_HANDLE, GetStdHandle(STD_OUTPUT_HANDLE));
    if (::CreateProcess(nullptr, (LPWSTR)commandline.c_str(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
    {
        DWORD exitCode;
        if (WAIT_OBJECT_0 == WaitForSingleObject(pi.hProcess, 100 * 1000))
        {
            if (GetExitCodeProcess(pi.hProcess, &exitCode))
            {
                std::wcout << L"logcli returns: " << exitCode << std::endl;
            }
        }
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }

    return 0;
}
