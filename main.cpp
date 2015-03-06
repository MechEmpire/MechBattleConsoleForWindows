#include <iostream>
#include <stdio.h>
#include <tchar.h>

#include <atlstr.h>
#include <conio.h>

#include "BattleCoreOutput.h"


#define MAX_AI_NUM 2

//win
#pragma comment(lib,"BattleCore.lib")	//链接库文件

using namespace std;

int main(int argc,char *argv[])
{
	//Former test battle methods
	//LoadGameData();
	//StartTestingBattleWithAssignedEquipment(1,0,6,3);

	
	//the command line input is provided by server program
	//seems no need to prove its correctness
	//command line: route_for_battleMode_config  numAIs  route_for_ai_0  route_for_ai_1 ...
	//				0 							 1 			2				3	

	if(argc<5)
	{
		//error occurs
		cout<<"too few inputs!\n";
		return -1;
	}

	cout<<"converting AI nums...\n";


	int numAIs;

	numAIs=atoi(argv[2]);
	//cout<<argc<<'\n';
	//cout<<argv[0]<<'\n';
	//cout<<argv[1]<<'\n';
	//cout<<numAIs<<'\n';
	if(numAIs<=1 || numAIs>MAX_AI_NUM)
	{
		//error occurs
		cout<<"numAIs input error!\n";
		return -2;
	}

	if(numAIs!=argc-3)
	{
		//error occurs
		cout<<"num of input AI routes is not consistent with numAIs!\n";
		return -3;
	}




	cout<<"loading game lua data...\n";

	//TODO: load battleMode config
	LoadGameData();

	cout<<"initializing new battle...\n";
	InitNewBattle();

	//StartTestingBattleWithRandomEquipment();
	
	cout<<"setting config file...\n";
	cout<<argv[1]<<'\n';
	SetBattleModeWithConfigFile(argv[1]);


	cout<<"loading AI agents...\n";


	//load AIs
	int i;
	//void * ai_handle[MAX_AI_NUM];
	HINSTANCE hdll[MAX_AI_NUM];
	typedef RobotAI_Interface* (*Export)();
	typedef void (*FreeRobotAIPointer)(RobotAI_Interface*);
	Export exportAIFunction[MAX_AI_NUM];
	FreeRobotAIPointer freeAIFunction[MAX_AI_NUM];
	RobotAI_Interface* pAI[MAX_AI_NUM];

	for(i=0;i<numAIs;i++)
	{
		CString dllname(argv[i+3]);
		hdll[i]=LoadLibrary(dllname.AllocSysString());
		if(hdll[i]==NULL)
		{
			//加载失败
			//assert("加载"+dll_name+".dll失败！"&&0);
			cout<<"loading dll "<<argv[i+3]<<" fails!";
			return false;
		}
		
		
		exportAIFunction[i] = (Export)GetProcAddress(hdll[i],"Export");
		freeAIFunction[i] = (FreeRobotAIPointer)GetProcAddress(hdll[i],"FreeRobotAIPointer");
		pAI[i] = (*exportAIFunction[i])();
	}

	
	//TODO: Add AIs to battlefield and 
	for(i=0;i<numAIs;i++)
	{
		AddRobotAI(pAI[i]);		//??return ai index? need to store?
	}

	cout<<"launching battle...\n";
	LaunchBattle();




	//free
	for(i=0;i<numAIs;i++)
	{
		(*freeAIFunction[i])(pAI[i]);
		//dlclose(ai_handle[i]);
		FreeLibrary(hdll[i]);
	}
	

	return 0;
}