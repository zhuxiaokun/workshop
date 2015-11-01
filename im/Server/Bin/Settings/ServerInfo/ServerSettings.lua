
ServerSettings={
	DemoChatServer=
	{
		LogLevel = 0x0fff,
		BindAddress={ public={ip='192.168.8.116',port=20000}, },
		MaxClient=10240,
		DatabaseThreadCount=5,
		Database={host='192.168.8.116',username='GameAdmin',dbname='jg_chat',password='jiguang',charset='utf8'},
		Storage=
		{
			Location = './Data',
			ThreadCount = 2,
		},
	},
}
