#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <QProcess>

STARTUPINFO sti = {0};
SECURITY_ATTRIBUTES sats = {0};
PROCESS_INFORMATION pi = {0};
HANDLE pipin_w, pipin_r, pipout_w, pipout_r;
BYTE buffer[2048];
DWORD writ, excode, read, available;


void ConnectToEngine(char* path)
{
   pipin_w = pipin_r = pipout_w = pipout_r = nullptr;
   sats.nLength = sizeof(sats);
   sats.bInheritHandle = TRUE;
   sats.lpSecurityDescriptor = nullptr;

   CreatePipe(&pipout_r, &pipout_w, &sats, 0);
   CreatePipe(&pipin_r, &pipin_w, &sats, 0);

   sti.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
   sti.wShowWindow = SW_HIDE;
   sti.hStdInput = pipin_r;
   sti.hStdOutput = pipout_w;
   sti.hStdError = pipout_w;

   CreateProcess(NULL, path, NULL, NULL, TRUE,0, NULL, NULL, &sti, &pi);
//   CreateProcess()
}


std::string getNextMove(std::string position)
{
    std::string str;
    position = "position startpos moves "+position+"\ngo\n";

    WriteFile(pipin_w, position.c_str(), position.length(),&writ, nullptr);
    Sleep(500);

    PeekNamedPipe(pipout_r, buffer,sizeof(buffer), &read, &available, nullptr);
    do
    {
        ZeroMemory(buffer, sizeof(buffer));
        if(!ReadFile(pipout_r, buffer, sizeof(buffer), &read, nullptr) || !read) break;
        buffer[read] = 0;
        str+=(char*)buffer;
    }
    while(read >= sizeof(buffer));

    int n = str.find("bestmove");
    if (n!=-1) return str.substr(n+9,4);

    return "error";
}


void CloseConnection()
{
    WriteFile(pipin_w, "quit\n", 5,&writ, nullptr);
    if(pipin_w != nullptr) CloseHandle(pipin_w);
    if(pipin_r != nullptr) CloseHandle(pipin_r);
    if(pipout_w != nullptr) CloseHandle(pipout_w);
    if(pipout_r != nullptr) CloseHandle(pipout_r);
    if(pi.hProcess != nullptr) CloseHandle(pi.hProcess);
    if(pi.hThread != nullptr) CloseHandle(pi.hThread);
}


#endif

