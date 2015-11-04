
function assureTableExist(cnn, tableName, templateTableName)
	local sql = string.format('create table if not exists %s like %s', tableName, templateTableName)
	if not cnn:execute(sql) then
		Log(LOG_CONSOLE+LOG_ERROR, 'error: sql %s, %s', sql, cnn:getLastError())
		return false
	end
	return true
end

function truncateTable(cnn, tableName)
	local sql = string.format('truncate table %s', tableName)
	if not cnn:execute(sql) then
		Log(LOG_CONSOLE+LOG_ERROR, 'error: sql %s, %s', sql, cnn:getLastError())
		return false
	end
	return true
end

function resetAnalysisTable(cnn, yyyymmdd)
	local tbMaster = string.format('t_trade_master_%d', yyyymmdd)
	local tbDetail = string.format('t_trade_detail_%d', yyyymmdd)
	local b3 = assureTableExist(cnn, tbMaster, 't_trade_master')
	local b4 = assureTableExist(cnn, tbDetail, 't_trade_detail')
	local b1 = truncateTable(cnn, tbMaster)
	local b2 = truncateTable(cnn, tbDetail)
	return b1 and b2 and b3 and b4
end

function resetHistoryTable(cnn, yyyymmdd)
	local tbHistory = string.format('t_trade_history_%d', yyyymmdd)
	local b2 = assureTableExist(cnn, tbHistory, 't_trade_history')
	local b1 = truncateTable(cnn, tbHistory)
	return b1 and b2
end

function insertAnalysis_tradeMaster(cnn, dbTableName, t)
	if not cnn:isReady() then
		return false
	end

	local yyyymmdd = t.yyyymmdd
	local platform = t.platform
	local uid = t.uid
	local tradeCount = t.trade_count
	local zoneCount = t.zone_count

	if platform == nil then
		platform = 'unknown'
	end

	local sql = string.format('insert into %s(yyyymmdd,platform,uid,trade_count,zone_count) \
		values(%d,"%s","%s",%d)', dbTableName,yyyymmdd,platform,uid,tradeCount, zoneCount)
	local ret = cnn:execute(sql)
	if not ret then
		Log(LOG_CONSOLE+LOG_ERROR, 'error: insert in t_dahao, %d %s, %s', yyyymmdd, uid, cnn:getLastError())
	end
	return ret
end

function insertAnalysis_tradeDetail(cnn, dbTableName, t)
	if not cnn:isReady() then
		return false
	end

	local yyyymmdd = t.yyyymmdd
	local platform = t.platform
	local zone_id = t.zone_id
	local time = t.time
	local s_uid = t.s_uid
	local t_uid = t.t_uid
	local s_rid = t.s_rid
	local t_rid = t.t_rid
	local s_ip = t.s_ip
	local t_ip = t.t_ip
	local i_list = t.i_list
	local o_list = t.o_list

	if platform == nil then
		platform = 'unknown'
	end

	local sql = string.format('insert into %s(yyyymmdd,platform,zone_id,time,s_uid,t_uid,s_rid,t_rid,s_ip,t_ip,i_list,o_list) values(%d,"%s","%s","%s","%s","%s","%s","%s","%s","%s","%s","%s")',
		dbTableName,yyyymmdd,platform,zone_id,time,s_uid,t_uid,s_rid,t_rid,s_ip,t_ip,i_list,o_list)
	local ret = cnn:execute(sql)
	if not ret then
		Log(LOG_CONSOLE+LOG_ERROR, 'error: insert in t_trade_detail, %d %s, %s', yyyymmdd, s_uid, cnn:getLastError())
	end
	return ret
end

function insertTradeHistory(cnn, dbTableName, t)
	local platform = t.platform
	local zone_id = t.zone_id
	local s_uid = t.s_uid
	local t_uid = t.t_uid
	local s_rid = t.s_rid
	local t_rid = t.t_rid
	local s_ip = t.s_ip
	local t_ip = t.t_ip
	local s_viplevel = t.s_viplevel
	local t_viplevel = t.t_viplevel
	local i_list = t.i_list
	local o_list = t.o_list
	local map = t.map
	local friend_degree = t.friend_degree
	local time = t.time

	if platform == nil then
		platform = 'unknown'
	end

	local sql = string.format('insert into %s(zone_id,s_uid,t_uid,s_rid,t_rid,s_ip,t_ip,s_viplevel,t_viplevel,i_list,o_list,map,friend_degree,time) \
		values("%s","%s","%s","%s","%s","%s","%s",%d,%d,"%s","%s",%d,%d,"%s")',
		dbTableName,zone_id,s_uid,t_uid,s_rid,t_rid,s_ip,t_ip,s_viplevel,t_viplevel,i_list,o_list,map,friend_degree,time)

	local ret = cnn:execute(sql)
	if not ret then
		Log(LOG_CONSOLE+LOG_ERROR, 'error: insert in t_trade_history, %s, %s', s_uid, cnn:getLastError())
	end
	return ret
end
