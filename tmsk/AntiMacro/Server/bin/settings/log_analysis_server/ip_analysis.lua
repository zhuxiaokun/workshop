
dofile('../settings/macro_analysis_server/util.lua')

local reload = true
local minAcctPerIp = 3

local function onEach_IpName(ipName, ctx)
	if ipAnalysis:loadIp(ipName) == nil then
		Log(LOG_CONSOLE+LOG_ERROR, 'error: load data for ip %s', ipName)
		return false
	end
	Log(LOG_CONSOLE+LOG_ERROR, 'load ip %s SUCCESS !!', ipName)
	return true
end

local function reloadIpData()
	ipAnalysis:reset()
	ipAnalysis:scanIpList()
	ipAnalysis:foreachIpName(onEach_IpName, nil)
end

if reload then
	reloadIpData()
end

local function onEach_IpObject(ipObj, ctx)
	local ipName = ipObj:name()
	local n = ipObj:getAcctCount()
	if n >= minAcctPerIp then
		local t = {ipName, n}
		table.insert(ctx, t)
	end
	return true
end
local function sort_ipAcctCount(a, b)
	return a[2] > b[2]
end

local t_ip = {}
ipAnalysis:foreachIpObject(onEach_IpObject, t_ip)
table.sort(t_ip, sort_ipAcctCount)

Log(LOG_CONSOLE+LOG_ERROR, '>>>>>>>>>>>>>>>>>>>>>>>>>>> 多账号IP列表 >>>>>>>>>>>>>>>>>>>>>>>>')
for i=1,#t_ip,1 do
	local t = t_ip[i]
	local ipName = t[1]
	local acctCount = t[2]
	Log(LOG_CONSOLE+LOG_ERROR, '%-4d  %-16s  %d', i, ipName, acctCount)
end

local function printIpPlayer(player, ctx)
	local ipObj = ctx
	local ipName = ipObj:name()
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
	
	Log(LOG_ERROR, '%s  %s  %s  %s  %d  %d  %s',
		serverName, ipName, acctName, name,
		level, viplevel, lastActive)
		
	return true
end

Log(LOG_ERROR, '>>>>>>>>> IP中的角色列表 >>>>>>>>>>>>>>>>>>>>>>>>')
for i=1,#t_ip,1 do
	local t = t_ip[i]
	local ipName = t[1]
	local ipObj = ipAnalysis:getIpObject(ipName)
	Log(LOG_ERROR, '>>>>>>>> %s >>>>', ipName)
	Log(LOG_ERROR, '服务器  IP  账号  角色  级别  VIP  时间')
	ipObj:foreachPlayer(printIpPlayer, ipObj)
end

Log(LOG_CONSOLE+LOG_ERROR, '<<<<<<<<<<<<<<<<<<<<<<< OVER !!!! <<<<<<<<<<<<<<<<<<<<<<<<<<')

