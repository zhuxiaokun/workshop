
dofile('../settings/macro_analysis_server/util.lua')

local hours = 72
local minTradeCount = 3

function loadAcct(name, ctx)
	if analysis:loadAcct(name, hours) then
		--print(ctx.c, 'load '..name..' success')
		ctx.c = ctx.c + 1
		return true
	else
		--print(ctx.c, 'fail to load '..name)
		ctx.c = ctx.c + 1
		return false
	end
end

local function isTrade_empty_i(tradeObj,ctx)
	return tradeObj:empty_i() and tradeObj:has_i(2000,2002,2003,2004,2016)
end
local function isTrade_empty_o(tradeObj,ctx)
	return tradeObj:empty_o() and tradeObj:has_i(2000,2002,2003,2004,2016)
end

local function onEach_Acct_for_tradeCount(acctObj, ctx)
	local n = acctObj:getTradeCount()
	if n >= minTradeCount then
		local t = {acctObj:name(), acctObj:getTradeCount()}
		table.insert(ctx, t)
	end
	return true
end

local function sort_t_tradeCount(a, b)
	return a[2] > b[2]
end

-- analysis:reset()
-- analysis:scanAcctList()
-- local allNameList = analysis:allNameList()
-- local t = { c = 1 }
-- allNameList:foreach(loadAcct, t)

--acctTable = {}
--analysis:foreachAcct(_on_acct, acctTable)

local t_tradeCount = {}
analysis:foreachAcct(onEach_Acct_for_tradeCount, t_tradeCount)
table.sort(t_tradeCount, sort_t_tradeCount)

-- 大号

local t_Acct_sh = {}
local t_tradeCount_sh = {}

for i=1,#t_tradeCount,1 do
	local acctName = t_tradeCount[i][1]
	--print('no.'..i, acctName, type(acctName))
	local acctObj = analysis:getAccount(acctName)
	local n = acctObj:countTrade(isTrade_empty_o,nil)
	if n >= 3 then
		local t = {acctName,n}
		table.insert(t_tradeCount_sh, t)
		t_Acct_sh[acctName] = {}
	end
end

Log(LOG_CONSOLE+LOG_ERROR, '>>>>>>>>>>>>>>>>>> 交易分析结果 >>>>>>>>>>>>>>>>>>>>')
Log(LOG_CONSOLE+LOG_ERROR, '大号 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>')

table.sort(t_tradeCount_sh, sort_t_tradeCount)
for i=1,#t_tradeCount_sh,1 do
	Log(LOG_CONSOLE+LOG_ERROR, '%  03d  %s %d', i, t_tradeCount_sh[i][1], t_tradeCount_sh[i][2])
end

-- 小号
local t_tmpAcct = {}

local function isTrade_gather(tradeObj,ctx)
	if tradeObj:empty_o() and tradeObj:has_i(2000,2002,2003,2004,2016) then
		local source = tradeObj:source()
		local target = tradeObj:target()
		if t_tmpAcct[target] == nil then
			table.insert(ctx[source], {target})
			t_tmpAcct[target] = true
		end
	end
	return true
end

for i=1,#t_tradeCount_sh,1 do
	local acctName = t_tradeCount_sh[i][1]
	--print('debug:', i, acctName, type(acctName))
	local acctObj = analysis:getAccount(acctName)
	t_tmpAcct = {}
	acctObj:foreachTrade(isTrade_gather, t_Acct_sh)
end

Log(LOG_CONSOLE+LOG_ERROR, '>>>>>>> 账号关联 >>>>>>>>>>>>')
for i=1,#t_tradeCount_sh,1 do
	local acctName = t_tradeCount_sh[i][1]
	local t = t_Acct_sh[acctName]
	Log(LOG_CONSOLE+LOG_ERROR, '大号 %s 下面的小号', acctName)
	if #t > 0 then
		for k=1,#t,1 do
			Log(LOG_CONSOLE+LOG_ERROR, '\t%03d  %s', k, t[k][1])
		end
	end
end


-- local t = acctObj2table(analysis:getAccount('VIVO73de5208adaa7a41'))
-- printTable(t)

Log(LOG_CONSOLE+LOG_ERROR, '<<<<<<<<<<<<<<<<<<<<<<< OVER !!!! <<<<<<<<<<<<<<<<<<<<<<<<<<')
