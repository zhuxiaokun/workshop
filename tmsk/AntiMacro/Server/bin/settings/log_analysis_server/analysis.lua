
local yyyymmdd = 20151027
local platform = 'android'
local dir = 'D:\\Data\\Android'
local truncateDatabase = true

local argc = select('#',...)
if argc > 0 then
	local arg = select(1,...)
	if type(arg) == 'number' then
		yyyymmdd = arg
	elseif type(arg) == 'table' then
		if arg['yyyymmdd'] ~= nil then yyyymmdd = arg['yyyymmdd'] end
		if arg['platform'] ~= nil then platform = arg['platform'] end
		if arg['dir'] ~= nil then dir = arg['dir'] end
		if arg['truncate'] ~= nil then truncateDatabase = arg['truncate'] end
	end
end

if yyyymmdd < 1 then
	yyyymmdd = to_yyyymmdd(getFirstDayTime(0, -1))
end

local analysisContext = {
	platform = platform,
	fromTime = getFirstDayTime(yyyymmdd),
	toTime = getFirstDayTime(yyyymmdd, 1),
	dir = dir,
	fileName = 'Analysis.log',
}

local reload = true
local minTradeCount = 5
local minMacroTradeCount = 5
local host = '127.0.0.1'
local port = 3306
local dbname = 'analysis'
local user = 'root'
local passwd = '123456'

Log(LOG_CONSOLE+LOG_ERROR, '>>>> start trade analysis >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>')
Log(LOG_CONSOLE+LOG_ERROR, '>>>>   parameters:')
Log(LOG_CONSOLE+LOG_ERROR, '>>>>      yyyymmdd = %d', yyyymmdd)
Log(LOG_CONSOLE+LOG_ERROR, '>>>>      data dir = %s', dir)
Log(LOG_CONSOLE+LOG_ERROR, '>>>>      platform = %s', platform)
Log(LOG_CONSOLE+LOG_ERROR, '>>>>      from = %s', analysisContext.fromTime)
Log(LOG_CONSOLE+LOG_ERROR, '>>>>      to = %s', analysisContext.toTime)
Log(LOG_CONSOLE+LOG_ERROR, '>>>>      log file name = %s', analysisContext.fileName)
Log(LOG_CONSOLE+LOG_ERROR, '>>>>      truncate database = %s', tostring(truncateDatabase))
Log(LOG_CONSOLE+LOG_ERROR, '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<')

dofile('../settings/util/util.lua')
dofile('../settings/util/dbutil.lua')

local tbTradeMaster = string.format('t_trade_master_%d', yyyymmdd)
local tbTradeDetail = string.format('t_trade_detail_%d', yyyymmdd)

local function reloadAcctData()
	local dirName = analysisContext.dir
	local platformName = analysisContext.platform
	local fileName = analysisContext.fileName
	local ftime = analysisContext.fromTime
	local ttime = analysisContext.toTime
	Log(LOG_CONSOLE+LOG_ERROR, '>>>> analysis start load acct info')
	analysis:loadData(dirName, platformName, fileName, ftime, ttime)
	Log(LOG_CONSOLE+LOG_ERROR, '<<<< analysis load acct info ok!')
end

local function isTrade_empty_i(tradeObj,ctx)
	return tradeObj:empty_i() and tradeObj:has_i(2000,2002,2003,2004,2016)
end
local function isTrade_empty_o(tradeObj,ctx)
	return tradeObj:empty_o() and tradeObj:has_i(2000,2002,2003,2004,2016)
end

local i_tradeCount = 0
local function onEach_Acct_for_tradeCount(acctObj, ctx)
	i_tradeCount = i_tradeCount + 1
	local name = acctObj:name()
	--Log(LOG_DEBUG, 'debug: %d. acct trade for %s', i_tradeCount, name)
	local n = acctObj:getTradeCount()
	if n >= minTradeCount then
		local c = acctObj:countTrade(isTrade_empty_o, nil)
		if c >= minMacroTradeCount then
			local t = {name, c }
			table.insert(ctx, t)
		end
	end
	return true
end

