// stdinLogCli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <string>
#include <regex>


/**
 * @brief This function reads lines from the standard input and stores them in a string, ignoring lines that start with '#' or ';'.
 *
 * @param str This is a reference to a std::wstring where the function will store the line read from the standard input.
 *
 * @details The function clears the input string if the standard input is not at the end-of-file. It then reads lines from the standard input until it reaches the end-of-file or a line that does not start with '#' or ';'. The function stores the line read in the input string and outputs it to the standard output.
 *
 * @remarks This function is used in the main function to read options and queries from the standard input. It is designed to ignore comment lines, which start with '#' or ';'. This allows the user to include comments in the input.
 */
void getLineWithComments(std::wstring &str)
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

/**
 * @brief This is the main function of the program. It reads options and queries from the standard input, constructs a command line, and executes it.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of wide string command-line arguments.
 *
 * @return Returns 0 upon successful execution.
 *
 * @details This function first checks if the standard input is available and if the program was called with one argument being "-". If so, it reads options and queries from the standard input using the getLineWithComments function. It then constructs a command line using the read options and queries, and executes it using the CreateProcess function. If the execution is successful, it retrieves and outputs the exit code of the executed process.
 *
 * @remarks This function is designed to be used with the logcli command-line tool for querying logs. It assumes that the standard input contains the options and queries for logcli, and that the options and queries are separated by lines starting with '#' or ';'. If the standard input is not available or the program was not called with one argument being "-", the function will use default options and queries.
 */
int wmain(int argc, wchar_t *argv[])
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
        std::wcout << L"# | regexp `(?s)(\\] | [^ ]{2} )(?P<msg>.*)` | line_format `{{.computer}} {{.module}} {{.msg}}` | label_format msg=`` " << std::endl
                   << std::endl;
    }

    std::wcout << L"# options: " << options << std::endl;
    std::wcout << L"# query: " << query << std::endl;

    commandline += options;
    auto commentCommandline = commandline;

    if (!query.empty())
    {
        std::wsmatch m;
        std::wregex re(LR"(.*?\})"); // Non-greedy match up to the first closing brace

        if (std::regex_search(query, m, re))
        {
            std::wstring selector = m[0];              // The matched substring including the closing brace
            std::wstring pureQuery = m.suffix().str(); // The remainder of the string after the closing brace

            selector = std::regex_replace(selector, std::wregex(LR"(")"), LR"(`)");
            pureQuery = std::regex_replace(pureQuery, std::wregex(LR"(")"), LR"(\x{22})");
            commandline += L" \"" + selector + pureQuery + L"\"";

            if (commandline.find(L"--output=jsonl") != std::wstring::npos)
            {
                commandline = LR"(cmd /c )" + commandline + LR"( | jq -r ".timestamp,.line" | sed ':begin;$!N;s/\(2024.*Z\)\n/\1 /;tbegin;P;D')";
            }
            else if (commentCommandline.find(L"--output=default") != std::wstring::npos)
            {
                commandline = LR"(cmd /c )" + commandline + LR"( | sed -E 's/^(....-..-..T..:..:..[Z+0123456789:]{1,6}) \{.*?"\} *(.*)$/\1 \2/g')";
            }
        }
    }

    std::wcout << L"# commandline: " << commandline << std::endl;
    std::wcout << L"# confluence: https://confluence.thinprint.de/x/WAIgC" << std::endl;
    std::wcout << L"# help *options:* https://grafana.com/docs/loki/latest/getting-started/logcli/" << std::endl;
    std::wcout << L"# help *query:*   https://grafana.com/docs/loki/latest/logql/" << std::endl;
    std::wcout << L"# help go regexp:*   https://pkg.go.dev/regexp/syntax" << std::endl
               << std::endl;

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
