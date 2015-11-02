
--LOG_FATAL = 1
--LOG_ERROR = 2
--LOG_WARN  = 4
--LOG_INFO  = 8
--LOG_DEBUG = 16
--LOG_CONSOLE = (1<<31)
--LOG_ALL   = (~0)

application_configure = {
	log_level = LOG_FATAL+LOG_ERROR+LOG_WARN+LOG_CONSOLE,
	status_file = '..\\.log_extract_status_2.dat',
	log_process_server = {ip='127.0.0.1',port=20000},
	watch_processes = {'GameServer.exe'},
	watch_files = {'Analysis.log'},
	watch_folders = { },
	
	searchFilePath = 1,
	log_dir = 'D:\\tmp\\Data\\Analysis20151030',
	yyyymmdd = 20151020,
	isPathAccept = function(self, filePath)
		local b, e = string.find(filePath, 'Analysis_%d+.log')
		return b ~= nil
	end,
}
