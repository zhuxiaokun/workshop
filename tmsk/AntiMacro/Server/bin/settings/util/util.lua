function tostr(v)
	if type(v) == 'string' then
		return '\'' .. v .. '\''
	else
		return tostring(v)
	end
end

function tabstr(level)
	local s = ''
	for i=1,level,1 do
		s = s .. '\t'
	end
	return s
end

function printTable(t,level)
	if t == nil or type(t) ~= 'table' then
		print('error: printTable', 'is not a table')
		return
	end
	if level == nil then
		level = 0
	end
	for k,v in pairs(t) do
		if type(v) == 'table' then
			print(string.format('%s%s=',tabstr(level), tostr(k)))
			printTable(v, level+1)
		else
			print(string.format('%s%s=%s',tabstr(level), tostr(k), tostr(v)))
		end
	end
end

function _on_player(player, ctx)
	local t3 = {}
	t3.server = player:serverName()
	t3.acctName = player:acctName()
	t3.name = player:name()
	t3.level = player:level()
	t3.viplevel = player:viplevel()
	t3.lastActive = player:lastActive()
	table.insert(ctx, t3)
	return true
end

function _on_login(login, ctx)
	local t3 = {}
	t3.serverName = login:serverName()
	t3.acctName = login:acctName()
	t3.begin = time2str(login:begin())
	t3.duration = login:duration()/3600.0
	t3.level = login:level()
	t3.viplevel = login:viplevel()
	t3.map = login:map()
	t3.ip = login:ip()
	table.insert(ctx, t3)
	return true
end

function _on_item(t, c, ctx)
	ctx[t] = c
	return true
end

function _on_trade(trade,ctx)
	local t3 = {server=trade:serverName(),
				time = time2str(trade:time()),
				target=trade:target(),
				srcPlayer=trade:srcPlayer(),
				dstPlayer=trade:dstPlayer(),
				map=trade:map(),
				sVipLevel=trade:sVipLevel(),
				tVipLevel=trade:tVipLevel(),
				sip=trade:sip(),
				tip=trade:tip(),
				friendDegree=trade:friendDegree(),
				ilist={},
				olist={}}
	trade:foreachInItem(_on_item,t3.ilist)
	trade:foreachOutItem(_on_item,t3.olist)
	table.insert(ctx, t3)
	return true
end

function _on_server(name, server, ctx)
	local t = {player={},login={},trade={}}
	ctx[name] = t
	server:foreachPlayer(_on_player, t.player)
	server:foreachLogin(_on_login, t.login)
	server:foreachTrade(_on_trade, t.trade)
	return true
end

function _on_acct(acct, ctx)
	local t = {name=acct:name(),lastActive = time2str(acct:lastActive()),server={}}
	acct:foreachServer(_on_server, t.server)
	ctx[t.name] = t
	return true
end

function _on_serverAcct(playerObj, ctx)
	local acctName = playerObj:acctName()
	local player = playerObj:name()
	local lastActive = playerObj:lastActiver()
	local t = { }
	t.acctName = acctName
	t.player = player
	t.lastActive = time2str(lastActive)
	table.insert(ctx, t)
	return true
end

function _on_ipServer(server, ctx)
	local t = {}
	server:foreach(_on_serverAcct, t)
	ctx[server:name()] = t
	return true
end

function _on_ipObject(obj,ctx)
	local t = {name=obj:name(), server={}}
	obj:foreachServer(_on_ipServer, t.server)
	ctx[t.name] = t
	return true
end

function ipObject2table(ipObj)
	local t = { name=ipObj:name(), server={} }
	ipObj:foreachServer(_on_ipServer, t.server)
	return t
end

function loadIpObject(ip, ctx)
	local ipObj = ipAnalysis:loadIp(ip)
	if ipObj ~= nil then
		--print('success, load ip object for '..ip)
	else
		--print('fail to load object for '..ip)
	end
	return true
end

function onEach_DevServerAcct(playerObj, ctx)
	local acctName = playerObj:acctName()
	local player = playerObj:name()
	local lastActive = playerObj:lastActiver()
	local t = { }
	t.acctName = acctName
	t.player = player
	t.lastActive = time2str(lastActive)
	table.insert(ctx, t)
	return true
end

function onEach_DevServer(devServer, ctx)
	local t = {}
	local name = devServer:name()
	devServer:foreach(onEach_DevServerAcct, t)
	ctx[name] = t
	return true
end

function devObject2table(dev)
	local t = {}
	local devName = dev:name()
	dev:foreachServer(onEach_DevServer, t)
	return t
end

function acctObj2table(acct)
	local t = {name=acct:name(),lastActive = time2str(acct:lastActive()),server={}}
	acct:foreachServer(_on_server, t.server)
	return t
end

function cleanAnalysisResult(cnn, yyyymmdd)
	if not cnn:isReady() then
		return false
	end
	
	Log(LOG_CONSOLE+LOG_ERROR, 'warn: clean analysis for %d in t_dahao', yyyymmdd)
	
	local sql = string.format('delete from t_dahao where yyyymmdd=%d', yyyymmdd)
	local ret = cnn:execute(sql)
	if ret then
		Log(LOG_CONSOLE+LOG_ERROR, 'warn: clean analysis for %d in t_dahao, success', yyyymmdd)
	else
		Log(LOG_CONSOLE+LOG_ERROR, 'error: clean analysis for %d in t_dahao, %s', yyyymmdd, cnn:getLastError())
	end
	
	sql = string.format('delete from t_trade_detail where yyyymmdd=%d', yyyymmdd)
	ret = cnn:execute(sql)
	if ret then
		Log(LOG_CONSOLE+LOG_ERROR, 'warn: clean analysis for %d in t_trade_detail, success', yyyymmdd)
	else
		Log(LOG_CONSOLE+LOG_ERROR, 'error: clean analysis for %d in t_trade_detail, %s', yyyymmdd, cnn:getLastError())
	end
	
	return ret
end
