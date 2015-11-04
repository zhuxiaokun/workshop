
--#######################################################
--#              这个脚本的使用方法                     #
--#                                                     #
--# lua > ldofile('a.lua', yyyymmdd)                    #
--#                                                     #
--# 例如:                                               #
--# lua > ldofile('a.lua', 20151029)                    #
--#                                                     #
--#######################################################


local yyyymmdd = 0
local androidDir = 'D:\\Data\\Android'
local appDir = 'D:\\Data\\App'

local argc = select('#',...)
if argc ~= 1 then
	print('usage: ldofile(\'a.lua\',yyyymmdd)')
	yyyymmdd = to_yyyymmdd(getFirstDayTime(0, -1))
	print('use default yyyymmdd', yyyymmdd)
else
	yyyymmdd = select(1,...)
end

local t1 = {yyyymmdd=yyyymmdd,platform='android',dir=androidDir,truncate=true}
ldofile('..\\settings\\log_analysis_server\\analysis.lua', t1)

local t2 = {yyyymmdd=yyyymmdd,platform='app',dir=appDir,truncate=false}
ldofile('..\\settings\\log_analysis_server\\analysis.lua', t2)
