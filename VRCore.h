#pragma once
//Basic Includes
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <thread>   
#include <ctime>
#include <map>
#include <list>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include <functional>
#define RAID_ASSERT assert

HWND InitWindow(const char* name, HINSTANCE inst, int show_cmd, int width, int height, bool fullscreen = false);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM w_param, LPARAM l_param);
void StartLoop(std::function<void()> init, std::function<void()> render);

void Init();
void D3D12Render();