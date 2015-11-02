
--LOG_FATAL = 1
--LOG_ERROR = 2
--LOG_WARN  = 4
--LOG_INFO  = 8
--LOG_DEBUG = 16
--LOG_CONSOLE = (1<<31)
--LOG_ALL   = (~0)

application_configure = {
	log_level = LOG_FATAL+LOG_ERROR+LOG_WARN+LOG_CONSOLE,
	
	-- 测试用，可以只是处理某个Server里面的文件
	getLogDir = function(self, dataDir, platformName, serverName)
		--if serverName ~= 'Server1094' then
		--	return nil
		--end
		return string.format('%s\\%s\\log', dataDir, serverName)
	end,
}
