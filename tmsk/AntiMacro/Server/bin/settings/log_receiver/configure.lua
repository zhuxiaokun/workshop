
--LOG_FATAL = 1
--LOG_ERROR = 2
--LOG_WARN  = 4
--LOG_INFO  = 8
--LOG_DEBUG = 16
--LOG_CONSOLE = (1<<31)
--LOG_ALL   = (~0)

application_configure = {
	log_level = LOG_FATAL+LOG_ERROR+LOG_WARN+LOG_CONSOLE+LOG_DEBUG,
	listen = {ip='127.0.0.1',port=20000},
	data_dir = 'd:\\tmp\\Data',
	platform_name = 'test2',
	base_filename = 'Analysis.log',
}
