#pragma once

#include <iostream>
#include <Windows.h>

#include <Eigen/Dense>

class ConsoleDebug
{
public:
	static ConsoleDebug* console;
	static bool consoleActive;

	static void Init();

	std::string Buffer() { return buffer; }
	void Clear() {
		buffer = "";
		rewind(stdout);
	}

	void Start() {
		std::exception e("Exception: Already an console is allocated.");

		if (!Allocation) {
			Clear();
			Allocation = true;
			AllocConsole();
			SetWindowPos(GetConsoleWindow(), 0, 0, 0, 400, 900, SWP_NOZORDER);
		}
		else throw e;
	}

	void Close() {
		std::exception e("Exception: Already an console is free.");

		if (Allocation) {
			Clear();
			Allocation = false;
			FreeConsole();
		}
		else throw e;
	}

	void WideCharEncode(UINT _CodePage) { Encode = _CodePage; }
	friend ConsoleDebug& operator<<(ConsoleDebug& cd, long long arg) noexcept {
		if (!cd.Allocation) return cd;
		char buf[30];
		sprintf_s(buf, "%ld", arg);
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, std::strlen(buf), 0, 0);
		cd.buffer += buf;
		return cd;
	}

	friend ConsoleDebug& operator<<(ConsoleDebug& cd, int arg) noexcept {
		if (!cd.Allocation) return cd;
		char buf[30];
		sprintf_s(buf, "%d", arg);
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, std::strlen(buf), 0, 0);
		cd.buffer += buf;
		return cd;
	}

	friend ConsoleDebug& operator<<(ConsoleDebug& cd, float arg) noexcept {
		if (!cd.Allocation) return cd;
		char buf[30];
		sprintf_s(buf, "%f", arg);
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, std::strlen(buf), 0, 0);
		cd.buffer += buf;
		return cd;
	}

	friend ConsoleDebug& operator<<(ConsoleDebug& cd, double arg) noexcept {
		if (!cd.Allocation) return cd;
		char buf[30];
		sprintf_s(buf, "%lf", arg);
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, std::strlen(buf), 0, 0);
		cd.buffer += buf;
		return cd;
	}

	friend ConsoleDebug& operator<<(ConsoleDebug& cd, char arg) noexcept {
		if (!cd.Allocation) return cd;
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), &arg, 1, 0, 0);
		cd.buffer += arg;
		return cd;
	}

	friend ConsoleDebug& operator<<(ConsoleDebug& cd, const char* arg) noexcept {
		if (!cd.Allocation) return cd;
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), arg, std::strlen(arg), 0, 0);
		cd.buffer += arg;
		return cd;
	}

	friend ConsoleDebug& operator<<(ConsoleDebug& cd, const wchar_t* arg) noexcept {
		if (!cd.Allocation) return cd;
		const int len = std::wcslen(arg) + 1;
		char* mul = new char[len];
		memset(mul, 0x00, len);

		WideCharToMultiByte(cd.Encode, 0, arg, len, mul, len, 0, 0);
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), mul, std::strlen(mul), 0, 0);
		cd.buffer += mul;

		delete[] mul;
		return cd;
	}

	friend ConsoleDebug& operator<<(ConsoleDebug& cd, std::string const& arg) noexcept {
		if (!cd.Allocation) return cd;
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), arg.c_str(), arg.size(), 0, 0);
		cd.buffer += arg;
		return cd;
	}

	friend ConsoleDebug& operator<<(ConsoleDebug& cd, Eigen::MatrixXd& arg) noexcept {
		if (!cd.Allocation) return cd;
		char buf[30];
		cd << "Matrix\n";
		for (int j = 0; j < arg.rows(); j++)
		{
			cd << "[ ";
			for (int i = 0; i < arg.cols(); i++)
			{
				sprintf_s(buf, i == 0 ? "%10.3lf" : "%10.3lf", arg(j, i));
				WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), buf, std::strlen(buf), 0, 0);
				cd.buffer += buf;
			}
			cd << " ]\n";
		}
		return cd;
	}

	ConsoleDebug() noexcept { }
	explicit(true) ConsoleDebug(bool _Begin = false, UINT _CodePage = CP_ACP) noexcept {
		Encode = _CodePage;
		if (_Begin) Start();
	}
	~ConsoleDebug() noexcept {
		Encode = CP_ACP;
		if (Allocation) {
			Clear();
			Allocation = false;
			FreeConsole();
		}
	}

private:
	std::string buffer;
	bool Allocation = false;
	UINT Encode = CP_ACP;
};

void debug(LPCTSTR str);

