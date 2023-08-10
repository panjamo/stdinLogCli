// stdinLogCli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <string>
#include <regex>


void getLineWithComments(std::wstring& str)
{
    if (std::wcin)
        str.clear();
    while (std::wcin)
    {
        getline(std::wcin, str);
        if (!std::wcin)
            break;
        std::wcout << str << std::endl;
        if (str[0] != L'#' && str[0] != L';')
            break;
    }
}

int wmain(int argc, wchar_t* argv[])
{
    std::wstring commandline = L"logcli query ";
    std::wstring options = L"";
    std::wstring query = L"";

    if (std::wcin && argc == 2 && wcscmp(argv[1], L"-") == 0)
    {
        std::wstring input_line;

        getLineWithComments(options);

        if (std::wcin)
            query.clear();
        while (std::wcin)
        {
            getLineWithComments(input_line);

            if (input_line.empty())
                break;
            else
                query += input_line;
        }
    }
    if (options.empty())
    {
        options = L"--timezone=UTC --since=1h --limit=1000 --output=default --quiet";
        std::wcout << options << std::endl;
        std::wcout << L"# --timezone=Local --since=1h --limit=1000 --output=raw" << std::endl;
    }
    if (query.empty())
    {
        query = L"{component=~`renderserver|EndToEndTests`,environment=`tst`}|~ `(?i)error`";
        std::wcout << L"{component=~`renderserver|EndToEndTests`,environment=`tst`}" << std::endl;
        std::wcout << L"|~ `(?i)error`" << std::endl;
        std::wcout << L"# | regexp `(?s)(\\] | [^ ]{2} )(?P<msg>.*)` | line_format `{{.computer}} {{.module}} {{.msg}}` | label_format msg=`` " << std::endl << std::endl;
    }

    std::wcout << L"# options: " << options << std::endl;
    std::wcout << L"# query: " << query << std::endl;
    // query = std::regex_replace(query, std::regex("\\\\"), "\\\\");
    query = std::regex_replace(query, std::wregex(L"\""), L"\\\"");
    commandline += options;
    if (!query.empty())
        commandline += L" \"" + query + L"\"";

    std::wcout << L"# commandline: " << commandline << std::endl;
    std::wcout << L"# confluence: https://confluence.thinprint.de/x/WAIgC" << std::endl;
    std::wcout << L"# help *options:* https://grafana.com/docs/loki/latest/getting-started/logcli/" << std::endl;
    std::wcout << L"# help *query:*   https://grafana.com/docs/loki/latest/logql/" << std::endl;
    std::wcout << L"# help go regexp:*   https://pkg.go.dev/regexp/syntax" << std::endl << std::endl;

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
