
dofile('../settings/macro_analysis_server/util.lua')

local reload = true
local minAcctPerDev = 3

local function onEach_DevName(devName, ctx)
	if devAnalysis:loadDev(devName) == nil then
		Log(LOG_CONSOLE+LOG_ERROR, 'error: load data for dev %s', devName)
		return false
	end
	Log(LOG_CONSOLE+LOG_ERROR, 'load dev %s SUCCESS !!', devName)
	return true
end

local function reloadDevData()
	devAnalysis:reset()
	devAnalysis:scanDevList()
	devAnalysis:foreachDevName(onEach_DevName, nil)
end

if reload then
	reloadDevData()
end

local function onEach_DevObject(devObj, ctx)
	local devName = devObj:name()
	local n = devObj:getAcctCount()
	if n >= minAcctPerDev then
		local t = {devName, n}
		table.insert(ctx, t)
	end
	return true
end
local function sort_ipAcctCount(a, b)
	return a[2] > b[2]
end

local t_dev = {}
devAnalysis:foreachDevObject(onEach_DevObject, t_dev)
table.sort(t_dev, sort_ipAcctCount)

Log(LOG_CONSOLE+LOG_ERROR, '>>>>>>>>>>>>>>>>>>>>>>>>>>> 多账号设备列表 >>>>>>>>>>>>>>>>>>>>>>>>')
for i=1,#t_dev,1 do
	local t = t_dev[i]
	local devName = t[1]
	local acctCount = t[2]
	Log(LOG_CONSOLE+LOG_ERROR, '%-4d  %-16s  %d', i, devName, acctCount)
end

local function printDevPlayer(player, ctx)
	local devObj = ctx
	local devName = devObj:name()
	local serverName = player:serverName()
	local acctName = player:acctName()
	local name = player:name()
	local lastActive = time2str(player:lastActive())
	
	local acctObj = analysis:getAcctObject(acctName)
	local level = 0
	local viplevel = 0
	
	if acctObj ~= nil then
		local kplayer = acctObj:getPlayer(serverName, name)
		if kplayer ~= nil then
			level = kplayer:level()
			viplevel = kplayer:viplevel()
		end
	end
	
	Log(LOG_CONSOLE+LOG_ERROR, '%s  %s  %s  %s  %d  %d  %s', serverName, devName, acctName, name, level, viplevel, lastActive)
	return true
end

Log(LOG_ERROR, '>>>>>>>>> 设备中的角色列表 >>>>>>>>>>>>>>>>>>>>>>>>')
for i=1,#t_dev,1 do
	local t = t_dev[i]
	local devName = t[1]
	local devObj = devAnalysis:getDevObject(devName)
	Log(LOG_ERROR, '>>>>>>>> %s >>>>', devName)
	Log(LOG_ERROR, '服务器  设备号  账号  角色  级别  VIP  时间')
	devObj:foreachPlayer(printDevPlayer, devObj)
end

Log(LOG_CONSOLE+LOG_ERROR, '<<<<<<<<<<<<<<<<<<<<<<< OVER !!!! <<<<<<<<<<<<<<<<<<<<<<<<<<')

