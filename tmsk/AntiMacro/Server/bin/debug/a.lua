
local yyyymmdd = 0

local argc = select('#',...)
if argc ~= 1 then
	print('usage: ldofile(\'a.lua\',yyyymmdd)')
	yyyymmdd = to_yyyymmdd(getFirstDayTime(0, -1))
	print('use default yyyymmdd', yyyymmdd)
else
	yyyymmdd = select(1,...)
end

local t1 = {yyyymmdd=yyyymmdd,platform='android',dir='D:\\Data\\Android',truncate=true}
ldofile('..\\settings\\log_analysis_server\\analysis.lua', t1)

local t2 = {yyyymmdd=yyyymmdd,platform='app',dir='D:\\Data\\App',truncate=false}
ldofile('..\\settings\\log_analysis_server\\analysis.lua', t2)
