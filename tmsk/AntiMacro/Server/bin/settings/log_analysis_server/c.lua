dofile('../settings/macro_analysis_server/util.lua')

local maxAcctCountPerIp = 3

local function sort_func(a, b)
	--print(a, b)
	if a[2] > b[2] then
		return true
	else
		return false
	end
end

function convIpObject2table(obj, ctx)
	local ipName = obj:name()
	ctx[ipName] = ipObject2table(obj)
	return true
end

function pickProblemIps(ipObj, ctx)
	if ipObj:getAcctCount() > maxAcctCountPerIp then
		local t = {ipObj:name(), ipObj:getAcctCount()}
		table.insert(ctx, t)
	end
	return true
end

-- 清空历史数据
ipAnalysis:reset()

-- 扫描所有符合特征的IP，并加载详细数据
ipAnalysis:scanPatternIpList('*')
ipAnalysis:foreachIpName(loadIpObject, nil)

--将C++对象转换到LUA的表中
local ipObjTables = {}
ipAnalysis:foreachIpObject(convIpObject2table, ipObjTables)

-- 显示数据
--printTable(ipObjTables)

--printTable(ipObjTables['192.168.0.1']['server']['1'])

-- 寻找在线数超过指定数量的IP
local problemIps = {}

ipAnalysis:foreachIpObject(pickProblemIps, problemIps)

if #problemIps < 1 then
	print('恭喜 no problem ip found.')
	return
end

table.sort(problemIps, sort_func)
print('problemIps is:')
for i=1,#problemIps,1 do
	print(problemIps[i][1], problemIps[i][2])
end

for i=1,10,1 do
	print('no.'..i, problemIps[i][1])
	printTable(ipObjTables[problemIps[i][1]])
end

