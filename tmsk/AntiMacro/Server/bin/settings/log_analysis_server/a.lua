
dofile('../settings/macro_analysis_server/util.lua')

local timeLine = 1445247323+1

function onEach_DevName(devName, ctx)
	print('loading', devName)
	local dev = devAnalysis:loadDev(devName)
	if dev ~= nil then
		print('load OK')
	else
		print('fail to load!')
	end
	return true
end

function onEach_DevObject(dev, ctx)
	local name = dev:name()
	ctx[name] = devObject2table(dev)
	return true
end


--local m = getmetatable(redisEraser)
--printTable(m)
--redisEraser:eraseAccount(timeLine)
--redisEraser:eraseLoginList(timeLine)
--redisEraser:eraseTradeList(timeLine)
--redisEraser:eraseIpAcctList(timeLine)

--redisEraser:eraseHistory(24)

local m = getmetatable(devAnalysis)
printTable(m)

devAnalysis:reset()
devAnalysis:scanDevList()

devAnalysis:foreachDevName(onEach_DevName, nil)

local devTable = {}
devAnalysis:foreachDevObject(onEach_DevObject, devTable)

printTable(devTable)