local function printMacroTrade(tradeObj, ctx)
	if isTrade_empty_o(tradeObj, nil) then
		local tm = time2str(tradeObj:time())
		local serverName = tradeObj:serverName()
		local target = tradeObj:target()
		local splayer = tradeObj:srcPlayer()
		local tplayer = tradeObj:dstPlayer()
		local sviplevel = tradeObj:sVipLevel()
		local tviplevel = tradeObj:tVipLevel()
		local sip = tradeObj:sip()
		local tip = tradeObj:tip()
		local ilist = tradeObj:ilist()
		local olist = tradeObj:olist()
		Log(LOG_DEBUG, '[result] %s  %s  %s  %s  %s  %d  %d  %s  %s  \'%s\'  \'%s\'',
			serverName, tm, target, splayer, tplayer,
			sviplevel, tviplevel, sip, tip, ilist, olist)
		local ti = {
				yyyymmdd = yyyymmdd,
				platform = platform,
				zone_id = serverName,
				time = tm,
				s_uid = tradeObj:source(),
				t_uid = target,
				s_rid = splayer,
				t_rid = tplayer,
				s_ip = sip,
				t_ip = tip,
				i_list = ilist,
				o_list = olist,
		}
		local cnn = ctx
		insertAnalysis_tradeDetail(cnn, tbTradeDetail, ti)
	end
	return true
end

local function sort_t_tradeCount(a, b)
	return a[2] > b[2]
end

Log(LOG_CONSOLE+LOG_ERROR, '[result] >>>>>>> start analysis account info')

-- 加载数据
if reload then
	reloadAcctData()
end

-- 统计问题交易次数超过规定次数的账号，并按交易次数从大到小排序

Log(LOG_DEBUG, '>>>>>>> count trade count for account')

-- 大号
local t_tradeCount = {}
analysis:foreachAcct(onEach_Acct_for_tradeCount, t_tradeCount)

Log(LOG_CONSOLE+LOG_ERROR, '>>>>>>> sort account by trade count desc')
table.sort(t_tradeCount, sort_t_tradeCount)

Log(LOG_CONSOLE+LOG_ERROR, '[result]>>>> 交易分析结果')
Log(LOG_CONSOLE+LOG_ERROR, '[result]>>>> 大号')

local cnn = mysql()
if not cnn:connect(host, port, dbname, user, passwd) then
	Log(LOG_CONSOLE+LOG_ERROR, 'error: connect to database %s:%d %s', host, port, dbname)
end

if truncateDatabase then
	Log(LOG_CONSOLE+LOG_ERROR, '[result]>>>> 清除原有数据库中的数据')
	resetAnalysisTable(cnn, yyyymmdd)
end

-- 输出大号列表
Log(LOG_CONSOLE+LOG_ERROR, '[result] >>>>> 交易大号入库 ')
Log(LOG_DEBUG, '[result] 序号  账号  数量')
for i=1,#t_tradeCount,1 do
	local acctName = t_tradeCount[i][1]
	local tradeCount = t_tradeCount[i][2]
	Log(LOG_DEBUG, '[result] %d  %s  %d', i, acctName, tradeCount)
	local ti = {
		yyyymmdd=yyyymmdd,
		platform = platform,
		uid = acctName,
		trade_count = tradeCount,
	}
	insertAnalysis_tradeMaster(cnn, tbTradeMaster, ti)
end

-- 小号
local function isTrade_gather(tradeObj,ctx)
	if tradeObj:empty_o() and tradeObj:has_i(2000,2002,2003,2004,2016) then
		local source = tradeObj:source()
		local target = tradeObj:target()
		local lst = ctx.list
		local map = ctx.map
		if map[target] == nil then
			table.insert(lst, {target})
			map[target] = true
		end
	end
	return true
end

local t_relation = {}
for i=1,#t_tradeCount,1 do
	local acctName = t_tradeCount[i][1]
	local acctObj = analysis:getAccount(acctName)
	local t = {acctName, { list={}, map={} } }
	acctObj:foreachTrade(isTrade_gather, t[2])
	table.insert(t_relation, t)
end

Log(LOG_DEBUG, '[result] >>>> 账号关联 [大号和小号]')
for i=1,#t_relation,1 do
	local acctName = t_relation[i][1]
	local t = t_relation[i][2].list
	Log(LOG_DEBUG, '[result]>>>> %s', acctName)
	if #t > 0 then
		for k=1,#t,1 do
			Log(LOG_DEBUG, '[result] \t%03d  %s', k, t[k][1])
		end
	end
end

-- 输出大号交易详细信息
Log(LOG_CONSOLE+LOG_ERROR, '[result] >>>>>>> 交易明细入库')
for i=1,#t_tradeCount,1 do
	local acctName = t_tradeCount[i][1]
	local acctObj = analysis:getAccount(acctName)
	Log(LOG_DEBUG, '[result] >>>> %s', acctName)
	Log(LOG_DEBUG, '[result] 服务器  时间  目标账号  源角色  目标角色  源VIP  目标VIP  源IP  目标IP  输入  输出')
	acctObj:foreachTrade(printMacroTrade, cnn)
end

Log(LOG_CONSOLE+LOG_ERROR, '[result] <<<<<<<<<<<<<<<<<<<<<<< OVER !!!! <<<<<<<<<<<<<<<<<<<<<<<<<<')
