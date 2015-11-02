#pragma once

const int s_nCL_Begin		= 1024;		// client - login
const int s_nCChr_Begin		= 1024*2;	// client - character
const int s_nCS_Begin		= 1024*3;	// client - game
const int s_nCCht_Begin		= 1024*4;	// client - chat
const int s_nLP_Begin		= 1024*5;	// login  - passport
const int s_nChrW_Begin		= 1024*6;	// character - world
const int s_nSW_Begin		= 1024*7;	// game - world
const int s_nSD_Begin		= 1024*8;	// game - dbAgent
const int s_nSA_Begin		= 1024*9;	// game - AIServer
const int s_nWP_Begin		= 1024*10;	// world - passport
const int s_nWL_Begin		= 1024*11;	// world - login
const int s_nDW_Begin		= 1024*12;	// world - DBAgent
const int s_nChatW_Begin	= 1024*13;	// world  - chat

const int s_nLC_Begin		= s_nCS_Begin;
const int s_nChrC_Begin		= s_nCChr_Begin;
const int s_nSC_Begin		= s_nCS_Begin;
const int s_nChtC_Begin		= s_nCCht_Begin;
const int s_PL_Begin		= s_nLP_Begin;
const int s_nWChr_Begin		= s_nChrW_Begin;
const int s_nWS_Begin		= s_nSW_Begin;
const int s_nDS_Begin		= s_nSD_Begin;
const int s_nAS_Begin		= s_nSA_Begin;
const int s_nPW_Begin		= s_nWP_Begin;
const int s_nLW_Begin		= s_nWL_Begin;
const int s_nWD_Begin		= s_nDW_Begin;
const int s_nWChat_Begin	= s_nChatW_Begin;
