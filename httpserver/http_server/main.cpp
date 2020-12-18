/*
 * vzsdk
 * Copyright 2013 - 2018, Vzenith Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(_WIN32)
#include <conio.h>
#elif defined(__linux__)
#include <signal.h>
#include <unistd.h>
#endif

#include "../src/CHttpServerMgr.h"



std::string GetCompileVersion()
{
#ifdef NDEBUG
	const char* configType = "Release";
#else
	const char* configType = "Debug";
#endif

	char buff[100] = { 0 };
	unsigned int nYear, nMonth, nDay;
	sscanf(__DATE__, "%36s %d %d", buff, &nDay, &nYear);
	std::string month_name("JanFebMarAprMayJunJulAugSepOctNovDec");
	std::string::size_type pos = month_name.find(buff);
	nMonth = pos / 3 + 1;

	//编译发行版本时，改GIT_VERSION为GIT代码版本号
	snprintf(buff, sizeof(buff), "Build %4d%.2d%.2d %s %s v", nYear, nMonth, nDay, configType, "test_httpserver");
	return buff;
}

int main(int argc,char* argv[])
{
  	int port = 8888;
	if (argc >= 2)
	{
		int opt;
		while ((opt = getopt(argc, argv, "vp:")) != -1)
		{
			printf("get opt:%d\n", opt);
			switch (opt)
			{
				case 'v':
					printf("%s\r\n", GetCompileVersion().c_str());
					break;
				case 'p':
					port = atoi(optarg);
					break;
				default: /* '?' */
					fprintf(stderr, "Usage: %s [-p port] [-v] version\n",
							argv[0]);
					exit(EXIT_FAILURE);
			}
		}
	}

	if (CHttpServerMgr::GetInstance()->Start() < 0)
		return -1;

	printf("http port:%d\n", port);
	printf(("HTTP服务已启动...\n"));

  CHttpServerMgr::GetInstance()->WebRun(port);
	
	printf("HTTP服务正在停止...\n");
	CHttpServerMgr::GetInstance()->Stop();

	printf("http服务已停止...\n");
	return 0;
}


